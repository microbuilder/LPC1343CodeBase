Bitmap LCDs
===========

This folder contains drivers for 1-bit graphic LCDs (128x64 pixel ST7565, etc.)
or text-only displays.  They are placed in a seperate folder because the
drawing routines are handled quite differently than the generic drawing code
for 16-bit TFT LCDs.

ST7565             Driver for  128x64 pixel I2C-based displays (available from
                   Adafruit Industries, for example).
SSD1306            Driver for 128x64 pixel OLED displays (also available from
                   Adafruit Industries).
