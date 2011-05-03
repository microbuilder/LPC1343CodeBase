HW-Specific LCD Drivers
=======================

ILI9325   - 240x320 16-bit display  (8-bit interface)
ILI9328   - 240x320 16-bit display  (8-bit interface)
st7735    - 128x160 16-bit display  (Bit-banged SPI interface)
st7783    - 240x320 16-bit displays (8-bit interface)

NOTE: Only the ILI9325 and ILI9328 drivers have been fully tested.
The others are incomplete or have only been partially tested. (The
ST7783 driver, for example, has issues when the screen orientation
is set to landscape.)