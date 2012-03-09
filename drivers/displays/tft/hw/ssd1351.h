/**************************************************************************/
/*! 
    @file     ssd1351.h
    @author   K. Townsend (www.adafruit.com)

    @section LICENSE

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
#ifndef __SSD1351_H__
#define __SSD1351_H__

#include "projectconfig.h"
#include "drivers/displays/tft/lcd.h"
#include "core/gpio/gpio.h"

/*=========================================================================
    SPI modes
    -----------------------------------------------------------------------
    The OLED supports both 3-pin and 4-pin SPI modes

    3-PIN MODE      Saves one GPIO pin (D/C) by adding the D/C bit before
                    every transfer, meaning that 9 bits are sent every frame
                    instead of 8.  You have slightly slower performance
                    but use less pins. Requires 3 GPIO pins (SCK, SID, RST)

    4-PIN MODE      Uses a normal SPI interface with 8-bits per transfer,
                    plus a dedicated D/C pin to indicate if this is a
                    command or data byte. Requires 4 GPIO pins (SCK, SID,
                    RST, DC).

    To select one of the SPI modes, make sure the BS0/1 pins are correctly
    set on the OLED display and uncomment the appropriate define below.
    -----------------------------------------------------------------------*/
    // #define SSD1351_BUS_SPI3
    #define SSD1351_BUS_SPI4
/*=========================================================================*/

// Control pins
#define SSD1351_SCK_PORT                (2) // SCK (D0)
#define SSD1351_SCK_PIN                 (1)
#define SSD1351_SID_PORT                (2) // DAT/MOSI (D1)
#define SSD1351_SID_PIN                 (2)
#define SSD1351_CS_PORT                 (2) // OLEDCS
#define SSD1351_CS_PIN                  (3)
#define SSD1351_RST_PORT                (2) // RST
#define SSD1351_RST_PIN                 (4)
#define SSD1351_DC_PORT                 (2) // D/C (only required for 4-pin SPI)
#define SSD1351_DC_PIN                  (5)

// Placed here to try to keep all pin specific values in the header file
#define SSD1351_DISABLEPULLUPS()  do {  gpioSetPullup(&IOCON_PIO2_1, gpioPullupMode_Inactive); \
                                        gpioSetPullup(&IOCON_PIO2_2, gpioPullupMode_Inactive); \
                                        gpioSetPullup(&IOCON_PIO2_3, gpioPullupMode_Inactive); \
                                        gpioSetPullup(&IOCON_PIO2_4, gpioPullupMode_Inactive); \
                                        gpioSetPullup(&IOCON_PIO2_5, gpioPullupMode_Inactive); } while (0)

// These registers allow fast single cycle clear+set of bits (see section 8.5.1 of LPC1343 UM)
#define SSD1351_GPIO2DATA_SCK           (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1351_SCK_PIN) << 2))))
#define SSD1351_GPIO2DATA_SID           (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1351_SID_PIN) << 2))))
#define SSD1351_GPIO2DATA_CS            (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1351_CS_PIN) << 2))))
#define SSD1351_GPIO2DATA_RST           (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1351_RST_PIN) << 2))))
#define SSD1351_GPIO2DATA_DC            (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1351_DC_PIN) << 2))))

// Macros for control line state
#define CLR_SCK                         SSD1351_GPIO2DATA_SCK = (0)
#define SET_SCK                         SSD1351_GPIO2DATA_SCK = (1 << SSD1351_SCK_PIN)
#define CLR_SID                         SSD1351_GPIO2DATA_SID = (0)
#define SET_SID                         SSD1351_GPIO2DATA_SID = (1 << SSD1351_SID_PIN)
#define CLR_CS                          SSD1351_GPIO2DATA_CS  = (0)
#define SET_CS                          SSD1351_GPIO2DATA_CS  = (1 << SSD1351_CS_PIN)
#define CLR_RST                         SSD1351_GPIO2DATA_RST = (0)
#define SET_RST                         SSD1351_GPIO2DATA_RST = (1 << SSD1351_RST_PIN)
#define CLR_DC                          SSD1351_GPIO2DATA_DC  = (0)
#define SET_DC                          SSD1351_GPIO2DATA_DC  = (1 << SSD1351_DC_PIN)

// SSD1351 Commands
enum
{
  SSD1351_CMD_SETCOLUMNADDRESS          = 0x15,
  SSD1351_CMD_SETROWADDRESS             = 0x75,
  SSD1351_CMD_WRITERAM                  = 0x5C, // Write data to GRAM and increment until another command is sent
  SSD1351_CMD_READRAM                   = 0x5D, // Read data from GRAM and increment until another command is sent
  SSD1351_CMD_COLORDEPTH                = 0xA0, // Numerous functions include increment direction ... see DS 
                                                // A0[0] = Address Increment Mode (0 = horizontal, 1 = vertical)
                                                // A0[1] = Column Address Remap (0 = left to right, 1 = right to left)
                                                // A0[2] = Color Remap (0 = ABC, 1 = CBA) - HW RGB/BGR switch
                                                // A0[4] = COM Scan Direction (0 = top to bottom, 1 = bottom to top)
                                                // A0[5] = Odd/Even Paid Split
                                                // A0[7:6] = Display Color Mode (00 = 8-bit, 01 = 65K, 10/11 = 262K, 8/16-bit interface only)
  SSD1351_CMD_SETDISPLAYSTARTLINE       = 0xA1,
  SSD1351_CMD_SETDISPLAYOFFSET          = 0xA2, 
  SSD1351_CMD_SETDISPLAYMODE_ALLOFF     = 0xA4, // Force entire display area to grayscale GS0
  SSD1351_CMD_SETDISPLAYMODE_ALLON      = 0xA5, // Force entire display area to grayscale GS63
  SSD1351_CMD_SETDISPLAYMODE_RESET      = 0xA6, // Resets the display area relative to the above two commands
  SSD1351_CMD_SETDISPLAYMODE_INVERT     = 0xA7, // Inverts the display contents (GS0 -> GS63, GS63 -> GS0, etc.)
  SSD1351_CMD_FUNCTIONSELECTION         = 0xAB, // Enable/Disable the internal VDD regulator
  SSD1351_CMD_SLEEPMODE_DISPLAYOFF      = 0xAE, // Sleep mode on (display off)
  SSD1351_CMD_SLEEPMODE_DISPLAYON       = 0xAF, // Sleep mode off (display on)
  SSD1351_CMD_SETPHASELENGTH            = 0xB1, // Larger capacitance may require larger delay to discharge previous pixel state
  SSD1351_CMD_ENHANCEDDRIVINGSCHEME     = 0xB2, 
  SSD1351_CMD_SETFRONTCLOCKDIV          = 0xB3, // DCLK divide ration fro CLK (from 1 to 16)
  SSD1351_CMD_SETSEGMENTLOWVOLTAGE      = 0xB4,
  SSD1351_CMD_SETGPIO                   = 0xB5,
  SSD1351_CMD_SETSECONDPRECHARGEPERIOD  = 0xB6,
  SSD1351_CMD_GRAYSCALELOOKUP           = 0xB8,
  SSD1351_CMD_LINEARLUT                 = 0xB9,
  SSD1351_CMD_SETPRECHARGEVOLTAGE       = 0xBB,
  SSD1351_CMD_SETVCOMHVOLTAGE           = 0xBE,
  SSD1351_CMD_SETCONTRAST               = 0xC1,
  SSD1351_CMD_MASTERCONTRAST            = 0xC7,
  SSD1351_CMD_SETMUXRRATIO              = 0xCA,
  SSD1351_CMD_NOP3                      = 0xD1,
  SSD1351_CMD_NOP4                      = 0xE3,
  SSD1351_CMD_SETCOMMANDLOCK            = 0xFD,
  SSD1351_CMD_HORIZONTALSCROLL          = 0x96,
  SSD1351_CMD_STOPMOVING                = 0x9E,
  SSD1351_CMD_STARTMOVING               = 0x9F  
};

#endif
