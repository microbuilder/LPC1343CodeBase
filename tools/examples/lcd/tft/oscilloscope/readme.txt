OVERVIEW
============================================================
This examples simulates a very crude oscilloscope, and
reads the values on an analog input pin (P1.4/Wakeup, which
can be configured as AD5) as well as a digital pin (P2.0).
The digital pin will simply be displayed as 'High' (3.3V)
or 'Low' (0V/GND).

The last 10 readings are rendered in a data grid on the LCD,
with new readings added approximately every 100ms.

This sample demonstrates the following features
============================================================

- Rotating the LCD orientation
- Rendering text with different colors and fonts
- Using the touch screen to enable or disable a feature

WARNING
============================================================
Please note that the ADC pins on the LPC1343 are NOT 5.0V
tolerant.  Supply more than 3.3V to an ADC pin will
permanently damage the MCU.  Digital I/O pins are 5.0V
tolerant and can safely test 5.0V logic.

Pin Usage
============================================================
This examples is based on the LPC1343 TFT LCD Stand-Alone
board, which has a very limited number of pins broken out.
P1.4 can be configure as GPIO, but also as WAKEUP and AD5.

This pin is also available on the LPC1343 Reference Design
Base Base and is marked as WAKEUP on the PCB, but it has
a 10K pullup on it to allow the pin to function as a WAKEUP
source.

For details on the differences between these boards, see
the schematics available in the ~/tools/schematics folder.