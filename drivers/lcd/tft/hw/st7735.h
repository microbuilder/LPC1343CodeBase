/**************************************************************************/
/*! 
    @file     ST7735.h
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
#ifndef __ST7735_H__
#define __ST7735_H__

#include "projectconfig.h"
#include "drivers/lcd/tft/lcd.h"

/**************************************************************************
    ST7735 CONNECTOR
    -----------------------------------------------------------------------
    Pin   Function        Notes
    ===   ==============  ===============================
      1   NC
      2   GND
      3   LED K/-
      4   LED A/+         3.0V
      5   GND
      6   RESET
      7   RS
      8   SDA             Serial Data
      9   SCL             Serial Clock
     10   VCC             2.8-3.4V
     11   VCC             2.8-3.4V
     12   CS
     13   GND
     14   NC

 **************************************************************************/

// Control pins
#define ST7735_GPIODATAREG     (*(pREG32 (0x50023FFC)))   // GPIO2DATA
#define ST7735_PORT            (2)
#define ST7735_RS_PIN          (1)
#define ST7735_SDA_PIN         (2)
#define ST7735_SCL_PIN         (3)
#define ST7735_CS_PIN          (4)
#define ST7735_RES_PIN         (5)
#define ST7735_BL_PIN          (6)

// Macros for control line state
#define CLR_RS      do { ST7735_GPIODATAREG &= ~(1<<ST7735_RS_PIN); } while(0)
#define SET_RS      do { ST7735_GPIODATAREG &= ~(1<<ST7735_RS_PIN); ST7735_GPIODATAREG |= (1<<ST7735_RS_PIN); } while(0)
#define CLR_SDA     do { ST7735_GPIODATAREG &= ~(1<<ST7735_SDA_PIN); } while(0)
#define SET_SDA     do { ST7735_GPIODATAREG &= ~(1<<ST7735_SDA_PIN); ST7735_GPIODATAREG |= (1<<ST7735_SDA_PIN); } while(0)
#define CLR_SCL     do { ST7735_GPIODATAREG &= ~(1<<ST7735_SCL_PIN); } while(0)
#define SET_SCL     do { ST7735_GPIODATAREG &= ~(1<<ST7735_SCL_PIN); ST7735_GPIODATAREG |= (1<<ST7735_SCL_PIN); } while(0)
#define CLR_CS      do { ST7735_GPIODATAREG &= ~(1<<ST7735_CS_PIN); } while(0)
#define SET_CS      do { ST7735_GPIODATAREG &= ~(1<<ST7735_CS_PIN); ST7735_GPIODATAREG |= (1<<ST7735_CS_PIN); } while(0)
#define CLR_RES     do { ST7735_GPIODATAREG &= ~(1<<ST7735_RES_PIN); } while(0)
#define SET_RES     do { ST7735_GPIODATAREG &= ~(1<<ST7735_RES_PIN); ST7735_GPIODATAREG |= (1<<ST7735_RES_PIN); } while(0)
#define CLR_BL      do { ST7735_GPIODATAREG &= ~(1<<ST7735_BL_PIN); } while(0)
#define SET_BL      do { ST7735_GPIODATAREG &= ~(1<<ST7735_BL_PIN); ST7735_GPIODATAREG |= (1<<ST7735_BL_PIN); } while(0)

#define ST7735_NOP      (0x0)
#define ST7735_SWRESET  (0x01)
#define ST7735_SLPIN    (0x10)
#define ST7735_SLPOUT   (0x11)
#define ST7735_PTLON    (0x12)
#define ST7735_NORON    (0x13)
#define ST7735_INVOFF   (0x20)
#define ST7735_INVON    (0x21)
#define ST7735_DISPON   (0x29)
#define ST7735_CASET    (0x2A)
#define ST7735_RASET    (0x2B)
#define ST7735_RAMWR    (0x2C)
#define ST7735_COLMOD   (0x3A)
#define ST7735_MADCTL   (0x36)
#define ST7735_FRMCTR1  (0xB1)
#define ST7735_INVCTR   (0xB4)
#define ST7735_DISSET5  (0xB6)
#define ST7735_PWCTR1   (0xC0)
#define ST7735_PWCTR2   (0xC1)
#define ST7735_PWCTR3   (0xC2)
#define ST7735_VMCTR1   (0xC5)
#define ST7735_PWCTR6   (0xFC)
#define ST7735_GMCTRP1  (0xE0)
#define ST7735_GMCTRN1  (0xE1)

#endif
