/**************************************************************************/
/*! 
    @file     ST7565.c
    @author   K. Townsend (microBuilder.eu)

    @section DESCRIPTION

    Driver for 128x64 pixel display based on the ST7565 LCD controller.

    This driver is based on the ST7565 Library from Limor Fried
    (Adafruit Industries) at: http://github.com/adafruit/ST7565-LCD/    
    
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
#include <string.h>

#include "st7565.h"

#include "core/gpio/gpio.h"
#include "core/systick/systick.h"
#include "drivers/lcd/smallfonts.h"

void sendByte(uint8_t byte);

#define CMD(c)        do { gpioSetValue( ST7565_A0_PORT, ST7565_A0_PIN, 0 ); sendByte( c ); } while (0);
#define DATA(d)       do { gpioSetValue( ST7565_A0_PORT, ST7565_A0_PIN, 1 ); sendByte( d ); } while (0);
#define DELAY(mS)     do { systickDelay( mS / CFG_SYSTICK_DELAY_IN_MS ); } while(0);

uint8_t _st7565buffer[128*64/8];

/**************************************************************************/
/* Private Methods                                                        */
/**************************************************************************/

/**************************************************************************/
/*! 
    @brief Renders the buffer contents

    @param[in]  buffer
                Pointer to the buffer containing the raw pixel data
*/
/**************************************************************************/
void writeBuffer(uint8_t *buffer) 
{
  uint8_t c, p;
  int pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };

  for(p = 0; p < 8; p++) 
  {
    CMD(ST7565_CMD_SET_PAGE | pagemap[p]);
    CMD(ST7565_CMD_SET_COLUMN_LOWER | (0x0 & 0xf));
    CMD(ST7565_CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));
    CMD(ST7565_CMD_RMW);
    DATA(0xff);
    
    for(c = 0; c < 128; c++) 
    {
      DATA(buffer[(128*p)+c]);
    }
  }
}

/**************************************************************************/
/*! 
    @brief Simulates an SPI write using GPIO

    @param[in]  byte
                The byte to send
*/
/**************************************************************************/
void sendByte(uint8_t byte)
{
  int8_t i;

  // Note: This code can be optimised to avoid the branches by setting
  // GPIO registers directly, but we'll leave it as is for the moment
  // for simplicity sake

  // Make sure clock pin starts high
  gpioSetValue(ST7565_SCLK_PORT, ST7565_SCLK_PIN, 1);

  // Write from MSB to LSB
  for (i=7; i>=0; i--) 
  {
    // Set clock pin low
    gpioSetValue(ST7565_SCLK_PORT, ST7565_SCLK_PIN, 0);
    // Set data pin high or low depending on the value of the current bit
    gpioSetValue(ST7565_SDAT_PORT, ST7565_SDAT_PIN, byte & (1 << i) ? 1 : 0);
    // Set clock pin high
    gpioSetValue(ST7565_SCLK_PORT, ST7565_SCLK_PIN, 1);
  }
}

/**************************************************************************/
/*!
    @brief  Draws a single graphic character using the supplied font
*/
/**************************************************************************/
void drawChar(uint16_t x, uint16_t y, uint8_t c, struct FONT_DEF font)
{
  uint8_t col, column[font.u8Width];

  // Check if the requested character is available
  if ((c >= font.u8FirstChar) && (c <= font.u8LastChar))
  {
    // Retrieve appropriate columns from font data
    for (col = 0; col < font.u8Width; col++)
    {
      column[col] = font.au8FontTable[((c - 32) * font.u8Width) + col];    // Get first column of appropriate character
    }
  }
  else
  {    
    // Requested character is not available in this font ... send a space instead
    for (col = 0; col < font.u8Width; col++)
    {
      column[col] = 0xFF;    // Send solid space
    }
  }

  // Render each column
  uint16_t xoffset, yoffset;
  for (xoffset = 0; xoffset < font.u8Width; xoffset++)
  {
    for (yoffset = 0; yoffset < (font.u8Height + 1); yoffset++)
    {
      uint8_t bit = 0x00;
      bit = (column[xoffset] << (8 - (yoffset + 1)));     // Shift current row bit left
      bit = (bit >> 7);                     // Shift current row but right (results in 0x01 for black, and 0x00 for white)
      if (bit)
      {
        st7565DrawPixel(x + xoffset, y + yoffset);
      }
    }
  }
}

/**************************************************************************/
/* Public Methods                                                         */
/**************************************************************************/

/**************************************************************************/
/*! 
    @brief Initialises the ST7565 LCD display
*/
/**************************************************************************/
void st7565Init(void)
{
  // Note: This can be optimised to set all pins to output and high
  // in two commands by manipulating the registers directly (assuming
  // that the pins are located in the same GPIO bank).  The code is left
  // as is for clarity sake in case the pins are not all located in the
  // same bank.

  // Set clock pin to output and high
  gpioSetDir(ST7565_SCLK_PORT, ST7565_SCLK_PIN, 1);
  gpioSetValue(ST7565_SCLK_PORT, ST7565_SCLK_PIN, 1);

  // Set data pin to output and high
  gpioSetDir(ST7565_SDAT_PORT, ST7565_SDAT_PIN, 1);
  gpioSetValue(ST7565_SDAT_PORT, ST7565_SDAT_PIN, 1);

  // Configure backlight pin to output and set high (off)
  gpioSetDir(ST7565_BL_PORT, ST7565_BL_PIN, 1);
  gpioSetValue(ST7565_BL_PORT, ST7565_BL_PIN, 1);

  // Configure A0 pin to output and set high
  gpioSetDir(ST7565_A0_PORT, ST7565_A0_PIN, 1);
  gpioSetValue(ST7565_A0_PORT, ST7565_A0_PIN, 1);

  // Configure Reset pin and set high
  gpioSetDir(ST7565_RST_PORT, ST7565_RST_PIN, 1);
  gpioSetValue(ST7565_RST_PORT, ST7565_RST_PIN, 1);

  // Configure select pin and set high
  gpioSetDir(ST7565_CS_PORT, ST7565_CS_PIN, 1);
  gpioSetValue(ST7565_CS_PORT, ST7565_CS_PIN, 1);

  // Reset
  gpioSetValue(ST7565_CS_PORT, ST7565_CS_PIN, 0);     // Set CS low
  gpioSetValue(ST7565_RST_PORT, ST7565_RST_PIN, 0);   // Set reset low
  DELAY(500 / CFG_SYSTICK_DELAY_IN_MS);               // Wait 500mS
  gpioSetValue(ST7565_RST_PORT, ST7565_RST_PIN, 1);   // Set reset high

  // Configure Display
  CMD(ST7565_CMD_SET_BIAS_7);                         // LCD Bias Select
  CMD(ST7565_CMD_SET_ADC_NORMAL);                     // ADC Select
  CMD(ST7565_CMD_SET_COM_NORMAL);                     // SHL Select
  CMD(ST7565_CMD_SET_DISP_START_LINE);                // Initial Display Line
  CMD(ST7565_CMD_SET_POWER_CONTROL | 0x04);           // Turn on voltage converter (VC=1, VR=0, VF=0)
  DELAY(50 / CFG_SYSTICK_DELAY_IN_MS);                // Wait 50mS
  CMD(ST7565_CMD_SET_POWER_CONTROL | 0x06);           // Turn on voltage regulator (VC=1, VR=1, VF=0)
  DELAY(50 / CFG_SYSTICK_DELAY_IN_MS);                // Wait 50mS
  CMD(ST7565_CMD_SET_POWER_CONTROL | 0x07);           // Turn on voltage follower
  DELAY(10 / CFG_SYSTICK_DELAY_IN_MS);                // Wait 10mS
  CMD(ST7565_CMD_SET_RESISTOR_RATIO | 0x6);           // Set LCD operating voltage

  // Turn display on
  CMD(ST7565_CMD_DISPLAY_ON);
  CMD(ST7565_CMD_SET_ALLPTS_NORMAL);
  st7565SetBrightness(0x18);
}

/**************************************************************************/
/*! 
    @brief Enables or disables the backlight
*/
/**************************************************************************/
void st7565Backlight(bool state)
{
  gpioSetValue( ST7565_BL_PORT, ST7565_BL_PIN, state ? 0 : 1 );
}

/**************************************************************************/
/*! 
    @brief Sets the display brightness
*/
/**************************************************************************/
void st7565SetBrightness(uint8_t val)
{
  CMD(ST7565_CMD_SET_VOLUME_FIRST);
  CMD(ST7565_CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

/**************************************************************************/
/*! 
    @brief Clears the screen
*/
/**************************************************************************/
void st7565ClearScreen(void) 
{
  memset(&_st7565buffer, 0x00, 128*64/8);
}

/**************************************************************************/
/*! 
    @brief Renders the contents of the pixel buffer on the LCD
*/
/**************************************************************************/
void st7565Refresh(void)
{
  writeBuffer(_st7565buffer);
}

/**************************************************************************/
/*! 
    @brief Draws a single pixel in image buffer

    @param[in]  x
                The x position (0..127)
    @param[in]  y
                The y position (0..63)
*/
/**************************************************************************/
void st7565DrawPixel(uint8_t x, uint8_t y) 
{
  if ((x >= 128) || (y >= 64))
    return;

  // x is which column
  _st7565buffer[x+ (y/8)*128] |= (1 << (7-(y%8)));
}

/**************************************************************************/
/*! 
    @brief Clears a single pixel in image buffer

    @param[in]  x
                The x position (0..127)
    @param[in]  y
                The y position (0..63)
*/
/**************************************************************************/
void st7565ClearPixel(uint8_t x, uint8_t y)
{
  if ((x >= 128) || (y >= 64))
    return;

  // x is which column
  _st7565buffer[x+ (y/8)*128] &= ~(1 << (7-(y%8)));
}

/**************************************************************************/
/*! 
    @brief Gets the value (1 or 0) of the specified pixel from the buffer

    @param[in]  x
                The x position (0..127)
    @param[in]  y
                The y position (0..63)

    @return     1 if the pixel is enabled, 0 if disabled
*/
/**************************************************************************/
uint8_t st7565GetPixel(uint8_t x, uint8_t y)
{
  if ((x >= 128) || (y >= 64)) return 0;
  return _st7565buffer[x+ (y/8)*128] & (1 << (7-(y%8)));
}

/**************************************************************************/
/*!
    @brief  Draws a string using the supplied font data.

    @param[in]  x
                Starting x co-ordinate
    @param[in]  y
                Starting y co-ordinate
    @param[in]  text
                The string to render
    @param[in]  font
                Pointer to the FONT_DEF to use when drawing the string

    @section Example

    @code 

    #include "drivers/lcd/bitmap/st7565/st7565.h"
    #include "drivers/lcd/smallfonts.h"
    
    // Configure the pins and initialise the LCD screen
    st7565Init();

    // Enable the backlight
    st7565BLEnable();

    // Render some text on the screen with different fonts
    st7565DrawString(1, 1, "3X6 SYSTEM", Font_System3x6);   // 3x6 is UPPER CASE only
    st7565DrawString(1, 10, "5x8 System", Font_System5x8);
    st7565DrawString(1, 20, "7x8 System", Font_System7x8);

    // Refresh the screen to see the results
    st7565Refresh();

    @endcode
*/
/**************************************************************************/
void st7565DrawString(uint16_t x, uint16_t y, char* text, struct FONT_DEF font)
{
  uint8_t l;
  for (l = 0; l < strlen(text); l++)
  {
    drawChar(x + (l * (font.u8Width + 1)), y, text[l], font);
  }
}

/**************************************************************************/
/*!
    @brief  Shifts the contents of the frame buffer up the specified
            number of pixels

    @param[in]  height
                The number of pixels to shift the frame buffer up, leaving
                a blank space at the bottom of the frame buffer x pixels
                high

    @section Example

    @code 

    #include "drivers/lcd/bitmap/st7565/st7565.h"
    #include "drivers/lcd/smallfonts.h"
    
    // Configure the pins and initialise the LCD screen
    st7565Init();

    // Enable the backlight
    st7565BLEnable();

    // Continually write some text, scrolling upward one line each time
    while (1)
    {
      // Shift the buffer up 8 pixels (adjust for font-height)
      st7565ShiftFrameBuffer(8);
      // Render some text on the screen with different fonts
      st7565DrawString(1, 56, "INSERT TEXT HERE", Font_System3x6);   // 3x6 is UPPER CASE only
      // Refresh the screen to see the results
      st7565Refresh();    
      // Wait a bit before writing the next line
      systickDelay(1000);
    }

    @endcode
*/
/**************************************************************************/
void st7565ShiftFrameBuffer( uint8_t height )
{
  if (height == 0) return;
  if (height >= 64)
  {
    // Clear the entire frame buffer
    st7565ClearScreen();
    return;
  }

  // This is horribly inefficient, but at least easy to understand
  // In a production environment, this should be significantly optimised

  uint8_t y, x;
  for (y = 0; y < 64; y++)
  {
    for (x = 0; x < 128; x++)
    {
      if (63 - y > height)
      {
        // Shift height from further ahead in the buffer
        st7565GetPixel(x, y + height) ? st7565DrawPixel(x, y) : st7565ClearPixel(x, y);
      }
      else
      {
        // Clear the entire line
        st7565ClearPixel(x, y);
      }
    }
  }
}

