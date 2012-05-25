/**************************************************************************/
/*!
    @file     hx8347d.h
    @author   Tauon {TauonTeilchen} Jabber ID Tauon[at]jabber.ccc.de

    @section  LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, TauonTeilchen 
    ----------------------------------------------------------------------------
    "THE BEER-WARE LICENSE" (Revision 42):
    JID: <Tauon@jabber.ccc.de> wrote this file. As long as you retain this notice you
    can do whatever you want with this stuff. If we meet some day, and you think
    this stuff is worth it, you can buy me a ClubMate in return Tauon
    ----------------------------------------------------------------------------
    ----------------------------------------------------------------------------
     "THE ClubMate-WARE LICENSE" (Revision 42):
     JID: <Tauon@jabber.ccc.de> schrieb diese Datei. Solange Sie diesen Vermerk nicht entfernen, koennen
     Sie mit dem Material machen, was Sie möechten. Wenn wir uns eines Tages treffen und Sie
     denken, das Material ist es wert, koennen Sie mir dafuer ein ClubMate ausgeben. Tauon
    ----------------------------------------------------------------------------
*/
/**************************************************************************/

/**************************************************************************/
// LCD BACK_LIGHT       Pin 6 of Port 2
// LCD CS               Pin 5 of Port 2
// LCD RESET            Pin 4 of Port 2
// More infos           https://github.com/watterott/MI0283QT-Adapter/raw/master/img/connecting-uno.jpg
//
/**************************************************************************/
#include "core/systick/systick.h"
#include "drivers/displays/tft/touchscreen.h"
#include "core/ssp/ssp.h"
#include "projectconfig.h"
#include "drivers/displays/tft/lcd.h"



// Screen width, height, has touchscreen, support orientation changes, support hw scrolling
static lcdOrientation_t hx8347dPOrientation = LCD_ORIENTATION_LANDSCAPE;

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


