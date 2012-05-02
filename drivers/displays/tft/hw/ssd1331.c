/**************************************************************************/
/*! 
    @file     SSD1331.c
    @author   K. Townsend (microBuilder.eu)

    @section  DESCRIPTION

    Driver for SSD1331 96x64 pixel RGB OLED displays.
    
    This driver uses a bit-banged SPI interface and 16-bit RGB565 colours.

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
#include "ssd1331.h"
#include "core/systick/systick.h"

static volatile lcdOrientation_t lcdOrientation = LCD_ORIENTATION_PORTRAIT;
static lcdProperties_t ssd1331Properties = { 96, 64, false, false, false, true, true };

/*************************************************/
/* Private Methods                               */
/*************************************************/

#define CMD(c)        do { SET_CS; CLR_DC; CLR_CS; ssd1331SendByte( c ); SET_CS; } while (0)
#define DATA(c)       do { SET_CS; SET_DC; CLR_CS; ssd1331SendByte( c ); SET_CS; } while (0);
#define DELAY(mS)     do { systickDelay( mS / CFG_SYSTICK_DELAY_IN_MS ); } while(0);

/**************************************************************************/
/*! 
    @brief Simulates an SPI write using GPIO

    @param[in]  byte
                The byte to send
*/
/**************************************************************************/
void ssd1331SendByte(uint8_t byte)
{
  int8_t i;

  // Make sure clock pin starts high
  SET_SCK;

  // Write from MSB to LSB
  for (i=7; i>=0; i--) 
  {
    // Set clock pin low
    CLR_SCK;
    // Set data pin high or low depending on the value of the current bit
    if (byte & (1 << i))
    {
      SET_SID;
    }
    else
    {
      CLR_SID;
    }
    // Set clock pin high
    SET_SCK;
  }
}

/**************************************************************************/
/*! 
    @brief  Sets the cursor to the specified X/Y position
*/
/**************************************************************************/
void ssd1331SetCursor(uint8_t x, uint8_t y)
{
  if ((x >= ssd1331Properties.width) || (y >= ssd1331Properties.height))
    return;
  // set x and y coordinate
  CMD(SSD1331_CMD_SETCOLUMN);
  CMD(x);
  CMD(ssd1331Properties.width-1);

  CMD(SSD1331_CMD_SETROW);
  CMD(y);
  CMD(ssd1331Properties.height-1);
}

/**************************************************************************/
/*! 
    @brief  Draws a solid line using HW acceleration
*/
/**************************************************************************/
void ssd1331DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color) 
{  
  uint16_t x;

  if ((x1 >= ssd1331Properties.width) || (x2 >= ssd1331Properties.width) ||
      (y1 >= ssd1331Properties.height) || (y2 >= ssd1331Properties.height)) {
      return;
  }

  // Switch x2 and x1 if required
  if (x2 < x1)
  {
    x = x2;
    x2 = x1;
    x1 = x;
  }

  // Switch y2 and y1 if required
  if (y2 < y1)
  {
    x = y2;
    y2 = y1;
    y1 = x;
  }

  CMD(SSD1331_CMD_DRAWLINE);
  CMD(x1);
  CMD(y1);
  CMD(x2);
  CMD(y2);
  CMD((uint8_t)((color >> 11) & 0x1F));
  CMD((uint8_t)((color >> 5) & 0x3F));
  CMD((uint8_t)(color & 0x1F));

}

/**************************************************************************/
/*! 
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/
void ssd1331FillRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t pencolor, uint16_t fillcolor) 
{
  if ((x1 >= ssd1331Properties.width) || (x2 >= ssd1331Properties.width) ||
      (y1 >= ssd1331Properties.height) || (y2 >= ssd1331Properties.height)) {
      return;
	}
  
  // fill!
  CMD(SSD1331_CMD_FILL);
  CMD(0x01);

  CMD(SSD1331_CMD_DRAWRECT);
  CMD(x1);
  CMD(y1);
  CMD(x2);
  CMD(y2);
  CMD((pencolor >> 11) << 1);
  CMD((pencolor >> 5) & 0x3F);
  CMD((pencolor << 1)& 0x3F);
 
  CMD((fillcolor >> 11) << 1);
  CMD((fillcolor >> 5) & 0x3F);
  CMD((fillcolor << 1)& 0x3F);
}

/**************************************************************************/
/*! 
    @brief  Inverts the R and B in an RGB565 color
*/
/**************************************************************************/
uint16_t invert565Color(uint16_t color)
{   
  uint16_t r, g, b;   
   
  b = (color>>0)  & 0x1f;   
  g = (color>>5)  & 0x3f;   
  r = (color>>11) & 0x1f;   
     
  return( (b<<11) + (g<<5) + (r<<0) );
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
  // Set all pins to output
  gpioSetDir(SSD1331_SCK_PORT, SSD1331_SCK_PIN, gpioDirection_Output);
  gpioSetDir(SSD1331_SID_PORT, SSD1331_SID_PIN, gpioDirection_Output);
  gpioSetDir(SSD1331_DC_PORT, SSD1331_DC_PIN, gpioDirection_Output);
  gpioSetDir(SSD1331_RST_PORT, SSD1331_RST_PIN, gpioDirection_Output);
  gpioSetDir(SSD1331_CS_PORT, SSD1331_CS_PIN, gpioDirection_Output);

  // Reset the LCD
  SET_RST;
  DELAY(1);
  CLR_RST;
  DELAY(10);
  SET_RST;

  // Disable pullups
  SSD1331_DISABLEPULLUPS();
  
  CMD(SSD1331_CMD_DISPLAYOFF);  // 0xAE
  CMD(SSD1331_CMD_SETREMAP); // 0xA0
  // A[2] = 1 = color order (0 = RGB, 1 = BGR)
  // A[7:6] = 01 = 65K color
  #if defined SSD1331_COLORORDER_BGR
  CMD(0x76);
  #else
  CMD(0x72);
  #endif
  CMD(SSD1331_CMD_STARTLINE); // 0xA1
  CMD(0x0);
  CMD(SSD1331_CMD_DISPLAYOFFSET); // 0xA2
  CMD(0x0);
  CMD(SSD1331_CMD_NORMALDISPLAY);  // 0xA4
  CMD(SSD1331_CMD_SETMULTIPLEX); // 0xA8
  CMD(0x3F);  // 0x3F 1/64 duty
  CMD(SSD1331_CMD_SETMASTER);  // 0xAD
  CMD(0x8E);
  CMD(SSD1331_CMD_POWERMODE);  // 0xB0
  CMD(0x0B);
  CMD(SSD1331_CMD_PRECHARGE);  // 0xB1
  CMD(0x31);
  CMD(SSD1331_CMD_CLOCKDIV);  // 0xB3
  CMD(0xF0);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
  CMD(SSD1331_CMD_PRECHARGEA);  // 0x8A
  CMD(0x64);
  CMD(SSD1331_CMD_PRECHARGEB);  // 0x8B
  CMD(0x78);
  CMD(SSD1331_CMD_PRECHARGEA);  // 0x8C
  CMD(0x64);
  CMD(SSD1331_CMD_PRECHARGELEVEL);  // 0xBB
  CMD(0x3A);
  CMD(SSD1331_CMD_VCOMH);  // 0xBE
  CMD(0x3E);
  CMD(SSD1331_CMD_MASTERCURRENT);  // 0x87
  CMD(0x06);
  CMD(SSD1331_CMD_CONTRASTA);  // 0x81
  CMD(0x91);
  CMD(SSD1331_CMD_CONTRASTB);  // 0x82
  CMD(0x50);
  CMD(SSD1331_CMD_CONTRASTC);  // 0x83
  CMD(0x7D);
  CMD(SSD1331_CMD_DISPLAYON);//--turn on oled panel
  
  // Fill black
  lcdFillRGB(COLOR_BLACK);
}

/**************************************************************************/
/*! 
    @brief  Enables or disables the LCD backlight
*/
/**************************************************************************/
void lcdBacklight(bool state)
{
  // No backlight ... do nothing
}

/**************************************************************************/
/*! 
    @brief  Renders a simple test pattern on the LCD
*/
/**************************************************************************/
void lcdTest(void)
{
  uint32_t i,j;
  ssd1331SetCursor(0, 0);
  
  for(i=0;i<64;i++)
  {
    for(j=0;j<96;j++)
    {
      if(i>55){DATA((uint8_t)COLOR_WHITE>>8);DATA((uint8_t)COLOR_WHITE);}
      else if(i>47){DATA((uint8_t)COLOR_BLUE>>8);DATA((uint8_t)COLOR_BLUE);}
      else if(i>39){DATA((uint8_t)COLOR_GREEN>>8);DATA((uint8_t)COLOR_GREEN);}
      else if(i>31){DATA((uint8_t)COLOR_CYAN>>8);DATA((uint8_t)COLOR_CYAN);}
      else if(i>23){DATA((uint8_t)COLOR_RED>>8);DATA((uint8_t)COLOR_RED);}
      else if(i>15){DATA((uint8_t)COLOR_MAGENTA>>8);DATA((uint8_t)COLOR_MAGENTA);}
      else if(i>7){DATA((uint8_t)COLOR_YELLOW>>8);DATA((uint8_t)COLOR_YELLOW);}
      else {DATA((uint8_t)COLOR_BLACK>>8);DATA((uint8_t)COLOR_BLACK);}
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
  ssd1331FillRect(0,0,ssd1331Properties.width-1,ssd1331Properties.height-1, data, data);
}

/**************************************************************************/
/*! 
    @brief  Draws a single pixel at the specified X/Y location
*/
/**************************************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if ((x >= ssd1331Properties.width) || (y >= ssd1331Properties.height))
    return;

  ssd1331SetCursor((uint8_t)x, (uint8_t)y);
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
  ssd1331DrawLine((uint8_t)x0, (uint8_t)y, (uint8_t)x1, (uint8_t)y, color); 
}

/**************************************************************************/
/*! 
    @brief  Optimised routine to draw a vertical line faster than
            setting individual pixels
*/
/**************************************************************************/
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
  ssd1331DrawLine((uint8_t)x, (uint8_t)y0, (uint8_t)x, (uint8_t)y1, color); 
}

/**************************************************************************/
/*! 
    @brief  Gets the 16-bit color of the pixel at the specified location
*/
/**************************************************************************/
uint16_t lcdGetPixel(uint16_t x, uint16_t y)
{
  // ToDo
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
  return ssd1331Properties.width;
}

/**************************************************************************/
/*! 
    @brief  Gets the height in pixels of the LCD screen (varies depending
            on the current screen orientation)
*/
/**************************************************************************/
uint16_t lcdGetHeight(void)
{
  return ssd1331Properties.height;
}

/**************************************************************************/
/*! 
    @brief  Scrolls the contents of the LCD screen vertically the
            specified number of pixels using a HW optimised routine
*/
/**************************************************************************/
void lcdScroll(int16_t pixels, uint16_t fillColor)
{
  // ToDo
}

/**************************************************************************/
/*! 
    @brief  Gets the controller's 16-bit (4 hexdigit) ID
*/
/**************************************************************************/
uint16_t lcdGetControllerID(void)
{
  return 0x1331;
}

/**************************************************************************/
/*! 
    @brief  Returns the LCDs 'lcdProperties_t' that describes the LCDs
            generic capabilities and dimensions
*/
/**************************************************************************/
lcdProperties_t lcdGetProperties(void)
{
  return ssd1331Properties;
}
