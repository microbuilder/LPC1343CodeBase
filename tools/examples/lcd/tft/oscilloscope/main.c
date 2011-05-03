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
#include "drivers/lcd/tft/drawing.h"
#include "drivers/lcd/tft/touchscreen.h"
#include "drivers/lcd/tft/fonts/dejavusans9.h"
#include "drivers/lcd/tft/fonts/dejavusansbold9.h"

static uint8_t adcBuffer[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static uint8_t digBuffer[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

bool adcEnabled = true;
bool digEnabled = false;

/**************************************************************************/
/*! 
    Renders the frame around the data grid
*/
/**************************************************************************/
void renderLCDFrame(void)
{
  // Clear the screen
  drawFill(COLOR_DARKGRAY);

  // Render V references
  drawString(245,  27, COLOR_BLACK, &dejaVuSansBold9ptFontInfo, "3.5V");
  drawString(244,  26, COLOR_WHITE, &dejaVuSansBold9ptFontInfo, "3.5V");
  drawString(245, 195, COLOR_BLACK, &dejaVuSansBold9ptFontInfo, "0.0V");
  drawString(244, 194, COLOR_WHITE, &dejaVuSansBold9ptFontInfo, "0.0V");

  // Div settings
  drawString( 10, 10, COLOR_BLACK, &dejaVuSansBold9ptFontInfo, "~100ms/Div");
  drawString(  9,  9, COLOR_WHITE, &dejaVuSansBold9ptFontInfo, "~100ms/Div");
  drawString( 95, 10, COLOR_BLACK, &dejaVuSansBold9ptFontInfo, "500mV/Div");
  drawString( 94,  9, COLOR_WHITE, &dejaVuSansBold9ptFontInfo, "500mV/Div");

  // Clear the ADC output level just in case
  drawRectangleFilled(175, 5, 250, 18, COLOR_DARKGRAY);

  // Render the channel text
  drawString( 25, 220, COLOR_BLACK,  &dejaVuSansBold9ptFontInfo, "P1.4 (Analog)");
  drawString( 24, 219, adcEnabled ? COLOR_YELLOW : COLOR_MEDIUMGRAY, &dejaVuSansBold9ptFontInfo, "P1.4 (Analog)");
  drawString(135, 220, COLOR_BLACK,  &dejaVuSansBold9ptFontInfo, "P2.0 (Digital)");
  drawString(134, 219, digEnabled ? COLOR_GREEN : COLOR_MEDIUMGRAY, &dejaVuSansBold9ptFontInfo, "P2.0 (Digital)");

  // ADC Warning
  drawString(245,  80, COLOR_BLACK, &dejaVuSansBold9ptFontInfo, "Warning:");
  drawString(244,  79, COLOR_WHITE, &dejaVuSansBold9ptFontInfo, "Warning:");
  drawString(244,  95, COLOR_WHITE, &dejaVuSans9ptFontInfo, "ADC input");
  drawString(244, 110, COLOR_WHITE, &dejaVuSans9ptFontInfo, "is not 5.0V");
  drawString(244, 125, COLOR_WHITE, &dejaVuSans9ptFontInfo, "tolerant!");
}

/**************************************************************************/
/*! 
    Converts the supplied 8-bit value to an approximate pixel height in
    the data grid
*/
/**************************************************************************/
uint16_t adcValToPixel(uint8_t value)
{
  // Since the chart is 175 pixels high and 3.3V is at
  // approximately 165 pixels height, we need to
  // divide the 8 bit ADC readings by 1.545 (255/165)
  // using fixed point math, and then substract
  // the number from the bottom of the frame
  uint16_t pixel = 0;
  pixel = 200 - (value * 1000 / 1545);
  return pixel;
}

/**************************************************************************/
/*! 
    Redraws the grid and renders the data points on the LCD
*/
/**************************************************************************/
void renderLCDGrid(void)
{
  if ((!adcEnabled) && (!digEnabled))
  {    
    return;
  }

  // Redraw the grid
  drawRectangle(9, 24, 236, 201, COLOR_LIGHTGRAY);
  drawRectangleFilled(10, 25, 235, 200, COLOR_BLACK);

  // Horizontal lines
  drawLine(10,  50, 235,  50, COLOR_DARKERGRAY);
  drawLine(10,  75, 235,  75, COLOR_DARKERGRAY);
  drawLine(10, 100, 235, 100, COLOR_DARKERGRAY);
  drawLine(10, 125, 235, 125, COLOR_DARKERGRAY);
  drawLine(10, 150, 235, 150, COLOR_DARKERGRAY);
  drawLine(10, 175, 235, 175, COLOR_DARKERGRAY);

  // Vertical lines
  drawLine( 35, 25,  35, 200, COLOR_DARKERGRAY);
  drawLine( 60, 25,  60, 200, COLOR_DARKERGRAY);
  drawLine( 85, 25,  85, 200, COLOR_DARKERGRAY);
  drawLine(110, 25, 110, 200, COLOR_DARKERGRAY);
  drawLine(135, 25, 135, 200, COLOR_DARKERGRAY);
  drawLine(160, 25, 160, 200, COLOR_DARKERGRAY);
  drawLine(185, 25, 185, 200, COLOR_DARKERGRAY);
  drawLine(210, 25, 210, 200, COLOR_DARKERGRAY);

  // Render the individual data points
  uint32_t counter;
  for (counter = 0; counter < 9; counter++)
  {
    // Draw historical data
    uint32_t arrayPosition = 9 - counter;

    // Draw analog data points (Yellow)
    if (adcEnabled)
    {
      drawLine(10 + counter * 25, adcValToPixel(adcBuffer[arrayPosition]), 10 + (counter + 1) * 25, adcValToPixel(adcBuffer[arrayPosition - 1]), COLOR_YELLOW);
    }

    // Draw digital data points (Green)
    if (digEnabled)
    {
      drawLine(10 + counter * 25, adcValToPixel(digBuffer[arrayPosition]), 10 + (counter + 1) * 25, adcValToPixel(digBuffer[arrayPosition - 1]), COLOR_GREEN);
    }
  }

  // Render ADC value in text if present
  if (adcEnabled)
  {
    char text[10];
    // Assuming 3.3V supply and 8-bit ADC values, 1 unit = 12.89mV (3300/256)
    sprintf(text, "%u.%u V", ((adcBuffer[0] * 1289) / 100) / 1000, ((adcBuffer[0] * 1294) / 100) % 1000);
    // Clear the previous text
    drawRectangleFilled(175, 5, 250, 18, COLOR_DARKGRAY);
    // Render the latest value in mV
    drawString(180, 10, COLOR_BLACK, &dejaVuSansBold9ptFontInfo, text);
    drawString(179,  9, COLOR_YELLOW, &dejaVuSansBold9ptFontInfo, text);
  }
}

/**************************************************************************/
/*! 
    Shifts the buffer contents right one byte, and inserts the latest
    value at the beginning.
*/
/**************************************************************************/
void addToBuffer(uint8_t *buffer, uint8_t value)
{
  uint32_t counter;
  for (counter = 9; counter > 0; counter--)
  {
    buffer[counter] = buffer[counter - 1];
  }

  // Append the latest value
  buffer[0] = value;
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
  #if defined CFG_INTERFACE
    #error "CFG_INTERFACE must be disabled in projectconfig.h for this test (to save space)"
  #endif

  // Configure cpu and mandatory peripherals
  systemInit();
  
  /* Set P2.0 to GPIO input (just in case) */
  gpioSetDir(2, 0, 0);

  /* Set P1.4/AD5 to analog input (only AD0..3 are configured by adcInit) */
  IOCON_PIO1_4 &= ~(IOCON_PIO1_4_ADMODE_MASK |
                    IOCON_PIO1_4_FUNC_MASK |
                    IOCON_PIO1_4_MODE_MASK);
  IOCON_PIO1_4 |=  (IOCON_PIO1_4_FUNC_AD5 &
                    IOCON_PIO1_4_ADMODE_ANALOG);

  // Rotate the screen and render the area around the data grid
  lcdSetOrientation(LCD_ORIENTATION_LANDSCAPE);
  renderLCDFrame();

  tsTouchData_t touch;

  // Start reading
  while (1)
  {
    // Wait up to 5ms for a touch event
    tsTouchError_t error = tsWaitForEvent(&touch, 5);
    if (!error)
    {
      if (touch.x > 25 && touch.x < 100 && touch.y > 210)
      {
        // Analog switch selected
        adcEnabled = adcEnabled ? false : true;
      }
      if (touch.x > 125 && touch.x < 200 && touch.y > 210)
      {
        // Digital switch selected
        digEnabled = digEnabled ? false : true;
      }
      // Refresh the frame
      renderLCDFrame();
    }

    // Read pins
    if (adcEnabled)
      addToBuffer(adcBuffer, adcRead(5) / 4); // 10-bit value converted to 8-bits
    if (digEnabled)
      addToBuffer(digBuffer, gpioGetValue(2, 0) ? 0xFF : 0x00);

    // Update the LCD is required
    renderLCDGrid();

    // Note, this will actually mean the timing is ~100mS + the amount of
    // time it took to get the readings and update the LCD
    // A timer interrupt could be used to get much more accurate results,
    // filling the buffer inside the IRQ and rendering the screen updates
    // every x milliseconds
    systickDelay(100);
  }

  return 0;
}
