/**************************************************************************/
/*! 
    @file     sharpmem.c
    @author   K. Townsend (microBuilder.eu)

    @section  DESCRIPTION

    Driver for Sharp Memory Displays.
    
    This driver uses a bit-banged 3-pin SPI interface.  For the SPI
    interface, the select line (CS) is active high, and the clock 
    line (SCK) is active high.  
    
    Note: The LCD expects the address and data
    bits to arrive LSB, though the M3 shifts the bits out MSB so they
    need to be switched in SW first.

    @section  EXAMPLE
    @code

    sharpmemInit();
    sharpmemEnable(true);
  
    // Render some text and a line into the image buffer
    uint32_t i;
    sharpmemDrawString(1, 10, "5x8 System", Font_System5x8);
    sharpmemDrawString(1, 20, "7x8 System", Font_System7x8);
    for (i = 0; i < 96; i++)
    {
      sharpmemDrawPixel(i, i);
    }
  
    while(1)
    {
      // Screen must be refreshed at least once per second
      sharpmemRefresh();
      systickDelay(1000);
    }

    @endcode

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
#include <string.h>

#include "sharpmem.h"

#include "core/systick/systick.h"
#include "core/gpio/gpio.h"
#include "drivers/lcd/smallfonts.h"

#define TOGGLE_VCOM   do { _sharpmem_vcom = _sharpmem_vcom ? 0x00 : SHARPMEM_BIT_VCOM; } while(0);

static uint8_t _sharpmembuffer[(SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8];
static volatile uint8_t _sharpmem_vcom = SHARPMEM_BIT_VCOM;

/*************************************************/
/* Private Methods                               */
/*************************************************/

/**************************************************************************/
/*!
    @brief  Swaps the bit order from MSB to LSB, since the LCD expects LSB,
            but the M3 shifts bits out MSB.
*/
/**************************************************************************/
uint8_t sharpmemSwap(uint8_t data)
{
  uint8_t out = 0;
  if (data)
  {
    if(data & 0x01) out |= 0x80;
    if(data & 0x02) out |= 0x40;
    if(data & 0x04) out |= 0x20;
    if(data & 0x08) out |= 0x10;
    if(data & 0x10) out |= 0x08;
    if(data & 0x20) out |= 0x04;
    if(data & 0x40) out |= 0x02;
    if(data & 0x80) out |= 0x01;
  }

  return out;
}

/*************************************************/
void sharpmemSendByte(uint8_t data)
{
  uint8_t i = 0;

  // Make sure clock pin starts low
  CLR_SCLK;

  // Write from MSB to LSB
  // LCD expects LSB first
  for (i=0; i<8; i++) 
  { 
    if (data & 0x80) 
    { 
      SET_MOSI;
    } 
    else 
    { 
      CLR_MOSI;
    } 
    // Clock is active high
    SET_SCLK;
    data <<= 1; 
    __asm volatile("nop");
    CLR_SCLK;
  } 
}

/**************************************************************************/
/*!
    @brief  Draws a single graphic character using the supplied font
*/
/**************************************************************************/
static void sharpmemDrawChar(uint16_t x, uint16_t y, const char c, struct FONT_DEF font)
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
      bit = (bit >> 7);                                   // Shift current row but right (results in 0x01 for black, and 0x00 for white)
      if (bit)
      {
        sharpmemDrawPixel(x + xoffset, y + yoffset);
      }
    }
  }
}

/*************************************************/
/* Public Methods                                */
/*************************************************/

/*************************************************/
void sharpmemInit(void)
{
  // Set control pins to output
  gpioSetDir(SHARPMEM_PORT, SHARPMEM_SCLK_PIN, 1);
  gpioSetDir(SHARPMEM_PORT, SHARPMEM_MOSI_PIN, 1);
  gpioSetDir(SHARPMEM_PORT, SHARPMEM_CS_PIN, 1);
  gpioSetDir(SHARPMEM_PORT, SHARPMEM_DISP_PIN, 1);

  // Set pins to default state
  CLR_SCLK;
  CLR_MOSI;
  CLR_CS;
  CLR_DISP;
  
  // Set the vcom bit to a defined state
  _sharpmem_vcom = SHARPMEM_BIT_VCOM;

  // Clear the display and turn it off by default
  sharpmemClearScreen();
  sharpmemEnable(false);
}

/**************************************************************************/
/*! 
    @brief Turns the display on or off (memory is retained even when the
           display is off)

    @param[in]  enabled
                Whether the display should be on (TRUE/1) or off (FALSE/0)
*/
/**************************************************************************/
void sharpmemEnable(bool enable)
{
  if (enable)
  {
    SET_DISP;
  }
  else
  {
    CLR_DISP;
  }
}

/**************************************************************************/
/*! 
    @brief Draws a single pixel in image buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)
*/
/**************************************************************************/
void sharpmemDrawPixel(uint16_t x, uint16_t y) 
{
  if ((x >= SHARPMEM_LCDWIDTH) || (y >= SHARPMEM_LCDHEIGHT))
    return;

  _sharpmembuffer[(y*SHARPMEM_LCDWIDTH + x) /8] |= (1 << x % 8);
}

/**************************************************************************/
/*! 
    @brief Clears a single pixel in image buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)
*/
/**************************************************************************/
void sharpmemClearPixel(uint16_t x, uint16_t y) 
{
  if ((x >= SHARPMEM_LCDWIDTH) || (y >= SHARPMEM_LCDHEIGHT))
    return;

  _sharpmembuffer[(y*SHARPMEM_LCDWIDTH + x) /8] &= ~(1 << x % 8);
}

/**************************************************************************/
/*! 
    @brief Gets the value (1 or 0) of the specified pixel from the buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)

    @return     1 if the pixel is enabled, 0 if disabled
*/
/**************************************************************************/
uint8_t sharpmemGetPixel(uint16_t x, uint16_t y)
{
  if ((x >=SHARPMEM_LCDWIDTH) || (y >=SHARPMEM_LCDHEIGHT)) return 0;
  return _sharpmembuffer[(y*SHARPMEM_LCDWIDTH + x) /8] & (1 << x % 8) ? 1 : 0;
}

/**************************************************************************/
/*! 
    @brief Clears the screen
*/
/**************************************************************************/
void sharpmemClearScreen() 
{
  memset(_sharpmembuffer, 0x00, (SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8);
  // Send the clear screen command rather than doing a HW refresh (quicker)
  SET_CS;
  sharpmemSendByte(_sharpmem_vcom | SHARPMEM_BIT_CLEAR);
  sharpmemSendByte(0x00);
  TOGGLE_VCOM;
  CLR_CS;  
}

/**************************************************************************/
/*! 
    @brief Renders the contents of the pixel buffer on the LCD
*/
/**************************************************************************/
void sharpmemRefresh(void) 
{
  uint16_t i, totalbytes, currentline, oldline;  
  totalbytes = (SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8;

  // Send the write command
  SET_CS;
  sharpmemSendByte(SHARPMEM_BIT_WRITECMD | _sharpmem_vcom);
  TOGGLE_VCOM;

  // Send the address for line 1
  oldline = currentline = 1;
  sharpmemSendByte(sharpmemSwap(currentline));

  // Send image buffer
  for (i=0; i<totalbytes; i++)
  {
    sharpmemSendByte(sharpmemSwap(_sharpmembuffer[i]));
    currentline = ((i+1)/(SHARPMEM_LCDWIDTH/8)) + 1;
    if(currentline != oldline)
    {
      // Send end of line and address bytes
      sharpmemSendByte(0x00);
      if (currentline < SHARPMEM_LCDHEIGHT)
      {
        sharpmemSendByte(sharpmemSwap(currentline));
      }
      oldline = currentline;
    }
  }

  // Send another trailing 8 bits for the last line
  sharpmemSendByte(0x00);
  CLR_CS;
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

    #include "drivers/lcd/bitmap/sharpmem/sharpmem.h"
    #include "drivers/lcd/smallfonts.h"
    
    // Configure the pins and initialise the LCD screen
    sharpmemInit();

    // Render some text on the screen
    sharpmemDrawString(1, 10, "5x8 System", Font_System5x8);
    sharpmemDrawString(1, 20, "7x8 System", Font_System7x8);

    // Refresh the screen to see the results
    sharpmemRefresh();

    @endcode
*/
/**************************************************************************/
void sharpmemDrawString(uint16_t x, uint16_t y, const char* text, struct FONT_DEF font)
{
  uint8_t l;
  for (l = 0; l < strlen(text); l++)
  {
    sharpmemDrawChar(x + (l * (font.u8Width + 1)), y, text[l], font);
  }
}
