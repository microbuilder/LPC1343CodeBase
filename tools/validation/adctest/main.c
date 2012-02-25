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
#include "core/adc/adc.h"
#include "core/systick/systick.h"

// ADC buffer
uint16_t buffer[1024][2];

/**************************************************************************/
/*! 
    Main program entry point.  After reset, normal code execution will
    begin here.
*/
/**************************************************************************/
int main(void)
{
  // Configure cpu and mandatory peripherals
  systemInit();

  // Initialize the ADC
  adcInit();

  uint32_t currentTick, lastTick, i;
  i = currentTick = lastTick = 0;

  // Clear the ADC buffer
  memset(&buffer, 0, sizeof(buffer));

  // Turn on LED during ADC read
  gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, CFG_LED_ON);

  // Sample ADC once per millisecond and store it in the buffer
  while (i < 1024)
  {
    currentTick = systickGetTicks();
    if (currentTick != lastTick)
    {
      lastTick = currentTick;
      buffer[i][0] = (uint16_t)(currentTick & 0xFFFF);
      buffer[i][1] = adcRead(0);    // ADC Results
      i++;
    }
  }

  // Turn LED off
  gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, CFG_LED_OFF);

  // Brief delay before spitting results out via USB CDC
  systickDelay(3000);

  // Print results out in CSV format
  i = 0;
  while (i < 1024)
  {
    printf("%d, %d %s", buffer[i][0], buffer[i][1], CFG_PRINTF_NEWLINE);
    i++;
  }

  // Wait forever
  while(1)
  {
  }

  return 0;
}
