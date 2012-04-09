# LPC1343 Code Base

This software library is based on the freely available LPC1343 Reference
Design available at http://www.microBuilder.eu.  Unless otherwise stated,
all files are licensed under the BSD license (see license.txt for details).

## Downloads

The latest version of the software is always available on github at:

https://github.com/microbuilder/LPC1343CodeBase

## Development Status

While there are almost certainly bugs and chunks of decidedly
mediocre code in the library, after two years or so of fairly
steady development effort, the core code should be fairly
reliable.  Some drivers in the /drivers folder are incomplete,
and may have been abandoned once the basic features were working
(setting pixels on a display, etc.), but they should serve as
a good starting point for a wide variety of peripherals.

## How Can Someone Support This Project

A lot of the recent development in this library was done
thanks to the financial support of Adafruit Industries (paying
me a salary to keep providing this stuff).  If you find this library
useful, please consider ordering something from the people that
make this financially possible:  http://www.adafruit.com

## Toolchains

The entire LPC1343 Code Base is based around GCC, and can be built
from the command-line using only free tools like the Yagarto
distribution for Windows or the Mac, etc.

Nonetheless, it's often more productive to develop in an IDE with a
full set of debugging tools at your disposal, and the code base also
includes project files for Rowley Associate's Crossworks for ARM, as
well as project files for the free and open source CodeLite, which can
be used in combination with a Segger J-Link for ARM and GDB Server.

Project files for the two IDEs are stored in the /build folder, and
project files for the free LPCXpresso IDE are stored in root (see
http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign/DebuggingWithLPCXpresso.aspx
for more information on using LPCXpresso).

## Documentation

Online documentation is available at the following URLs, though the
current documentation reflects and earlier version of the library, and some
inconsistencies are present since parts of the library have been 
reorganized working towards a 1.0.0 release.  The graphics library in
particular has been reorganized quite a bit.

General API Documentation:
http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign/CodeBaseDocumentation.aspx

TFT/OLED Graphics Sub-System:
http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign/TFTLCDAPI_v1_1_0.aspx

Other tutorials and links:
http://www.microbuilder.eu/Projects/LPC1343ReferenceDesign.aspx
