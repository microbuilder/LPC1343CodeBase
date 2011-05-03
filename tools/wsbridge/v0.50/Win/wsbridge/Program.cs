using System;
using System.IO;
using System.IO.Ports;
using System.IO.Pipes;

namespace wsbridge
{
    class Program
    {
        static NamedPipeServerStream wspipe;
        static BinaryWriter ws;
        static SerialPort p;
        const uint BUFSIZE = 1024;
        static byte[] b = new byte[BUFSIZE];
        static uint wr_idx = 0;
        static uint rd_idx = 0;
        static byte len;
        static FSM state;
        static bool file_write = false;
        static long start_time_in_ticks;
        static byte byte_ctr;
        static String port;

        enum FSM
        {
            START_CAPTURE,
            PACKET_CAPTURE
        }

        const int PACKET_FCS = 2;
        const int PACKET_LEN = 1;
        const bool DEBUG_PRINT = true;

        static void Main(string[] args)
        {
            // Commane line options
            if (args.Length == 0)
            {
                // generate list of active serial ports
                string[] names = SerialPort.GetPortNames();
                Console.WriteLine("Serial ports:");
                foreach (string name in names) Console.WriteLine(name);
                Console.Write("Choose one:");
                port = Console.ReadLine();
            }
            else if (args.Length == 1)
            {
                port = args[0];
            }
            else
            {
                Console.WriteLine("Usage: wsbridge <portname>");
                Console.WriteLine("or leave portname blank for a list of ports.");
                Environment.Exit(0);
            }

            // Open serial port
            try
            {
                p = new SerialPort(port, 115200, Parity.None, 8, StopBits.One);
                p.Open();
            }
            catch (Exception e)
            {
                // ooops, serial port can't be opened. throw exception, print message, and exit
                Console.WriteLine("Error opening serial port. Msg = " + e.Message);
                Environment.Exit(0);
            }
            Console.WriteLine("Serial port opened successfully.");

            // create pipe
            try
            {
                wspipe = new NamedPipeServerStream("wireshark", PipeDirection.Out);
            }
            catch (Exception e)
            {
                Console.WriteLine("Error opening pipe. Msg = " + e.Message);
                p.Close();
                Environment.Exit(0);
            }
            
            // wait for wireshark to connect to pipe
            Console.WriteLine("Waiting for connection to wireshark.");
            Console.WriteLine("Open wireshark and connect to interface: Local:\\\\.\\pipe\\wireshark");
            wspipe.WaitForConnection();
            Console.WriteLine("Client connected.");

            // connect binary writer to pipe to write binary data into it
            ws = new BinaryWriter(wspipe);

            // add serial data capture
            p.DataReceived += new SerialDataReceivedEventHandler(serialPort_DataReceived);
            state = FSM.START_CAPTURE;
            
            // keep track of time started. this will be used for timestamps
            start_time_in_ticks = DateTime.Now.Ticks; 

            // generate header to identify the packet capture
            write_global_hdr();

            // run forever
            while (true)
            {
            }
        }

        // serial port handler. this gets executed whenever data is available on the serial port
        static void serialPort_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            uint frame_len, bytes_in_buf;

            // loop until serial port buffer is empty
            while (p.BytesToRead != 0)
            {
                switch (state)
                {
                    case FSM.START_CAPTURE:
                        // starting a new frame. the first byte will indicate the length
                        len = (byte)p.ReadByte();
                        b[wr_idx] = len;
                        byte_ctr = 0;

                        if (DEBUG_PRINT)
                        {
                            Console.WriteLine();
                            Console.Write(String.Format("{0,2:X}", b[wr_idx]) + ' ');
                        }

                        // increment the buffer index and wrap back to 0 if it reaches the max size
                        // (standard circular buffer behavior)
                        wr_idx = (wr_idx + 1) % BUFSIZE;
                        state = FSM.PACKET_CAPTURE;
                        break;

                    case FSM.PACKET_CAPTURE:
                        // capture bytes until the total length of the frame
                        b[wr_idx] = (byte)p.ReadByte();

                        if (DEBUG_PRINT)
                        {
                            Console.Write(String.Format("{0,2:X}", b[wr_idx]) + ' ');
                        }

                        wr_idx = (wr_idx + 1) % BUFSIZE;
                        byte_ctr++;

                        // we've captured all bytes in frame. tell the next section we're ready to write
                        // the frame into wireshark
                        if (byte_ctr == (len - 1))
                        {
                            state = FSM.START_CAPTURE;
                            file_write = true;
                        }
                        break;
                }
            }

            // at least one frame has been captured. write it into wireshark
            while (file_write == true)
            {
                frame_len = b[rd_idx];
                bytes_in_buf = calc_bytes_in_buf();

                // compare the frame lengto the number of bytes in the buffer. if the bytes in buffer are
                // greater than the frame length, then we have more than one frame. loop through until 
                // all complete frames have been written to wireshark
                if (bytes_in_buf > frame_len)
                {
                    // more than one frame in buffer. don't indicate we're finished yet in case we have 
                    // other frames we can send out.
                    write_frame(frame_len);
                }
                else if (bytes_in_buf == frame_len)
                {
                    // only one frame in buffer. indicate we're finished after we write this frame out.
                    write_frame(frame_len);
                    file_write = false;
                }
                else
                {
                    // what?! no frames in buffer? we shouldn't reach here. 
                    file_write = false;
                }
            }
        }

        // return the number of bytes in the buffer
        static uint calc_bytes_in_buf()
        {
            if (rd_idx > wr_idx)
            {
                // since we're using a circular buffer, its possible for the write index to be less
                // than the read index if a wraparound occurred. handle this case here.
                return (BUFSIZE - (rd_idx - wr_idx));
            }
            else
            {
                // normal way to calculate bytes in the buffer
                return (wr_idx - rd_idx);
            }
        }

        // this is the global header that starts any packet capture file. this will tell wireshark what 
        // kind of protocol it is (indicated by the DLT) as well as other information like endianness, etc.
        static void write_global_hdr()
        {
            uint magic_num = 0xa1b2c3d4;    // used for endianness
            short version_major = 2;        // version
            short version_minor = 4;        // version
            int thiszone = 0;               // zone (unused)
            uint sigfigs = 0;               // significant figures (unused)
            uint snaplen = 65535;           // snapshot length (max value)
            uint network = 195;             // Data Link Type (DLT): indicates link layer protocol

            try
            {
                // write to wireshark pipe
                ws.Write(magic_num);
                ws.Write(version_major);
                ws.Write(version_minor);
                ws.Write(thiszone);
                ws.Write(sigfigs);
                ws.Write(snaplen);
                ws.Write(network);
            }
            catch
            {
                Console.WriteLine("Pipe has been closed.");
                close();
            }
        }

        // this writes a frame header into wireshark in libpcap format. the format is simple and just
        // requires a timestamp and length
        static void write_frm_hdr(long sec, long usec, uint incl_len, uint orig_len)
        {
            try
            {
                // write to wireshark
                ws.Write((uint)sec);
                ws.Write((uint)usec);
                ws.Write(incl_len);
                ws.Write(orig_len);
            }
            catch
            {
                Console.WriteLine("Pipe has been closed.");
                close();
            }
       }

        // this writes a frame into wireshark. it calculates the timestamp and length and uses that 
        // for the frame header. it then writes captured bytes into wireshark
        static void write_frame(uint frame_len)
        {
            uint incl_len, orig_len;
            long sec, usec;

            // generating timestamp. its kind of cheesy but there isn't a unix timestamp mechanism in win. 
            // just counting ticks from when program was started. each tick is 100 nsec. 
            long diff_in_ticks = DateTime.Now.Ticks - start_time_in_ticks;  // get difference in ticks
            sec = diff_in_ticks / TimeSpan.TicksPerSecond;                  // get seconds
            diff_in_ticks -= (sec * TimeSpan.TicksPerSecond);               // subtract off seconds from total
            usec = diff_in_ticks / 10;                                      // get usec

            // calculate frame length. we won't be feeding frame checksum (FCS) into wireshark. 
            incl_len = (uint)frame_len - PACKET_FCS;
            orig_len = frame_len;

            // increment over the length byte. we won't feed that into wireshark either. it doesn't seem to like it. 
            rd_idx = (rd_idx + 1) % BUFSIZE;

            // write frame header first
            write_frm_hdr(sec, usec, incl_len, orig_len);

            // loop through entire length and write data into wireshark
            for (int i = 0; i < incl_len; i++)
            {
                try
                {
                    ws.Write(b[rd_idx]);
                }
                catch
                {
                    Console.WriteLine("Pipe has been closed.");
                    close();
                }
                rd_idx = (rd_idx + 1) % BUFSIZE;
            }
            rd_idx = (rd_idx + 1) % BUFSIZE;
        }

        // Received some type of termination. Close everything and wrap up.
        static void close()
        {
            p.Close();
            wspipe.Close();
            ws.Close();
            Console.WriteLine("Press <Enter> key to quit.");
            Console.ReadLine();
            Environment.Exit(0);
        }
    }
}
