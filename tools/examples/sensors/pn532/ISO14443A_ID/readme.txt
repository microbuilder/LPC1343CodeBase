OVERVIEW
============================================================
This example will wait for an ISO14443A card to enter
the RF field, and then try to determine the specific card
model (SENS_RES and SEL_RES) as well as the card's NFCID.

All information will be sent to USBCDC by default, with the
PN532 breakout board connected via either UART or I2C (the
bus can be selected in PN532_bus.h).

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

When a card is placed in the field you should see results 
similar to the following (results shown for several
different card types):

	Waiting for an ISO14443A card (Mifare Classic, etc.)

	Received    : 00 00 ff 0c f4 d5 4b 01 01 00 04 08 04 9e b3 6e 66 a9 00
	Tags Found  : 1
	SENS_RES    : 00 04
	SEL_RES     : 08
	NFCID       : 9e b3 6e 66
	Seems to be a Mifare Classic 1K Card

	Waiting for an ISO14443A card (Mifare Classic, etc.)

	Received    : 00 00 ff 0c f4 d5 4b 01 01 00 02 18 04 8a 4c b5 12 23 00
	Tags Found  : 1
	SENS_RES    : 00 02
	SEL_RES     : 18
	NFCID       : 8a 4c b5 12

	Waiting for an ISO14443A card (Mifare Classic, etc.)

	Received    : 00 00 ff 0f f1 d5 4b 01 01 00 44 00 07 04 7b cb 51 96 22 80 c0 00
	Tags Found  : 1
	SENS_RES    : 00 44
	SEL_RES     : 00
	NFCID       : 04 7b cb 51 96 22 80

	Waiting for an ISO14443A card (Mifare Classic, etc.)

	Received    : 00 00 ff 0c f4 d5 4b 01 01 00 04 08 04 6e 8a e2 b0 44 00
	Tags Found  : 1
	SENS_RES    : 00 04
	SEL_RES     : 08
	NFCID       : 6e 8a e2 b0
	Seems to be a Mifare Classic 1K Card

	Waiting for an ISO14443A card (Mifare Classic, etc.)

