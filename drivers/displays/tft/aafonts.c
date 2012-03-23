/**************************************************************************/
/*! 
    @file     aafonts.c
    @author   K. Townsend (microBuilder.eu)

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
#include "aafonts.h"

#include "drivers/displays/tft/lcd.h"
#include "drivers/displays/tft/drawing.h"

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief Renders a single AA2 character on the screen

    This text rendering method used a lookup table of pre-calculated
    colors, and doesn't require any reads from the LCD (not all displays
    support reading pixels back).  This offers the best performance and
    high-quality text, but can only be used on solid backgrounds where
    the bgcolor is known.

    @param[in]  x
                Top-left x position
    @param[in]  y
                Top-left y position
    @param[in]  height
                Font height in pixels
    @param[in]  character
                Pointer to the aafontsCharInfo_t array with the char data
    @param[in]  colorTable
                Pointer to the 4 element color lookup table
*/
/**************************************************************************/
int aafontsDrawCharAA2( uint16_t x, uint16_t y, uint16_t height, aafontsCharInfo_t character, const uint16_t * colorTable)
{
  uint16_t w, h, xp, yp, pos;
  uint8_t color;

  for (h = 0; h < height; h++)
  {
    pos = 0;
    for (w = 0; w < character.width; w++)
    {
      color = character.charData[h*character.bytesPerRow + w/4];
      switch (pos)
      {
        case 0:
          color = (color >> 6) & 0x03;
          break;
        case 1:
          color = (color >> 4) & 0x03;
          break;
        case 2:
          color = (color >> 2) & 0x03;
          break;
        case 3:
          color = color & 0x03;
          break;
      }
      if (color) lcdDrawPixel(x+w, y+h, colorTable[color & 0xF]);
      pos++;
      if (pos == 4) pos = 0;
    }
  }
} 

/**************************************************************************/
/*!
    @brief Renders a single AA4 character on the screen

    This text rendering method used a lookup table of pre-calculated
    colors, and doesn't require any reads from the LCD (not all displays
    support reading pixels back).  This offers the best performance and
    high-quality text, but can only be used on solid backgrounds where
    the bgcolor is known.

    @param[in]  x
                Top-left x position
    @param[in]  y
                Top-left y position
    @param[in]  height
                Font height in pixels
    @param[in]  character
                Pointer to the aafontsCharInfo_t array with the char data
    @param[in]  colorTable
                Pointer to the 16 element color lookup table
*/
/**************************************************************************/
int aafontsDrawCharAA4( uint16_t x, uint16_t y, uint16_t height, aafontsCharInfo_t character, const uint16_t * colorTable)
{
  uint16_t w, h, xp, yp;
  uint8_t color;

  for (h = 0; h < height; h++)
  {
    for (w = 0; w < character.width; w++)
    {
      color = character.charData[h*character.bytesPerRow + w/2];
      if (!(w % 2)) color = (color >> 4);
      if (color) lcdDrawPixel(x+w, y+h, colorTable[color & 0xF]);
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
    @brief  Draws a string using the supplied anti-aliased font

    @param[in]  x
                Starting x co-ordinate
    @param[in]  y
                Starting y co-ordinate
    @param[in]  colorTable
                The color lookup table to use for the antialiased pixels
    @param[in]  font
                Pointer to the aafontsFont_t to use when drawing the string
    @param[in]  str
                The string to render

    @section Example

    @code 

    #include "drivers/displays/tft/aafonts.h"
    #include "drivers/displays/tft/aafonts/aa2/DejaVuSansCondensed14_AA2.h"
    #include "drivers/displays/tft/aafonts/aa2/DejaVuSansCondensedBold14_AA2.h"

    // Fill screen with white (so that we can use the pre-defined color tables in aafonts.h)
    lcdFillRGB(COLOR_WHITE);

    aafontsDrawString(10, 100, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensed14_AA2, "1234567890");
    aafontsDrawString(10, 120, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensed14_AA2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    aafontsDrawString(10, 140, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensed14_AA2, "abcdefghijklmnopqrstuvwxyz");
    aafontsDrawString(10, 160, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensed14_AA2, "!\"#$%&'()*+,-./  :;<=>?");
    aafontsDrawString(10, 180, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensed14_AA2, "@  [\\]^_  {|}~");

    aafontsDrawString(10, 215, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensedBold14_AA2, "1234567890");
    aafontsDrawString(10, 235, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensedBold14_AA2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    aafontsDrawString(10, 255, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensedBold14_AA2, "abcdefghijklmnopqrstuvwxyz");
    aafontsDrawString(10, 275, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensedBold14_AA2, "!\"#$%&'()*+,-./  :;<=>?");
    aafontsDrawString(10, 295, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensedBold14_AA2, "@  [\\]^_  {|}~");

    @endcode
*/
/**************************************************************************/
void aafontsDrawString(uint16_t x, uint16_t y, const uint16_t * colorTable, const aafontsFont_t *font, char *str)
{
  uint16_t currentX, charWidth, characterToOutput;
  const aafontsCharInfo_t *charInfo;
  uint16_t charOffset;

  // set current x, y to that of requested
  currentX = x;

  // while not NULL
  while (*str != '\0')
  {
    // get character to output
    characterToOutput = *str;
    uint16_t last = font->lastChar;

    // Check if the character is within the font boundaries
    if ((characterToOutput > font->lastChar) || (characterToOutput < font->firstChar))
    {
      // Character is out of bounds
      // Insert space instead
      charWidth = font->unknownCharWidth;
    }
    else
    {
      // get char info
      charInfo = &(font->charTable[characterToOutput - font->firstChar]);    
      // get width from char info
      charWidth = charInfo->width;
      // Send individual characters
      switch (font->fontType)
      {
        case AAFONTS_FONTTYPE_AA2:
          aafontsDrawCharAA2(currentX, y, font->fontHeight, *charInfo, &colorTable[0]);
          break;
        case AAFONTS_FONTTYPE_AA4:
          aafontsDrawCharAA4(currentX, y, font->fontHeight, *charInfo, &colorTable[0]);
          break;
      }
    }
    
    // Adjust x for the next character
    currentX += charWidth;
    
    // next char in string
    str++;
  }
}

/**************************************************************************/
/*!
    @brief  Returns the width in pixels of a string when it is rendered

    This method can be used to determine whether a string will fit
    inside a specific area, or if it needs to be broken up into multiple
    lines to be properly rendered on the screen.

    @param[in]  font
                Pointer to aafontsFont_t of the font that will be used
    @param[in]  str
                The string that will be rendered

    @section Example

    @code 

    #include "drivers/displays/tft/aafonts.h"
    #include "drivers/displays/tft/aafonts/aa2/DejaVuSansCondensed14_AA2.h"

    uint32_t w = aafontsGetStringWidth(&DejaVuSansCondensed14_AA2, "This is a simple test 123!!! (AA2)");

    @endcode
*/
/**************************************************************************/
uint16_t aafontsGetStringWidth(const aafontsFont_t *font, char *str)
{
  uint16_t width = 0;
  const aafontsCharInfo_t *charInfo;
  uint32_t currChar;
  uint32_t startChar = font->firstChar;

  // until termination
  for (currChar = *str; currChar; currChar = *(++str))
  {
    // Check if the character is within the font boundaries
    if ((currChar > font->lastChar) || (currChar < font->firstChar))
    {
      // Character is out of bounds
      width += font->unknownCharWidth;
    }
    else
    {
      // get char info
      charInfo = &(font->charTable[currChar - font->firstChar]);
      // get width from char info
      width += charInfo->width;
    }
  }

  /* return the string width */
  return width;
}
