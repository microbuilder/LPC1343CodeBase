/**************************************************************************/
/*! 
    @file     st7783.c
    @author   K. Townsend (microBuilder.eu)

    @section  DESCRIPTION

    Driver for st7783 240x320 pixel TFT LCD displays.
    
    This driver uses an 8-bit interface and a 16-bit RGB565 colour palette.

    @section  UPDATES

    26-11-2010: st7783ReadData contributed by Adafruit Industries

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
#include "st7783.h"
#include "core/systick/systick.h"
#include "drivers/lcd/tft/touchscreen.h"

static lcdOrientation_t lcdOrientation = LCD_ORIENTATION_PORTRAIT;
static lcdProperties_t st7783Properties = { 240, 320, TRUE, TRUE, FALSE };

/*************************************************/
/* Private Methods                               */
/*************************************************/

/*************************************************/
void st7783Delay(unsigned int t)
{
  unsigned char t1;
  while(t--)
  for ( t1=10; t1 > 0; t1-- )
  {
    __asm("nop");
  }
}

/*************************************************/
void st7783WriteCmd(uint16_t command) 
{
  // Compiled with -Os on GCC 4.4 this works out to 25 cycles
  // (versus 36 compiled with no optimisations).  I'm not sure it
  // can be improved further, so that means 25 cycles/350nS for
  // continuous writes (cmd, data, data, data, ...) or ~150 cycles/
  // ~2.1uS for a random pixel (Set X [cmd+data], Set Y [cmd+data],
  // Set color [cmd+data]) (times assumes 72MHz clock).

  CLR_CS_CD_SET_RD_WR;  // Saves 18 commands compared to "CLR_CS; CLR_CD; SET_RD; SET_WR;" 
  ST7783_GPIO2DATA_DATA = (command >> (8 - ST7783_DATA_OFFSET));
  CLR_WR;
  SET_WR;
  ST7783_GPIO2DATA_DATA = command << ST7783_DATA_OFFSET;
  CLR_WR;
  SET_WR_CS;            // Saves 7 commands compared to "SET_WR; SET_CS;"
}

/*************************************************/
void st7783WriteData(uint16_t data)
{
  CLR_CS_SET_CD_RD_WR;  // Saves 18 commands compared to SET_CD; SET_RD; SET_WR; CLR_CS"
  ST7783_GPIO2DATA_DATA = (data >> (8 - ST7783_DATA_OFFSET));
  CLR_WR;
  SET_WR;
  ST7783_GPIO2DATA_DATA = data << ST7783_DATA_OFFSET;
  CLR_WR;
  SET_WR_CS;            // Saves 7 commands compared to "SET_WR, SET_CS;"
}

/*************************************************/
uint16_t st7783ReadData(void)
{
  // ToDo: Optimise this method!

  uint16_t high, low;
  high = low = 0;
  uint16_t d;

  SET_CD_RD_WR;   // Saves 14 commands compared to "SET_CD; SET_RD; SET_WR"
  CLR_CS;
  
  // set inputs
  ST7783_GPIO2DATA_SETINPUT;
  CLR_RD;
  st7783Delay(100);
  high = ST7783_GPIO2DATA_DATA;  
  high >>= ST7783_DATA_OFFSET;
  high &= 0xFF;
  SET_RD;
  
  CLR_RD;
  st7783Delay(100);
  low = ST7783_GPIO2DATA_DATA;
  low >>= ST7783_DATA_OFFSET;
  low &=0xFF;
  SET_RD;
  
  SET_CS;
  ST7783_GPIO2DATA_SETOUTPUT;

  d = high;
  d <<= 8;
  d |= low;
  
  return d;
}

/*************************************************/
uint16_t st7783Read(uint16_t addr)
{
  st7783WriteCmd(addr);
  return st7783ReadData();
}

/*************************************************/
void st7783Command(uint16_t command, uint16_t data)
{
  st7783WriteCmd(command);
  st7783WriteData(data);
}

/*************************************************/
/* Returns the 4-hexdigit controller code        */
/*************************************************/
uint16_t st7783Type(void)
{
  st7783WriteCmd(0x0);
  return st7783ReadData();
}

/*************************************************/
void st7783SetCursor(uint16_t x, uint16_t y)
{
  uint16_t he, ve, al, ah;
  
  switch (lcdOrientation) 
  {
  case LCD_ORIENTATION_LANDSCAPE:
          he = st7783Properties.width-1-y;
          ve = st7783Properties.height-1-x;
          al = y;
          ah = x;
          break;
  case LCD_ORIENTATION_PORTRAIT:
  default:
          he = st7783Properties.width-1;
          ve = st7783Properties.height-1;
          al = x;
          ah = y;
          break;
  }
  st7783Command(0x0051, he);
  st7783Command(0x0053, ve);
  st7783Command(0x0020, al);
  st7783Command(0x0021, ah);
}

/*************************************************/
void st7783InitDisplay(void)
{
  // Clear data line
  GPIO_GPIO2DATA &= ~ST7783_DATA_MASK;
    
  SET_RD;
  SET_WR;
  SET_CS;
  SET_CD;

  // Reset display
  CLR_RESET;
  st7783Delay(10000);
  SET_RESET;
  st7783Delay(500);

  st7783Command(0x00FF, 0x0001);
  st7783Command(0x00F3, 0x0008);
  st7783WriteCmd(0x00F3);

  st7783Command(0x0001, 0x0100);     // Driver Output Control Register (R01h)
  st7783Command(0x0002, 0x0700);     // LCD Driving Waveform Control (R02h)
  st7783Command(0x0003, 0x1030);     // Entry Mode (R03h)  
  st7783Command(0x0008, 0x0302);
  st7783Command(0x0009, 0x0000);
  st7783Command(0x0010, 0x0000);     // Power Control 1 (R10h)
  st7783Command(0x0011, 0x0007);     // Power Control 2 (R11h)  
  st7783Command(0x0012, 0x0000);     // Power Control 3 (R12h)
  st7783Command(0x0013, 0x0000);     // Power Control 4 (R13h)
  st7783Delay(1000);  
  st7783Command(0x0010, 0x14B0);     // Power Control 1 (R10h)  
  st7783Delay(500);  
  st7783Command(0x0011, 0x0007);     // Power Control 2 (R11h)  
  st7783Delay(500);  
  st7783Command(0x0012, 0x008E);     // Power Control 3 (R12h)
  st7783Command(0x0013, 0x0C00);     // Power Control 4 (R13h)
  st7783Command(0x0029, 0x0015);     // NVM read data 2 (R29h)
  st7783Delay(500);
  st7783Command(0x0030, 0x0000);     // Gamma Control 1
  st7783Command(0x0031, 0x0107);     // Gamma Control 2
  st7783Command(0x0032, 0x0000);     // Gamma Control 3
  st7783Command(0x0035, 0x0203);     // Gamma Control 6
  st7783Command(0x0036, 0x0402);     // Gamma Control 7
  st7783Command(0x0037, 0x0000);     // Gamma Control 8
  st7783Command(0x0038, 0x0207);     // Gamma Control 9
  st7783Command(0x0039, 0x0000);     // Gamma Control 10
  st7783Command(0x003C, 0x0203);     // Gamma Control 13
  st7783Command(0x003D, 0x0403);     // Gamma Control 14
  st7783Command(0x0050, 0x0000);     // Window Horizontal RAM Address Start (R50h)
  st7783Command(0x0051, st7783Properties.width - 1);    // Window Horizontal RAM Address End (R51h)
  st7783Command(0x0052, 0X0000);     // Window Vertical RAM Address Start (R52h)
  st7783Command(0x0053, st7783Properties.height - 1);    // Window Vertical RAM Address End (R53h)
  st7783Command(0x0060, 0xa700);     // Driver Output Control (R60h)
  st7783Command(0x0061, 0x0001);     // Driver Output Control (R61h)
  st7783Command(0x0090, 0X0029);     // Panel Interface Control 1 (R90h)

  // Display On
  st7783Command(0x0007, 0x0133);     // Display Control (R07h)
  st7783Delay(500);
  st7783WriteCmd(0x0022);
}

/*************************************************/
void st7783Home(void)
{
  st7783SetCursor(0, 0);
  st7783WriteCmd(0x0022);            // Write Data to GRAM (R22h)
}

/*************************************************/
void st7783SetWindow(uint16_t x, uint16_t y, uint16_t height, uint16_t width)
{
  // Window horizontal RAM address start
  if (x >= height) st7783Command(0x50, (x - height + 1));
  else st7783Command(0x50, 0);
  // Window horizontal GRAM address end
  st7783Command(0x51, x);
  // Window vertical GRAM address start
  if (y >= width) st7783Command(0x52, (y - width + 1));
  else st7783Command(0x52, 0);
  // Window vertical GRAM address end
  st7783Command(0x53, y);

  st7783SetCursor(x, y);
}

/*************************************************/
/* Public Methods                                */
/*************************************************/

/*************************************************/
void lcdInit(void)
{
  // Set control line pins to output
  gpioSetDir(ST7783_CS_PORT, ST7783_CS_PIN, 1);
  gpioSetDir(ST7783_CD_PORT, ST7783_CD_PIN, 1);
  gpioSetDir(ST7783_WR_PORT, ST7783_WR_PIN, 1);
  gpioSetDir(ST7783_RD_PORT, ST7783_RD_PIN, 1);
  
  // Set data port pins to output
  ST7783_GPIO2DATA_SETOUTPUT;

  // Disable pullups
  ST7783_DISABLEPULLUPS();
  
  // Set backlight pin to output and turn it on
  gpioSetDir(ST7783_BL_PORT, ST7783_BL_PIN, 1);      // set to output
  lcdBacklight(TRUE);

  // Set reset pin to output
  gpioSetDir(ST7783_RES_PORT, ST7783_RES_PIN, 1);    // Set to output
  gpioSetValue(ST7783_RES_PORT, ST7783_RES_PIN, 0);  // Low to reset
  systickDelay(50);
  gpioSetValue(ST7783_RES_PORT, ST7783_RES_PIN, 1);  // High to exit

  // Initialize the display
  st7783InitDisplay();

  // Set lcd to default orientation
  lcdSetOrientation(lcdOrientation);

  // Fill black
  lcdFillRGB(COLOR_BLACK);
  
  // Initialise the touch screen (and calibrate if necessary)
  tsInit();
}

/*************************************************/
void lcdBacklight(bool state)
{
  // Set the backlight
  gpioSetValue(ST7783_BL_PORT, ST7783_BL_PIN, state ? 0 : 1);
}

/*************************************************/
void lcdTest(void)
{
  uint32_t i,j;
  st7783Home();
  
  for(i=0;i<320;i++)
  {
    for(j=0;j<240;j++)
    {
      if(i>279)st7783WriteData(COLOR_WHITE);
      else if(i>239)st7783WriteData(COLOR_BLUE);
      else if(i>199)st7783WriteData(COLOR_GREEN);
      else if(i>159)st7783WriteData(COLOR_CYAN);
      else if(i>119)st7783WriteData(COLOR_RED);
      else if(i>79)st7783WriteData(COLOR_MAGENTA);
      else if(i>39)st7783WriteData(COLOR_YELLOW);
      else st7783WriteData(COLOR_BLACK);
    }
  }
}

/*************************************************/
void lcdFillRGB(uint16_t data)
{
  unsigned int i;
  st7783Home();
  
  uint32_t pixels = 320*240;
  for ( i=0; i < pixels; i++ )
  {
    st7783WriteData(data);
  } 
}

/*************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  st7783SetCursor(x, y);
  st7783WriteCmd(0x0022);  // Write Data to GRAM (R22h)
  st7783WriteData(color);
}

/**************************************************************************/
/*! 
    @brief  Draws an array of consecutive RGB565 pixels (much
            faster than addressing each pixel individually)
*/
/**************************************************************************/
void lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t len)
{
  uint32_t i = 0;
  st7783SetCursor(x, y);
  st7783WriteCmd(0x0022);  // Write Data to GRAM (R22h)
  do
  {
    st7783WriteData(data[i]);
    i++;
  } while (i<len);
}

/*************************************************/
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{
  // Allows for slightly better performance than setting individual pixels
  uint16_t x, pixels;

  if (x1 < x0)
  {
    // Switch x1 and x0
    x = x1;
    x1 = x0;
    x0 = x;
  }
  st7783SetCursor(x0, y);
  st7783WriteCmd(0x0022);  // Write Data to GRAM (R22h)
  for (pixels = 0; pixels < x1 - x0 + 1; pixels++)
  {
    st7783WriteData(color);
  }
}

/*************************************************/
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
  // Allows for slightly better performance than setting individual pixels
  lcdOrientation_t orientation = lcdOrientation;

  // Switch orientation
  lcdSetOrientation(orientation == LCD_ORIENTATION_PORTRAIT ? LCD_ORIENTATION_LANDSCAPE : LCD_ORIENTATION_PORTRAIT);

  // Draw horizontal line like usual
  lcdDrawHLine(y0, y1, lcdGetHeight() - x, color);

  // Switch orientation back
  lcdSetOrientation(orientation);
}

/*************************************************/
uint16_t lcdGetPixel(uint16_t x, uint16_t y)
{
  uint16_t preFetch = 0;

  st7783SetCursor(x, y);
  st7783WriteCmd(0x0022);
  preFetch = st7783ReadData();

  // Eeek ... why does this need to be done twice for a proper value?!?
  st7783SetCursor(x, y);
  st7783WriteCmd(0x0022);
  return st7783ReadData();
}

/*************************************************/
void lcdSetOrientation(lcdOrientation_t orientation)
{
  uint16_t entryMode = 0x1030;

  switch (orientation)
  {
    case LCD_ORIENTATION_PORTRAIT:
      entryMode = 0x1030;
      break;
    case LCD_ORIENTATION_LANDSCAPE:
      entryMode = 0x1028;
      break;
  }
  st7783WriteCmd(0x0003);
  st7783WriteData(entryMode);
  lcdOrientation = orientation;
  st7783SetCursor(0, 0);
}

/*************************************************/
lcdOrientation_t lcdGetOrientation(void)
{
  return lcdOrientation;
}

/*************************************************/
uint16_t lcdGetWidth(void)
{
  switch (lcdOrientation) 
  {
    case LCD_ORIENTATION_PORTRAIT:
      return st7783Properties.width;
      break;
    case LCD_ORIENTATION_LANDSCAPE:
    default:
      return st7783Properties.height;
  }
}

/*************************************************/
uint16_t lcdGetHeight(void)
{
  switch (lcdOrientation) 
  {
    case LCD_ORIENTATION_PORTRAIT:
      return st7783Properties.height;
      break;
    case LCD_ORIENTATION_LANDSCAPE:
    default:
      return st7783Properties.width;
  }
}

/*************************************************/
void lcdScroll(int16_t pixels, uint16_t fillColor)
{
  // Not implemented in ST7783
}

/*************************************************/
uint16_t lcdGetControllerID(void)
{
  return st7783Type();
}

/*************************************************/
lcdProperties_t lcdGetProperties(void)
{
    return st7783Properties;
}
