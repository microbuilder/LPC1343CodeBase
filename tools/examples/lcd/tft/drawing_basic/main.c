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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "projectconfig.h"
#include "sysinit.h"

#include "core/gpio/gpio.h"
#include "core/systick/systick.h"

#include "drivers/lcd/tft/lcd.h"
#include "drivers/lcd/tft/drawing.h"
#include "drivers/lcd/tft/touchscreen.h"
#include "drivers/lcd/tft/dialogues/alphanumeric.h"

#include "drivers/lcd/tft/fonts/dejavusans9.h"
#include "drivers/lcd/tft/fonts/dejavusansbold9.h"
#include "drivers/lcd/tft/fonts/dejavusansmono8.h"

/**************************************************************************/
/*! 
    Main program entry point.  After reset, normal code execution will
    begin here.
*/
/**************************************************************************/
int main(void)
{
  // Configure cpu and mandatory peripherals
  systemInit();

  #if !defined CFG_TFTLCD
    #error "CFG_TFTLCD must be enabled in projectconfig.h for this test"
  #endif
  #if defined CFG_INTERFACE
    #error "CFG_INTERFACE must be disabled in projectconfig.h for this test (to save space)"
  #endif
  
  // Clear the screen
  // ---------------------------------------------------------------------
  drawFill(COLOR_WHITE);

  // Render some text using DejaVu Sans 9 and Sans Mono 8
  // ---------------------------------------------------------------------
  drawString(5, 10, COLOR_BLACK, &dejaVuSansBold9ptFontInfo, "DejaVu Sans 9 Bold");
  drawString(5, 30, COLOR_BLACK, &dejaVuSans9ptFontInfo, "DejaVu Sans 9");
  drawString(5, 50, COLOR_BLACK, &dejaVuSansMono8ptFontInfo, "DejaVu Sans Mono 8");

  // Change the LCD orientation to render text horizontally
  // ---------------------------------------------------------------------
  lcdProperties_t lcdProperties = lcdGetProperties();
  // Check if the screen orientation can be changed
  if (lcdProperties.orientation)
  {
    // Change the orientation
    lcdSetOrientation(lcdGetOrientation() == LCD_ORIENTATION_PORTRAIT ? 
      LCD_ORIENTATION_LANDSCAPE : LCD_ORIENTATION_PORTRAIT);
    // Render some text in the new orientation
    drawString(5, 10, COLOR_BLACK, &dejaVuSans9ptFontInfo, "DejaVu Sans 9 (Rotated)");
    // Change the orientation back
    lcdSetOrientation(lcdGetOrientation() == LCD_ORIENTATION_PORTRAIT ? 
      LCD_ORIENTATION_LANDSCAPE : LCD_ORIENTATION_PORTRAIT);
  }

  // Draw some primitive shapes
  // ---------------------------------------------------------------------
  drawLine(5, 65, 200, 65, COLOR_RED);
  drawLine(5, 67, 200, 67, COLOR_GREEN);
  drawLine(5, 69, 200, 69, COLOR_BLUE);
  drawCircleFilled(30, 105, 25, COLOR_BLACK);
  drawCircleFilled(30, 105, 23, drawRGB24toRGB565(0x33, 0x00, 0x00));
  drawCircleFilled(30, 105, 19, drawRGB24toRGB565(0x66, 0x00, 0x00));
  drawCircleFilled(30, 105, 15, drawRGB24toRGB565(0x99, 0x00, 0x00));
  drawCircleFilled(30, 105, 11, drawRGB24toRGB565(0xCC, 0x00, 0x00));
  drawCircleFilled(30, 105,  7, drawRGB24toRGB565(0xFF, 0x00, 0x00));
  drawRectangleFilled( 80,  80, 180, 125, COLOR_DARKERGRAY);
  drawRectangleFilled( 85,  85, 175, 120, COLOR_DARKGRAY);
  drawRectangleFilled( 90,  90, 170, 115, COLOR_MEDIUMGRAY);
  drawRectangleFilled( 95,  95, 165, 110, COLOR_LIGHTGRAY);
  drawRectangleFilled(100, 100, 160, 105, COLOR_PALEGRAY);
  
  // Draw some compound shapes
  // ---------------------------------------------------------------------
  drawProgressBar(70, 140, 75, 12, COLOR_BLACK, COLOR_MEDIUMGRAY, 78);
  drawString(5, 144, COLOR_BLACK, &dejaVuSansBold9ptFontInfo, "Progress");
  drawString(155, 144, COLOR_BLACK, &dejaVuSans9ptFontInfo, "78%");
  drawRectangleFilled(0, 175, 239, 210, COLOR_DARKERGRAY);
  drawButton(20, 180, 200, 25, &dejaVuSans9ptFontInfo, 7, "Click For Text Entry", false);

  // Wait for a valid touch event
  // ---------------------------------------------------------------------
  tsTouchData_t data;
  tsTouchError_t error = -1;
  bool success = false;
  while(!success)
  {
    // Wait forever for a valid touch event to occur (ignoring faulty readings)
    while (error)
    {
      // Only exit this loop when '0' is returned
      error = tsWaitForEvent(&data, 0);
      // printf("Error: %d X: %u Y: %u \r\n", error, data.x, data.y);
    }

    // Reset error to an error state in case we got a valid reading, but it's not
    // within the expected range
    error = -1;

    // Check if the captured touch event is within the specified X/Y range
    if (data.x > 20 && data.x < 220 && data.y > 180 && data.y < 205)
    {
      // Wait a few milliseconds then display the text input dialogue
      systickDelay(100);
      char* results = alphaShowDialogue();
      // At this point, results contains the text from the dialogue ... 
      // clear the screen and show the results
      drawFill(COLOR_WHITE);
      drawString(10, 10, COLOR_BLACK, &dejaVuSans9ptFontInfo, "You Entered:");
      drawString(10, 30, COLOR_BLACK, &dejaVuSansBold9ptFontInfo, results);
      drawString(10, 155, COLOR_BLACK, &dejaVuSans9ptFontInfo, "Thanks ... starting blinky!");
      // Setting success to true allow the code to move in to blinky
      success = true;
    }
  }

  uint32_t currentSecond, lastSecond;
  currentSecond = lastSecond = 0;

  while (1)
  {
    // Toggle LED once per second ... rollover = 136 years :)
    currentSecond = systickGetSecondsActive();
    if (currentSecond != lastSecond)
    {
      lastSecond = currentSecond;
      if (gpioGetValue(CFG_LED_PORT, CFG_LED_PIN) == CFG_LED_OFF)
      {
        gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_ON); 
      }
      else
      {
        gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_OFF); 
      }
    }
  }

  return 0;
}
