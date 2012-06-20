/**************************************************************************/
/*! 
    @file     mpl115a2.c
    @author   K. Townsend (microBuilder.eu)
	
    @brief    Drivers for the Freescale MPL115A2 I2C Digital Barometer

    @section DESCRIPTION

    The MPL115A2 is an I2C absolute pressure sensor, employing a MEMS
    pressure sensor with a conditioning IC to provide accurate pressure
    measurements from 50 to 115 kPa.

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
#include "mpl115a2.h"
#include "core/systick/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static float _mpl115a2_a0;
static float _mpl115a2_b1;
static float _mpl115a2_b2;
static float _mpl115a2_c12;

static bool _mpl115a2Initialised = false;

/**************************************************************************/
/*! 
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************/
mpl115a2Error_t mpl115a2ReadPressureTemp(uint16_t *pressure, uint16_t *temp)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
    I2CSlaveBuffer[i] = 0x00;
  }

  I2CWriteLength = 3;
  I2CReadLength = 1;
  I2CMasterBuffer[0] = MPL115A2_ADDRESS;
  I2CMasterBuffer[1] = MPL115A2_REGISTER_STARTCONVERSION;
  I2CMasterBuffer[2] = 0x00;  // Why is this necessary to get results?
  i2cEngine();

  // Wait a bit for the conversion to complete (3ms max)
  systickDelay(5);

  I2CWriteLength = 2;
  I2CReadLength = 4;
  I2CMasterBuffer[0] = MPL115A2_ADDRESS;
  I2CMasterBuffer[1] = MPL115A2_REGISTER_PRESSURE_MSB;
  I2CMasterBuffer[2] = MPL115A2_ADDRESS | MPL115A2_READBIT;
  i2cEngine();

  // Shift values to create properly formed integers
  *pressure = ((I2CSlaveBuffer[0] << 8) | (I2CSlaveBuffer[1])) >> 6;
  *temp = ((I2CSlaveBuffer[2] << 8) | (I2CSlaveBuffer[3])) >> 6;

  return MPL115A2_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
mpl115a2Error_t mpl115a2ReadCoefficients(void)
{
  int16_t a0coeff;
  int16_t b1coeff;
  int16_t b2coeff;
  int16_t c12coeff;

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
    I2CSlaveBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 8;
  I2CMasterBuffer[0] = MPL115A2_ADDRESS;
  I2CMasterBuffer[1] = MPL115A2_REGISTER_A0_COEFF_MSB;
  I2CMasterBuffer[2] = MPL115A2_ADDRESS | MPL115A2_READBIT;  
  i2cEngine();

  a0coeff = (I2CSlaveBuffer[0] << 8 ) | I2CSlaveBuffer[1];
  b1coeff = (I2CSlaveBuffer[2] << 8 ) | I2CSlaveBuffer[3];
  b2coeff = (I2CSlaveBuffer[4] << 8 ) | I2CSlaveBuffer[5];
  c12coeff = ((I2CSlaveBuffer[6] << 8 ) | I2CSlaveBuffer[7]) >> 2; 

  _mpl115a2_a0 = (float)a0coeff / 8;
  _mpl115a2_b1 = (float)b1coeff / 8192;
  _mpl115a2_b2 = (float)b2coeff / 16384;
  _mpl115a2_c12 = (float)c12coeff / 4194304;

  return MPL115A2_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
mpl115a2Error_t mpl115a2Init(void)
{
  mpl115a2Error_t error = MPL115A2_ERROR_OK;

  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return MPL115A2_ERROR_I2CINIT;    /* Fatal error */
  }  

  // Coefficients only need to be read once
  mpl115a2ReadCoefficients();

  _mpl115a2Initialised = true;
  return error;
}

/**************************************************************************/
/*! 
    @brief  Gets the compensated pressure level in kPa
*/
/**************************************************************************/
mpl115a2Error_t mpl115a2GetPressure(float *pressure)
{
  uint16_t  Padc, Tadc;
  float     Pcomp;

  mpl115a2Error_t error = MPL115A2_ERROR_OK;

  // Make sure the coefficients have been read, etc.
  if (!_mpl115a2Initialised) mpl115a2Init();

  // Get raw pressure and temperature settings
  error = mpl115a2ReadPressureTemp(&Padc, &Tadc);
  if (error) return error;

  // See datasheet p.6 for evaluation sequence
  Pcomp = _mpl115a2_a0 + (_mpl115a2_b1 + _mpl115a2_c12 * Tadc ) * Padc + _mpl115a2_b2 * Tadc;

  // Return pressure as floating point value
  *pressure = ((65.0F / 1023.0F)*(float)Pcomp) + 50;

  return error;
}
