/**************************************************************************/
/*! 
    @file     as1115.c
    @author   K. Townsend (microBuilder.eu)
	
    @brief    Drivers for the AS1115 I2C LED/segment-display driver

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, microBuilder SARL
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
#include "as1115.h"
#include "core/systick/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static bool _as1115Initialised = false;

/**************************************************************************/
/*! 
    @brief  Sends a single command byte over I2C to the specified address
*/
/**************************************************************************/
as1115Error_t as1115WriteCmd (uint8_t address, uint8_t cmd)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  // ToDo: Add real I2C error checking
  I2CWriteLength = 2;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = address;               // device sub address
  I2CMasterBuffer[1] = cmd;                   // Command register
  i2cEngine();
  return AS1115_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Writes an command byte followed by a data byte over I2C
*/
/**************************************************************************/
as1115Error_t as1115WriteCmdData (uint8_t address, uint8_t cmd, uint8_t data)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  // ToDo: Add real I2C error checking
  I2CWriteLength = 3;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = address;               // I2C device address
  I2CMasterBuffer[1] = cmd;                   // Command register
  I2CMasterBuffer[2] = data;                  // Value to write
  i2cEngine();
  return AS1115_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Reads a single byte over I2C
*/
/**************************************************************************/
as1115Error_t as1115Read8(uint8_t address, uint8_t *value)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  // ToDo: Add real I2C error checking
  I2CWriteLength = 0;
  I2CReadLength = 1;
  I2CMasterBuffer[0] = address | AS1115_READBIT;
  i2cEngine();

  *value = I2CSlaveBuffer[0];

  return AS1115_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
as1115Error_t as1115Init(void)
{
  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return AS1115_ERROR_I2CINIT;    /* Fatal error */
  }

  // Reset AS1115 to normal operation with default settings
  as1115WriteCmdData(AS1115_ADDRESS, AS1115_SHUTDOWN, 0x01);

  // Use user-set I2C address (000000 + address pins)
  as1115WriteCmdData(AS1115_ADDRESS, AS1115_SELFADDR, 0x01);

  systickDelay(20);

  // Use hex decoding
  as1115SetDecodeMode(0);

  // Set the brightness to the maximum value (0x0F)
  as1115SetBrightness(0xF);

  // Turn on all digits by default (0..7)
  as1115WriteCmdData(AS1115_SUBADDRESS, AS1115_SCANLIMIT, 0x07);

  _as1115Initialised = true;

  return AS1115_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Send test command
*/
/**************************************************************************/
as1115Error_t as1115Test(void)
{
  as1115Error_t error = AS1115_ERROR_OK;
  uint8_t results;

  if (!_as1115Initialised) as1115Init();

  // Use test register to detect LED status
  error = as1115WriteCmd(AS1115_SUBADDRESS, AS1115_DISPTEST);
  if (error) return error;

  error = as1115Read8(AS1115_SUBADDRESS, &results);
  if (error) return error;

  // Throw an error since something is wrong with the wiring (use 'results' to check)
  if ((results & 0xF8) != 0x80) 
  {
    return AS1115_ERROR_UNEXPECTEDRESPONSE;
  } 

  // Turn everything on
  error = as1115WriteCmdData(AS1115_SUBADDRESS, AS1115_DISPTEST, 0x01);
  if (error) return error;  
  systickDelay(1000);

  // Turn everything off
  error = as1115WriteCmdData(AS1115_SUBADDRESS, AS1115_DISPTEST, 0x00);
  if (error) return error;
  
  return error;
}

/**************************************************************************/
/*! 
    @brief  Sets the decode enable register to indicate if BCD or HEX
            decoding should be used.  Set to 0 for no BCD on all digits.
*/
/**************************************************************************/
as1115Error_t as1115SetDecodeMode(uint8_t x) 
{
  as1115Error_t error = AS1115_ERROR_OK;

  error = as1115WriteCmdData(AS1115_SUBADDRESS, AS1115_DECODEMODE, x);
  return error;
}

/**************************************************************************/
/*! 
    @brief  Sets the intensity control register (0..15 or 0x00..0x0F)
*/
/**************************************************************************/
as1115Error_t as1115SetBrightness(uint8_t x) 
{
  as1115Error_t error = AS1115_ERROR_OK;

  if (x > 0xF) x = 0xF;

  error = as1115WriteCmdData(AS1115_SUBADDRESS, AS1115_INTENSITY, x);
  return error;
}

/**************************************************************************/
/*! 
    @brief  Sets the feature bit (controls blinking, clock source, etc.)
*/
/**************************************************************************/
as1115Error_t as1115SetFeature(uint8_t feature) 
{
  as1115Error_t error = AS1115_ERROR_OK;

  error = as1115WriteCmdData(AS1115_SUBADDRESS, AS1115_FEATURE, feature);
  return error;
}

/**************************************************************************/
/*! 
    @brief  Writes an 8-bit buffer to the display
*/
/**************************************************************************/
as1115Error_t as1115WriteBuffer(uint8_t *buffer) 
{
  as1115Error_t error = AS1115_ERROR_OK;
  uint32_t i;

  if (!_as1115Initialised) as1115Init();

  // Clear write buffers
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  // Update individual digits
  for ( i = 0; i < 8; i++ )
  {
    I2CWriteLength = 3;
    I2CReadLength = 0;
    I2CMasterBuffer[0] = AS1115_SUBADDRESS; // I2C device address
    I2CMasterBuffer[1] = i+1;               // Digit register
    I2CMasterBuffer[2] = buffer[i];         // Value
    i2cEngine();
  }
  
  return AS1115_ERROR_OK;
}  

