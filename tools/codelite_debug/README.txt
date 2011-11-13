This slightly modified debug DLL can be used to allow full step-through and HW debugging in CodeLite using a Segger J-Link.

It was created by AC Verbeck, and the original announcement can be found here:

http://codelite.org/forum/viewtopic.php?f=11&t=1537

To include debug information in the compiled firmware, open the make file and set the following field to TRUE

--------------

##########################################################################
# Debug settings
##########################################################################

# Set DEBUGBUILD to 'TRUE' for full debugging (larger, slower binaries), 
# or to 'FALSE' for release builds (smallest, fastest binaries)
DEBUGBUILD = FALSE

--------------