/**************************************************************************/
/*! 
    @file     lm75b.c
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    @section DESCRIPTION

    Driver for NXP's LM75B I2C temperature sensor.  This temperature
    sensor has an accuracy of 0.125°C, and returns a temperature value
    in degrees celsius where each unit is equal to 0.125°C.  For example,
    if the temperature reading is 198, it means that the temperature in
    degree celsius is: 198 / 8 = 24.75°C.
    
    @section Example

    @code 
    #include "core/cpu/cpu.h"
    #include "drivers/sensors/lm75b/lm75b.h"

    int main(void)
    {
      cpuInit();

      int32_t temp = 0;

      // Initialise the LM75B
      lm75bInit();

      while (1)
      {
        // Get the current temperature (in 0.125°C units)
        lm75bGetTemperature(&temp);
  
        // Multiply value by 125 for fixed-point math (0.125°C per unit)
        temp *= 125;

        // Use modulus operator to display decimal value
        printf("Current Temperature: %d.%d C\n", temp / 1000, temp % 1000);

        // Alternatively, you could also use floating point math, though 
        // this will result in larger compiled code if you add in floating
        // point support for printf, etc.
        //
        // float tempFloat = 0.0F;
        // lm75bGetTemperature(&temp);
        // tempFloat = (float)temp / 8.0F;
      }
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

#include "lm75b.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static bool _lm75bInitialised = false;

/**************************************************************************/
/*! 
    @brief  Writes an 8 bit values over I2C
*/
/**************************************************************************/
lm75bError_e lm75bWrite8 (uint8_t reg, uint32_t value)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 3;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = LM75B_ADDRESS;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  I2CMasterBuffer[2] = (value & 0xFF);            // Value to write
  i2cEngine();
  return LM75B_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Reads a 16 bit values over I2C
*/
/**************************************************************************/
lm75bError_e lm75bRead16(uint8_t reg, int32_t *value)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 2;
  I2CMasterBuffer[0] = LM75B_ADDRESS;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = LM75B_ADDRESS | LM75B_READBIT;  
  i2cEngine();

  // Shift values to create properly formed integer
  *value = ((I2CSlaveBuffer[0] << 8) | I2CSlaveBuffer[1]) >> 5;

  //  Sign extend negative numbers
  if (I2CSlaveBuffer[0] & 0x80)
  {
    // Negative number
    *value |= 0xFFFFFC00;
  }
  return LM75B_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
lm75bError_e lm75bInit(void)
{
  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return LM75B_ERROR_I2CINIT;    /* Fatal error */
  }
  
  _lm75bInitialised = true;

  return LM75B_ERROR_OK;
  
  // Set device to shutdown mode by default (saves power)
  return lm75bConfigWrite (LM75B_CONFIG_SHUTDOWN_SHUTDOWN);
}

/**************************************************************************/
/*! 
    @brief  Reads the current temperature from the LM75B

    @note   This method will assign a signed 32-bit value (int32) to 'temp',
            where each unit represents +/- 0.125°C.  To convert the numeric
            value to degrees celsius, you must divide the value of 'temp'
            by 8.  This conversion is not done automatically, since you may
            or may not want to use floating point math for the calculations.
*/
/**************************************************************************/
lm75bError_e lm75bGetTemperature (int32_t *temp)
{
  if (!_lm75bInitialised) lm75bInit();

  // Turn device on
  lm75bConfigWrite (LM75B_CONFIG_SHUTDOWN_POWERON);

  // Read temperature
  lm75bError_e error = LM75B_ERROR_OK;
  error = lm75bRead16 (LM75B_REGISTER_TEMPERATURE, temp);

  // Shut device back down
  lm75bConfigWrite (LM75B_CONFIG_SHUTDOWN_SHUTDOWN);

  return error;
}

/**************************************************************************/
/*! 
    @brief  Writes the supplied 8-bit value to the LM75B config register
*/
/**************************************************************************/
lm75bError_e lm75bConfigWrite (uint8_t configValue)
{
  if (!_lm75bInitialised) lm75bInit();

  return lm75bWrite8 (LM75B_REGISTER_CONFIGURATION, configValue);
}
