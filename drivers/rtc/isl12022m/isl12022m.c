/**************************************************************************/
/*! 
    @file     isl12022m.c
    @author   K. Townsend (microBuilder.eu)
	
    @brief    Drivers for the ISL12022M RTC

    @section DESCRIPTION

    The ISL12022M is an I2C RTC with 128 bytes battery-backed up SRAM.

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
#include "isl12022m.h"
#include "core/systick/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static bool _isl12022mInitialised = false;

/**************************************************************************/
/*! 
    @brief  Standard decimal to binary coded decimal
*/
/**************************************************************************/
uint8_t isl12022mDecToBCD(uint8_t val)
{
  return ( (val/10*16) + (val%10) );
}

/**************************************************************************/
/*! 
    @brief  Binary coded decimal to standard decimal
*/
/**************************************************************************/
uint8_t isl12022mBCDToDec(uint8_t val)
{
  return ( (val/16*10) + (val%16) );
}
   
/**************************************************************************/
/*! 
    @brief  Writes an 8 bit value
*/
/**************************************************************************/
isl12022mError_t isl12022mWrite8 (uint8_t address, uint8_t reg, uint32_t value)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 3;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = address;
  I2CMasterBuffer[1] = reg;                       // Command register
  I2CMasterBuffer[2] = (value & 0xFF);            // Value to write
  i2cEngine();
  return ISL12022M_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Reads x bytes into a buffer
*/
/**************************************************************************/
isl12022mError_t isl12022mReadBuffer(uint8_t address, uint8_t reg, uint8_t *buffer, uint32_t len)
{
  if (len > I2C_BUFSIZE)
    return ISL12022M_ERROR_I2C_BUFFEROVERFLOW;

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = len;
  I2CMasterBuffer[0] = address;
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = address | ISL12022M_READBIT;  
  i2cEngine();

  // Push response into buffer
  for ( i = 0; i < len; i++ )
  {
    buffer[i] = I2CSlaveBuffer[i];
  }

  return ISL12022M_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
isl12022mError_t isl12022mInit(void)
{
  isl12022mError_t error = ISL12022M_ERROR_OK;
  uint8_t buffer[1];

  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return ISL12022M_ERROR_I2C_INIT;    /* Fatal error */
  }
  
  // Make sure write is enabled on the ISL12202M (factory default = disabled)
  error = isl12022mReadBuffer(ISL12022M_RTC_ADDRESS, ISL12022M_REG_CSR_INT, buffer, sizeof(buffer));
  if (!error)
  {
    if (!(buffer[0] & ISL12022M_INT_WRITEENABLE))
    {
      // Write is not enabled on the RTC ... enable it now
      error = isl12022mWrite8(ISL12022M_RTC_ADDRESS, ISL12022M_REG_CSR_INT, buffer[0] | ISL12022M_INT_WRITEENABLE);
    }
    _isl12022mInitialised = true;
  }

  return error;
}

/**************************************************************************/
/*! 
    @brief  Gets the current date/time from the RTC

    @section EXAMPLE

    @code
    #include "drivers/rtc/isl12022m.h"
    ...
    isl12022mInit();

    // Set the time to 12 June 2011 @ 11:59:30
    isl12022mSetTime(0, 11, 6, 12, 11, 59, 30);

    // Wait 5 seconds
    systickDelay(5000);

    // Display the current time
    isl12022mTime_t time;
    isl12022mGetTime(&time);
    printf("DW:%d, Y:%d, M:%d, D:%d, H:%d, M:%d, S:%d\r\n", 
            time.dayofweek, 
            time.year, 
            time.month, 
            time.day, 
            time.hour, 
            time.minute, 
            time.second);
    @endcode
*/
/**************************************************************************/
isl12022mError_t isl12022mGetTime(isl12022mTime_t *time)
{
  isl12022mError_t error = ISL12022M_ERROR_OK;
  uint8_t buffer[9];

  if (!_isl12022mInitialised)
  {
    error = isl12022mInit();
    if (error) return error;
  }

  // Read 9 bytes at once into buffer
  error = isl12022mReadBuffer(ISL12022M_RTC_ADDRESS, 
                              ISL12022M_REG_RTC_SC, 
                              buffer, sizeof(buffer));

  if (!error)
  {
    // Check status register
    if (buffer[7] & (ISL12022M_STATUS_LOWBATT85 | ISL12022M_STATUS_LOWBATT75))
    {
      // Set low battery flag to indicate that the RTC value may not be accurate
      error = ISL12022M_ERROR_RTC_LOWBATT;
    }
  
    time->second = isl12022mBCDToDec(buffer[0]);
    time->minute = isl12022mBCDToDec(buffer[1]);
    time->hour = buffer[2] & 0x1F; // 0x3F;
    time->day = buffer[3] & 0x3F;
    time->month = buffer[4] & 0x1F;
    time->year = buffer[5];
    time->dayofweek = buffer[6] & 0x07;
    time->status = buffer[7];
    time->interrupt = buffer[8];
  }

  return error;
}

/**************************************************************************/
/*! 
    @brief  Sets the current date/time from the RTC

    @section EXAMPLE

    @code
    #include "drivers/rtc/isl12022m.h"
    ...
    isl12022mInit();

    // Set the time to 12 June 2011 @ 11:59:30
    isl12022mSetTime(0, 11, 6, 12, 11, 59, 30);

    // Wait 5 seconds
    systickDelay(5000);

    // Display the current time
    isl12022mTime_t time;
    isl12022mGetTime(&time);
    printf("DW:%d, Y:%d, M:%d, D:%d, H:%d, M:%d, S:%d\r\n", 
            time.dayofweek, 
            time.year, 
            time.month, 
            time.day, 
            time.hour, 
            time.minute, 
            time.second);
    @endcode
*/
/**************************************************************************/
isl12022mError_t isl12022mSetTime(uint8_t dayofweek, uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
  isl12022mError_t error = ISL12022M_ERROR_OK;

  if (!_isl12022mInitialised)
  {
    error = isl12022mInit();
    if (error) return error;
  }

  error = isl12022mWrite8(ISL12022M_RTC_ADDRESS, ISL12022M_REG_RTC_SC, isl12022mDecToBCD(second));
  if (error) return error;
  error = isl12022mWrite8(ISL12022M_RTC_ADDRESS, ISL12022M_REG_RTC_MN, isl12022mDecToBCD(minute));
  if (error) return error;
  // Always append military flag to hour (24 hour only)
  error = isl12022mWrite8(ISL12022M_RTC_ADDRESS, ISL12022M_REG_RTC_HR, hour & 0x3F) | ISL12022M_HR_MILITARY;
  if (error) return error;
  error = isl12022mWrite8(ISL12022M_RTC_ADDRESS, ISL12022M_REG_RTC_DT, day & 0x3F);
  if (error) return error;
  error = isl12022mWrite8(ISL12022M_RTC_ADDRESS, ISL12022M_REG_RTC_MO, month & 0x1F);
  if (error) return error;
  error = isl12022mWrite8(ISL12022M_RTC_ADDRESS, ISL12022M_REG_RTC_YR, year);
  if (error) return error;
  error = isl12022mWrite8(ISL12022M_RTC_ADDRESS, ISL12022M_REG_RTC_DW, dayofweek & 0x07);
  if (error) return error;

  return error;
}

/**************************************************************************/
/*! 
    @brief  Reads the current temperature from the ISL12022

    @section EXAMPLE

    @code
    #include "drivers/rtc/isl12022m.h"
    ...
    isl12022mInit();

    uint32_t temperature;
    isl12022mGetTemp(&temperature);
    printf("Temperature: %u C\r\n", temperature);

    @endcode
*/
/**************************************************************************/
isl12022mError_t isl12022mGetTemp(uint8_t *celsius)
{
  isl12022mError_t error = ISL12022M_ERROR_OK;
  uint8_t buffer[2];
  uint32_t temp;

  if (!_isl12022mInitialised)
  {
    error = isl12022mInit();
    if (error) return error;
  }

  // Enable temperature sensing if required
  error = isl12022mReadBuffer(ISL12022M_RTC_ADDRESS, ISL12022M_REG_CSR_BETA, buffer, 1);
  if (!error)
  {
    if (!(buffer[0] & ISL12022M_BETA_TEMPENABLE))
    {
      // Temp sensor is not enabled ... enable it now
      error = isl12022mWrite8(ISL12022M_RTC_ADDRESS, ISL12022M_REG_CSR_BETA, buffer[0] | ISL12022M_BETA_TEMPENABLE);
      if (error)
        return error;
    }
  }

  // Wait 100ms for conversion to complete
  systickDelay(100);
  // Read low and high temp bytes (0x28 and 0x29)
  error = isl12022mReadBuffer(ISL12022M_RTC_ADDRESS, ISL12022M_REG_TEMP_TKOL, buffer, 2);
  if (error)
    return error;
  // Convert value to degrees celsius (value/2 - 273 = degrees C)
  temp = ((buffer[0]) | (buffer[1] << 8)) / 2 - 273;
  *celsius = (uint8_t)temp & 0xFF;

  return error;
}
