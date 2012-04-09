# TFT/OLED Graphics Sub-System

The LPC1343 Code Base includes a reasonably complete graphic
sub-system that can be used with a variety of RGB565 TFT or OLED
displays. 

It includes all the basic drawing primitives you'd expect, as well
as color conversion and alpha-blending, support for bitmap and
anti-aliased fonts, loading and saving of bitmap images (from/to an
SD card or elsewhere), and a few basic controls to help you get
started developing your own custom look and feel in your application.

The core drawing routines (/drivers/displays/tft/drawing.c) are
seperated from the actual HW (/drivers/displays/tft/hw/*) so that
the underlying display can be easily changed without having to
significantly modify your project code.  

A number of drivers are provided for common displays and controllers,
and it's relatively straight-forward to extend the library to support
new ones: you simply need to implement the set of functions defined 
in lcd.h, and the graphics sub-system will call these functions
directly when rendering any text, graphics, etc..

## Documentation

Complete documentation for the graphics sub-system is available
online at microbuilder.eu:

http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign/TFTLCDAPI_v1_1_0.aspx
