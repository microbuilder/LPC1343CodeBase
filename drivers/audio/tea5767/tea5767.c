/**************************************************************************/
/*! 
    @file     tea5767.c
    @author   K. Townsend
    
    @brief    Driver for the TEA5767 FM receiver.

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
#include "tea5767.h"
#include "core/i2c/i2c.h"
#include "core/systick/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];

static bool _tea5767Initialised = false;

/**************************************************************************/
/*! 
    @brief Sends 5 bytes over I2C
*/
/**************************************************************************/
void tea5767SendData(uint8_t * bytes)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 6;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = TEA5767_ADDRESS;
  I2CMasterBuffer[1] = bytes[0];
  I2CMasterBuffer[2] = bytes[1];
  I2CMasterBuffer[3] = bytes[2];
  I2CMasterBuffer[4] = bytes[3];
  I2CMasterBuffer[5] = bytes[4];
  i2cEngine();
}

/**************************************************************************/
/*! 
    @brief Reads 5 bytes over I2C
*/
/**************************************************************************/
void tea5767ReadData(uint8_t * bytes)
{
  // Clear buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
    I2CSlaveBuffer[i] = 0x00;
  }

  I2CWriteLength = 1;
  I2CReadLength = 5;
  I2CMasterBuffer[0] = TEA5767_ADDRESS | TEA5767_READ;
  i2cEngine();

  bytes[0] = I2CSlaveBuffer[0];
  bytes[1] = I2CSlaveBuffer[1];
  bytes[2] = I2CSlaveBuffer[2];
  bytes[3] = I2CSlaveBuffer[3];
  bytes[4] = I2CSlaveBuffer[4];
}

/**************************************************************************/
/*! 
    @brief   This is a test function to evaluate the quality of crystals
             used on 3rd party modules.  Most modules use cheap 32.768kHz 
             crystals which are prone to de-tuning.  Checking the IF bits
             at 81.4MHz can check if the crystal is problematic or not.

    @returns True (1) if the IF bits are equal to 0x37 (good), otherwise
             false (0).  The device may still function if the IF values
             are slightly off, but auto-scan and tuning will likely be
             less reliable.
*/
/**************************************************************************/
bool tea5767CheckCrystal(void)
{
  /*  AN10133 (p.38) states:
  
      The choice of 32768Hz reference frequency makes it possible to use a
      cheap 32.768kHz watch crystal. A drawback of these clocks is that they
      have a very high second order temperature coefficient. This may result
      in de-tuning the radio or a search action may fail.
      
      Care should be taken when using this crystal. The accuracy of the
      32768Hz crystal can be checked by tuning the radio to 81.4 MHz
      with high/low side injection and reading the IF via the bus. The IF
      must be 37Hex.

      An other issue when using this crystal is the grid position. It is
      not possible to create a 100kHz grid position, but 98.304kHz
      (3*32768Hz). This should not be a problem if this is resolved in
      software.
      
      The motional capacitance of the 32768Hz crystal should be between
      1.5fF and 3fF. Shunt capacitance must be max 3.5pF. The series
      resistance should not exceed 75KOhm.
      
      Further, the frequency accuracy of this crystal must not exceed ±20
      ppm, while the temperature drift should be in the order of ±50 ppm
      over a temperature range of –10oC to +60oC.
  */

  uint8_t ifValue = 0;
  uint8_t buffer[5] = { 0, 0, 0, 0, 0 };

  // Set the frequency to 81.4MHz
  tea5767SetFrequency(81400000);
  systickDelay(100);

  // Read back the IF bits
  tea5767ReadData(&buffer[0]);
  ifValue = buffer[2] & 0x7F;

  // Reconfigure the chip to a known frequency to avoid user problems
  // tea5767SetFrequency(TEA5767_FMBANDSTART_JAPAN);
  tea5767SetFrequency(TEA5767_FMBANDSTART_US_EUROPE);

  // Return true if the crystal is OK (IF = 0x37 @ 81.4MHz),
  // false if it's something else
  if (0x37 == ifValue)
    return true;
  else
    return false;
}

/**************************************************************************/
/*! 
    @brief Initialises I2C for the TEA5767.
*/
/**************************************************************************/
uint32_t tea5767Init()
{
  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    /* Fatal error */
    return -1;
  }

  /* Set initialisation flag */
  _tea5767Initialised = true;

  // Set the frequency to a known value to make sure the PLL is properly configured 
  // tea5767SetFrequency(TEA5767_FMBANDSTART_JAPAN);
  tea5767SetFrequency(TEA5767_FMBANDSTART_US_EUROPE);

  return 0;
}

/**************************************************************************/
/*! 
    @brief  Sets the frequency to the specified value in Hz

    @param[in]  frequency
                The frequency in Hz, meaning that 95.7MHz = 97,500,000 
                101.4 MHz = 101,400,000 etc.
*/
/**************************************************************************/
void tea5767SetFrequency( uint32_t frequency )
{
  uint32_t pllValue;
  uint8_t buffer[5] = { 0, 0, 0, 0, 0 };
  
  // Make sure I2C is initialised
  if (!_tea5767Initialised) tea5767Init();

  // Calculate PLL word for high side injection mode
  // NDEC = (4*(FRF + FIF)) / FREFS
  // where:
  // FRF = Desired tuning frequency in Hz
  // FIF = Intermediate frequency in Hz (225kHz)
  // FREFS = Reference frequency in Hz (32.768kHz)
  pllValue = (4 * (frequency + 225000)) / 32768;
  
  buffer[0] = (pllValue >> 8) & 0x3F;              // Upper 6 PLL bits (also turns mute and search mode off!)
  buffer[1] = (pllValue & 0xFF);                   // Lower 8 PLL bits
  buffer[2] = TEA5767_WBYTE3_HLSI;                 // High side injection mode
  buffer[3] = TEA5767_WBYTE4_XTAL;                 // XTAL bit = 1 for 32.768kHz crystal
  buffer[4] = 0;                                   // PLLREF bit = 0 for 32.768kHz crystal

  // Send data over I2C
  tea5767SendData(buffer);
}

/**************************************************************************/
/*! 
    @brief  Returns the current frequency in Hz (meaning 97.5MHz will be
            returned as 97,500,000 etc.
*/
/**************************************************************************/
uint32_t  tea5767GetFrequency( void )
{
  uint32_t frequency = 0;
  uint8_t buffer[5] = { 0, 0, 0, 0, 0 };

  if (!_tea5767Initialised) tea5767Init();

  tea5767ReadData(&buffer[0]);

  // Retrieve the raw 14-bit PLL value from word 1 and 2
  frequency = ((buffer[0] & 0x3F) << 8) + buffer[1];
  // Determine the current frequency using the same high side formula as above
  frequency = frequency * 32768 / 4 - 225000;

  return frequency;
}

/**************************************************************************/
/*! 
    @brief  Starts the auto-scan process from the current frequency

    @param[in]  scanDirection
                Set this to 0 to scan down, or one to scan up, starting
                at the current frequency.
*/
/**************************************************************************/
void tea5767Scan( uint8_t scanDirection )
{
  uint8_t rbuffer[5] = { 0, 0, 0, 0, 0 };
  uint8_t wbuffer[5] = { 0, 0, 0, 0, 0 };
  
  // Make sure I2C is initialised
  if (!_tea5767Initialised) tea5767Init();

  // First we need to get the current PLL word to know where to start from
  tea5767ReadData(&rbuffer[0]);

  // Set the PLL value again and append the search enable bu
  wbuffer[0] = TEA5767_WBYTE1_SEARCHMODE +          // Search mode enabled
               (rbuffer[0] & 0x3F);                 // Upper PLL bits
  wbuffer[1] = rbuffer[1];                          // Lower PLL bits
  wbuffer[2] = TEA5767_WBYTE3_SEARCHSTOPLEVEL_MID | // Mid level ADC for search steps
               TEA5767_WBYTE3_HLSI;                 // HLSI (must be 1 for PLL)
  wbuffer[3] = TEA5767_WBYTE4_XTAL;                 // Must be 1 for 32.768kHz crystal
  wbuffer[4] = 0x00;

  // Set the scan direction bit to 1 (scan up) if scanDirection is non-zero
  if (scanDirection) wbuffer[2] |= TEA5767_WBYTE3_SEARCHUPDOWN;

  // Send data over I2C
  tea5767SendData(wbuffer);
}
