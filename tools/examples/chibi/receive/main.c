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
#include "drivers/chibi/chb.h"
#include "drivers/chibi/chb_drvr.h"

static chb_rx_data_t rx_data;

/**************************************************************************/
/*! 
    Converts the ED (Energy Detection) value to dBm using the following
    formula: dBm = RSSI_BASE_VAL + 1.03 * ED

    For more information see section 6.5 of the AT86RF212 datasheet
*/
/**************************************************************************/
int edToDBM(uint32_t ed)
{
  #if CFG_CHIBI_MODE == 0 || CFG_CHIBI_MODE == 1 || CFG_CHIBI_MODE == 2
    // Calculate for OQPSK (RSSI Base Value = -100)
    int dbm = (103 * ed - 10000);
  #else
    // Calculate for BPSK (RSSI Base Value = -98)
    int dbm = (103 * ed - 9800);
  #endif

  return dbm / 100;
}

/**************************************************************************/
/*! 
    Constantly checks for incoming messages, and displays them using
    printf when they arrive.  This program will display messages sent
    to the global broadcast address (0xFFFF) or messages addressed to
    this node using it's unique 16-bit ID.

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

  // Get a reference to the Chibi peripheral control block
  chb_pcb_t *pcb = chb_get_pcb();

  while(1)
  {
    // Check for incoming messages 
    while (pcb->data_rcv) 
    { 
      // Enable LED to indicate message reception 
      gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_ON); 
      // get the length of the data
      rx_data.len = chb_read(&rx_data);
      // make sure the length is nonzero
      if (rx_data.len)
      {
        int dbm = edToDBM(pcb->ed);
        printf("Message received from node %02X: %s, len=%d, dBm=%d.%s", rx_data.src_addr, rx_data.data, rx_data.len, dbm, CFG_PRINTF_NEWLINE);
      }
      // Disable LED
      gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_OFF); 
    }
  }

  return 0;
}
