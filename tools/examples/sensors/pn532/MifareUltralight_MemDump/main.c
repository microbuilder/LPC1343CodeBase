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
#include <stdio.h>

#include "projectconfig.h"
#include "sysinit.h"

#include "core/i2c/i2c.h"
#include "drivers/rf/pn532/pn532.h"
#include "drivers/rf/pn532/pn532_bus.h"
#include "drivers/rf/pn532/helpers/pn532_mifare_classic.h"
#include "drivers/rf/pn532/helpers/pn532_mifare_ultralight.h"

/**************************************************************************/
/*! 
    Main program entry point.  After reset, normal code execution will
    begin here.

    Note: CFG_INTERFACE is normally enabled by default.  If you wish to
          enable the blinking LED code in main, you will need to open
          projectconfig.h, comment out "#define CFG_INTERFACE" and
          rebuild the project.
*/
/**************************************************************************/
int main (void)
{
  #ifdef CFG_INTERFACE
    //#error "CFG_INTERFACE must be disabled in projectconfig.h for this demo"
  #endif
  #if !defined CFG_PRINTF_USBCDC
    #error "CFG_PRINTF_USBCDC must be enabled in projectconfig.h for this demo"
  #endif

  // Configure cpu and mandatory peripherals
  systemInit();
  
  // Wait a bit for someone to open the USB connection for printf
  systickDelay(2000);

  // Initialise the PN532
  pn532Init();

  pn532_error_t error;
  byte_t        abtBuffer[8];
  size_t        szUIDLen;

  while(1)
  {
    printf("Please insert a Mifare Ultralight card%s", CFG_PRINTF_NEWLINE);

    // Try to do a memory dump of a Mifare Ultralight card
    // First wait for a card to arrive (will wake the PN532 if required)
    error = pn532_mifareultralight_WaitForPassiveTarget(abtBuffer, &szUIDLen);
    if (error)
    {
      switch (error)
      {
        case PN532_ERROR_WRONGCARDTYPE:
          printf("Not a Mifare Ultralight card%s", CFG_PRINTF_NEWLINE);
          break;
        default:
          printf("Error establishing passive connection (0x%02x)%s", error, CFG_PRINTF_NEWLINE);
          break;
      }
    }
    else
    {
      // Display the card's UID (normally 7 bytes long)
      printf("UID: ");
      pn532PrintHex(abtBuffer, szUIDLen);
      printf("%s", CFG_PRINTF_NEWLINE);
      printf("Page  Hex       Text%s", CFG_PRINTF_NEWLINE);
      printf("----  --------  ----%s", CFG_PRINTF_NEWLINE);
      // Dump the memory contents page by page
      uint8_t i;
      for (i = 0; i < 16; i++)
      {
        // Try to read the current page
        error = pn532_mifareultralight_ReadPage(i, abtBuffer);
        if (!error)
        {
          printf("0x%02x  ", i);
          pn532PrintHexChar(abtBuffer, 4);
        }
      }
    }
    // Wait a bit before trying again
    printf("%s", CFG_PRINTF_NEWLINE);
    systickDelay(2000);
  }
}
