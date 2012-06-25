/**************************************************************************/
/*!
    @file     ads1015.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Drivers for the TI ADS1015 12-Bit I2C ADC

    @section DESCRIPTION

    The ADS1015 is a 4-channel, 12-bit I2C ADC with a programmable
    comparator and an internal PGA (from 2/3 to 16x gain).  It can be
    configured for four single-ended channels or two differential inputs.
    The comparator can be used in both traditional and windowed mode.

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
#include "ads1015.h"
#include "core/systick/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static bool _ads1015Initialised = false;

/**************************************************************************/
/*!
    @brief  Sends a single command byte over I2C
*/
/**************************************************************************/
static ads1015Error_t ads1015WriteRegister (uint8_t reg, uint16_t value)
{
  ads1015Error_t error = ADS1015_ERROR_OK;

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 4;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = ADS1015_ADDRESS;       // I2C device address
  I2CMasterBuffer[1] = reg;                   // Register
  I2CMasterBuffer[2] = value >> 8;            // Upper 8-bits
  I2CMasterBuffer[3] = value & 0xFF;          // Lower 8-bits
  i2cEngine();

  // ToDo: Add in proper I2C error-checking
  return error;
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit values over I2C
*/
/**************************************************************************/
static ads1015Error_t ina219Read16(uint8_t reg, uint16_t *value)
{
  ads1015Error_t error = ADS1015_ERROR_OK;

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 2;
  I2CMasterBuffer[0] = ADS1015_ADDRESS;           // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = ADS1015_ADDRESS | ADS1015_READBIT;
  i2cEngine();

  // Shift values to create properly formed integer
  *value = ((I2CSlaveBuffer[0] << 8) | I2CSlaveBuffer[1]);

  // ToDo: Add in proper I2C error-checking
  return error;
}

/**************************************************************************/
/*!
    @brief  Initialises the I2C block
*/
/**************************************************************************/
ads1015Error_t ads1015Init(void)
{
  ads1015Error_t error = ADS1015_ERROR_OK;

  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return ADS1015_ERROR_I2CINIT;    /* Fatal error */
  }

  _ads1015Initialised = true;
  return error;
}

/**************************************************************************/
/*!
    @brief  Reads the 12-bit conversion results from the specified channel
*/
/**************************************************************************/
ads1015Error_t ads1015ReadADC_SingleEnded(uint8_t channel, uint16_t *value)
{
  ads1015Error_t error = ADS1015_ERROR_OK;

  if (!(_ads1015Initialised))
  {
    ads1015Init();
  }

  if (channel > 3)
  {
    *value = 0;
    return ADS1015_ERROR_INVALIDCHANNEL;
  }

  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= ADS1015_REG_CONFIG_PGA_6_144V;            // +/- 6.144V range (limited to VDD +0.3V max!)

  // Set single-ended input channel
  switch (channel)
  {
    case (0):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
      break;
    case (1):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
      break;
    case (2):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
      break;
    case (3):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
      break;
  }

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  error = ads1015WriteRegister(ADS1015_REG_POINTER_CONFIG, config);
  if (error) return error;

  // Wait for the conversion to complete
  systickDelay(1);

  // Read the conversion results
  error = ina219Read16(ADS1015_REG_POINTER_CONVERT, value);
  if (error) return error;

  // Shift results 4-bits to the right
  *value = *value >> 4;

  return error;
}

/**************************************************************************/
/*!
    @brief  Reads the 12-bit conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN1) input.  Generates
            a signed 12-bit value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
ads1015Error_t ads1015ReadADC_Differential_0_1(int16_t *value)
{
  ads1015Error_t error = ADS1015_ERROR_OK;

  if (!(_ads1015Initialised))
  {
    ads1015Init();
  }

  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= ADS1015_REG_CONFIG_PGA_6_144V;            // +/- 6.144V range (limited to VDD +0.3V max!)

  // Set channels
  config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1;          // AIN0 = P, AIN1 = N

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  error = ads1015WriteRegister(ADS1015_REG_POINTER_CONFIG, config);
  if (error) return error;

  // Wait for the conversion to complete
  systickDelay(1);

  // Read the conversion results
  error = ina219Read16(ADS1015_REG_POINTER_CONVERT, value);
  if (error) return error;

  // Shift results 4-bits to the right
  *value = *value >> 4;

  return error;
}

/**************************************************************************/
/*!
    @brief  Reads the 12-bit conversion results, measuring the voltage
            difference between the P (AIN2) and N (AIN3) input.  Generates
            a signed 12-bit value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
ads1015Error_t ads1015ReadADC_Differential_2_3(int16_t *value)
{
  ads1015Error_t error = ADS1015_ERROR_OK;

  if (!(_ads1015Initialised))
  {
    ads1015Init();
  }

  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= ADS1015_REG_CONFIG_PGA_6_144V;            // +/- 6.144V range (limited to VDD +0.3V max!)

  // Set channels
  config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3;          // AIN2 = P, AIN3 = N

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  error = ads1015WriteRegister(ADS1015_REG_POINTER_CONFIG, config);
  if (error) return error;

  // Wait for the conversion to complete
  systickDelay(1);

  // Read the conversion results
  error = ina219Read16(ADS1015_REG_POINTER_CONVERT, value);
  if (error) return error;

  // Shift results 4-bits to the right
  *value = *value >> 4;

  return error;
}

/**************************************************************************/
/*!
    @brief  Sets up the comparator to operate in basic mode, causing the
            ALERT/RDY pin to assert (go from high to low) when the ADC
            value exceeds the specified threshold.

            This will also set the ADC in continuous conversion mode.

    @section EXAMPLE

    @code
    ads1015Init();

    // Setup 3V comparator on channel 0
    ads1015StartComparator_SingleEnded(0, 1000);

    int16_t results;
    while(1)
    {
      // Need to read to clear com bit once it's set
      ads1015GetLastConversionResults(&results);
      printf("%d\r\n", results);
    }
    @endcode
*/
/**************************************************************************/
ads1015Error_t ads1015StartComparator_SingleEnded(uint8_t channel, int16_t threshold)
{
  uint16_t value;

  ads1015Error_t error = ADS1015_ERROR_OK;

  if (!(_ads1015Initialised))
  {
    ads1015Init();
  }

  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_1CONV   | // Comparator enabled and asserts on 1 match
                    ADS1015_REG_CONFIG_CLAT_LATCH   | // Latching mode
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_CONTIN  | // Continuous conversion mode
                    ADS1015_REG_CONFIG_PGA_6_144V   | // +/- 6.144V range (limited to VDD +0.3V max!)
                    ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

  // Set single-ended input channel
  switch (channel)
  {
    case (0):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
      break;
    case (1):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
      break;
    case (2):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
      break;
    case (3):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
      break;
  }

  // Set the high threshold register
  error = ads1015WriteRegister(ADS1015_REG_POINTER_HITHRESH, threshold << 4);
  if (error) return error;

  // Write config register to the ADC
  error = ads1015WriteRegister(ADS1015_REG_POINTER_CONFIG, config);
  if (error) return error;
}

/**************************************************************************/
/*!
    @brief  In order to clear the comparator, we need to read the
            conversion results.  This function reads the last conversion
            results without changing the config value.
*/
/**************************************************************************/
ads1015Error_t ads1015GetLastConversionResults(int16_t *value)
{
  ads1015Error_t error = ADS1015_ERROR_OK;

  if (!(_ads1015Initialised))
  {
    ads1015Init();
  }

  // Wait for the conversion to complete
  systickDelay(1);

  // Read the conversion results
  error = ina219Read16(ADS1015_REG_POINTER_CONVERT, value);
  if (error) return error;

  // Shift results 4-bits to the right
  *value = *value >> 4;

  return error;
}




