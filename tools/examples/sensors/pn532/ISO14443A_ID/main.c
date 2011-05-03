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

#include "drivers/sensors/pn532/pn532.h"
#include "drivers/sensors/pn532/pn532_drvr.h"

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
    #error "CFG_INTERFACE must be disabled in projectconfig.h for this demo"
  #endif
  #if !defined CFG_PRINTF_USBCDC
    #error "CFG_PRINTF_USBCDC must be enabled in projectconfig.h for this demo"
  #endif

  // Configure cpu and mandatory peripherals
  systemInit();
  
  // Wait 5 second for someone to open the USB connection for printf
  systickDelay(5000);

  // Initialise the PN532
  pn532Init();

  byte_t response[256];
  size_t responseLen;
  pn532_error_t error;

  // Setup command to initialise a single ISO14443A target at 106kbps 
  byte_t abtCommand[] = { PN532_COMMAND_INLISTPASSIVETARGET, 0x01, PN532_MODULATION_ISO14443A_106KBPS };

  while (1)
  {
    printf("%s", CFG_PRINTF_NEWLINE);
    printf("Wait for an ISO14443A card (Mifare Classic, etc.)%s", CFG_PRINTF_NEWLINE);

    // Send the command
    error = pn532Write(abtCommand, sizeof(abtCommand));

    // Wait until we get a response or an unexpected error message
    do
    {
      error = pn532Read(response, &responseLen);
      systickDelay(25);
    }
    #ifdef PN532_UART
    while (error == PN532_ERROR_RESPONSEBUFFEREMPTY);
    #endif
    #ifdef PN532_SPI
    while ((error == PN532_ERROR_RESPONSEBUFFEREMPTY) || (error = PN532_ERROR_SPIREADYSTATUSTIMEOUT));
    #endif

    // Print the card details if possible
    if (!error)
    {
      /* Response for ISO14443A 106KBPS (Mifare Classic, etc.)
         See UM0701-02 section 7.3.5 for more information

         byte            Description
         -------------   ------------------------------------------
         b7              Tags Found
         b8              Tag Number (only one used in this example)
         b9..10          SENS_RES
         b11             SEL_RES
         b12             NFCID Length
         b13..NFCIDLen   NFCID                                      
         
         SENS_RES   SEL_RES     Manufacturer/Card Type    NFCID Len
         --------   -------     -----------------------   ---------
         00 04      08          NXP Mifare Classic 1K     4 bytes   */

      printf("%s", CFG_PRINTF_NEWLINE);
      printf("%-12s: %d %s", "Tags Found", response[7], CFG_PRINTF_NEWLINE);
      printf("%-12s: %02X %02X %s", "SENS_RES", response[9], response[10], CFG_PRINTF_NEWLINE);
      printf("%-12s: %02X %s", "SEL_RES", response[11], CFG_PRINTF_NEWLINE);
      printf("%-12s: ", "NFCID");
      size_t pos;
      for (pos=0; pos < response[12]; pos++) 
      {
        printf("%02x ", response[13 + pos]);
      }
      printf(CFG_PRINTF_NEWLINE);
    }
    else
    {
      // Oops .... something bad happened.  Check 'error'
      printf("Ooops! Error %02X %s", error, CFG_PRINTF_NEWLINE);
    }

    // Wait at least one second before trying again
    systickDelay(1000);
  }
}
