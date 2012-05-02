/**************************************************************************/
/*! 
    @file     SSD1351.c
    @author   K. Townsend (www.adafruit.com)

    @section  DESCRIPTION

    Driver for SSD1351 128x128 pixel RGB OLED displays.
    
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
#include "ssd1351.h"
#include "core/systick/systick.h"

static volatile lcdOrientation_t lcdOrientation = LCD_ORIENTATION_PORTRAIT;
static lcdProperties_t ssd1351Properties = { 128, 128, false, false, false, true, false };

/*************************************************/
/* Private Methods                               */
/*************************************************/

#define CMD(c)        do { SET_CS; CLR_CS; CLR_DC; ssd1351SendByte( c, 1 ); SET_CS; } while (0)
#define DATA(c)       do { SET_CS; CLR_CS; SET_DC; ssd1351SendByte( c, 0 ); SET_CS; } while (0)
#define DELAY(mS)     do { systickDelay( mS / CFG_SYSTICK_DELAY_IN_MS ); } while(0)

/**************************************************************************/
/*! 
    @brief Simulates an SPI write using GPIO.  
    
    @param[in]  byte
                The byte to send
    @param[in]  command
                1 if this is a command, 0 if it is data
*/
/**************************************************************************/
void ssd1351SendByte(uint8_t byte, uint8_t command)
{
  int8_t i;

  // Make sure clock pin starts high
  SET_SCK;

#if defined SSD1351_BUS_SPI3
  // Prepend D/C bit (cmd = 0, data = 1)
  CLR_SCK;
  if (command)
  {
    CLR_SID;
  }
  else
  {
    SET_SID;
  }
  SET_SCK;
#endif

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
void ssd1351SetCursor(uint8_t x, uint8_t y)
{
  if ((x >= ssd1351Properties.width) || (y >= ssd1351Properties.height))
    return;

  CMD(SSD1351_CMD_SETCOLUMNADDRESS);
  DATA(x);                            // Start Address
  DATA(ssd1351Properties.width-1);    // End Address (0x7F)

  CMD(SSD1351_CMD_SETROWADDRESS);
  DATA(y);                            // Start Address
  DATA(ssd1351Properties.height-1);   // End Address (0x7F)
}

/**************************************************************************/
/*! 
    @brief  Sets the cursor to 0, 0 and resets the window size
*/
/**************************************************************************/
void ssd1351GoHome(void)
{
  ssd1351SetCursor(0, 0);
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
  gpioSetDir(SSD1351_SCK_PORT, SSD1351_SCK_PIN, gpioDirection_Output);
  gpioSetDir(SSD1351_SID_PORT, SSD1351_SID_PIN, gpioDirection_Output);
  gpioSetDir(SSD1351_RST_PORT, SSD1351_RST_PIN, gpioDirection_Output);
  gpioSetDir(SSD1351_CS_PORT, SSD1351_CS_PIN, gpioDirection_Output);

#if !defined SSD1351_BUS_SPI3
  gpioSetDir(SSD1351_DC_PORT, SSD1351_DC_PIN, gpioDirection_Output);
#endif

  // Make sure CS starts low
  // CLR_CS;

  // Reset the LCD
  SET_RST;
  DELAY(20);
  CLR_RST;
  DELAY(200);
  SET_RST;
  DELAY(20);

  // Disable internal pullup resistors
  SSD1351_DISABLEPULLUPS();

  CMD(SSD1351_CMD_SETCOMMANDLOCK);
  DATA(0x12);                               // Unlocked to enter commands

  CMD(SSD1351_CMD_SETCOMMANDLOCK);
  DATA(0xB1);                               // Make all commands accessible 

  CMD(SSD1351_CMD_SLEEPMODE_DISPLAYOFF);

  CMD(SSD1351_CMD_SETFRONTCLOCKDIV);
  DATA(0xF1);                               // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)

  CMD(SSD1351_CMD_SETMUXRRATIO);
  DATA(0x7F);

  CMD(SSD1351_CMD_COLORDEPTH);
  DATA(0x74);                               // Bit 7:6 = 65,536 Colors, Bit 3 = BGR or RGB

  CMD(SSD1351_CMD_SETCOLUMNADDRESS);
  DATA(0x00);
  DATA(0x7F);

  CMD(SSD1351_CMD_SETROWADDRESS);
  DATA(0x00);
  DATA(0x7F);

  CMD(SSD1351_CMD_SETDISPLAYSTARTLINE);
  DATA(0x00);

  CMD(SSD1351_CMD_SETDISPLAYOFFSET);
  DATA(0x00);
 
  CMD(SSD1351_CMD_SETGPIO);
  DATA(0x00);                               // Disable GPIO pins

  CMD(SSD1351_CMD_FUNCTIONSELECTION);
  DATA(0x01);                               // External VDD (1 = External bias using diode drop, 0 = internal)

  CMD(SSD1351_CMD_SETPHASELENGTH);
  DATA(0x32);

  CMD(SSD1351_CMD_SETPRECHARGEVOLTAGE);
  DATA(0x32);  // was 0x17

  CMD(SSD1351_CMD_SETVCOMHVOLTAGE);
  DATA(0x05);

  CMD(SSD1351_CMD_SETDISPLAYMODE_RESET);

  CMD(SSD1351_CMD_SETCONTRAST);
  DATA(0xC8);
  DATA(0x80);
  DATA(0xC8);

  CMD(SSD1351_CMD_MASTERCONTRAST);
  DATA(0x0F);                               // Maximum contrast

  CMD(SSD1351_CMD_SETSEGMENTLOWVOLTAGE);
  DATA(0xA0);                               // Enable External VSL
  DATA(0xB5);
  DATA(0x55);

  CMD(SSD1351_CMD_SETSECONDPRECHARGEPERIOD);
  DATA(0x01);

  // Use default grayscale for now to save flash space, but here are
  // the values if someone wants to change them ...
  //  CMD(SSD1351_CMD_GRAYSCALELOOKUP);
  //  DATA(0x02);
  //  DATA(0x03);
  //  DATA(0x04);
  //  DATA(0x05);
  //  DATA(0x06);
  //  DATA(0x07);
  //  DATA(0x08);
  //  DATA(0x09);
  //  DATA(0x0A);
  //  DATA(0x0B);
  //  DATA(0x0C);
  //  DATA(0x0D);
  //  DATA(0x0E);
  //  DATA(0x0F);
  //  DATA(0x10);
  //  DATA(0x11);
  //  DATA(0x12);
  //  DATA(0x13);
  //  DATA(0x15);
  //  DATA(0x17);
  //  DATA(0x19);
  //  DATA(0x1B);
  //  DATA(0x1D);
  //  DATA(0x1F);
  //  DATA(0x21);
  //  DATA(0x23);
  //  DATA(0x25);
  //  DATA(0x27);
  //  DATA(0x2A);
  //  DATA(0x2D);
  //  DATA(0x30);
  //  DATA(0x33);
  //  DATA(0x36);
  //  DATA(0x39);
  //  DATA(0x3C);
  //  DATA(0x3F);
  //  DATA(0x42);
  //  DATA(0x45);
  //  DATA(0x48);
  //  DATA(0x4C);
  //  DATA(0x50);
  //  DATA(0x54);
  //  DATA(0x58);
  //  DATA(0x5C);
  //  DATA(0x60);
  //  DATA(0x64);
  //  DATA(0x68);
  //  DATA(0x6C);
  //  DATA(0x70);
  //  DATA(0x74);
  //  DATA(0x78);
  //  DATA(0x7D);
  //  DATA(0x82);
  //  DATA(0x87);
  //  DATA(0x8C);
  //  DATA(0x91);
  //  DATA(0x96);
  //  DATA(0x9B);
  //  DATA(0xA0);
  //  DATA(0xA5);
  //  DATA(0xAA);
  //  DATA(0xAF);
  //  DATA(0xBF);

  // Clear screen
  lcdFillRGB(COLOR_BLACK);

  // Turn the display on
  CMD(SSD1351_CMD_SLEEPMODE_DISPLAYON);  
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
  CMD(SSD1351_CMD_WRITERAM);
  ssd1351SetCursor(0, 0);
  CMD(SSD1351_CMD_WRITERAM);
  
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
  uint16_t i;
  ssd1351SetCursor(0, 0);
  CMD(SSD1351_CMD_WRITERAM);
  for (i=1; i<=((ssd1351Properties.width)*(ssd1351Properties.height)) * 2;i++)
  {
    DATA(data >> 8);
    DATA(data);
  }
}

/**************************************************************************/
/*! 
    @brief  Draws a single pixel at the specified X/Y location
*/
/**************************************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if ((x >= ssd1351Properties.width) || (y >= ssd1351Properties.height))
    return;

  ssd1351SetCursor((uint8_t)x, (uint8_t)y);
  CMD(SSD1351_CMD_WRITERAM);
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
  // Allows for slightly better performance than setting individual pixels
  uint16_t x, pixels;

  if (x1 < x0)
  {
    // Switch x1 and x0
    x = x1;
    x1 = x0;
    x0 = x;
  }

  // Check limits
  if (x1 >= ssd1351Properties.width)
  {
    x1 = ssd1351Properties.width - 1;
  }
  if (x0 >= ssd1351Properties.width)
  {
    x0 = ssd1351Properties.width - 1;
  }

  ssd1351SetCursor(x0, y);
  CMD(SSD1351_CMD_WRITERAM);
  for (pixels = 0; pixels < x1 - x0 + 1; pixels++)
  {
    DATA(color >> 8);
    DATA(color);
  }
  ssd1351GoHome();
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
  return ssd1351Properties.width;
}

/**************************************************************************/
/*! 
    @brief  Gets the height in pixels of the LCD screen (varies depending
            on the current screen orientation)
*/
/**************************************************************************/
uint16_t lcdGetHeight(void)
{
  return ssd1351Properties.height;
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
  return 0x1351;
}

/**************************************************************************/
/*! 
    @brief  Returns the LCDs 'lcdProperties_t' that describes the LCDs
            generic capabilities and dimensions
*/
/**************************************************************************/
lcdProperties_t lcdGetProperties(void)
{
  return ssd1351Properties;
}
