This simple code can be used to test the startup delay, or more precisely
the delay between the moment that reset is deasserted and code execution
begins in main().

GPIO pin 2.1 is set to output and high as soon as main is entered, so you
simply need to monitor the reset line and GPIO 2.1 on a two-channel
oscilliloscope, and measure the delay between the two rising edges.

Two sample tests results are showing for GCC with a Makefile and with
Crossworks for ARM.  Both examples are included since the startup code
for both project types is slightly different, with Crossworks using a
custom startup file provided by Rowley Associates, and the LPC1343 
Code Base using it's own startup code when compiling with make and GCC.

Note: This code is only useful for measuring the time coming out of
reset, not from a cold boot (applying power to an unpowered device).