/**************************************************************************/
/*! 
    @file     aafonts.c
    @author   K. Townsend (microBuilder.eu)

    drawString based on an example from Eran Duchan:
    http://www.pavius.net/downloads/tools/53-the-dot-factory

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, microBuilder SARL
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
#include "fonts.h"
#include "lcd.h"
#include "drawing.h"

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Draws a single bitmap character
*/
/**************************************************************************/
void fontsDrawCharBitmap(const uint16_t xPixel, const uint16_t yPixel, uint16_t color, const char *glyph, uint8_t cols, uint8_t rows)
{
  uint16_t currentY, currentX, indexIntoGlyph;
  uint16_t _row, _col, _colPages;

  // set initial current y
  currentY = yPixel;
  currentX = xPixel;

  // Figure out how many columns worth of data we have
  if (cols % 8)
    _colPages = cols / 8 + 1;
  else
    _colPages = cols / 8;

  for (_row = 0; _row < rows; _row++)
  {
    for (_col = 0; _col < _colPages; _col++)
    {
      if (_row == 0)
        indexIntoGlyph = _col;
      else
        indexIntoGlyph = (_row * _colPages) + _col;

      currentY = yPixel + _row;
      currentX = xPixel + (_col*8);
      // send the data byte
      if (glyph[indexIntoGlyph] & (0X80)) drawPixel(currentX, currentY, color);
      if (glyph[indexIntoGlyph] & (0X40)) drawPixel(currentX+1, currentY, color);
      if (glyph[indexIntoGlyph] & (0X20)) drawPixel(currentX+2, currentY, color);
      if (glyph[indexIntoGlyph] & (0X10)) drawPixel(currentX+3, currentY, color);
      if (glyph[indexIntoGlyph] & (0X08)) drawPixel(currentX+4, currentY, color);
      if (glyph[indexIntoGlyph] & (0X04)) drawPixel(currentX+5, currentY, color);
      if (glyph[indexIntoGlyph] & (0X02)) drawPixel(currentX+6, currentY, color);
      if (glyph[indexIntoGlyph] & (0X01)) drawPixel(currentX+7, currentY, color);
    }
  }
}

/**************************************************************************/
/*                                                                        */
/* ----------------------- Public Methods ------------------------------- */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Draws a string using the supplied font

    @param[in]  x
                Starting x co-ordinate
    @param[in]  y
                Starting y co-ordinate
    @param[in]  color
                Color to use when rendering the font
    @param[in]  fontInfo
                Pointer to the FONT_INFO to use when drawing the string
    @param[in]  str
                The string to render

    @section Example

    @code 

    #include "drivers/displays/tft/fonts/dejavusans9.h"
    
    fontsDrawString(0, 90,  COLOR_BLACK, &dejaVuSans9ptFontInfo, "DejaVu Sans 9");
    fontsDrawString(0, 105, COLOR_BLACK, &dejaVuSans9ptFontInfo, "123456789012345678901234567890");

    @endcode
*/
/**************************************************************************/
void fontsDrawString(uint16_t x, uint16_t y, uint16_t color, const FONT_INFO *fontInfo, char *str)
{
  uint16_t currentX, charWidth, characterToOutput;
  const FONT_CHAR_INFO *charInfo;
  uint16_t charOffset;

  // set current x, y to that of requested
  currentX = x;

  // while not NULL
  while (*str != '\0')
  {
    // get character to output
    characterToOutput = *str;
    
    // get char info
    charInfo = fontInfo->charInfo;
    
    // some fonts have character descriptors, some don't
    if (charInfo != NULL)
    {
      // get correct char offset
      charInfo += (characterToOutput - fontInfo->startChar);
      
      // get width from char info
      charWidth = charInfo->widthBits;
      
      // get offset from char info
      charOffset = charInfo->offset;
    }        
    else
    {
      // if no char info, char width is always 5
      charWidth = 5;
      
      // char offset - assume 5 * letter offset
      charOffset = (characterToOutput - fontInfo->startChar) * 5;
    }        
    
    // Send individual characters
    // We need to manually calculate width in pages since this is screwy with variable width fonts
    //uint8_t heightPages = charWidth % 8 ? charWidth / 8 : charWidth / 8 + 1;
    fontsDrawCharBitmap(currentX, y, color, (const char *)(&fontInfo->data[charOffset]), charWidth, fontInfo->height);

    // next char X
    currentX += charWidth + 1;
    
    // next char
    str++;
  }
}

/**************************************************************************/
/*!
    @brief  Returns the width in pixels of a string when it is rendered

    This method can be used to determine whether a string will fit
    inside a specific area, or if it needs to be broken up into multiple
    lines to be properly rendered on the screen.

    This function only applied to bitmap fonts (which can have variable
    widths).  All smallfonts (if available) are fixed width and can
    easily have their width calculated without costly functions like
    this one.

    @param[in]  fontInfo
                Pointer to the FONT_INFO for the font that will be used
    @param[in]  str
                The string that will be rendered
*/
/**************************************************************************/
uint16_t fontsGetStringWidth(const FONT_INFO *fontInfo, char *str)
{
  uint16_t width = 0;
  uint32_t currChar;
  uint32_t startChar = fontInfo->startChar;

  // until termination
  for (currChar = *str; currChar; currChar = *(++str))
  {
    // if char info exists for the font, use width from there
    if (fontInfo->charInfo != NULL)
    {
      width += fontInfo->charInfo[currChar - startChar].widthBits + 1;
    }
    else
    {
      width += 5 + 1;
    }
  }

  /* return the width */
  return width > 0 ? width - 1 : width;
}
