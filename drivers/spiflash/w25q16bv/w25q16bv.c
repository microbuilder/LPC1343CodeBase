/**************************************************************************/
/*! 
    @file     w25q16bv.c
    @author   K. Townsend (microBuilder.eu)

    @section DESCRIPTION

    Driver for Winbond's 16 MBit SPI/Quad-SPI Flash (W25Q16BV)
    
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

#include "w25q16bv.h"
#include "../spiflash.h"
#include "core/ssp/ssp.h"
#include "core/gpio/gpio.h"
#include "core/systick/systick.h"

// Macros to toggle the CS/SSEL line on the SPI bus
#define W25Q16BV_SELECT()       gpioSetValue(0, 2, 0)
#define W25Q16BV_DESELECT()     gpioSetValue(0, 2, 1)

// Flag to indicate whether the SPI flash has been initialised or not
static bool _w25q16bvInitialised = false;

/**************************************************************************/
/*   HW Specific Functions                                                */
/*   -------------------------------------------------------------------  */
/*   These functions are specific to the chip being used, and are         */
/*   seperate from the 'generic' functions defined in spiflash.h          */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    This function both reads and writes data. For write operations, include data
    to be written as argument. For read ops, use dummy data as arg. Returned
    data is read byte val.
*/
/**************************************************************************/
uint8_t w25q16bv_TransferByte(uint8_t data)
{
    /* Move on only if NOT busy and TX FIFO not full */
    while ((SSP_SSP0SR & (SSP_SSP0SR_TNF_MASK | SSP_SSP0SR_BSY_MASK)) != SSP_SSP0SR_TNF_NOTFULL);
    SSP_SSP0DR = data;
  
    /* Wait until the busy bit is cleared and receive buffer is not empty */
    while ((SSP_SSP0SR & (SSP_SSP0SR_BSY_MASK | SSP_SSP0SR_RNE_MASK)) != SSP_SSP0SR_RNE_NOTEMPTY);

    // Read the queue
    return SSP_SSP0DR;
}

/**************************************************************************/
/*! 
    @brief Gets the value of the Read Status Register (0x05)

    @return     The 8-bit value returned by the Read Status Register
*/
/**************************************************************************/
uint8_t w25q16bvGetStatus()
{
  uint8_t status;

  W25Q16BV_SELECT();
  w25q16bv_TransferByte(W25Q16BV_CMD_READSTAT1);  // Send read status 1 cmd
  status = w25q16bv_TransferByte(0xFF);           // Dummy write
  W25Q16BV_DESELECT();

  return status & (W25Q16BV_STAT1_BUSY | W25Q16BV_STAT1_WRTEN);
}

/**************************************************************************/
/*! 
    @brief  Waits for the SPI flash to indicate that it is ready (not
            busy) or until a timeout occurs.

    @return An error message indicating that a timeoout occured
            (SPIFLASH_ERROR_TIMEOUT_READY) or an OK signal to indicate that
            the SPI flash is ready (SPIFLASH_ERROR_OK)
*/
/**************************************************************************/
spiflashError_e w25q16bvWaitForReady()
{
  uint32_t timeout = 0;
  uint8_t status;

  while ( timeout < SSP_MAX_TIMEOUT )
  {
    status = w25q16bvGetStatus() & W25Q16BV_STAT1_BUSY;
    if (status == 0)
    {
      break;
    }
    timeout++;
  }
  if ( timeout == SSP_MAX_TIMEOUT )
  {
    return SPIFLASH_ERROR_TIMEOUT_READY;
  }

  return SPIFLASH_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Gets the unique 64-bit ID assigned to this IC (useful for
            security purposes to detect if the flash was changed, etc.)

    @param[out] *buffer
                Pointer to the uint8_t buffer that will store the 8 byte
                long unique ID

    @note   The unique ID is return in bit order 63..0
*/
/**************************************************************************/
void w25q16bvGetUniqueID(uint8_t *buffer)
{
  uint8_t i;

  W25Q16BV_SELECT();
  w25q16bv_TransferByte(W25Q16BV_CMD_READUNIQUEID); // Unique ID cmd
  w25q16bv_TransferByte(0xFF);                      // Dummy write
  w25q16bv_TransferByte(0xFF);                      // Dummy write
  w25q16bv_TransferByte(0xFF);                      // Dummy write
  w25q16bv_TransferByte(0xFF);                      // Dummy write
  // Read 8 bytes worth of data
  for (i = 0; i < 8; i++)
  {
    buffer[i] = w25q16bv_TransferByte(0xFF);
  }
  W25Q16BV_DESELECT();
}

/**************************************************************************/
/*   Generic spiflash.h Functions                                         */
/*   -------------------------------------------------------------------  */
/*   These functions are part of the common interface defined in          */
/*   spiflash.h and must be implemented by any spi flash device           */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*! 
    @brief Tries to initialise the flash device, and setups any HW
           required by the SPI flash
*/
/**************************************************************************/
void spiflashInit (void)
{
  // Initialise the SPI bus
  sspInit(0, sspClockPolarity_Low, sspClockPhase_RisingEdge);

  // ToDo: Set the WP, Hold, etc. pins to an appropriate state

  _w25q16bvInitialised = TRUE;
}

/**************************************************************************/
/*! 
    @brief Gets an instance of spiflashSizeInfo_t that describes the
           storage limits of the SPI flash like page size (minimum write
           size) and sector size (minimum erase size).
*/
/**************************************************************************/
spiflashSizeInfo_t spiflashGetSizeInfo(void)
{
  spiflashSizeInfo_t size;
  size.pageSize = W25Q16BV_PAGESIZE;
  size.pageCount = W25Q16BV_PAGES;
  size.sectorSize = W25Q16BV_SECTORSIZE;
  size.sectorCount = W25Q16BV_SECTORS;
  return size;
}

/**************************************************************************/
/*! 
    @brief Gets the 8-bit manufacturer ID and device ID for the flash

    @param[out] *manufID
                Pointer to the uint8_t that will store the manufacturer ID
    @param[out] *deviceID
                Pointer to the uint8_t that will store the device ID
*/
/**************************************************************************/
void spiflashGetManufacturerInfo (uint8_t *manufID, uint8_t *deviceID)
{
  if (!_w25q16bvInitialised) spiflashInit();

  // W25Q16BV_CMD_MANUFDEVID (0x90) provides both the JEDEC manufacturer
  // ID and the device ID

  W25Q16BV_SELECT();
  w25q16bv_TransferByte(W25Q16BV_CMD_MANUFDEVID); 
  w25q16bv_TransferByte(0x00);            // Dummy write
  w25q16bv_TransferByte(0x00);            // Dummy write
  w25q16bv_TransferByte(0x00);            // Dummy write
  *manufID = w25q16bv_TransferByte(0xFF);
  *deviceID = w25q16bv_TransferByte(0xFF);
  W25Q16BV_DESELECT();
}

/**************************************************************************/
/*! 
    @brief Sets the write flag on the SPI flash, and if required puts the
           WP pin in an appropriate state

    @param[in]  enable
                True (1) to enable writing, false (0) to disable it
*/
/**************************************************************************/
void spiflashWriteEnable (bool enable)
{
  if (!_w25q16bvInitialised) spiflashInit();

  // ToDo: Put the WP pin in an appropriate state if required

  W25Q16BV_SELECT();
  w25q16bv_TransferByte(enable ? W25Q16BV_CMD_WRITEENABLE : W25Q16BV_CMD_WRITEDISABLE);
  W25Q16BV_DESELECT();
}

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
spiflashError_e spiflashReadBuffer (uint32_t address, uint8_t *buffer, uint32_t len)
{
  if (!_w25q16bvInitialised) spiflashInit();

  uint32_t a, i;
  a = i = 0;

  // Make sure the address is valid
  if (address >= W25Q16BV_MAXADDRESS)
  {
    return SPIFLASH_ERROR_ADDROUTOFRANGE;
  }

  // Wait until the device is ready or a timeout occurs
  if (w25q16bvWaitForReady())
    return SPIFLASH_ERROR_TIMEOUT_READY;

  // Send the read data command
  W25Q16BV_SELECT();
  w25q16bv_TransferByte(W25Q16BV_CMD_READDATA);      // 0x03
  w25q16bv_TransferByte((address >> 16) & 0xFF);     // address upper 8
  w25q16bv_TransferByte((address >> 8) & 0xFF);      // address mid 8
  w25q16bv_TransferByte(address & 0xFF);             // address lower 8
  // Fill response buffer
  for (a = address; a < address + len; a++, i++)
  {
    if (a > W25Q16BV_MAXADDRESS)
    {
      // Oops ... we're at the end of the flash memory
      return SPIFLASH_ERROR_ADDROVERFLOW;
    }
    buffer[i] = w25q16bv_TransferByte(0xFF);
  }
  W25Q16BV_DESELECT();

  return SPIFLASH_ERROR_OK;
}

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
spiflashError_e spiflashEraseSector (uint32_t sectorNumber)
{
  if (!_w25q16bvInitialised) spiflashInit();

  // Make sure the address is valid
  if (sectorNumber >= W25Q16BV_SECTORS)
  {
    return SPIFLASH_ERROR_ADDROUTOFRANGE;
  }  

  // Wait until the device is ready or a timeout occurs
  if (w25q16bvWaitForReady())
    return SPIFLASH_ERROR_TIMEOUT_READY;

  // Make sure the chip is write enabled
  spiflashWriteEnable (TRUE);

  // Make sure the write enable latch is actually set
  uint8_t status;
  status = w25q16bvGetStatus();
  if (!(status & W25Q16BV_STAT1_WRTEN))
  {
    // Throw a write protection error (write enable latch not set)
    return SPIFLASH_ERROR_PROTECTIONERR;
  }

  // Send the erase sector command
  uint32_t address = sectorNumber * W25Q16BV_SECTORSIZE;
  W25Q16BV_SELECT();
  w25q16bv_TransferByte(W25Q16BV_CMD_SECTERASE4); 
  w25q16bv_TransferByte((address >> 16) & 0xFF);     // address upper 8
  w25q16bv_TransferByte((address >> 8) & 0xFF);      // address mid 8
  w25q16bv_TransferByte(address & 0xFF);             // address lower 8
  W25Q16BV_DESELECT();

  // Wait until the busy bit is cleared before exiting
  // This can take up to 400ms according to the datasheet
  while (w25q16bvGetStatus() & W25Q16BV_STAT1_BUSY);

  return SPIFLASH_ERROR_OK;
}

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
spiflashError_e spiflashEraseChip (void)
{
  if (!_w25q16bvInitialised) spiflashInit();

  // Wait until the device is ready or a timeout occurs
  if (w25q16bvWaitForReady())
    return SPIFLASH_ERROR_TIMEOUT_READY;

  // Make sure the chip is write enabled
  spiflashWriteEnable (TRUE);

  // Make sure the write enable latch is actually set
  uint8_t status;
  status = w25q16bvGetStatus();
  if (!(status & W25Q16BV_STAT1_WRTEN))
  {
    // Throw a write protection error (write enable latch not set)
    return SPIFLASH_ERROR_PROTECTIONERR;
  }

  // Send the erase chip command
  W25Q16BV_SELECT();
  w25q16bv_TransferByte(W25Q16BV_CMD_CHIPERASE); 
  W25Q16BV_DESELECT();

  // Wait until the busy bit is cleared before exiting
  // This can take up to 10 seconds according to the datasheet!
  while (w25q16bvGetStatus() & W25Q16BV_STAT1_BUSY);

  return SPIFLASH_ERROR_OK;
}

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
spiflashError_e spiflashWritePage (uint32_t address, uint8_t *buffer, uint32_t len)
{
  uint8_t status;
  uint32_t i;

  if (!_w25q16bvInitialised) spiflashInit();

  // Make sure the address is valid
  if (address >= W25Q16BV_MAXADDRESS)
  {
    return SPIFLASH_ERROR_ADDROUTOFRANGE;
  }

  // Make sure that the supplied data is no larger than the page size
  if (len > W25Q16BV_PAGESIZE)
  {
    return SPIFLASH_ERROR_DATAEXCEEDSPAGESIZE;
  }

  // Make sure that the data won't wrap around to the beginning of the sector
  if ((address % W25Q16BV_PAGESIZE) + len > W25Q16BV_PAGESIZE)
  {
    // If you try to write to a page beyond the last byte, it will
    // wrap around to the start of the page, almost certainly
    // messing up your data
    return SPIFLASH_ERROR_PAGEWRITEOVERFLOW;
  }

  // Wait until the device is ready or a timeout occurs
  if (w25q16bvWaitForReady())
    return SPIFLASH_ERROR_TIMEOUT_READY;

  // Make sure the chip is write enabled
  spiflashWriteEnable (TRUE);

  // Make sure the write enable latch is actually set
  status = w25q16bvGetStatus();
  if (!(status & W25Q16BV_STAT1_WRTEN))
  {
    // Throw a write protection error (write enable latch not set)
    return SPIFLASH_ERROR_PROTECTIONERR;
  }

  // Send page write command (0x02) plus 24-bit address
  W25Q16BV_SELECT();
  w25q16bv_TransferByte(W25Q16BV_CMD_PAGEPROG);      // 0x02
  w25q16bv_TransferByte((address >> 16) & 0xFF);     // address upper 8
  w25q16bv_TransferByte((address >> 8) & 0xFF);      // address mid 8
  if (len == 256)
  {
    // If len = 256 bytes, lower 8 bits must be 0 (see datasheet 11.2.17)
    w25q16bv_TransferByte(0);
  }
  else
  {
    w25q16bv_TransferByte(address & 0xFF);           // address lower 8
  }
  // Transfer data
  for (i = 0; i < len; i++)
  {
    w25q16bv_TransferByte(buffer[i]);
  }
  // Write only occurs after the CS line is de-asserted
  W25Q16BV_DESELECT();

  // Wait at least 3ms (max page program time according to datasheet)
  systickDelay(3);
  
  // Wait until the device is ready or a timeout occurs
  if (w25q16bvWaitForReady())
    return SPIFLASH_ERROR_TIMEOUT_READY;

  return SPIFLASH_ERROR_OK;
}

