/**************************************************************************/
/*! 
    @file     ST7565.h
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
#ifndef __ST7565_H__
#define __ST7565_H__

#include "projectconfig.h"

#include "drivers/lcd/smallfonts.h"

// Pin Definitions
#define ST7565_A0_PORT                    (2)     // Register Select Pin (A0)
#define ST7565_A0_PIN                     (1)
#define ST7565_RST_PORT                   (2)     // Reset
#define ST7565_RST_PIN                    (2)
#define ST7565_CS_PORT                    (2)     // Select
#define ST7565_CS_PIN                     (3)
#define ST7565_BL_PORT                    (2)     // Backlight
#define ST7565_BL_PIN                     (4)
#define ST7565_SCLK_PORT                  (2)     // Serial Clock
#define ST7565_SCLK_PIN                   (5)
#define ST7565_SDAT_PORT                  (2)     // Serial Data
#define ST7565_SDAT_PIN                   (6)

// Commands
#define ST7565_CMD_DISPLAY_OFF            0xAE
#define ST7565_CMD_DISPLAY_ON             0xAF
#define ST7565_CMD_SET_DISP_START_LINE    0x40
#define ST7565_CMD_SET_PAGE               0xB0
#define ST7565_CMD_SET_COLUMN_UPPER       0x10
#define ST7565_CMD_SET_COLUMN_LOWER       0x00
#define ST7565_CMD_SET_ADC_NORMAL         0xA0
#define ST7565_CMD_SET_ADC_REVERSE        0xA1
#define ST7565_CMD_SET_DISP_NORMAL        0xA6
#define ST7565_CMD_SET_DISP_REVERSE       0xA7
#define ST7565_CMD_SET_ALLPTS_NORMAL      0xA4
#define ST7565_CMD_SET_ALLPTS_ON          0xA5
#define ST7565_CMD_SET_BIAS_9             0xA2 
#define ST7565_CMD_SET_BIAS_7             0xA3
#define ST7565_CMD_RMW                    0xE0
#define ST7565_CMD_RMW_CLEAR              0xEE
#define ST7565_CMD_INTERNAL_RESET         0xE2
#define ST7565_CMD_SET_COM_NORMAL         0xC0
#define ST7565_CMD_SET_COM_REVERSE        0xC8
#define ST7565_CMD_SET_POWER_CONTROL      0x28
#define ST7565_CMD_SET_RESISTOR_RATIO     0x20
#define ST7565_CMD_SET_VOLUME_FIRST       0x81
#define ST7565_CMD_SET_VOLUME_SECOND      0
#define ST7565_CMD_SET_STATIC_OFF         0xAC
#define ST7565_CMD_SET_STATIC_ON          0xAD
#define ST7565_CMD_SET_STATIC_REG         0x0
#define ST7565_CMD_SET_BOOSTER_FIRST      0xF8
#define ST7565_CMD_SET_BOOSTER_234        0
#define ST7565_CMD_SET_BOOSTER_5          1
#define ST7565_CMD_SET_BOOSTER_6          3
#define ST7565_CMD_NOP                    0xE3
#define ST7565_CMD_TEST                   0xF0

// Initialisation/Config Prototypes
void st7565Init( void );
void st7565Command( uint8_t c );
void st7565Data( uint8_t d );
void st7565SetBrightness( uint8_t val );

// Backlight Prototypes
void st7565Backlight(bool state);

// Drawing Prototypes
void st7565ClearScreen( void );
void st7565Refresh( void );
void st7565DrawPixel( uint8_t x, uint8_t y );
void st7565ClearPixel( uint8_t x, uint8_t y );
uint8_t st7565GetPixel( uint8_t x, uint8_t y );
void st7565DrawString( uint16_t x, uint16_t y, char* text, struct FONT_DEF font );
void st7565ShiftFrameBuffer( uint8_t pixels );

#endif
