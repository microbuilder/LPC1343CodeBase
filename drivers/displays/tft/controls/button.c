/**************************************************************************/
/*! 
    @file     button.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Renders a button

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

#include "button.h"

/**************************************************************************/
/*!
    @brief  Draws a simple button with centered text

    @param[in]  x
                Starting x co-ordinate
    @param[in]  y
                Starting y co-ordinate
    @param[in]  width
                Button width in pixels
    @param[in]  height
                Button height in pixels
    @param[in]  fontColor
                Color used when rendering the text
    @param[in]  text
                Text to center inside the button
*/
/**************************************************************************/
void buttonRender(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t fontColor, char *text, theme_t theme)
{
  // Draw background gradient then outline
  drawGradient(x+2, y+2, x+width-2, y+height-2, theme.colorFill, theme.colorBorder);
  drawRoundedRectangle(x+1, y+1, x+width-1, y+height-1, theme.colorFill, 5, DRAW_CORNERS_ALL);
  drawRoundedRectangle(x, y, x+width, y+height, theme.colorBorderDarker, 5, DRAW_CORNERS_ALL);

  #if CFG_TFTLCD_USEAAFONTS
    uint16_t ctable[4];

    // Calculate 4 color lookup table using the appropriate fore and bg colors
    // This should really be optimized out into theme.h!
    aafontsCalculateColorTable(theme.colorFill, fontColor, &ctable[0], 4);

    if (text != NULL)
    {
      aafontsCenterString(x + width / 2, y + 1 + (height / 2) - (THEME_FONT.fontHeight / 2), ctable, &THEME_FONT, text);
    }
  #else
    // Render text
    if (text != NULL)
    {
      uint16_t textWidth = fontsGetStringWidth(&THEME_FONT, text);
      uint16_t xStart = x + (width / 2) - (textWidth / 2);
      uint16_t yStart = y + (height / 2) - (THEME_FONT.height / 2) + 1;
      fontsDrawString(xStart, yStart, fontColor, &THEME_FONT, text);
    }
  #endif
}
