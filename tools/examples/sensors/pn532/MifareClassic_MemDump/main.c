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

  // To dump an NDEF formatted Mifare Classic Card (formatted using NXP TagWriter on Android
  // for example), you must use the following authentication keys:
  //
  //    Sector 0:       0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5
  //    Sectors 1..15:  0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7
  //
  // For more information on NDEF see the following document:
  // 
  //    AN1305 - MIFARE Classic as NFC Type MIFARE Classic Tag
  //    http://www.nxp.com/documents/application_note/AN1305.pdf

  // Set this to one for NDEF cards, or 0 for blank factory default cards
  #define CARDFORMAT_NDEF (0)

  pn532_error_t error;
  byte_t abtUID[8];
  byte_t abtBlock[32];
  #if CARDFORMAT_NDEF == 1
  byte_t abtAuthKey1[6] = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5 };   // Sector 0 of NXP formatter NDEF cards
  byte_t abtAuthKey2[6] = { 0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7 };   // All other sectors use standard key (AN1305 p.20, Table 6)
  #else
  byte_t abtAuthKey1[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  byte_t abtAuthKey2[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  #endif
  size_t szUIDLen;
   
  // Use the MIFARE Classic Helper to read/write to the tag's EEPROM storage
  while(1)
  {
    printf("Please insert a Mifare Classic 1K or 4K card%s%s", CFG_PRINTF_NEWLINE, CFG_PRINTF_NEWLINE);

    // Wait for any ISO14443A card
    error = pn532_mifareclassic_WaitForPassiveTarget(abtUID, &szUIDLen);
    if (!error)
    {
      // Mifare classic card found ... cycle through each sector
      uint8_t block;
      bool authenticated = false;
      for (block = 0; block < 64; block++)
      {
        // Check if this is a new block so that we can reauthenticate
        if (is_first_block(block)) authenticated = false;
        if (!authenticated)
        {
          // Start of a new sector ... try to to authenticate
          printf("-------------------------Sector %02d--------------------------%s", block / 4, CFG_PRINTF_NEWLINE);
          error = pn532_mifareclassic_AuthenticateBlock (abtUID, szUIDLen, block, PN532_MIFARE_CMD_AUTH_A, block / 4 ? abtAuthKey2 : abtAuthKey1);
          if (error)
          {
            switch(error)
            {
              default:
                printf("Authentication error (0x%02x)%s", error, CFG_PRINTF_NEWLINE);
                break;
            }
          }
          else
          {
            authenticated = true;
          }
        }
        // If we're still not authenticated just skip the block
        if (!authenticated)
        {
          printf("Block %02d: ", block);
          printf("Unable to authenticate%s", CFG_PRINTF_NEWLINE);
        }
        else
        {
          // Authenticated ... we should be able to read the block now
          error = pn532_mifareclassic_ReadDataBlock (block, abtBlock);
          if (error)
          {
            switch(error)
            {
              default:
                printf("Block %02d: ", block);
                printf("Unable to read this block%s", CFG_PRINTF_NEWLINE);
                break;
            }
          }
          else
          {
            // Read successful
            printf("Block %02d: ", block);
            pn532PrintHexChar(abtBlock, 16);
          }
        }
      }
    }
    else
    {
      switch (error)
      {
        case PN532_ERROR_WRONGCARDTYPE:
          printf("Not a Mifare Classic 1K or 4K card%s", CFG_PRINTF_NEWLINE);
          break;
        default:
          printf("Error establishing passive connection (0x%02x)%s", error, CFG_PRINTF_NEWLINE);
          break;
      }
    }

    // Wait a bit before trying again
    systickDelay(2000);
  }
}
