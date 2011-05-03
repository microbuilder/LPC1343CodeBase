/**************************************************************************/
/*! 
    @file     at25040.c
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    @section DESCRIPTION

    Driver for Atmel's AT25010a/AT25020a/AT25040a 1K/2K/4K serial EEPROM.
    
    @note     The AT25xxx has an 8-byte buffer, including 1 command byte
              and one address offset byte, meaning that a maximum of 6
              bytes can be read or written in one operation.  An error
              will be returned if a value greater than 6 is passed in
              for bufferLength with the eepromRead and eepromWrite
              methods.

    @section Example

    @code 
    #include "core/cpu/cpu.h"
    #include "drivers/eeprom/at25040/at25040.h"

    int main(void)
    {
      cpuInit();
      at25Init();

      // Set read and write buffers
      uint8_t wBuffer[1];
      uint8_t rBuffer[1];

      // Instantiate error message placeholder
      at25Error_e error = AT25_ERROR_OK;
    
      // Write 0xAA to EEPROM at address 0x0000
      wBuffer[0] = 0xAA;
      error = at25Write(0x0000, wBuffer, 1);
      if (error)
      {
        // Log the error message or take appropriate actions
        switch (error)
        {
          case (AT25_ERROR_TIMEOUT_WFINISH):
            // EEPROM timed out waiting for the write to finish
            break;
          case (AT25_ERROR_TIMEOUT_WE):
            // EEPROM timed out waiting for write-enable
            break;
          case (AT25_ERROR_ADDRERR):
            // Address is out of range
            break;
        }
      }

      // Read the EEPROM at address 0x0000
      at25Read(0x0000, rBuffer, 1);
      ...
    }
    @endcode
	
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

#include "at25040.h"
#include "core/ssp/ssp.h"
#include "core/gpio/gpio.h"

#define AT25_SELECT()       gpioSetValue(0, 2, 0)
#define AT25_DESELECT()     gpioSetValue(0, 2, 1)

uint32_t i, timeout;
uint8_t src_addr[SSP_FIFOSIZE]; 
uint8_t dest_addr[SSP_FIFOSIZE];

/**************************************************************************/
/*! 
    @brief Sends the write enable command (WREN/0x06)
*/
/**************************************************************************/
void at25WriteEnable()
{
  AT25_SELECT();
  src_addr[0] = AT25_WREN;
  sspSend(0, (uint8_t *)src_addr, 1);
  AT25_DESELECT();

  // Delay for at least 250nS (1nS @ 72MHz = ~0.0072 ticks)
  for (i = 0; i < 100; i++);
}

/**************************************************************************/
/*! 
    @brief Gets the value of the Read Status Register (RDSR/0x05)

    @return     The 8-bit value returned by the Read Status Register
*/
/**************************************************************************/
uint8_t at25GetRSR()
{
  AT25_SELECT();
  src_addr[0] = AT25_RDSR;
  sspSend(0, (uint8_t *)src_addr, 1);
  sspReceive(0, (uint8_t *)dest_addr, 1);
  AT25_DESELECT();
  return dest_addr[0] & (AT25_RDSR_WEN | AT25_RDSR_RDY);
}

/**************************************************************************/
/*! 
    @brief  Initialises the SPI block (CLK set low when inactive, trigger
            on leading edge).
*/
/**************************************************************************/
void at25Init (void)
{
  sspInit(0, sspClockPolarity_Low, sspClockPhase_RisingEdge);
}

/**************************************************************************/
/*! 
    @brief Reads the specified number of bytes from the supplied address.

    This function will read one or more bytes starting at the supplied
    address.

    @param[in]  address
                The 16-bit address where the read will start.  The maximum
                value for the address depends on the size of the EEPROM
    @param[in]  *buffer
                Pointer to the buffer that will store the read results
    @param[in]  bufferLength
                Length of the buffer
*/
/**************************************************************************/
at25Error_e at25Read (uint16_t address, uint8_t *buffer, uint32_t bufferLength)
{
  if (address >= AT25_MAXADDRESS)
  {
    return AT25_ERROR_ADDRERR;
  }

  if (bufferLength > 6)
  {
    return AT25_ERROR_BUFFEROVERFLOW;
  }

  timeout = 0;
  while ( timeout < SSP_MAX_TIMEOUT )
  {
    // Wait until the device is ready
    uint8_t status = at25GetRSR() & AT25_RDSR_RDY;
    if (status == 0)
    {
      break;
    }
    timeout++;
  }
  if ( timeout == SSP_MAX_TIMEOUT )
  {
    return AT25_ERROR_TIMEOUT_WE;
  }

  AT25_SELECT();
  // Read command (0x03), append A8 if > addr 256 bytes
  src_addr[0] = address > 0xFF ? AT25_READ | AT25_A8 : AT25_READ;
  src_addr[1] = (address);
  sspSend(0, (uint8_t *)src_addr, 2); 
  sspReceive(0, (uint8_t *)&dest_addr[2], bufferLength);
  AT25_DESELECT();

  // Fill response buffer
  for (i = 0; i < bufferLength; i++)
  {
    buffer[i] = dest_addr[i + 2];
  }

  return AT25_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief Writes the supplied bytes at a specified address.

    This function will write one or more bytes starting at the supplied
    address.

    @param[in]  address
                The 16-bit address where the write will start.  The
                maximum value for the address depends on the size of the
                EEPROM
    @param[in]  *buffer
                Pointer to the buffer that contains the values to write.
    @param[in]  bufferLength
                Length of the buffer
*/
/**************************************************************************/
at25Error_e at25Write (uint16_t address, uint8_t *buffer, uint32_t bufferLength)
{
  if (address >= AT25_MAXADDRESS)
  {
    return AT25_ERROR_ADDRERR;
  }

  if (bufferLength > 6)
  {
    return AT25_ERROR_BUFFEROVERFLOW;
  }

  // Set write enable latch
  at25WriteEnable();

  timeout = 0;
  while ( timeout < SSP_MAX_TIMEOUT )
  {
    // Wait until the device is write enabled
    if (at25GetRSR() == AT25_RDSR_WEN)
    {
      break;
    }
    timeout++;
  }
  if ( timeout == SSP_MAX_TIMEOUT )
  {
    return AT25_ERROR_TIMEOUT_WE;
  }

  for (i = 0; i < bufferLength; i++)        // Init RD and WR buffer
  {
    src_addr[i+2] = buffer[i];              // leave two bytes for cmd and offset(8 bits)
    dest_addr[i] = 0;
  }

  AT25_SELECT();
  // Write command (0x02), append A8 if addr > 256 bytes
  src_addr[0] = address > 0xFF ? AT25_WRITE | AT25_A8 : AT25_WRITE;
  src_addr[1] = (address);
  sspSend(0, (uint8_t *)src_addr, bufferLength + 2);
  AT25_DESELECT();

  // Wait at least 3ms
  for (i = 0; i < ((CFG_CPU_CCLK / 1000) * 3); i++);
  
  timeout = 0;
  while ( timeout < SSP_MAX_TIMEOUT )
  {
    // Check status to see if write cycle is done or not
    AT25_SELECT();
    src_addr[0] = AT25_RDSR;
    sspSend(0, (uint8_t *)src_addr, 1);
    sspReceive(0, (uint8_t *)dest_addr, 1);
    AT25_DESELECT();
    // Wait until device is ready
    if ((dest_addr[0] & AT25_RDSR_RDY) == 0x00)
    {
      break;
    }
    timeout++;
  }
  if ( timeout == SSP_MAX_TIMEOUT )
  {
    return AT25_ERROR_TIMEOUT_WFINISH;
  }

  for (i = 0; i < 300; i++);                // Wait at least 250ns

  return AT25_ERROR_OK;
}
