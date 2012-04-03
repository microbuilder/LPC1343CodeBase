/**************************************************************************/
/*! 
    @file     hsbchart.c
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

#include "hsbchart.h"

/**************************************************************************/
/*! 
    @brief  Draws a square HSB (hue, saturation, brightness) chart

    @param[in]  x
                Starting x co-ordinate
    @param[in]  y
                Starting y co-ordinate
    @param[in]  size
                Width/height of the chart
    @param[in]  baseColor
                The fully saturated color to use for the chart
*/
/**************************************************************************/
void hsbchartRender(uint16_t x, uint16_t y, uint16_t size, uint16_t baseColor, theme_t theme)
{
  uint32_t delta;   // Alpha channel difference per pixel
  uint16_t colorS;  // Alpha-blended color for saturation
  uint16_t color;   // Alpha-blended color for saturation + brightness
  uint32_t b;       // Brightness counter
  uint32_t s;       // Saturation counter

  drawRectangle(x, y, x+size, y+size, theme.colorBorderDarker);

  if (size > 2)
  {
    delta = 10000/(size-2);       // Calculate difference in perfect per pixel (fixed point math, * 100)
    for (b = 0; b<size-1; b++)
    {
      // Calculate color for saturation
      colorS = colorsAlphaBlend(COLOR_WHITE, baseColor, 100 - (b*delta) / 100);
      for (s = 0; s<size-1; s++)
      {
        // Calculate color for brightness
        color = colorsAlphaBlend(colorS, COLOR_BLACK, 100 - (s*delta) / 100);
        drawPixel(x+b+1, y+s+1, color);
      }
    }
  }
}
