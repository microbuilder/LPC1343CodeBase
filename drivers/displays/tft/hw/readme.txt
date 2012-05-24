HW-Specific TFT and RGB OLED Drivers
====================================

hx8340b   - 176x220 16-bit display (Bit-banged SPI interface)
hx8347g   - 240x320 16-bit display (8-bit interface)
hx8347h   - 240x320 16-bit display (Hardware SPI via SSP0)
ILI9325   - 240x320 16-bit display  (8-bit interface)
ILI9328   - 240x320 16-bit display  (8-bit interface)
st7735    - 128x160 16-bit display  (Bit-banged SPI interface)
st7783    - 240x320 16-bit display  (8-bit interface)
ssd1331   - 96x64 16-bit OLED display (Bit-banged SPI interface) 
ssd1351   - 128x128 16-bit OLED display (Bit-banged SPI interface)

NOTE: Only ILI9328 driver have been fully tested. The others may
be incomplete, or have only been partially tested. (The
ST7783 driver, for example, has issues when the screen orientation
is set to landscape.)  The basic initialisation sequences should
all work, but some further optimisation is likely required on some
of these to get the best possible performance out of them.