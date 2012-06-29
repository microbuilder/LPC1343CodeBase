/**************************************************************************/
/*! 
    @file     adxl345.c
    @author   K. Townsend (microBuilder.eu)
	
    @brief    Drivers for Analog Devices ADXL345 Accelerometer

    @section DESCRIPTION

    The ADXL345 is a digital accelerometer with 13-bit resolution, capable
    of measuring up to +/-16g.  This driver communicate using I2C.

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, K. Townsend
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
#include "adxl345.h"
#include "core/systick/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static bool _adxl345Initialised = false;

/**************************************************************************/
/*! 
    @brief  Sends a single command byte over I2C
*/
/**************************************************************************/
static adxl345Error_t adxl345Write8 (uint8_t reg, uint8_t value)
{
  adxl345Error_t error = ADXL345_ERROR_OK;

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 3;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = ADXL345_ADDRESS;       // I2C device address
  I2CMasterBuffer[1] = reg;                   // Register
  I2CMasterBuffer[2] = value;
  i2cEngine();

  // ToDo: Add in proper I2C error-checking
  return error;
}

/**************************************************************************/
/*! 
    @brief  Reads a 16 bit values over I2C
*/
/**************************************************************************/
static adxl345Error_t adxl345Read8(uint8_t reg, uint8_t *value)
{
  adxl345Error_t error = ADXL345_ERROR_OK;

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 1;
  I2CMasterBuffer[0] = ADXL345_ADDRESS;         // I2C device address
  I2CMasterBuffer[1] = reg;                     // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = ADXL345_ADDRESS | ADXL345_READBIT;  
  i2cEngine();

  // Shift values to create properly formed integer
  *value = I2CSlaveBuffer[0];

  // ToDo: Add in proper I2C error-checking
  return error;
}

/**************************************************************************/
/*! 
    @brief  Read the device ID (can be used to check connection)
*/
/**************************************************************************/
adxl345Error_t adxl345GetDeviceID(uint8_t *id)
{
  adxl345Error_t error = ADXL345_ERROR_OK;

  // Check device ID register
  error = adxl345Read8(ADXL345_REG_DEVID, id);
  return error;
}

/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
adxl345Error_t adxl345Init(void)
{
  uint8_t devid = 0x00;
  adxl345Error_t error = ADXL345_ERROR_OK;

  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return ADXL345_ERROR_I2CINIT;    /* Fatal error */
  }

  // Check device ID register to see if everything is properly connected
  error = adxl345Read8(ADXL345_REG_DEVID, &devid);
  if (error)
  {
    return error;
  }
  else if (devid != 0xE5)
  {
    return ADXL345_ERROR_NOCONNECTION;
  }

  // Enable measurements
  error = adxl345Write8(ADXL345_REG_POWER_CTL, 0x08);
  if (error)
  {
    return error;
  }

  _adxl345Initialised = true;
  return error;
}

/**************************************************************************/
/*! 
    @brief  Gets the latest X/Y/Z values
*/
/**************************************************************************/
adxl345Error_t adxl345GetXYZ(int16_t *x, int16_t *y, int16_t *z)
{
  adxl345Error_t error = ADXL345_ERROR_OK;

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 6;
  I2CMasterBuffer[0] = ADXL345_ADDRESS;         // I2C device address
  I2CMasterBuffer[1] = ADXL345_REG_DATAX0;
  // Append address w/read bit
  I2CMasterBuffer[2] = ADXL345_ADDRESS | ADXL345_READBIT;  
  i2cEngine();

  // Shift values to create properly formed integer
  *x = (I2CSlaveBuffer[1] << 8) | (I2CSlaveBuffer[0]);
  *y = (I2CSlaveBuffer[3] << 8) | (I2CSlaveBuffer[2]);
  *z = (I2CSlaveBuffer[5] << 8) | (I2CSlaveBuffer[4]);

  // ToDo: Add in proper I2C error-checking
  return error;
}

