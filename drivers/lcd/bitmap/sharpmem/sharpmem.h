/**************************************************************************/
/*! 
    @file     sharpmem.h
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
#ifndef __SHARPMEM_H__
#define __SHARPMEM_H__

#include "projectconfig.h"
#include "drivers/lcd/smallfonts.h"

/**************************************************************************
    Sharp Memory Display Connector
    -----------------------------------------------------------------------
    Pin   Function        Notes
    ===   ==============  ===============================
      1   VIN             1.0-5.0V (Boost Supply)
      2   VDD             5.0V (Boost output, or 5V supply)
      3   GND
      4   SCLK            Serial Clock
      5   MOSI            Serial Data Input
      6   CS              Serial Chip Select
      7   EXTCOMIN        External COM Inversion Signal
      8   DISP            Display On(High)/Off(Low)
      9   EXTMODE         COM Inversion Select (Low = SW clock/serial)

 **************************************************************************/

// LCD Dimensions
#define SHARPMEM_LCDWIDTH       (96)
#define SHARPMEM_LCDHEIGHT      (96) 

// Control pins
#define SHARPMEM_GPIODATAREG     (*(pREG32 (0x50023FFC)))   // GPIO2DATA
#define SHARPMEM_PORT            (2)
#define SHARPMEM_SCLK_PIN        (1)
#define SHARPMEM_MOSI_PIN        (2)
#define SHARPMEM_CS_PIN          (3)
#define SHARPMEM_DISP_PIN        (4)

// Macros for control line state
#define CLR_SCLK    do { SHARPMEM_GPIODATAREG &= ~(1<<SHARPMEM_SCLK_PIN); } while(0)
#define SET_SCLK    do { SHARPMEM_GPIODATAREG &= ~(1<<SHARPMEM_SCLK_PIN); SHARPMEM_GPIODATAREG |= (1<<SHARPMEM_SCLK_PIN); } while(0)
#define CLR_MOSI    do { SHARPMEM_GPIODATAREG &= ~(1<<SHARPMEM_MOSI_PIN); } while(0)
#define SET_MOSI    do { SHARPMEM_GPIODATAREG &= ~(1<<SHARPMEM_MOSI_PIN); SHARPMEM_GPIODATAREG |= (1<<SHARPMEM_MOSI_PIN); } while(0)
#define CLR_CS      do { SHARPMEM_GPIODATAREG &= ~(1<<SHARPMEM_CS_PIN); } while(0)
#define SET_CS      do { SHARPMEM_GPIODATAREG &= ~(1<<SHARPMEM_CS_PIN); SHARPMEM_GPIODATAREG |= (1<<SHARPMEM_CS_PIN); } while(0)
#define CLR_DISP    do { SHARPMEM_GPIODATAREG &= ~(1<<SHARPMEM_DISP_PIN); } while(0)
#define SET_DISP    do { SHARPMEM_GPIODATAREG &= ~(1<<SHARPMEM_DISP_PIN); SHARPMEM_GPIODATAREG |= (1<<SHARPMEM_DISP_PIN); } while(0)

#define SHARPMEM_BIT_WRITECMD   (0x80)
#define SHARPMEM_BIT_VCOM       (0x40)
#define SHARPMEM_BIT_CLEAR      (0x20)

// Method Prototypes
void    sharpmemInit(void);
void    sharpmemEnable(bool enable);
void    sharpmemDrawPixel(uint16_t x, uint16_t y);
void    sharpmemClearPixel(uint16_t x, uint16_t y);
uint8_t sharpmemGetPixel(uint16_t x, uint16_t y);
void    sharpmemClearScreen();
void    sharpmemRefresh(void);
void    sharpmemDrawString(uint16_t x, uint16_t y, const char* text, struct FONT_DEF font);

#endif
