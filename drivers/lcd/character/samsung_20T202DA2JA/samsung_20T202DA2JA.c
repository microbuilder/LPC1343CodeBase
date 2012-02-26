/**************************************************************************/
/*! 
    @file     samsung_20T202DA2JA.c
    @author   Original source : Limor Fried/ladyada (Adafruit Industries)
              LPC1343 port    : K. Townsend

    @section DESCRIPTION

    Driver for Samsung 20T202DA2JA 20x2 VFD display module.

    This driver is based on the SPI_VFD Library from Limor Fried
    (Adafruit Industries) at: https://github.com/adafruit/SPI_VFD  

    The displays can be purchased at:
    https://www.adafruit.com/products/347
    
    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012 Adafruit Industries
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

#include "samsung_20T202DA2JA.h"

#include "core/gpio/gpio.h"
#include "core/systick/systick.h"

/**************************************************************************/
/* Private Methods                                                        */
/**************************************************************************/

uint8_t samsungvfd_displayfunction;
uint8_t samsungvfd_displaycontrol;
uint8_t samsungvfd_displaymode;
uint8_t samsungvfd_initialized;
uint8_t samsungvfd_numlines, samsungvfd_currline;

/**************************************************************************/
/*! 
    @brief Low-level SPI bit-banging routing
*/
/**************************************************************************/
static inline void samsungvfd_sendByte(uint8_t c) {
  int8_t i;

  // Make sure clock pin starts high
  gpioSetValue(SAMSUNGVFD_SCK_PORT, SAMSUNGVFD_SCK_PIN, 1);

  // Write from MSB to LSB
  for (i=7; i>=0; i--) 
  {
    // Set clock pin low
    gpioSetValue(SAMSUNGVFD_SCK_PORT, SAMSUNGVFD_SCK_PIN, 0);
    // Set data pin high or low depending on the value of the current bit
    gpioSetValue(SAMSUNGVFD_SIO_PORT, SAMSUNGVFD_SIO_PIN, c & (1 << i) ? 1 : 0);
    // Set clock pin high
    gpioSetValue(SAMSUNGVFD_SCK_PORT, SAMSUNGVFD_SCK_PIN, 1);
  }
}

/**************************************************************************/
/*! 
    @brief Sends a command to the VFD display
*/
/**************************************************************************/
void samsungvfd_command(uint8_t value) 
{
  gpioSetValue(SAMSUNGVFD_STB_PORT, SAMSUNGVFD_STB_PIN, 0);
  samsungvfd_sendByte(SAMSUNGVFD_SPICOMMAND);
  samsungvfd_sendByte(value);
  gpioSetValue(SAMSUNGVFD_STB_PORT, SAMSUNGVFD_STB_PIN, 1);
}

/**************************************************************************/
/*! 
    @brief Initialises the VFD character display
*/
/**************************************************************************/
void samsungvfd_begin(uint8_t cols, uint8_t lines, uint8_t brightness) 
{
  // set number of lines
  if (lines > 1)
    samsungvfd_displayfunction = SAMSUNGVFD_2LINE;
  else
    samsungvfd_displayfunction = SAMSUNGVFD_1LINE;

  if (brightness>SAMSUNGVFD_BRIGHTNESS25)	//catch bad values
    brightness = SAMSUNGVFD_BRIGHTNESS100;

  // set the brightness and push the linecount with VFD_SETFUNCTION
  samsungvfdSetBrightness(brightness);
  
  samsungvfd_numlines = lines;
  samsungvfd_currline = 0;
  
  // Initialize to default text direction (for romance languages#include "SPI_VFD.h"
  samsungvfd_displaymode = SAMSUNGVFD_ENTRYLEFT | SAMSUNGVFD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  samsungvfd_command(SAMSUNGVFD_ENTRYMODESET | samsungvfd_displaymode);

  samsungvfd_command(SAMSUNGVFD_SETDDRAMADDR);  // go to address 0

  // turn the display on with no cursor or blinking default
  samsungvfd_displaycontrol = SAMSUNGVFD_DISPLAYON; 
  samsungvfdDisplay();

  samsungvfdClear();
  samsungvfdHome();
}

/**************************************************************************/
/* Public Methods                                                         */
/**************************************************************************/

/**************************************************************************/
/*! 
    @brief Initialises the VFD character display
*/
/**************************************************************************/
void samsungvfdInit(uint8_t brightness)
{
  // Set all pins to output
  gpioSetDir(SAMSUNGVFD_SIO_PORT, SAMSUNGVFD_SIO_PIN, gpioDirection_Output);
  gpioSetDir(SAMSUNGVFD_STB_PORT, SAMSUNGVFD_STB_PIN, gpioDirection_Output);
  gpioSetDir(SAMSUNGVFD_SCK_PORT, SAMSUNGVFD_SCK_PIN, gpioDirection_Output);

  // Set strobe and clock pins high by default
  gpioSetValue(SAMSUNGVFD_STB_PORT, SAMSUNGVFD_STB_PIN, 1);
  gpioSetValue(SAMSUNGVFD_SCK_PORT, SAMSUNGVFD_SCK_PIN, 1);

  // Default to 2x20 display (SAMSUNG 20T202DA2JA)
  samsungvfd_begin(20, 2, brightness);
}

/**************************************************************************/
/*! 
    @brief Writes a single character to the VFD display
*/
/**************************************************************************/
void samsungvfdWrite(uint8_t value) 
{
  gpioSetValue(SAMSUNGVFD_STB_PORT, SAMSUNGVFD_STB_PIN, 0);
  samsungvfd_sendByte(SAMSUNGVFD_SPIDATA);
  samsungvfd_sendByte(value);
  gpioSetValue(SAMSUNGVFD_STB_PORT, SAMSUNGVFD_STB_PIN, 1);
}

/**************************************************************************/
/*! 
    @brief Writes a full string to the VFD display
*/
/**************************************************************************/
void samsungvfdWriteString(const char * str)
{
  while(*str)
  {
    gpioSetValue(SAMSUNGVFD_STB_PORT, SAMSUNGVFD_STB_PIN, 0);
    samsungvfd_sendByte(SAMSUNGVFD_SPIDATA);
    samsungvfd_sendByte(*str++);
    gpioSetValue(SAMSUNGVFD_STB_PORT, SAMSUNGVFD_STB_PIN, 1);
  }
}

/**************************************************************************/
/*! 
    @brief Sets the display brightness
*/
/**************************************************************************/
void samsungvfdSetBrightness(uint8_t brightness)
{
  // set the brightness (only if a valid value is passed
  if (brightness <= SAMSUNGVFD_BRIGHTNESS25) 
  {
    samsungvfd_displayfunction &= ~SAMSUNGVFD_BRIGHTNESS25;
    samsungvfd_displayfunction |= brightness;

    samsungvfd_command(SAMSUNGVFD_FUNCTIONSET | samsungvfd_displayfunction);
  }
}

/**************************************************************************/
/*! 
    @brief Quickly turn the display off
*/
/**************************************************************************/
void samsungvfdNoDisplay(void) 
{
  samsungvfd_displaycontrol &= ~SAMSUNGVFD_DISPLAYON;
  samsungvfd_command(SAMSUNGVFD_DISPLAYCONTROL | samsungvfd_displaycontrol);
}

/**************************************************************************/
/*! 
    @brief Quickly turn the display on
*/
/**************************************************************************/
void samsungvfdDisplay(void) 
{
  samsungvfd_displaycontrol |= SAMSUNGVFD_DISPLAYON;
  samsungvfd_command(SAMSUNGVFD_DISPLAYCONTROL | samsungvfd_displaycontrol);
}

/**************************************************************************/
/*! 
    @brief Turns the cursor off
*/
/**************************************************************************/
void samsungvfdNoCursor(void) 
{
  samsungvfd_displaycontrol &= ~SAMSUNGVFD_CURSORON;
  samsungvfd_command(SAMSUNGVFD_DISPLAYCONTROL | samsungvfd_displaycontrol);
}

/**************************************************************************/
/*! 
    @brief Enables the cursor
*/
/**************************************************************************/
void samsungvfdCursor(void) 
{
  samsungvfd_displaycontrol |= SAMSUNGVFD_CURSORON;
  samsungvfd_command(SAMSUNGVFD_DISPLAYCONTROL | samsungvfd_displaycontrol);
}

/**************************************************************************/
/*! 
    @brief Disable cursor blinking
*/
/**************************************************************************/
void samsungvfdNoBlink(void) 
{
  samsungvfd_displaycontrol &= ~SAMSUNGVFD_BLINKON;
  samsungvfd_command(SAMSUNGVFD_DISPLAYCONTROL | samsungvfd_displaycontrol);
}

/**************************************************************************/
/*! 
    @brief Enable cursor blinking
*/
/**************************************************************************/
void samsungvfdBlink(void) 
{
  samsungvfd_displaycontrol |= SAMSUNGVFD_BLINKON;
  samsungvfd_command(SAMSUNGVFD_DISPLAYCONTROL | samsungvfd_displaycontrol);
}

/**************************************************************************/
/*! 
    @brief Scroll the display left without changing the RAM
*/
/**************************************************************************/
void samsungvfdScrollDisplayLeft(void) 
{
  samsungvfd_command(SAMSUNGVFD_CURSORSHIFT | SAMSUNGVFD_DISPLAYMOVE | SAMSUNGVFD_MOVELEFT);
}

/**************************************************************************/
/*! 
    @brief Scroll the display right without changing the RAM
*/
/**************************************************************************/
void samsungvfdScrollDisplayRight(void) 
{
  samsungvfd_command(SAMSUNGVFD_CURSORSHIFT | SAMSUNGVFD_DISPLAYMOVE | SAMSUNGVFD_MOVERIGHT);
}

/**************************************************************************/
/*! 
    @brief Cause text to flow left to right
*/
/**************************************************************************/
void samsungvfdLeftToRight(void) 
{
  samsungvfd_displaymode |= SAMSUNGVFD_ENTRYLEFT;
  samsungvfd_command(SAMSUNGVFD_ENTRYMODESET | samsungvfd_displaymode);
}

/**************************************************************************/
/*! 
    @brief Cause text to flow from right to left
*/
/**************************************************************************/
void samsungvfdRightToLeft(void) 
{
  samsungvfd_displaymode &= ~SAMSUNGVFD_ENTRYLEFT;
  samsungvfd_command(SAMSUNGVFD_ENTRYMODESET | samsungvfd_displaymode);
}

/**************************************************************************/
/*! 
    @brief Right justify text from the cursor
*/
/**************************************************************************/
void samsungvfdAutoscroll(void) 
{
  samsungvfd_displaymode |= SAMSUNGVFD_ENTRYSHIFTINCREMENT;
  samsungvfd_command(SAMSUNGVFD_ENTRYMODESET | samsungvfd_displaymode);
}

/**************************************************************************/
/*! 
    @brief Left justify text from the cursor
*/
/**************************************************************************/
void samsungvfdNoAutoscroll(void) 
{
  samsungvfd_displaymode &= ~SAMSUNGVFD_ENTRYSHIFTINCREMENT;
  samsungvfd_command(SAMSUNGVFD_ENTRYMODESET | samsungvfd_displaymode);
}

/**************************************************************************/
/*! 
    @brief Clears the display
*/
/**************************************************************************/
void samsungvfdClear(void)
{
  samsungvfd_command(SAMSUNGVFD_CLEARDISPLAY);  // clear display, set cursor position to zero
  systickDelay(2);                              // this command takes a long time!
}

/**************************************************************************/
/*! 
    @brief Places the cursor at 0, 0
*/
/**************************************************************************/
void samsungvfdHome(void)
{
  samsungvfd_command(SAMSUNGVFD_RETURNHOME);    // Set cursor position to zero
  systickDelay(2);                              // this command takes a long time!
}

/**************************************************************************/
/*! 
    @brief Sets the cursor to the specified row and column position
*/
/**************************************************************************/
void samsungvfdSetCursor(uint8_t row, uint8_t col)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > samsungvfd_numlines ) 
  {
    row = samsungvfd_numlines-1;                // we count rows starting w/0
  }
  
  samsungvfd_command(SAMSUNGVFD_SETDDRAMADDR | (col + row_offsets[row]));
}

