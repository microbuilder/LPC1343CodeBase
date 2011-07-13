/**************************************************************************/
/*! 
    @file     tcs3414.c
    @author   K. Townsend (microBuilder.eu)
              Morten Hjerde (tcs3414CalculateCCT)
	
    @brief    Drivers for the TAOS TCS3414 I2C RGB sensor

    @section  DESCRIPTION

    The TAOS TCS3414 is a digital color/light sensor that can be used
    to derive the color chromaticity and illuminance of ambient light 
    with 16-bit resolution.  The device has an array of filtered
    photo-diodes with 4 red, 4 green, 4 blue and 4 unfiltered
    captors.  The sensor has digital gain and prescalar support so that
    the sensitivty of the device can be dynamically adjusted with a
    1,000,000:1 dynamic range.

    @section  LICENSE

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
#include "tcs3414.h"
#include "core/systick/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static bool _tcs3414Initialised = false;

/**************************************************************************/
/*! 
    @brief  Sends a single command byte over I2C
*/
/**************************************************************************/
tcs3414Error_e tcs3414WriteCmd (uint8_t cmd)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = TCS3414_ADDRESS;       // I2C device address
  I2CMasterBuffer[1] = cmd;                   // Command register
  i2cEngine();
  return TCS3414_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Writes an 8 bit values over I2C
*/
/**************************************************************************/
tcs3414Error_e tcs3414Write8 (uint8_t reg, uint32_t value)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 3;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = TCS3414_ADDRESS;           // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  I2CMasterBuffer[2] = (value & 0xFF);            // Value to write
  i2cEngine();
  return TCS3414_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Reads a 16 bit values over I2C
*/
/**************************************************************************/
tcs3414Error_e tcs3414Read16(uint8_t reg, uint16_t *value)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 2;
  I2CMasterBuffer[0] = TCS3414_ADDRESS;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = TCS3414_ADDRESS | TCS3414_READBIT;  
  i2cEngine();

  // Shift values to create properly formed integer (low byte first)
  *value = (I2CSlaveBuffer[0] | (I2CSlaveBuffer[1] << 8));

  return TCS3414_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
tcs3414Error_e tcs3414Init(void)
{
  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return TCS3414_ERROR_I2CINIT;    /* Fatal error */
  }
  
  _tcs3414Initialised = true;

  // Note: by default, the device is in power down mode on bootup

  return TCS3414_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Sets the gain and prescalar to control sensitivty
*/
/**************************************************************************/
tcs3414Error_e tcs3414SetSensitivity(tcs3414Gain_t gain, tcs3414Prescalar_t prescalar)
{
  if (!_tcs3414Initialised) tcs3414Init();

  tcs3414Error_e error = TCS3414_ERROR_OK;

  // Enable the device by setting the control bit to 0x03 (power + ADC on)
  error = tcs3414Write8(TCS3414_COMMAND_BIT | TCS3414_REGISTER_CONTROL, TCS3414_CONTROL_POWERON);
  if (error) return error;  

  // Set the gani and prescalar values using the GAIN register
  error = tcs3414Write8(TCS3414_COMMAND_BIT | TCS3414_REGISTER_GAIN, gain | prescalar);
  if (error) return error;  

  // Turn the device off to save power
  error = tcs3414Write8(TCS3414_COMMAND_BIT | TCS3414_REGISTER_CONTROL, TCS3414_CONTROL_POWEROFF);
  if (error) return error;  

  return error;
}

/**************************************************************************/
/*! 
    @brief  Reads the RGB and clear luminosity from the TCS3414
*/
/**************************************************************************/
tcs3414Error_e tcs3414GetRGBL(uint16_t *red, uint16_t *green, uint16_t *blue, uint16_t *clear)
{
  if (!_tcs3414Initialised) tcs3414Init();

  tcs3414Error_e error = TCS3414_ERROR_OK;

  // Enable the device by setting the control bit to 0x03 (power + ADC on)
  error = tcs3414Write8(TCS3414_COMMAND_BIT | TCS3414_REGISTER_CONTROL, TCS3414_CONTROL_POWERON);
  if (error) return error;  

  // Wait >12ms for ADC to complete
  systickDelay(13);

  // Reads two byte red value
  error = tcs3414Read16(TCS3414_COMMAND_BIT | TCS3414_WORD_BIT | TCS3414_REGISTER_REDLOW, red);
  if (error) return error;

  // Reads two byte green value
  error = tcs3414Read16(TCS3414_COMMAND_BIT | TCS3414_WORD_BIT | TCS3414_REGISTER_GREENLOW, green);
  if (error) return error;

  // Reads two byte blue value
  error = tcs3414Read16(TCS3414_COMMAND_BIT | TCS3414_WORD_BIT | TCS3414_REGISTER_BLUELOW, blue);
  if (error) return error;

  // Reads two byte clear value
  error = tcs3414Read16(TCS3414_COMMAND_BIT | TCS3414_WORD_BIT | TCS3414_REGISTER_CLEARLOW, clear);
  if (error) return error;

  // Turn the device off to save power
  error = tcs3414Write8(TCS3414_COMMAND_BIT | TCS3414_REGISTER_CONTROL, TCS3414_CONTROL_POWEROFF);
  if (error) return error;  

  return error;
}

/**************************************************************************/
/*! 
    @brief    Reads the RGB values from the TCS3414 color sensor and
              calculates CCT (Correlated Color Temperature)

    @return   The Correlated Color Temperature in Kelvin
*/
/**************************************************************************/
uint32_t tcs3414CalculateCCT (uint16_t red, uint16_t green, uint16_t blue)
{
    volatile float R;
    volatile float G;
    volatile float B;
    volatile float X;
    volatile float Y;
    volatile float Z;
    volatile float x;
    volatile float y;
    volatile float n;
    volatile float CCT;

    // Convert RGB values to a 0-100 scale
    R = (((float) red) / 65536) * 100;
    G = (((float) green) / 65536) * 100;
    B = (((float) blue)/ 65536) * 100;

    //do matrix transformation
    X = (-0.14282 * R) + (1.54924 * G) + (-0.95641 * B);
    Y = (-0.32466 * R) + (1.57837 * G) + (-0.73191 * B);
    Z = (-0.68202 * R) + (0.77073 * G) + (0.56332 * B);

    //calc chromaticity coordinates
    x = (X)/(X + Y + Z);
    y = (Y)/(X + Y + Z);

    //use McCamy’s formula to get CCT:
    n = (x - 0.3320) / (0.1858 - y);
    CCT =  (449 * n *  n * n);    // we don't have pow
    CCT += (3525 * n * n);
    CCT += (6823.3 * n);
    CCT += 5520.33;

    return ((uint32_t) CCT);
}
