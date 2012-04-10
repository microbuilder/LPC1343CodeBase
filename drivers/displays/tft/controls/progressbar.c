/**************************************************************************/
/*! 
    @file     progressbar.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Renders a progress bar

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, K. Townsend
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
#include <string.h>

#include "progressbar.h"

/**************************************************************************/
/*! 
    @brief  Draws a simple (empty) rectangle

    @param[in]  x
                Starting x co-ordinate
    @param[in]  y
                Starting y co-ordinate
    @param[in]  width
                Button width in pixels
    @param[in]  height
                Button height in pixels
    @param[in]  progress
                Progress in percent from 0..100
    @param[in]  color
                Color used when rendering the progress bar

    @note   Because all displays with built-in controllers and SRAM are
            inherently single-buffered -- meaning you can see the updates
            as you draw them -- you need to be very careful to try to avoid
            tearing or 'blinking' effects.  This is usually caused by
            writing the same pixel twice with different colors (for example
            filling a background and then drawing something on top of it).

            This progress bar avoids this blinking effect by only drawing
            the background gradient where it is visible, and no beneath the
            progress bar itself.
*/
/**************************************************************************/
void progressbarRender(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress, uint16_t color, theme_t theme)
{
  uint16_t brighter, darker;
  uint16_t progressEnd;

  // Figure out the size of the progress bar
  progressEnd = x + (((width-2) * progress)/100);

  // Make sure we don't end up in negative territory with really small values
  if (progressEnd < x + 2) progressEnd = x+2;

  // Draw the outline and background gradient
  // This needs to be square to avoid flickering with rapid updates :/
  drawRectangle(x, y, x+width, y+height, theme.colorBorderDarker);
  drawRectangle(x+1, y+1, x+width-1, y+height-1, theme.colorFill);
  drawGradient(progressEnd+1, y+2, x+width-2, y+height-2, theme.colorFill, theme.colorBorder);

  // Draw the progress gradient if required
  if (progress)
  { 
    // Calculate slightly brighter and darker colors for the border and gradient
    brighter = colorsAlphaBlend(COLOR_WHITE, color, 50);
    darker = colorsAlphaBlend(COLOR_BLACK, color, 35);
    // Draw border rectangle and gradient fill
    drawRectangle(x+2, y+2, progressEnd, y+height-2, darker);
    drawGradient(x+3, y+3, progressEnd-1, y+height-3, brighter, color);
  }
}
