/**************************************************************************/
/*! 
    @file     mcp4725.c
    @author   K. Townsend (microBuilder.eu)
    
    @brief    Driver for the I2C-based MCP4725 12-Bit DAC.

    @section Example

    @code
    #include "drivers/dac/mcp4725/mcp4725.h"
    ...
    
    mcp4725Init();

    // Set the voltage to 50% of vref and don't save the value in EEPROM
    mcp4725SetVoltage(2048, false);

    // Request the current value from the DAC
    uint8_t status = 0;
    uint16_t value = 0;
    mcp472ReadConfig(&status, &value);

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
#include "mcp4725.h"
#include "core/i2c/i2c.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static bool _mcp4725Initialised = false;

/**************************************************************************/
/*! 
    @brief Initialises I2C for the MCP4725.
*/
/**************************************************************************/
int mcp4725Init()
{
  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    /* Fatal error */
    return -1;
  }

  /* Set initialisation flag */
  _mcp4725Initialised = true;

  return 0;
}

/**************************************************************************/
/*! 
    @brief  Sets the output voltage to a fraction of source vref.  (Value
            can be 0..4095)

    @param[in]  output
                The 12-bit value representing the relationship between
                the DAC's input voltage and its output voltage.
    @param[in]  writeEEPROM
                If this value is true, 'output' will also be written
                to the MCP4725's internal non-volatile memory, meaning
                that the DAC will retain the current voltage output
                after power-down or reset.
*/
/**************************************************************************/
void mcp4725SetVoltage( uint16_t output, bool writeEEPROM )
{
  if (!_mcp4725Initialised) mcp4725Init();

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 4;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = MCP4725_ADDRESS;                 // I2C device address
  if (writeEEPROM)                                      // command and config bits  (C2.C1.C0.x.x.PD1.PD0.x)
  {
    I2CMasterBuffer[1] = (MCP4726_CMD_WRITEDACEEPROM);
  }
  else
  {
    I2CMasterBuffer[1] = (MCP4726_CMD_WRITEDAC);
  }
  I2CMasterBuffer[2] = (output / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
  I2CMasterBuffer[3] = (output % 16) << 4;              // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
  i2cEngine();
}

/**************************************************************************/
/*! 
    @brief  Reads the current configuration and output settings for the
            DAC.

    @param[out] status
                Pointer to hold the contents of the status register
    @param[out] value
                Pointer to hold the output value of the 12-bit DAC
*/
/**************************************************************************/
void mcp472ReadConfig( uint8_t *status, uint16_t *value )
{
  if (!_mcp4725Initialised) mcp4725Init();

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 1;
  I2CReadLength = 3;
  I2CMasterBuffer[0] = MCP4725_ADDRESS | MCP4725_READ;
  i2cEngine();

  // Shift values to create properly formed integers
  *status = I2CSlaveBuffer[0];
  *value = ((I2CSlaveBuffer[1] << 4) | (I2CSlaveBuffer[2] >> 4));
}

