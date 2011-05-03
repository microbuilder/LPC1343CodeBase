/**************************************************************************/
/*! 
    @file     main.c
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2010, microBuilder SARL
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
#include "projectconfig.h"
#include "sysinit.h"

#include "core/gpio/gpio.h"

#if defined CFG_CHIBI
  #include <string.h>
  #include <stdlib.h>
  #include "drivers/chibi/chb.h"
  #include "drivers/chibi/chb_drvr.h"
#endif

/**************************************************************************/
/*! 
    Broadcast a basic message every 250 milliseconds
  
    projectconfig.h settings:
    --------------------------------------------------
    CFG_CHIBI             -> Enabled
    CFG_CHIBI_PROMISCUOUS -> 0
    CFG_CHIBI_BUFFERSIZE  -> 128
*/
/**************************************************************************/
int main(void)
{
  // Configure cpu and mandatory peripherals
  systemInit();

  // Make sure that projectconfig.h is properly configured for this example
  #if !defined CFG_CHIBI
    #error "CFG_CHIBI must be enabled in projectconfig.h for this example"
  #endif
  #if CFG_CHIBI_PROMISCUOUS != 0
    #error "CFG_CHIBI_PROMISCUOUS must be set to 0 in projectconfig.h for this example"
  #endif

  #ifdef CFG_CHIBI
    uint32_t counter = 0;
    chb_pcb_t *pcb = chb_get_pcb();

    while(1)
    {
      // Enable LED
      gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_ON); 
      // Create and send the message
      char text[10];
      counter++;
      itoa(counter, text, 10);
      chb_write(0xFFFF, text, strlen(text) + 1);
      // Disable LED
      gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_OFF); 
      systickDelay(250);
    }
  #endif

  return 0;
}
