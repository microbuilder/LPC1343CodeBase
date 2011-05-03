/**************************************************************************/
/*! 
    @file     main.c
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2011, microBuilder SARL
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
#include <stdio.h>

#include "projectconfig.h"
#include "sysinit.h"

#include "core/gpio/gpio.h"
#include "core/adc/adc.h"
#include "core/systick/systick.h"

#include "drivers/lcd/tft/lcd.h"
#include "drivers/lcd/tft/bmp.h"
#include "drivers/lcd/tft/drawing.h"
#include "drivers/lcd/tft/touchscreen.h"
#include "drivers/lcd/tft/fonts/dejavusans9.h"
#include "drivers/lcd/tft/fonts/dejavusansbold9.h"
#include "drivers/lcd/tft/fonts/dejavusansmono8.h"

#include "drivers/lcd/icons16.h"

// Color scheme
#define FONT_REGULAR              &dejaVuSans9ptFontInfo
#define FONT_BOLD                 &dejaVuSansBold9ptFontInfo
#define COL_BACKGROUND            COLOR_GRAY_80
#define COL_TEXT                  COLOR_WHITE

#define COL_BUTTON                COLOR_GRAY_50
#define COL_BUTTONBORDER          COLOR_GRAY_50
#define COL_BUTTONTEXT            COLOR_WHITE
#define COL_BUTTONACTIVE          COLOR_THEME_DEFAULT_BASE
#define COL_BUTTONACTIVEBORDER    COLOR_THEME_DEFAULT_DARKER
#define COL_BUTTONACTIVETEXT      COLOR_BLACK

#define COL_MENU                  COLOR_GRAY_30
#define COL_MENULIGHTER           COLOR_GRAY_50
#define COL_MENUTEXT              COLOR_WHITE
#define COL_MENUACTIVE            COLOR_THEME_DEFAULT_DARKER
#define COL_MENUACTIVELIGHTER     COLOR_THEME_DEFAULT_BASE
#define COL_MENUACTIVETEXT        COLOR_BLACK

/**************************************************************************/
/*! 
    Draws a single entry in the menu (adjusting the display depending
    on whether the item is currently active or not)
*/
/**************************************************************************/
void renderMenuItem(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t headerWidth, bool active, char* headerText, char* bodyText)
{
  drawRectangleRounded(x, y, x+headerWidth, y+height, active ? COL_MENUACTIVE : COL_MENU, 10, DRAW_ROUNDEDCORNERS_NONE);
  drawRectangleRounded(x+headerWidth, y, x+width, y+height, active ? COL_MENUACTIVELIGHTER : COL_MENULIGHTER, 10, DRAW_ROUNDEDCORNERS_NONE);
  drawString(x+10, y+height/2, active ? COL_MENUACTIVETEXT : COL_MENUTEXT, FONT_BOLD, headerText);
  drawString(x+headerWidth+25, y+height/2, active ? COL_MENUACTIVETEXT: COL_MENUTEXT, FONT_REGULAR, bodyText);

  if (active)
  {
    drawArrow(x+headerWidth+5, y+height/2, 7, DRAW_DIRECTION_LEFT, COL_MENUACTIVETEXT);
    drawArrow(x+width-5, y+height/2, 7, DRAW_DIRECTION_RIGHT, COL_MENUACTIVETEXT);
  }
}

/**************************************************************************/
/*! 
    Renders the 16x16 icons found in "/drivers/lcd/icons16.h"
*/
/**************************************************************************/
void renderIcons(void)
{
  // Cross/Failed
  drawRectangleRounded(10, 190, 30, 210, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(12, 192, COLOR_RED, icons16_failed);
  drawRectangleRounded(10, 220, 30, 240, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(12, 222, COLOR_RED, icons16_failed);
  drawIcon16(12, 222, COLOR_WHITE, icons16_failed_interior);
  drawRectangleRounded(10, 250, 30, 270, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(12, 252, COLOR_WHITE, icons16_failed_interior);

  // Alert
  drawRectangleRounded(40, 190, 60, 210, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(42, 192, COLOR_YELLOW, icons16_alert);
  drawRectangleRounded(40, 220, 60, 240, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(42, 222, COLOR_YELLOW, icons16_alert);
  drawIcon16(42, 222, COLOR_WHITE, icons16_alert_interior);
  drawRectangleRounded(40, 250, 60, 270, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(42, 252, COLOR_WHITE, icons16_alert_interior);
 
  // Checkmark/Passed
  drawRectangleRounded(70, 190, 90, 210, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(72, 192, COLOR_GREEN, icons16_passed);
  drawRectangleRounded(70, 220, 90, 240, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(72, 222, COLOR_GREEN, icons16_passed);
  drawIcon16(72, 222, COLOR_WHITE, icons16_passed_interior);
  drawRectangleRounded(70, 250, 90, 270, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(72, 252, COLOR_WHITE, icons16_passed_interior);
 
  // Info
  drawRectangleRounded(100, 190, 120, 210, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(102, 192, COLOR_BLUE, icons16_info);
  drawRectangleRounded(100, 220, 120, 240, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(102, 222, COLOR_BLUE, icons16_info);
  drawIcon16(102, 222, COLOR_WHITE, icons16_info_interior);
  drawRectangleRounded(100, 250, 120, 270, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(102, 252, COLOR_WHITE, icons16_info_interior);
 
  // Tools/Config
  drawRectangleRounded(130, 190, 150, 210, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(132, 192, COLOR_GREEN, icons16_tools);

  // Pointer
  drawRectangleRounded(160, 190, 180, 210, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(162, 192, COLOR_MAGENTA, icons16_pointer);
  drawRectangleRounded(160, 220, 180, 240, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(162, 222, COLOR_MAGENTA, icons16_pointer);
  drawIcon16(162, 222, COLOR_WHITE, icons16_pointer_dot);
  drawRectangleRounded(160, 250, 180, 270, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(162, 252, COLOR_WHITE, icons16_pointer_dot);
 
  // Tag
  drawRectangleRounded(190, 190, 210, 210, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(192, 192, COLOR_CYAN, icons16_tag);
  drawRectangleRounded(190, 220, 210, 240, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(192, 222, COLOR_CYAN, icons16_tag);
  drawIcon16(192, 222, COLOR_WHITE, icons16_tag_dot);
  drawRectangleRounded(190, 250, 210, 270, COL_BUTTON, 5, DRAW_ROUNDEDCORNERS_ALL);
  drawIcon16(192, 252, COLOR_WHITE, icons16_tag_dot);
}

/**************************************************************************/
/*! 
    Main program entry point.  After reset, normal code execution will
    begin here.
*/
/**************************************************************************/
int main(void)
{
  #if !defined CFG_TFTLCD
    #error "CFG_TFTLCD must be enabled in projectconfig.h for this test"
  #endif

  // Configure cpu and mandatory peripherals
  systemInit();
  
  // Background
  drawFill(COL_BACKGROUND);

  // Top/bottom action bars
  drawRectangleFilled(0, 0, 239, 19, COL_MENU);
  drawRectangleFilled(0, 280, 239, 319, COL_MENU);

  // Menu
  drawRectangleRounded(6, 30, 232, 160, COL_MENU, 10, DRAW_ROUNDEDCORNERS_ALL);
  drawString(20, 45, COL_MENUTEXT, FONT_BOLD, "SYSTEM SETTINGS");
  renderMenuItem(8,  65, 222, 23, 90, false,  "LANGUAGE", "English");
  renderMenuItem(8,  90, 222, 23, 90, false, "TIMEZONE", "GMT+1");
  renderMenuItem(8, 115, 222, 23, 90, true, "SLEEP", "5 Minutes");

  // Progress bar
  drawProgressBar (70, 165, 160, 15, DRAW_ROUNDEDCORNERS_ALL, DRAW_ROUNDEDCORNERS_ALL, COL_MENU, COL_MENULIGHTER, COL_MENUACTIVE, COL_MENUACTIVELIGHTER, 72 );

  // Render some icons
  renderIcons();

  // Action bar buttons
  drawButton(5, 285, 75, 29, FONT_BOLD, 7, COL_BUTTONBORDER, COL_BUTTON, COL_BUTTONTEXT, "CANCEL");
  drawButton(160, 285, 75, 29, FONT_BOLD, 7, COL_BUTTONBORDER, COL_BUTTON, COL_BUTTONTEXT, "SAVE");

  tsTouchData_t touch;
  tsTouchError_t error;

  // Draw pixels whenever a touch screen event is detected
  while (1)
  {
    // Wait for a valid touch event
    error = tsWaitForEvent(&touch, 0);
    if (!error)
    {
      drawPixel(touch.xlcd, touch.ylcd, COLOR_WHITE);
    }
  }

  return 0;
}
