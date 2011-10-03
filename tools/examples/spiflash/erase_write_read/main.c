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
#include "drivers/spiflash/spiflash.h"

#ifdef CFG_INTERFACE
  #include "core/cmd/cmd.h"
#endif

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

  spiflashError_e error;
  uint8_t buffer[256];

  // Wait a few seconds to let people connect to USB CDC to see the output
  systickDelay(5000);

  // Initialise SPI, etc.
  printf("Initialising flash\r\n");
  spiflashInit();

  printf("Erasing sector 0 (page 0..15)\r\n");
  error = spiflashEraseSector (0);
  if (error)
  {
    // Check what went wrong
    switch (error)
    {
      case SPIFLASH_ERROR_ADDROUTOFRANGE:
        // Specified starting address is out of range
        break;
      case SPIFLASH_ERROR_TIMEOUT_READY:
        // Timeout waiting for ready status (can be pre or post write)
        break;
      case SPIFLASH_ERROR_PROTECTIONERR:
        // Flash is write protected
        break;
    }
  }

  printf("Writing 8 bytes to page 0 starting at address 0x04 (byte 5)\r\n");
  buffer[0] = 0x12;
  buffer[1] = 0x34;
  buffer[2] = 0x56;
  buffer[3] = 0x78;
  buffer[4] = 0xAB;
  buffer[5] = 0xCD;
  buffer[6] = 0xEF;
  buffer[7] = 0xAA;
  error = spiflashWritePage (0x04, buffer, 8);
  if (error)
  {
    // Check what went wrong
    switch (error)
    {
      case SPIFLASH_ERROR_ADDROUTOFRANGE:
        // Specified starting address is out of range
        break;
      case SPIFLASH_ERROR_DATAEXCEEDSPAGESIZE:
        // Supplied data exceeds max page size
        break;
      case SPIFLASH_ERROR_PAGEWRITEOVERFLOW:
        // The data length plus the start address offset exceeeds page limits
        break;
      case SPIFLASH_ERROR_TIMEOUT_READY:
        // Timeout waiting for ready status (can be pre or post write)
        break;
      case SPIFLASH_ERROR_PROTECTIONERR:
        // Unable to set write latch
        break;
    }
  }

  printf("Reading 16 bytes from page 1 (bytes 0..15)\r\n");
  // Clear buffer just to prove the point
  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  buffer[4] = 0;
  buffer[5] = 0;
  buffer[6] = 0;
  buffer[7] = 0;
  buffer[8] = 0;
  buffer[9] = 0;
  buffer[10] = 0;
  buffer[11] = 0;
  buffer[12] = 0;
  buffer[13] = 0;
  buffer[14] = 0;
  buffer[15] = 0;
  error = spiflashReadBuffer (0, buffer, 16);
  if (error)
  {
    // Check what went wrong
    switch (error)
    {
      case SPIFLASH_ERROR_ADDROUTOFRANGE:
        // Specified starting address is out of range
        break;
      case SPIFLASH_ERROR_TIMEOUT_READY:
        // Timed out waiting for flash to return ready state
        break;
      case SPIFLASH_ERROR_ADDROVERFLOW:
        // Ran over the upper address during read
        break;
    }
  }

  // Display read results
  printf("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X %s", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15], CFG_PRINTF_NEWLINE);

  while(1)
  {
  }

  return 0;
}
