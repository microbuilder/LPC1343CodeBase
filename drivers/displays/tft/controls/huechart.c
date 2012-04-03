/**************************************************************************/
/*! 
    @file     huechart.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Renders a hue/saturation/brightness chart

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

#include "huechart.h"

/**************************************************************************/
/*! 
    @brief  Draws a rectangular hue chart showing the full spectrum
			of fully saturated colors

    @param[in]  x
                Starting x co-ordinate
    @param[in]  y
                Starting y co-ordinate
    @param[in]  width
                Width of the chart in pixels
    @param[in]  height
                Width of the chart in pixels
*/
/**************************************************************************/
void huechartRender(uint16_t x, uint16_t y, uint16_t width, uint16_t height, theme_t theme)
{
  uint32_t delta = (height - 2) / 6;

  // Draw border
  drawRectangle(x, y, x+width, y+height, theme.colorBorderDarker);

  // Draw gradient (R > M > B > C > G > Y > R)
  drawGradient(x+1, y+1, x+width-1, y+delta-1, COLOR_RED, COLOR_MAGENTA);
  drawGradient(x+1, y+delta, x+width-1, y+(delta*2)-1, COLOR_MAGENTA, COLOR_BLUE);
  drawGradient(x+1, y+(delta*2), x+width-1, y+(delta*3)-1, COLOR_BLUE, COLOR_CYAN);
  drawGradient(x+1, y+(delta*3), x+width-1, y+(delta*4)-1, COLOR_CYAN, COLOR_GREEN);
  drawGradient(x+1, y+(delta*4), x+width-1, y+(delta*5)-1, COLOR_GREEN, COLOR_YELLOW);
  drawGradient(x+1, y+(delta*5), x+width-1, y+height-1, COLOR_YELLOW, COLOR_RED);
}
