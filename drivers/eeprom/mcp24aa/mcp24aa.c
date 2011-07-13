/**************************************************************************/
/*! 
    @file     mcp24aa.c
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    @section DESCRIPTION

    Driver for Microchip's 24AA32AF serial EEPROM.  This driver assumes
    that the address is set to 1010 000.
    
    @section Example

    @code 
    #include "core/cpu/cpu.h"
    #include "drivers/eeprom/mcp24aa/mcp24aa.h"

    int main(void)
    {
      cpuInit();

      mcp24aaInit();

      // Instantiate error message placeholder
      mcp24aaError_e error = MCP24AA_ERROR_OK;

      // Create read buffer (1 byte)
      uint8_t buffer[1] = { 0x00 };

      // Write 0xCC at address 0x125
      error = mcp24aaWriteByte(0x0125, 0xCC);
      if (error)
      {
        // Handle any errors
        switch (error)
        {
          case (MCP24AA_ERROR_I2CINIT):
            // Unable to initialise I2C
            break;
          case (MCP24AA_ERROR_ADDRERR):
            // Address out of range
            break;
          default:
            break;
        }
      }

      // Read the contents of address 0x0125
      error = MCP24AA_ERROR_OK;
      error = mcp24aaReadByte(0x0125, buffer);
      if (error)
      {
        // Handle any errors
        switch (error)
        {
          case (MCP24AA_ERROR_I2CINIT):
            // Unable to initialise I2C
            break;
          case (MCP24AA_ERROR_ADDRERR):
            // Address out of range
            break;
          default:
            break;
        }
      }

      uint8_t results = buffer[0];
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

#include "mcp24aa.h"
#include "core/systick/systick.h"
#include "core/i2c/i2c.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static bool _mcp24aaInitialised = false;

/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
mcp24aaError_e mcp24aaInit()
{
  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return MCP24AA_ERROR_I2CINIT;    /* Fatal error */
  }

  // Set initialisation flag
  _mcp24aaInitialised = true;

  return MCP24AA_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief Reads the specified number of bytes from the supplied address.

    This function will read one or more bytes starting at the supplied
    address.  A maximum of 8 bytes can be read in one operation.

    @param[in]  address
                The 16-bit address where the read will start.  The maximum
                value for the address depends on the size of the EEPROM
    @param[in]  *buffer
                Pointer to the buffer that will store the read results
    @param[in]  bufferLength
                Length of the buffer
*/
/**************************************************************************/
mcp24aaError_e mcp24aaReadBuffer (uint16_t address, uint8_t *buffer, uint32_t bufferLength)
{
  if (!_mcp24aaInitialised) mcp24aaInit();

  if (address >= MCP24AA_MAXADDR)
  {
    return MCP24AA_ERROR_ADDRERR;
  }

  if (bufferLength > 8)
  {
    return MCP24AA_ERROR_BUFFEROVERFLOW;
  }

  // ToDo: Check if I2C is ready

  // Clear buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
    I2CSlaveBuffer[i] = 0x00;
  }

  // Write address bits to enable random read
  I2CWriteLength = 3;
  I2CReadLength = bufferLength;
  I2CMasterBuffer[0] = MCP24AA_ADDR;                    // I2C device address
  I2CMasterBuffer[1] = (address >> 8);                  // Address (high byte)
  I2CMasterBuffer[2] = (address & 0xFF);                // Address (low byte)
  // If you wish to read, you need to append the address w/read bit, though this
  // needs to be placed one bit higher than the size of I2CWriteLength which 
  // may be unexpected
  I2CMasterBuffer[3] = MCP24AA_ADDR | MCP24AA_READBIT;  

  // Transmit command
  i2cEngine();

  // Fill response buffer
  for (i = 0; i < bufferLength; i++)
  {
    buffer[i] = I2CSlaveBuffer[i];
  }

  return MCP24AA_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief Writes the supplied bytes at a specified address.

    This function will write one or more bytes starting at the supplied
    address.  A maximum of 8 bytes can be written in one operation.

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
mcp24aaError_e mcp24aaWriteBuffer (uint16_t address, uint8_t *buffer, uint32_t bufferLength)
{
  if (!_mcp24aaInitialised) mcp24aaInit();

  if (address >= MCP24AA_MAXADDR)
  {
    return MCP24AA_ERROR_ADDRERR;
  }

  if (bufferLength > 8)
  {
    return MCP24AA_ERROR_BUFFEROVERFLOW;
  }

  // ToDo: Check if I2C is ready

  // Clear write buffer
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  // Write address bits and data to the master buffer
  I2CWriteLength = 3 + bufferLength;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = MCP24AA_ADDR;                // I2C device address
  I2CMasterBuffer[1] = (address >> 8);              // Address (high byte)
  I2CMasterBuffer[2] = (address & 0xFF);            // Address (low byte)
  for (i = 0; i < bufferLength; i++)
  {
    I2CMasterBuffer[i+3] = buffer[i];
  }

  // Transmit command
  i2cEngine();

  // Wait at least 10ms
  systickDelay(10);
  
  return MCP24AA_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief Reads one byte from the supplied address.

    This function will read one byte starting at the supplied address.

    @param[in]  address
                The 16-bit address where the read will start.  The maximum
                value for the address depends on the size of the EEPROM
    @param[in]  *buffer
                Pointer to the buffer that will store the read results

    @code
    #include "core/cpu/cpu/h"
    #include "drivers/eeprom/mcp24aa/mcp24aa.h"
    ...
    cpuInit();
    mcp24aaInit();

    // Create read buffer (1 byte)
    uint8_t buffer[1] = { 0x00 };

    // Write 0xEE and address 0x0120
    mcp24aaWriteByte(0x0120, 0xEE);

    // Populate buffer with contents of 0x0120
    mcp24aaReadByte(0x0120, buffer);

    // results should equal 0xEE
    uint8_t results = buffer[0];
    @endcode
*/
/**************************************************************************/
mcp24aaError_e mcp24aaReadByte (uint16_t address, uint8_t *buffer)
{
  if (!_mcp24aaInitialised) mcp24aaInit();

  return mcp24aaReadBuffer(address, buffer, 1);
}

/**************************************************************************/
/*! 
    @brief Writes one byte to the supplied address.

    This function will write one byte at the supplied address.

    @param[in]  address
                The 16-bit address where the write will start.  The maximum
                value for the address depends on the size of the EEPROM
    @param[in]  value
                The data to be written to the EEPROM

    @code
    #include "core/cpu/cpu/h"
    #include "drivers/eeprom/mcp24aa/mcp24aa.h"
    ...
    cpuInit();
    mcp24aaInit();

    // Create read buffer (1 byte)
    uint8_t buffer[1] = { 0x00 };

    // Write 0xEE and address 0x0120
    mcp24aaWriteByte(0x0120, 0xEE);

    // Populate buffer with contents of 0x0120
    mcp24aaReadByte(0x0120, buffer);

    // results should equal 0xEE
    uint8_t results = buffer[0];
    @endcode
*/
/**************************************************************************/
mcp24aaError_e mcp24aaWriteByte (uint16_t address, uint8_t value)
{
  if (!_mcp24aaInitialised) mcp24aaInit();

  // Set read buffer
  uint8_t wBuffer[1];

  // Write byte to EEPROM at specified address
  wBuffer[0] = value;
  return mcp24aaWriteBuffer(address, wBuffer, 1);
}

