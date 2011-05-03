This folder contains a number of tools that may be useful when developing with
the LPC1343 Reference Board:


===============================================================================
  /dotfactory
  -----------------------------------------------------------------------------
  An open-source (GPL) Windows-based program to convert TTF fonts to .c
  source files.  This application is included here for convenience sake and
  can be used to convert TTF fonts for use with the TFT LCD display with
  minor modifications to the generated source files.  See the existing fonts
  in 'drivers/lcd/tft/fonts' for an example.
===============================================================================


===============================================================================
  /examples
  -----------------------------------------------------------------------------
  Example software showing how to use the LPC1343 to accomplish certain tasks
  or how to use it with external devices, such as communicating with the PC
  using USB HID, etc.
===============================================================================


===============================================================================
  /lpcrc
  -----------------------------------------------------------------------------
  This utility fixes the CRC of any .bin files generated with GCC from the
  command line.  You must use this utility to patch any compiled firmware that
  will be deployed via the USB bootloader.

  The GCC src is included in the root folder and should build on any platform
  where a native GCC toolchain is available.  A pre=compiled windows binary is
  included in /bin for convenience sake, as well as in the root folder of the
  LPC1343 Code Base.
        
  If you are using the provided CodeLite project files, lpcrc will
  automatically be executed after every build.  You only need to run lpcrc if
  you are building directly from the command=line.
===============================================================================


===============================================================================
  /schematics
  -----------------------------------------------------------------------------
  Schematics showing the pin connections that are assumed to be used by the
  LPC1343 Code Base.
===============================================================================

          
===============================================================================
  /testfirmware
  -----------------------------------------------------------------------------
  Various pre-compiled binaires files that can be used to test the
  functionality of the LPC1343.  This firmware assumes that the board follows
  the layout show in 'tools/schematics'
===============================================================================


