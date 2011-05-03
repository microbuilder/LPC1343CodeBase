/**************************************************************************/
/*! 
    @file     main.c
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2011, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "projectconfig.h"
#include "sysinit.h"

#include "core/gpio/gpio.h"
#include "core/systick/systick.h"

#ifdef CFG_INTERFACE
  #include "core/cmd/cmd.h"
#endif

#ifdef CFG_PWM
  #include "core/pwm/pwm.h"
#endif

/**************************************************************************/
/*! 
    Main program entry point.  After reset, normal code execution will
    begin here.
*/
/**************************************************************************/
int main(void)
{
  #ifndef CFG_PWM
    #ERROR "CFG_PWM must be enabled in projectconfig.h for this example."
  #endif

  // Configure cpu and mandatory peripherals
  // PWM is initialised in systemInit and defaults to using P1.9
  systemInit();

  // Set duty cycle to 50% for square wave output
  pwmSetDutyCycle(50);

  // Frequency can be set anywhere from 2khz and 10khz (4khz is loudest)
  // Note: Since this is a 16-bit timer, make sure the delay is not
  //       greater than 0xFFFF (65535), though anything 2khz and higher
  //       is within an acceptable range
  // The piezo buzzer used for this example is the PS1240, available at
  // http://www.adafruit.com/index.php?main_page=product_info&cPath=35&products_id=160
  pwmSetFrequencyInTicks(CFG_CPU_CCLK / 2000);

  uint32_t currentSecond, lastSecond;
  currentSecond = lastSecond = 0;

  while (1)
  {
    // Beep the piezo buzzer very briefly once per second, toggling the LED at the same time
    currentSecond = systickGetSecondsActive();
    // Make sure that at least one second has passed
    if (currentSecond != lastSecond)
    {
      // Update the second tracker
      lastSecond = currentSecond;
      // Set the LED state and buzzer loudness depending on the current LED state
      if (gpioGetValue(CFG_LED_PORT, CFG_LED_PIN) == CFG_LED_OFF)
      {
        pwmSetFrequencyInTicks(CFG_CPU_CCLK / 4000);            // 4khz (louder)
        pwmStartFixed(200);                                     // 2x as long as @ 2khz since it's 2x faster
        gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_ON);   // Turn the LED on
      }
      else
      {
        pwmSetFrequencyInTicks(CFG_CPU_CCLK / 2000);            // 2khz (softer)
        pwmStartFixed(100);                                    
        gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_OFF);  // Turn the LED off
      }
    }
  }

  return 0;
}
