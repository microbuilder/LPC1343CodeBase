This example uses a 16-bit timer for PWM, and makes a common
piezo buzzer beep briefly once per second at an alternating
frequency.

The piezo-buzzer used in this example is the PS1240, available
from Adafruit Industries at:

http://www.adafruit.com/index.php?main_page=product_info&cPath=35&products_id=160

The buzzer should have one pin (either is fine) connected to P1.9
on the LPC1343, and the other pin connected to GND.  CFG_PWM must also
be enabled in projectconfig.h.