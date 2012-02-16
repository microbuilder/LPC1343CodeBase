OVERVIEW
============================================================
This example will wait for a Mifare Ultralight card to enter
the RF field, and then try to dump the card's contents to
USB CDC.

HOW TO USE THIS EXAMPLE
============================================================
1.) Connect the PN532 NFC Breakout Board to UART on the
    LPC1343 as follows:

    PN532   LPC1343
    -----   -------
    GND     GND
    TXD     RXD
    RXD     TXD
    3.3V    3.3V

    Set SEL0 to Off and SEL1 to Off (= UART)

    or via I2C as follows:

    PN532   LPC1343
    -----   -------
    GND     GND
    SDA     SDA
    SCL     SCL
    IRQ     3.2
    3.3V    3.3V

    Set SEL0 to On and SEL1 to Off (= I2C)

2.) Configure your terminal software to open the USB COM
    port at 115K.

3.) When the application starts, there is a 5 second delay
    (to allow you time to connect via USB CDC), after which 
    point the device will wait for a single ISO14443A card
    (Mifare Classic, etc.) and try to read it's ID.  Once a
    card is found, the reader will start looking again for a
    card after a 1 second delay.

SAMPLE OUTPUT
============================================================

When a Mifare Ultralight card is placed in the field you
should see results similar to the following:

	Please insert a Mifare Ultralight card
	UID: 04 7b cb 51 96 22 80

	Page  Hex       Text
	----  --------  ----
	0x00  047bcb3c  .{Ë<
	0x01  51962280  Q–"€
	0x02  6548ff7f  eHÿ
	0x03  e1100600  á...
	0x04  031dd101  ..Ñ.
	0x05  19550174  .U.t
	0x06  7461672e  tag.
	0x07  62652f6d  be/m
	0x08  2f303437  /047
	0x09  42434235  BCB5
	0x0a  31393632  1962
	0x0b  32383050  280P
	0x0c  c7badbbb  ÇºÛ»
	0x0d  3df622c1  =ö"Á
	0x0e  68e88f24  hè$
	0x0f  00000000  ....
