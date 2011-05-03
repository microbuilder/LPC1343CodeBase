TFT LCDs
========

The top level of this folder contains generic drawing routines for 16-bit TFT
LCD displays.  The core drawing functions are defined in drawing.c, and the
specific HW implementation of these routines (setting the individual pixels,
initialising the display, etc.) takes place in a seperate HW drivers that
implements the methods defined in lcd.h.  To change the LCD used, you simply
need to change which HW-specific file is compiled and linked during the build
process, without having to change anything else in your application code.

For example:

- main.c             - drawing.c          - hw\ili9325.c
----------------     ----------------     ----------------
  APPLICATION    --> GENERIC DRAWING  -->   SPECIFIC HW 
     CODE        -->       CODE       -->      DRIVER
----------------     ----------------     ----------------


drawing.c          Generic drawing routines such as drawing pixels, lines,
                   rectangles, as well as basic text-rendering.

lcd.h              This file contains the prototypes of HW-specific functions
                   that must be implemented in the LCD driver, since
                   drawing.c will redirect all requests to these lower level
                   functions.
                   
touchscreen.c      Contains a very simple example of how to use ADC to read
                   the current position on a touchscreen.  No signal debouncing
                   takes places, and this code will need to be improved for us
                   in a real-world situation.

hw\*               HW-specific drivers based on lcd.h                   