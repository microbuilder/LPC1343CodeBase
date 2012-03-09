/**************************************************************************/
/*! 
    @file     ssd1331.h
    @author   K. Townsend (microBuilder.eu)

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
#ifndef __SSD1331_H__
#define __SSD1331_H__

#include "projectconfig.h"

#include "drivers/displays/tft/lcd.h"
#include "core/gpio/gpio.h"

// Select one of these defines to set the pixel color order
#define SSD1331_COLORORDER_RGB
// #define SSD1331_COLORORDER_BGR

#if defined SSD1331_COLORORDER_RGB && defined SSD1331_COLORORDER_BGR
  #error "RGB and BGR can not both be defined for SSD1331_COLORODER."
#endif

// Control pins
#define SSD1331_SID_PORT          2     // DAT
#define SSD1331_SID_PIN           1
#define SSD1331_SCK_PORT          2     // SCK
#define SSD1331_SCK_PIN           2
#define SSD1331_DC_PORT           2     // D/C
#define SSD1331_DC_PIN            3
#define SSD1331_RST_PORT          2     // RST
#define SSD1331_RST_PIN           4
#define SSD1331_CS_PORT           2     // OLEDCS
#define SSD1331_CS_PIN            5

// Placed here to try to keep all pin specific values in header file
#define SSD1331_DISABLEPULLUPS() do { gpioSetPullup(&IOCON_PIO2_1, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_2, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_3, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_4, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_5, gpioPullupMode_Inactive); } while (0)

// These registers allow fast single operation clear+set of bits (see section 8.5.1 of LPC1343 UM)
#define SSD1331_GPIO2DATA_SID         (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1331_SID_PIN) << 2))))
#define SSD1331_GPIO2DATA_SCK         (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1331_SCK_PIN) << 2))))
#define SSD1331_GPIO2DATA_DC          (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1331_DC_PIN) << 2))))
#define SSD1331_GPIO2DATA_RST         (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1331_RST_PIN) << 2))))
#define SSD1331_GPIO2DATA_CS          (*(pREG32 (GPIO_GPIO2_BASE + ((1 << SSD1331_CS_PIN) << 2))))

// Macros for control line state
#define CLR_DC          SSD1331_GPIO2DATA_DC = (0)
#define SET_DC          SSD1331_GPIO2DATA_DC = (1 << SSD1331_DC_PIN)
#define CLR_RST         SSD1331_GPIO2DATA_RST = (0)
#define SET_RST         SSD1331_GPIO2DATA_RST = (1 << SSD1331_RST_PIN)
#define CLR_CS          SSD1331_GPIO2DATA_CS = (0)
#define SET_CS          SSD1331_GPIO2DATA_CS = (1 << SSD1331_CS_PIN)
#define CLR_SCK         SSD1331_GPIO2DATA_SCK = (0)
#define SET_SCK         SSD1331_GPIO2DATA_SCK = (1 << SSD1331_SCK_PIN)
#define CLR_SID         SSD1331_GPIO2DATA_SID = (0)
#define SET_SID         SSD1331_GPIO2DATA_SID = (1 << SSD1331_SID_PIN)

enum
{
  SSD1331_CMD_DRAWLINE 		= 0x21,
  SSD1331_CMD_DRAWRECT 		= 0x22,
  SSD1331_CMD_FILL 		= 0x26,
  SSD1331_CMD_SETCOLUMN 	= 0x15,
  SSD1331_CMD_SETROW    	= 0x75,
  SSD1331_CMD_CONTRASTA 	= 0x81,
  SSD1331_CMD_CONTRASTB 	= 0x82,
  SSD1331_CMD_CONTRASTC		= 0x83,
  SSD1331_CMD_MASTERCURRENT 	= 0x87,
  SSD1331_CMD_SETREMAP 		= 0xA0,
  SSD1331_CMD_STARTLINE 	= 0xA1,
  SSD1331_CMD_DISPLAYOFFSET 	= 0xA2,
  SSD1331_CMD_NORMALDISPLAY 	= 0xA4,
  SSD1331_CMD_DISPLAYALLON  	= 0xA5,
  SSD1331_CMD_DISPLAYALLOFF 	= 0xA6,
  SSD1331_CMD_INVERTDISPLAY 	= 0xA7,
  SSD1331_CMD_SETMULTIPLEX  	= 0xA8,
  SSD1331_CMD_SETMASTER 	= 0xAD,
  SSD1331_CMD_DISPLAYOFF 	= 0xAE,
  SSD1331_CMD_DISPLAYON     	= 0xAF,
  SSD1331_CMD_POWERMODE 	= 0xB0,
  SSD1331_CMD_PRECHARGE 	= 0xB1,
  SSD1331_CMD_CLOCKDIV 		= 0xB3,
  SSD1331_CMD_PRECHARGEA 	= 0x8A,
  SSD1331_CMD_PRECHARGEB 	= 0x8B,
  SSD1331_CMD_PRECHARGEC 	= 0x8C,
  SSD1331_CMD_PRECHARGELEVEL 	= 0xBB,
  SSD1331_CMD_VCOMH 		= 0xBE
};

#endif
