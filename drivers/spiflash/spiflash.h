/**************************************************************************/
/*! 
    @file     spiflash.h
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

#ifndef _SPIFLASH_H_
#define _SPIFLASH_H_

#include "projectconfig.h"

/**************************************************************************/
/*! 
    @brief  Error messages
*/
/**************************************************************************/
typedef enum
{
  SPIFLASH_ERROR_OK = 0,                    // Everything executed normally
  SPIFLASH_ERROR_ADDROUTOFRANGE = 1,        // Address out of range
  SPIFLASH_ERROR_TIMEOUT_READY = 2,         // Timeout waiting for ready status
  SPIFLASH_ERROR_WRITEERR = 3,              // Write Error
  SPIFLASH_ERROR_PROTECTIONERR = 4,         // Write Protection Error
  SPIFLASH_ERROR_ADDROVERFLOW = 5,          // Address overflow during read/write
  SPIFLASH_ERROR_UNEXPECTEDID = 6,          // The manufacturer and/or device ID are different than expected
  SPIFLASH_ERROR_NOTSTARTOFPAGE = 7,        // The supplied address is not the start of a new page
  SPIFLASH_ERROR_DATAEXCEEDSPAGESIZE = 9,   // When writing page data, you can't exceed page size
  SPIFLASH_ERROR_PAGEWRITEOVERFLOW = 10,    // Page data will overflow beause (start address + len) > page size
  SPIFLASH_ERROR_LAST
}
spiflashError_e;

/**************************************************************************/
/*! 
    @brief  Describes the storage capacity of the SPI flash, including the
            size of the minimum HW write (page) and erase (sector) units.
*/
/**************************************************************************/
typedef struct
{
  uint32_t pageSize;                    // Page size to write data (in bytes)
  uint32_t pageCount;                   // Number of pages
  uint32_t sectorSize;                  // Sector size to erase data (in bytes)
  uint32_t sectorCount;                 // Number of sectors
}
spiflashSizeInfo_t;

/**************************************************************************/
/*! 
    @brief Tries to initialise the flash device, and setups any HW
           required by the SPI flash
*/
/**************************************************************************/
void spiflashInit (void);

/**************************************************************************/
/*! 
    @brief Gets an instance of spiflashSizeInfo_t that describes the
           storage limits of the SPI flash like page size (minimum write
           size) and sector size (minimum erase size).
*/
/**************************************************************************/
spiflashSizeInfo_t spiflashGetSizeInfo(void);

/**************************************************************************/
/*! 
    @brief Gets the 8-bit manufacturer ID and device ID for the flash

    @param[out] *manufID
                Pointer to the uint8_t that will store the manufacturer ID
    @param[out] *deviceID
                Pointer to the uint8_t that will store the device ID
*/
/**************************************************************************/
void spiflashGetManufacturerInfo (uint8_t *manufID, uint8_t *deviceID);

/**************************************************************************/
/*! 
    @brief Sets the write flag on the SPI flash, and if required puts the
           WP pin in an appropriate state

    @param[in]  enable
                True (1) to enable writing, false (0) to disable it
*/
/**************************************************************************/
void spiflashWriteEnable (bool enable);

/**************************************************************************/
/*! 
    @brief Reads the specified number of bytes from the supplied address.

    This function will read one or more bytes starting at the supplied
    address. Please note that bufferLength is zero-based, meaning you
    should  supply '0' to read a single byte, '3' to read 4 bytes of data,
    etc.

    @param[in]  address
                The 24-bit address where the read will start.
    @param[out] *buffer
                Pointer to the buffer that will store the read results
    @param[in]  len
                Length of the buffer.

    @section EXAMPLE

    @code
    uint8_t buffer[64];
    spiflashError_e error;
    error = spiflashReadBuffer (0, buffer, 64);
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
    @endcode
*/
/**************************************************************************/
spiflashError_e spiflashReadBuffer (uint32_t address, uint8_t *buffer, uint32_t len);

/**************************************************************************/
/*! 
    @brief Erases the contents of a single sector

    @param[in]  sectorNumber
                The sector number to erase (zero-based).

    @section  EXAMPLE

    @code
    spiflashError_e error;
    error = spiflashEraseSector(0);
    if (error)
    {
      // Check what went wrong
      switch (error)
      {
        case SPIFLASH_ERROR_ADDROUTOFRANGE:
          // Specified starting address is out of range
          break;
        case SPIFLASH_ERROR_PROTECTIONERR:
          // Couldn't set the write enable bit
          break;
        case SPIFLASH_ERROR_TIMEOUT_READY:
          // Timed out waiting for flash to return ready state
          break;
      }
    }
    @endcode
*/
/**************************************************************************/
spiflashError_e spiflashEraseSector (uint32_t sectorNumber);

/**************************************************************************/
/*! 
    @brief Erases the entire flash chip

    @section  EXAMPLE

    @code
    spiflashError_e error;
    error = spiflashEraseChip();
    if (error)
    {
      // Check what went wrong
      switch (error)
      {
        case SPIFLASH_ERROR_PROTECTIONERR:
          // Couldn't set the write enable bit
          break;
        case SPIFLASH_ERROR_TIMEOUT_READY:
          // Timed out waiting for flash to return ready state
          break;
      }
    }
    @endcode
*/
/**************************************************************************/
spiflashError_e spiflashEraseChip (void);

/**************************************************************************/
/*! 
    @brief      Writes up to 256 bytes of data to the specified page.
                
    @note       Before writing data to a page, make sure that the 4K sector
                containing the specific page has been erased, otherwise the
                data will be meaningless.

    @param[in]  address
                The 24-bit address where the write will start.
    @param[out] *buffer
                Pointer to the buffer that will store the read results
    @param[in]  len
                Length of the buffer.  Valid values are from 1 to 256,
                within the limits of the starting address and page length.

    @section  EXAMPLE

    @code
    spiflashError_e error;
    uint8_t buffer[256];

    buffer[0] = 0x12;
    buffer[1] = 0x34;
    buffer[2] = 0x56;
    buffer[3] = 0x78;
    buffer[4] = 0xDE;
    buffer[5] = 0xAD;
    buffer[6] = 0xC0;
    buffer[7] = 0xDE;

    error = spiflashWritePage (0, buffer, 8);
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
    @endcode
*/
/**************************************************************************/
spiflashError_e spiflashWritePage (uint32_t address, uint8_t *buffer, uint32_t len);

#endif
