OVERVIEW
============================================================
This example will wait for an ISO14443A card to enter
the RF field, and then try to determine the specific card
model (SENS_RES and SEL_RES) as well as the card's NFCID.

All information will be sent to USBCDC by default, with the
PN532 breakout board connected via UART.

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

2.) Configure your terminal software to open the USB COM
    port at 115K.

3.) When the application starts, there is a 5 second delay
    (to allow you time to connect via USB CDC), after which 
    point the device will wait for a single ISO14443A card
    (Mifare Classic, etc.) and try to read it's ID.  Once a
    card is found, the reader will start looking again for a
    card after a 1 second delay.
