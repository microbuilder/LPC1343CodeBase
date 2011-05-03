Uses 'PROMISCUOUS' mode in Chibi, which listens to ANY message available within
hearing range, and retransmits the raw frame data over UART in libpcap format.
Using the open source 'wsbridge' application (tools/wsbridge), the data can
be piped into wireshark on any Windows or Linux PC.  This useful functionality
is perfect for debugging wireless sensor networks since you can capture, log and
analyse all traffic and frame data moving around the wireless sensor network.

For more information on wsbridge see: 
http://freaklabs.org/index.php/Tutorials/Software/Feeding-the-Shark-Turning-the-Freakduino-into-a-Realtime-Wireless-Protocol-Analyzer-with-Wireshark.html