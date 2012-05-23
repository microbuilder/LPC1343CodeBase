/**************************************************************************/
/*!
    @file     hx8347d.h
    @author   TauonTeilchen

    @section  DESCRIPTION
    Is written for MI0283QT-2 LCD from watterott.com
    More infos: http://www.watterott.com/de/MI0283QT-2-Adapter
    http://www.watterott.com/index.php?page=product&info=1597&dl_media=3202

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

/**************************************************************************/
// LCD BACK_LIGHT       Pin 6 of Port 2
// LCD CS               Pin 5 of Port 2
// LCD RESET            Pin 4 of Port 2
// Morr infos           https://github.com/watterott/MI0283QT-Adapter/raw/master/img/connecting-uno.jpg
//
/**************************************************************************/
#include "core/systick/systick.h"
#include "drivers/displays/tft/touchscreen.h"
#include "core/ssp/ssp.h"
#include "projectconfig.h"
#include "drivers/displays/tft/lcd.h"



// Screen width, height, has touchscreen, support orientation changes, support hw scrolling
static lcdOrientation_t hx8347dPOrientation = LCD_ORIENTATION_PORTRAIT;

// Screen width, height, has touchscreen, support orientation changes, support hw scrolling
static lcdProperties_t hx8347dProperties = { 320, 240, false, true, true };

/*************************************************/
/* Private Methods                               */
/*************************************************/


/*************************************************/
/* Public Methods                                */
/*************************************************/

// The following functions add need to be written to match the generic
// lcd interface defined by lcd.h

/**************************************************************************/
/*!
    @brief  Configures any pins or HW and initialises the LCD controller
*/
/**************************************************************************/
uint16_t pwmDutyCycleLCDBacklight = 50;
void lcdInit(void);
void lcdBacklight(bool state);
void lcdTest(void);
void lcdFillRGB(uint16_t data);
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color);
void lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t len);
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color);
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color);
uint16_t lcdGetPixel(uint16_t x, uint16_t y);
void lcdSetOrientation(lcdOrientation_t orientation);
lcdOrientation_t lcdGetOrientation(void);
uint16_t lcdGetWidth(void);
uint16_t lcdGetHeight(void);
void lcdScroll(int16_t pixels, uint16_t fillColor);
uint16_t lcdGetControllerID(void);
lcdProperties_t lcdGetProperties(void);

void hx8347d_Scroll(uint16_t tfa,uint16_t vsa,uint16_t bfa, uint16_t vsp);
void hx8347d_Standby(bool deep);
void hx8347d_Wakeup(bool deep);


