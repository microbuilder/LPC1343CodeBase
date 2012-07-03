/**************************************************************************/
/*!
    @file     s6b33b6x.c
    @author   K. Townsend (www.adafruit.com)

    @section  DESCRIPTION

    Driver for S6B33B6X 128x128 pixel RGB OLED displays.

    This driver uses a 3 or 4-pin SPI interface and 16-bit RGB565 colours.

    @section  LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, Adafruit Industries
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
#include "s6b33b6x.h"
#include "core/systick/systick.h"

static volatile lcdOrientation_t lcdOrientation = LCD_ORIENTATION_PORTRAIT;
static lcdProperties_t s6b33b6xProperties = { 128, 128, false, false, false, false, false };

/*************************************************/
/* Private Methods                               */
/*************************************************/

#define CMD(c)        do { CLR_DC; CLR_CS; s6b33b6xSendByte( c, 1 ); SET_CS; } while (0)
#define DATA(c)       do { SET_DC; CLR_CS; s6b33b6xSendByte( c, 0 ); SET_CS; } while (0)
#define DELAY(ms)     do { systickDelay( ms / CFG_SYSTICK_DELAY_IN_MS ); } while(0)

/**************************************************************************/
/*!
    @brief Simulates an SPI write using GPIO.

    @param[in]  byte
                The byte to send
    @param[in]  command
                1 if this is a command, 0 if it is data
*/
/**************************************************************************/
void s6b33b6xSendByte(uint8_t byte, uint8_t command)
{
  int8_t i;

  // Make sure clock pin starts high (CPOL = 1)
  SET_SCK;

#if defined S6B33B6X_BUS_SPI3
  CLR_SCK;
  if (command)
  {
    CLR_SID;
  }
  else
  {
    SET_SID;
  }
  // Read on rising edge (CPHA = 1)
  SET_SCK
#endif

  for (i=0; i<8; i++)
  {
    // Set clock pin low
    CLR_SCK;
    // Set data pin high or low depending on the value of the current bit
    if((byte & 0x80) == 0x80)
    {
      SET_SID;
    }
    else
    {
      CLR_SID;
    }
    // Set clock pin high (CPHA = 1, read on rising edge)
    SET_SCK;
    byte = (byte << 1);
  }
}

/**************************************************************************/
/*!
    @brief  Sets the cursor to the specified X/Y position
*/
/**************************************************************************/
void s6b33b6xSetCursor(uint8_t x, uint8_t y)
{
  if ((x >= s6b33b6xProperties.width) || (y >= s6b33b6xProperties.height))
    return;

  CMD(S6B33B6X_CMD_ROWADDRESSAREASET);
  CMD(2+y);     // Start Address
  CMD(129);   // End Address (129)

  CMD(S6B33B6X_CMD_COLUMNADDRESSAREASET);
  CMD(2+x);   // Start Address
  CMD(129);   // End Address (129)
}

/*************************************************/
/* Public Methods                                */
/*************************************************/

/**************************************************************************/
/*!
    @brief  Configures any pins or HW and initialises the LCD controller
*/
/**************************************************************************/
void lcdInit(void)
{
  gpioInit();

  // Make sure CS starts high
  SET_CS;

  // Make sure clock pin starts high (CPOL = 1)
  SET_SCK;

  // Set all pins to output
  gpioSetDir(S6B33B6X_SCK_PORT, S6B33B6X_SCK_PIN, gpioDirection_Output);
  gpioSetDir(S6B33B6X_SID_PORT, S6B33B6X_SID_PIN, gpioDirection_Output);
  gpioSetDir(S6B33B6X_RST_PORT, S6B33B6X_RST_PIN, gpioDirection_Output);
  gpioSetDir(S6B33B6X_CS_PORT, S6B33B6X_CS_PIN, gpioDirection_Output);

#if !defined S6B33B6X_BUS_SPI3
  gpioSetDir(S6B33B6X_DC_PORT, S6B33B6X_DC_PIN, gpioDirection_Output);
#endif

  // Reset the LCD
  SET_RST;
  DELAY(20);
  CLR_RST;
  DELAY(50);
  SET_RST;
  DELAY(20);

  // Init sequence
  CMD(S6B33B6X_CMD_STANDBYMODEOFF);
  DELAY(50);
  CMD(S6B33B6X_CMD_OSCILLATIONMODESET);
  CMD(0x01);
  CMD(S6B33B6X_CMD_DCDCCONTROL);
  CMD(0x0f);
  CMD(S6B33B6X_CMD_DCDCAMPONOFFSET);
  CMD(0x0f);
  DELAY(50);
  CMD(S6B33B6X_CMD_TEMPCOMPENSATIONSET);
  CMD(0x01);
  CMD(0x2e);
  CMD(S6B33B6X_CMD_RAMSKIPAREASET);
  CMD(0x00);
  CMD(S6B33B6X_CMD_SPECIFIEDDISPLAYPATTERN);
  CMD(0x00);
  CMD(S6B33B6X_CMD_DRIVEROUTPUTMODESET);
  CMD(0x03);
  CMD(S6B33B6X_CMD_DCDCCLOCKDIVISIONSET);
  CMD(0x22);
  CMD(S6B33B6X_CMD_ADDRESSINGMODESET);
  CMD(0x1d);
  CMD(S6B33B6X_CMD_ROWVECTORMODESET);
  CMD(0x0e);
  CMD(S6B33B6X_CMD_ENTRYMODESET);
  CMD(0x80);
  CMD(S6B33B6X_CMD_ROWADDRESSAREASET);
  CMD(0x00);
  CMD(0x83);
  CMD(S6B33B6X_CMD_COLUMNADDRESSAREASET);
  CMD(0x00);
  CMD(0x83);
  CMD(S6B33B6X_CMD_NBLOCKINVERSIONSET);
  CMD(0x8d);
  CMD(S6B33B6X_CMD_CONTRASTCONTROL);
  CMD(0x45);
  CMD(0x2b);
  CMD(0x69);
  CMD(0x22);
  CMD(0x11);
  CMD(S6B33B6X_CMD_PARTIALDISPLAYMODESET);
  CMD(0x00);
  CMD(S6B33B6X_CMD_PARTIALDISPLAYSTARTLINE);
  CMD(0x00);
  CMD(S6B33B6X_CMD_PARTIALDISPLAYENDLINE);
  CMD(0x9f);
  CMD(0x59);
  CMD(0x00);
  CMD(0x00);
  CMD(0x9f);
  CMD(0x00);
  CMD(S6B33B6X_CMD_DISPLAYON);
  DELAY(500);

  // Clear screen
  lcdFillRGB(COLOR_RED);
}

/**************************************************************************/
/*!
    @brief  Enables or disables the LCD backlight
*/
/**************************************************************************/
void lcdBacklight(bool state)
{
  // ToDo: Add BL pin and toggle here
}

/**************************************************************************/
/*!
    @brief  Renders a simple test pattern on the LCD
*/
/**************************************************************************/
void lcdTest(void)
{
  uint32_t i,j;
  s6b33b6xSetCursor(0, 0);

  for(i=0;i<128;i++)
  {
    for(j=0;j<128;j++)
    {
      if(i>111){DATA(COLOR_WHITE>>8);DATA((uint8_t)COLOR_WHITE);}
      else if(i>95){DATA(COLOR_BLUE>>8);DATA((uint8_t)COLOR_BLUE);}
      else if(i>79){DATA(COLOR_GREEN>>8);DATA((uint8_t)COLOR_GREEN);}
      else if(i>63){DATA(COLOR_CYAN>>8);DATA((uint8_t)COLOR_CYAN);}
      else if(i>47){DATA(COLOR_RED>>8);DATA((uint8_t)COLOR_RED);}
      else if(i>31){DATA(COLOR_MAGENTA>>8);DATA((uint8_t)COLOR_MAGENTA);}
      else if(i>15){DATA(COLOR_YELLOW>>8);DATA((uint8_t)COLOR_YELLOW);}
      else {DATA(COLOR_BLACK>>8);DATA((uint8_t)COLOR_BLACK);}
    }
  }
}

/**************************************************************************/
/*!
    @brief  Fills the LCD with the specified 16-bit color
*/
/**************************************************************************/
void lcdFillRGB(uint16_t data)
{
  uint16_t x,y;

  s6b33b6xSetCursor(0, 0);

  for(x=0;x<128;x++)
  {
    for(y=0;y<128;y++)
    {
      DATA(data>>8);
      DATA(data & 0xFF);
    }
  }
}

/**************************************************************************/
/*!
    @brief  Draws a single pixel at the specified X/Y location
*/
/**************************************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if ((x >= s6b33b6xProperties.width) || (y >= s6b33b6xProperties.height))
    return;

  s6b33b6xSetCursor((uint8_t)x, (uint8_t)y);
  DATA(color >> 8);
  DATA(color);
}

/**************************************************************************/
/*!
    @brief  Draws an array of consecutive RGB565 pixels (much
            faster than addressing each pixel individually)
*/
/**************************************************************************/
void lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t len)
{
  // ToDo
}

/**************************************************************************/
/*!
    @brief  Optimised routine to draw a horizontal line faster than
            setting individual pixels
*/
/**************************************************************************/
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{
  // ToDo
}

/**************************************************************************/
/*!
    @brief  Optimised routine to draw a vertical line faster than
            setting individual pixels
*/
/**************************************************************************/
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
  // Not supported
}

/**************************************************************************/
/*!
    @brief  Gets the 16-bit color of the pixel at the specified location
*/
/**************************************************************************/
uint16_t lcdGetPixel(uint16_t x, uint16_t y)
{
  // Not supported
  return 0;
}

/**************************************************************************/
/*!
    @brief  Sets the LCD orientation to horizontal and vertical
*/
/**************************************************************************/
void lcdSetOrientation(lcdOrientation_t orientation)
{
  // Not supported
}

/**************************************************************************/
/*!
    @brief  Gets the current screen orientation (horizontal or vertical)
*/
/**************************************************************************/
lcdOrientation_t lcdGetOrientation(void)
{
  return lcdOrientation;
}

/**************************************************************************/
/*!
    @brief  Gets the width in pixels of the LCD screen (varies depending
            on the current screen orientation)
*/
/**************************************************************************/
uint16_t lcdGetWidth(void)
{
  return s6b33b6xProperties.width;
}

/**************************************************************************/
/*!
    @brief  Gets the height in pixels of the LCD screen (varies depending
            on the current screen orientation)
*/
/**************************************************************************/
uint16_t lcdGetHeight(void)
{
  return s6b33b6xProperties.height;
}

/**************************************************************************/
/*!
    @brief  Scrolls the contents of the LCD screen vertically the
            specified number of pixels using a HW optimised routine
*/
/**************************************************************************/
void lcdScroll(int16_t pixels, uint16_t fillColor)
{
  // Not Supported
}

/**************************************************************************/
/*!
    @brief  Gets the controller's 16-bit (4 hexdigit) ID
*/
/**************************************************************************/
uint16_t lcdGetControllerID(void)
{
  return 0x33b6;
}

/**************************************************************************/
/*!
    @brief  Returns the LCDs 'lcdProperties_t' that describes the LCDs
            generic capabilities and dimensions
*/
/**************************************************************************/
lcdProperties_t lcdGetProperties(void)
{
  return s6b33b6xProperties;
}
