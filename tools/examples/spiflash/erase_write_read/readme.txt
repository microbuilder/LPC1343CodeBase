OVERVIEW
============================================================
This example will erae one 4KB page of SPI Flash, write
8 bytes worth of data, and then read back 16 bytes of
data to show how all three operations work.

HOW TO USE THIS EXAMPLE
============================================================
1.) Connect the SPI Flash to SPI on the LPC1343

2.) Configure your terminal software to open the USB COM
    port at 115K.

3.) When the application starts, there is a 5 second delay
    (to allow you time to connect via USB CDC), after which 
    point the device will run through the erase, write, read
    sequence once and then wait in an infinite loop.
