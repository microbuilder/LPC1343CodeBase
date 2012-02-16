OVERVIEW
============================================================
This example will wait for a Mifare Classic 1K or 4K card to
enter the RF field, and then try to dump the card's contents
to USB CDC.

It will work with empty Mifare Classic cards using the
default authentication keys:

  Key 1 = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  Key 2 = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

Or with NDEF records using the following keys:

  Key 1 = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5 };
  Key 2 = { 0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7 };

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
	
4.) Select whether the default or NDEF keys should be used
    by setting CARDFORMAT_NDEF to either 1 or 0.

SAMPLE OUTPUT
============================================================

Bare Mifare Classic Card Example
------------------------------------------------------------
When a new Mifare Classic card is placed in the field you
should see results similar to the following:

	-------------------------Sector 00--------------------------
	Block 00: 6e8ae2b0b60804006263646566676869  nŠâ°¶...bcdefghi
	Block 01: 00000000000000000000000000000000  ................
	Block 02: 00000000000000000000000000000000  ................
	Block 03: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 01--------------------------
	Block 04: 00000000000000000000000000000000  ................
	Block 05: 00000000000000000000000000000000  ................
	Block 06: 00000000000000000000000000000000  ................
	Block 07: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 02--------------------------
	Block 08: 00000000000000000000000000000000  ................
	Block 09: 00000000000000000000000000000000  ................
	Block 10: 00000000000000000000000000000000  ................
	Block 11: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 03--------------------------
	Block 12: 00000000000000000000000000000000  ................
	Block 13: 00000000000000000000000000000000  ................
	Block 14: 00000000000000000000000000000000  ................
	Block 15: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 04--------------------------
	Block 16: 00000000000000000000000000000000  ................
	Block 17: 00000000000000000000000000000000  ................
	Block 18: 00000000000000000000000000000000  ................
	Block 19: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 05--------------------------
	Block 20: 00000000000000000000000000000000  ................
	Block 21: 00000000000000000000000000000000  ................
	Block 22: 00000000000000000000000000000000  ................
	Block 23: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 06--------------------------
	Block 24: 00000000000000000000000000000000  ................
	Block 25: 00000000000000000000000000000000  ................
	Block 26: 00000000000000000000000000000000  ................
	Block 27: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 07--------------------------
	Block 28: 00000000000000000000000000000000  ................
	Block 29: 00000000000000000000000000000000  ................
	Block 30: 00000000000000000000000000000000  ................
	Block 31: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 08--------------------------
	Block 32: 00000000000000000000000000000000  ................
	Block 33: 00000000000000000000000000000000  ................
	Block 34: 00000000000000000000000000000000  ................
	Block 35: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 09--------------------------
	Block 36: 00000000000000000000000000000000  ................
	Block 37: 00000000000000000000000000000000  ................
	Block 38: 00000000000000000000000000000000  ................
	Block 39: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 10--------------------------
	Block 40: 00000000000000000000000000000000  ................
	Block 41: 00000000000000000000000000000000  ................
	Block 42: 00000000000000000000000000000000  ................
	Block 43: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 11--------------------------
	Block 44: 00000000000000000000000000000000  ................
	Block 45: 00000000000000000000000000000000  ................
	Block 46: 00000000000000000000000000000000  ................
	Block 47: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 12--------------------------
	Block 48: 00000000000000000000000000000000  ................
	Block 49: 00000000000000000000000000000000  ................
	Block 50: 00000000000000000000000000000000  ................
	Block 51: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 13--------------------------
	Block 52: 00000000000000000000000000000000  ................
	Block 53: 00000000000000000000000000000000  ................
	Block 54: 00000000000000000000000000000000  ................
	Block 55: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 14--------------------------
	Block 56: 00000000000000000000000000000000  ................
	Block 57: 00000000000000000000000000000000  ................
	Block 58: 00000000000000000000000000000000  ................
	Block 59: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ
	-------------------------Sector 15--------------------------
	Block 60: 00000000000000000000000000000000  ................
	Block 61: 00000000000000000000000000000000  ................
	Block 62: 00000000000000000000000000000000  ................
	Block 63: 000000000000ff078069ffffffffffff  ......ÿ.€iÿÿÿÿÿÿ

NDEF Example
------------------------------------------------------------
When an NDEF formatted Mifare Classic card is placed in the
field you should see results similar to the following:

Note: In this case, only the first two sectors are formatted
for NDEF using the authentication key, so attempts to read
the other sectors fail because the wrong keys are provided

	-------------------------Sector 00--------------------------
	Block 00: 9eb36e66250804006263646566676869  ž³nf%...bcdefghi
	Block 01: 140103e103e103e103e103e103e103e1  ...á.á.á.á.á.á.á
	Block 02: 03e103e103e103e103e103e103e103e1  .á.á.á.á.á.á.á.á
	Block 03: 000000000000787788c1000000000000  ......xwˆÁ......
	-------------------------Sector 01--------------------------
	Block 04: 00000311d1010d550161646166727569  ....Ñ..U.adafrui
	Block 05: 742e636f6dfe00000000000000000000  t.comþ..........
	Block 06: 00000000000000000000000000000000  ................
	Block 07: 0000000000007f078840000000000000  .......ˆ@......
	-------------------------Sector 02--------------------------
	Block 08: Unable to read this block
	Block 09: Unable to read this block
	Block 10: Unable to read this block
	Block 11: Unable to read this block
	-------------------------Sector 03--------------------------
	Block 12: Unable to read this block
	Block 13: Unable to read this block
	Block 14: Unable to read this block
	Block 15: Unable to read this block
	-------------------------Sector 04--------------------------
	Block 16: Unable to read this block
	Block 17: Unable to read this block
	Block 18: Unable to read this block
	Block 19: Unable to read this block
	-------------------------Sector 05--------------------------
	Block 20: Unable to read this block
	Block 21: Unable to read this block
	Block 22: Unable to read this block
	Block 23: Unable to read this block
	-------------------------Sector 06--------------------------
	Block 24: Unable to read this block
	Block 25: Unable to read this block
	Block 26: Unable to read this block
	Block 27: Unable to read this block
	-------------------------Sector 07--------------------------
	Block 28: Unable to read this block
	Block 29: Unable to read this block
	Block 30: Unable to read this block
	Block 31: Unable to read this block
	-------------------------Sector 08--------------------------
	Block 32: Unable to read this block
	Block 33: Unable to read this block
	Block 34: Unable to read this block
	Block 35: Unable to read this block
	-------------------------Sector 09--------------------------
	Block 36: Unable to read this block
	Block 37: Unable to read this block
	Block 38: Unable to read this block
	Block 39: Unable to read this block
	-------------------------Sector 10--------------------------
	Block 40: Unable to read this block
	Block 41: Unable to read this block
	Block 42: Unable to read this block
	Block 43: Unable to read this block
	-------------------------Sector 11--------------------------
	Block 44: Unable to read this block
	Block 45: Unable to read this block
	Block 46: Unable to read this block
	Block 47: Unable to read this block
	-------------------------Sector 12--------------------------
	Block 48: Unable to read this block
	Block 49: Unable to read this block
	Block 50: Unable to read this block
	Block 51: Unable to read this block
	-------------------------Sector 13--------------------------
	Block 52: Unable to read this block
	Block 53: Unable to read this block
	Block 54: Unable to read this block
	Block 55: Unable to read this block
	-------------------------Sector 14--------------------------
	Block 56: Unable to read this block
	Block 57: Unable to read this block
	Block 58: Unable to read this block
	Block 59: Unable to read this block
	-------------------------Sector 15--------------------------
	Block 60: Unable to read this block
	Block 61: Unable to read this block
	Block 62: Unable to read this block
	Block 63: Unable to read this block
