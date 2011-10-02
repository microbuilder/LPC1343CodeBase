/**************************************************************************/
/*! 
    @file     drawing.c
    @author   K. Townsend (microBuilder.eu)

    drawLine and drawCircle adapted from a tutorial by Leonard McMillan:
    http://www.cs.unc.edu/~mcmillan/

    drawString based on an example from Eran Duchan:
    http://www.pavius.net/downloads/tools/53-the-dot-factory

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2010, microBuilder SARL
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

#include "drawing.h"

#ifdef CFG_SDCARD
  #include "bmp.h"
#endif

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Swaps values a and b
*/
/**************************************************************************/
void drawSwap(uint32_t a, uint32_t b)
{
  uint32_t t;
  t = a;
  a = b;
  b = t;
}

/**************************************************************************/
/*!
    @brief  Draws a single bitmap character
*/
/**************************************************************************/
void drawCharBitmap(const uint16_t xPixel, const uint16_t yPixel, uint16_t color, const char *glyph, uint8_t cols, uint8_t rows)
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

#if defined CFG_TFTLCD_INCLUDESMALLFONTS & CFG_TFTLCD_INCLUDESMALLFONTS == 1
/**************************************************************************/
/*!
    @brief  Draws a single smallfont character
*/
/**************************************************************************/
void drawCharSmall(uint16_t x, uint16_t y, uint16_t color, uint8_t c, struct FONT_DEF font)
{
  uint8_t col, column[font.u8Width];

  // Check if the requested character is available
  if ((c >= font.u8FirstChar) && (c <= font.u8LastChar))
  {
    // Retrieve appropriate columns from font data
    for (col = 0; col < font.u8Width; col++)
    {
      column[col] = font.au8FontTable[((c - 32) * font.u8Width) + col];    // Get first column of appropriate character
    }
  }
  else
  {    
    // Requested character is not available in this font ... send a space instead
    for (col = 0; col < font.u8Width; col++)
    {
      column[col] = 0xFF;    // Send solid space
    }
  }

  // Render each column
  uint16_t xoffset, yoffset;
  for (xoffset = 0; xoffset < font.u8Width; xoffset++)
  {
    for (yoffset = 0; yoffset < (font.u8Height + 1); yoffset++)
    {
      uint8_t bit = 0x00;
      bit = (column[xoffset] << (8 - (yoffset + 1)));     // Shift current row bit left
      bit = (bit >> 7);                     // Shift current row but right (results in 0x01 for black, and 0x00 for white)
      if (bit)
      {
        drawPixel(x + xoffset, y + yoffset, color);
      }
    }
  }
}
#endif

/**************************************************************************/
/*!
    @brief  Helper method to accurately draw individual circle points
*/
/**************************************************************************/
void drawCirclePoints(int cx, int cy, int x, int y, uint16_t color)
{    
  if (x == 0) 
  {
      drawPixel(cx, cy + y, color);
      drawPixel(cx, cy - y, color);
      drawPixel(cx + y, cy, color);
      drawPixel(cx - y, cy, color);
  } 
  else if (x == y) 
  {
      drawPixel(cx + x, cy + y, color);
      drawPixel(cx - x, cy + y, color);
      drawPixel(cx + x, cy - y, color);
      drawPixel(cx - x, cy - y, color);
  } 
  else if (x < y) 
  {
      drawPixel(cx + x, cy + y, color);
      drawPixel(cx - x, cy + y, color);
      drawPixel(cx + x, cy - y, color);
      drawPixel(cx - x, cy - y, color);
      drawPixel(cx + y, cy + x, color);
      drawPixel(cx - y, cy + x, color);
      drawPixel(cx + y, cy - x, color);
      drawPixel(cx - y, cy - x, color);
  }
}

/**************************************************************************/
/*                                                                        */
/* ----------------------- Public Methods ------------------------------- */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Draws a single pixel at the specified location

    @param[in]  x
                Horizontal position
    @param[in]  y
                Vertical position
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if ((x >= lcdGetWidth()) || (y >= lcdGetHeight()))
  {
    // Pixel out of range
    return;
  }

  // Redirect to LCD
  lcdDrawPixel(x, y, color);
}

/**************************************************************************/
/*!
    @brief  Fills the screen with the specified color

    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawFill(uint16_t color)
{
  lcdFillRGB(color);
}

/**************************************************************************/
/*!
    @brief  Draws a simple color test pattern
*/
/**************************************************************************/
void drawTestPattern(void)
{
  lcdTest();
}

#if defined CFG_TFTLCD_INCLUDESMALLFONTS & CFG_TFTLCD_INCLUDESMALLFONTS == 1
/**************************************************************************/
/*!
    @brief  Draws a string using a small font (6 of 8 pixels high).

    @param[in]  x
                Starting x co-ordinate
    @param[in]  y
                Starting y co-ordinate
    @param[in]  color
                Color to use when rendering the font
    @param[in]  text
                The string to render
    @param[in]  font
                Pointer to the FONT_DEF to use when drawing the string

    @section Example

    @code 

    #include "drivers/lcd/fonts/smallfonts.h"
    
    drawStringSmall(1, 210, COLOR_WHITE, "5x8 System (Max 40 Characters)", Font_System5x8);
    drawStringSmall(1, 220, COLOR_WHITE, "7x8 System (Max 30 Characters)", Font_System7x8);

    @endcode
*/
/**************************************************************************/
void drawStringSmall(uint16_t x, uint16_t y, uint16_t color, char* text, struct FONT_DEF font)
{
  uint8_t l;
  for (l = 0; l < strlen(text); l++)
  {
    drawCharSmall(x + (l * (font.u8Width + 1)), y, color, text[l], font);
  }
}
#endif

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

    #include "drivers/lcd/tft/fonts/veramono9.h"
    
    drawString(0, 90,  COLOR_BLACK, &bitstreamVeraSansMono9ptFontInfo, "Vera Mono 9 (30 chars wide)");
    drawString(0, 105, COLOR_BLACK, &bitstreamVeraSansMono9ptFontInfo, "123456789012345678901234567890");

    @endcode
*/
/**************************************************************************/
void drawString(uint16_t x, uint16_t y, uint16_t color, const FONT_INFO *fontInfo, char *str)
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
    drawCharBitmap(currentX, y, color, (const char *)(&fontInfo->data[charOffset]), charWidth, fontInfo->height);

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
uint16_t drawGetStringWidth(const FONT_INFO *fontInfo, char *str)
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

/**************************************************************************/
/*!
    @brief  Draws a bresenham line

    @param[in]  x0
                Starting x co-ordinate
    @param[in]  y0
                Starting y co-ordinate
    @param[in]  x1
                Ending x co-ordinate
    @param[in]  y1
                Ending y co-ordinate
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawLine ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color )
{
  drawLineDotted(x0, y0, x1, y1, 0, 1, color);
}

/**************************************************************************/
/*!
    @brief  Draws a bresenham line with a fixed pattern of empty
            and solid pixels

    Based on: http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html

    @param[in]  x0
                Starting x co-ordinate
    @param[in]  y0
                Starting y co-ordinate
    @param[in]  x1
                Ending x co-ordinate
    @param[in]  y1
                Ending y co-ordinate
    @param[in]  empty
                The number of 'empty' pixels to render
    @param[in]  solid
                The number of 'solid' pixels to render
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawLineDotted ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t empty, uint16_t solid, uint16_t color )
{
  if (solid == 0)
  {
    return;
  }

  // If a negative y int was passed in it will overflow to 65K something
  // Ugly, but drawCircleFilled() can pass in negative values so we need
  // to check the values here
  y0 = y0 > 65000 ? 0 : y0;
  y1 = y1 > 65000 ? 0 : y1;

  // Check if we can use the optimised horizontal line method
  if ((y0 == y1) && (empty == 0))
  {
    lcdDrawHLine(x0, x1, y0, color);
    return;
  }

  // Check if we can use the optimised vertical line method.
  // This can make a huge difference in performance, but may
  // not work properly on every LCD controller:
  if ((x0 == x1) && (empty == 0))
  {
    // Warning: This may actually be slower than drawing individual pixels on 
    // short lines ... Set a minimum line size to use the 'optimised' method
    // (which changes the screen orientation) ?
    lcdDrawVLine(x0, y0, y1, color);
    return;
  }

  // Draw non-horizontal or dotted line
  int dy = y1 - y0;
  int dx = x1 - x0;
  int stepx, stepy;
  int emptycount, solidcount;

  if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
  if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
  dy <<= 1;                               // dy is now 2*dy
  dx <<= 1;                               // dx is now 2*dx

  emptycount = empty;
  solidcount = solid;

  drawPixel(x0, y0, color);               // always start with solid pixels
  solidcount--;
  if (dx > dy) 
  {
    int fraction = dy - (dx >> 1);        // same as 2*dy - dx
    while (x0 != x1) 
    {
      if (fraction >= 0) 
      {
        y0 += stepy;
        fraction -= dx;                   // same as fraction -= 2*dx
      }
      x0 += stepx;
      fraction += dy;                     // same as fraction -= 2*dy
      if (empty == 0)
      {
        // always draw a pixel ... no dotted line requested
        drawPixel(x0, y0, color);
      }
      else if (solidcount)
      {
        // Draw solid pxiel and decrement counter
        drawPixel(x0, y0, color);
        solidcount--;
      }
      else if(emptycount)
      {
        // Empty pixel ... don't draw anything an decrement counter
        emptycount--;
      }
      else
      {
        // Reset counters and draw solid pixel
        emptycount = empty;
        solidcount = solid;
        drawPixel(x0, y0, color);
        solidcount--;
      }
    }
  } 
  else 
  {
    int fraction = dx - (dy >> 1);
    while (y0 != y1) 
    {
      if (fraction >= 0) 
      {
        x0 += stepx;
        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      if (empty == 0)
      {
        // always draw a pixel ... no dotted line requested
        drawPixel(x0, y0, color);
      }
      if (solidcount)
      {
        // Draw solid pxiel and decrement counter
        drawPixel(x0, y0, color);
        solidcount--;
      }
      else if(emptycount)
      {
        // Empty pixel ... don't draw anything an decrement counter
        emptycount--;
      }
      else
      {
        // Reset counters and draw solid pixel
        emptycount = empty;
        solidcount = solid;
        drawPixel(x0, y0, color);
        solidcount--;
      }
    }
  }
}

/**************************************************************************/
/*!
    @brief  Draws a circle

    Based on: http://www.cs.unc.edu/~mcmillan/comp136/Lecture7/circle.html

    @param[in]  xCenter
                The horizontal center of the circle
    @param[in]  yCenter
                The vertical center of the circle
    @param[in]  radius
                The circle's radius in pixels
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawCircle (uint16_t xCenter, uint16_t yCenter, uint16_t radius, uint16_t color)
{
  int x = 0;
  int y = radius;
  int p = (5 - radius*4)/4;

  drawCirclePoints(xCenter, yCenter, x, y, color);
  while (x < y) 
  {
    x++;
    if (p < 0) 
    {
      p += 2*x+1;
    } 
    else 
    {
      y--;
      p += 2*(x-y)+1;
    }
    drawCirclePoints(xCenter, yCenter, x, y, color);
  }
}

/**************************************************************************/
/*!
    @brief  Draws a filled circle

    @param[in]  xCenter
                The horizontal center of the circle
    @param[in]  yCenter
                The vertical center of the circle
    @param[in]  radius
                The circle's radius in pixels
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawCircleFilled (uint16_t xCenter, uint16_t yCenter, uint16_t radius, uint16_t color)
{
  int16_t f = 1 - radius;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * radius;
  int16_t x = 0;
  int16_t y = radius;
  int16_t xc_px, yc_my, xc_mx, xc_py, yc_mx, xc_my;
  int16_t lcdWidth = lcdGetWidth();

  if (xCenter < lcdWidth) drawLine(xCenter, yCenter-radius < 0 ? 0 : yCenter-radius, xCenter, (yCenter-radius) + (2*radius), color);
  
  while (x<y) 
  {
    if (f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    xc_px = xCenter+x;
    xc_mx = xCenter-x;
    xc_py = xCenter+y;
    xc_my = xCenter-y;
    yc_mx = yCenter-x;
    yc_my = yCenter-y;

    // Make sure X positions are not negative or too large or the pixels will
    // overflow.  Y overflow is handled in drawLine().
    if ((xc_px < lcdWidth) && (xc_px >= 0)) drawLine(xc_px, yc_my, xc_px, yc_my + 2*y, color);
    if ((xc_mx < lcdWidth) && (xc_mx >= 0)) drawLine(xc_mx, yc_my, xc_mx, yc_my + 2*y, color);
    if ((xc_py < lcdWidth) && (xc_py >= 0)) drawLine(xc_py, yc_mx, xc_py, yc_mx + 2*x, color);
    if ((xc_my < lcdWidth) && (xc_my >= 0)) drawLine(xc_my, yc_mx, xc_my, yc_mx + 2*x, color);
  }
}

/**************************************************************************/
/*!
    @brief  Draws a filled rounded corner

    @param[in]  xCenter
                The horizontal center of the circle
    @param[in]  yCenter
                The vertical center of the circle
    @param[in]  radius
                The circle's radius in pixels
    @param[in]  position
                The position of the corner, which affects how it will
                be rendered
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawCornerFilled (uint16_t xCenter, uint16_t yCenter, uint16_t radius, drawCornerPosition_t position, uint16_t color)
{
  int16_t f = 1 - radius;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * radius;
  int16_t x = 0;
  int16_t y = radius;
  int16_t xc_px, yc_my, xc_mx, xc_py, yc_mx, xc_my;
  int16_t lcdWidth = lcdGetWidth();

  switch (position)
  {
    case DRAW_CORNERPOSITION_TOPRIGHT:
    case DRAW_CORNERPOSITION_TOPLEFT:
      if (xCenter < lcdWidth) drawLine(xCenter, yCenter-radius < 0 ? 0 : yCenter-radius, xCenter, yCenter, color);
      break;
    case DRAW_CORNERPOSITION_BOTTOMRIGHT:
    case DRAW_CORNERPOSITION_BOTTOMLEFT:
      if (xCenter < lcdWidth) drawLine(xCenter, yCenter-radius < 0 ? 0 : yCenter, xCenter, (yCenter-radius) + (2*radius), color);
      break;
  }
  
  while (x<y) 
  {
    if (f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    xc_px = xCenter+x;
    xc_mx = xCenter-x;
    xc_py = xCenter+y;
    xc_my = xCenter-y;
    yc_mx = yCenter-x;
    yc_my = yCenter-y;

    switch (position)
    {
      case DRAW_CORNERPOSITION_TOPRIGHT:
        if ((xc_px < lcdWidth) && (xc_px >= 0)) drawLine(xc_px, yc_my, xc_px, yCenter, color);
        if ((xc_py < lcdWidth) && (xc_py >= 0)) drawLine(xc_py, yc_mx, xc_py, yCenter, color);
        break;
      case DRAW_CORNERPOSITION_BOTTOMRIGHT:
        if ((xc_px < lcdWidth) && (xc_px >= 0)) drawLine(xc_px, yCenter, xc_px, yc_my + 2*y, color);
        if ((xc_py < lcdWidth) && (xc_py >= 0)) drawLine(xc_py, yCenter, xc_py, yc_mx + 2*x, color);
        break;
      case DRAW_CORNERPOSITION_TOPLEFT:
        if ((xc_mx < lcdWidth) && (xc_mx >= 0)) drawLine(xc_mx, yc_my, xc_mx, yCenter, color);
        if ((xc_my < lcdWidth) && (xc_my >= 0)) drawLine(xc_my, yc_mx, xc_my, yCenter, color);
        break;
      case DRAW_CORNERPOSITION_BOTTOMLEFT:
        if ((xc_mx < lcdWidth) && (xc_mx >= 0)) drawLine(xc_mx, yCenter, xc_mx, yc_my + 2*y, color);
        if ((xc_my < lcdWidth) && (xc_my >= 0)) drawLine(xc_my, yCenter, xc_my, yc_mx + 2*x, color);
        break;
    }
  }
}

/**************************************************************************/
/*!
    @brief  Draws a simple arrow of the specified width

    @param[in]  x
                X co-ordinate of the smallest point of the arrow
    @param[in]  y
                Y co-ordinate of the smallest point of the arrow
    @param[in]  size
                Total width/height of the arrow in pixels
    @param[in]  direction
                The direction that the arrow is pointing
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawArrow(uint16_t x, uint16_t y, uint16_t size, drawDirection_t direction, uint16_t color)
{
  drawPixel(x, y, color);

  if (size == 1)
  {
    return;
  }

  uint32_t i;
  switch (direction)
  {
    case DRAW_DIRECTION_LEFT:
      for (i = 1; i<size; i++)
      {
        drawLine(x+i, y-i, x+i, y+i, color);
      }
      break;
    case DRAW_DIRECTION_RIGHT:
      for (i = 1; i<size; i++)
      {
        drawLine(x-i, y-i, x-i, y+i, color);
      }
      break;
    case DRAW_DIRECTION_UP:
      for (i = 1; i<size; i++)
      {
        drawLine(x-i, y+i, x+i, y+i, color);
      }
      break;
    case DRAW_DIRECTION_DOWN:
      for (i = 1; i<size; i++)
      {
        drawLine(x-i, y-i, x+i, y-i, color);
      }
      break;
    default:
      break;
  }
}

/**************************************************************************/
/*!
    @brief  Draws a simple (empty) rectangle

    @param[in]  x0
                Starting x co-ordinate
    @param[in]  y0
                Starting y co-ordinate
    @param[in]  x1
                Ending x co-ordinate
    @param[in]  y1
                Ending y co-ordinate
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawRectangle ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  uint16_t x, y;

  if (y1 < y0)
  {
    // Switch y1 and y0
    y = y1;
    y1 = y0;
    y0 = y;
  }

  if (x1 < x0)
  {
    // Switch x1 and x0
    x = x1;
    x1 = x0;
    x0 = x;
  }

  drawLine (x0, y0, x1, y0, color);
  drawLine (x1, y0, x1, y1, color);
  drawLine (x1, y1, x0, y1, color);
  drawLine (x0, y1, x0, y0, color);
}

/**************************************************************************/
/*!
    @brief  Draws a filled rectangle

    @param[in]  x0
                Starting x co-ordinate
    @param[in]  y0
                Starting y co-ordinate
    @param[in]  x1
                Ending x co-ordinate
    @param[in]  y1
                Ending y co-ordinate
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawRectangleFilled ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  int height;
  uint16_t x, y;

  if (y1 < y0)
  {
    // Switch y1 and y0
    y = y1;
    y1 = y0;
    y0 = y;
  }

  if (x1 < x0)
  {
    // Switch x1 and x0
    x = x1;
    x1 = x0;
    x0 = x;
  }

  height = y1 - y0;
  for (height = y0; y1 > height - 1; ++height)
  {
    drawLine(x0, height, x1, height, color);
  }
}

/**************************************************************************/
/*!
    @brief  Draws a filled rectangle with rounded corners

    @param[in]  x0
                Starting x co-ordinate
    @param[in]  y0
                Starting y co-ordinate
    @param[in]  x1
                Ending x co-ordinate
    @param[in]  y1
                Ending y co-ordinate
    @param[in]  color
                Color used when drawing
    @param[in]  radius
                Corner radius in pixels
    @param[in]  corners
                Which corners to round
*/
/**************************************************************************/
void drawRectangleRounded ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint16_t radius, drawRoundedCorners_t corners )
{
  int height;
  uint16_t y;

  if (corners == DRAW_ROUNDEDCORNERS_NONE)
  {
    drawRectangleFilled(x0, y0, x1, y1, color);
    return;
  }

  // Calculate height
  if (y1 < y0)
  {
    y = y1;
    y1 = y0;
    y0 = y;
  }
  height = y1 - y0;

  // Check radius
  if (radius > height / 2)
  {
    radius = height / 2;
  }
  radius -= 1;

  // Draw body  
  drawRectangleFilled(x0 + radius, y0, x1 - radius, y1, color);

  switch (corners)
  {
    case DRAW_ROUNDEDCORNERS_ALL:
      drawCircleFilled(x0 + radius, y0 + radius, radius, color);
      drawCircleFilled(x1 - radius, y0 + radius, radius, color);
      drawCircleFilled(x0 + radius, y1 - radius, radius, color);
      drawCircleFilled(x1 - radius, y1 - radius, radius, color);
      if (radius*2+1 < height)
      {
        drawRectangleFilled(x0, y0 + radius, x0 + radius, y1 - radius, color);
        drawRectangleFilled(x1 - radius, y0 + radius, x1, y1 - radius, color);
      }
      break;
    case DRAW_ROUNDEDCORNERS_TOP:
      drawCircleFilled(x0 + radius, y0 + radius, radius, color);
      drawCircleFilled(x1 - radius, y0 + radius, radius, color);
      drawRectangleFilled(x0, y0 + radius, x0 + radius, y1, color); 
      drawRectangleFilled(x1 - radius, y0 + radius, x1, y1, color); 
      break;
    case DRAW_ROUNDEDCORNERS_BOTTOM:
      drawCircleFilled(x0 + radius, y1 - radius, radius, color);
      drawCircleFilled(x1 - radius, y1 - radius, radius, color);
      drawRectangleFilled(x0, y0, x0 + radius, y1 - radius, color);
      drawRectangleFilled(x1 - radius, y0, x1, y1 - radius, color);
      break;
    case DRAW_ROUNDEDCORNERS_LEFT:
      drawCircleFilled(x0 + radius, y0 + radius, radius, color);
      drawCircleFilled(x0 + radius, y1 - radius, radius, color);
      if (radius*2+1 < height)
      {
        drawRectangleFilled(x0, y0 + radius, x0 + radius, y1 - radius, color);
      }
      drawRectangleFilled(x1 - radius, y0, x1, y1, color);
      break;
    case DRAW_ROUNDEDCORNERS_RIGHT:
      drawCircleFilled(x1 - radius, y0 + radius, radius, color);
      drawCircleFilled(x1 - radius, y1 - radius, radius, color);
      if (radius*2+1 < height)
      {
        drawRectangleFilled(x1 - radius, y0 + radius, x1, y1 - radius, color);
      }
      drawRectangleFilled(x0, y0, x0 + radius, y1, color);
      break;
    default:
      break;
  }
}

/**************************************************************************/
/*!
    @brief  Draws a triangle

    @param[in]  x0
                x co-ordinate for point 0
    @param[in]  y0
                y co-ordinate for point 0
    @param[in]  x1
                x co-ordinate for point 1
    @param[in]  y1
                y co-ordinate for point 1
    @param[in]  x2
                x co-ordinate for point 2
    @param[in]  y2
                y co-ordinate for point 2
    @param[in]  color
                Color used when drawing
*/
/**************************************************************************/
void drawTriangle ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);  
}

/**************************************************************************/
/*!
    @brief  Draws a filled triangle

    @param[in]  x0
                x co-ordinate for point 0
    @param[in]  y0
                y co-ordinate for point 0
    @param[in]  x1
                x co-ordinate for point 1
    @param[in]  y1
                y co-ordinate for point 1
    @param[in]  x2
                x co-ordinate for point 2
    @param[in]  y2
                y co-ordinate for point 2
    @param[in]  color
                Fill color

    @section Example

    @code

    // Draw a white triangle
    drawTriangleFilled ( 100, 10, 20, 120, 230, 290, COLOR_WHITE);
    // Draw black circles at each point of the triangle
    drawCircleFilled(100, 10, 2, COLOR_BLACK);
    drawCircleFilled(20, 120, 2, COLOR_BLACK);
    drawCircleFilled(230, 290, 2, COLOR_BLACK);

    @endcode
*/
/**************************************************************************/
void drawTriangleFilled ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  // Re-order vertices by ascending Y values (smallest first)
  if (y0 > y1) {
    drawSwap(y0, y1); drawSwap(x0, x1);
  }
  if (y1 > y2) {
    drawSwap(y2, y1); drawSwap(x2, x1);
  }
  if (y0 > y1) {
    drawSwap(y0, y1); drawSwap(x0, x1);
  }

  int32_t dx1, dx2, dx3;    // Interpolation deltas
  int32_t sx1, sx2, sy;     // Scanline co-ordinates

  sx1=sx2=x0 * 1000;        // Use fixed point math for x axis values
  sy=y0;

  // Calculate interpolation deltas
  if (y1-y0 > 0) dx1=((x1-x0)*1000)/(y1-y0);
    else dx1=0;
  if (y2-y0 > 0) dx2=((x2-x0)*1000)/(y2-y0);
    else dx2=0;
  if (y2-y1 > 0) dx3=((x2-x1)*1000)/(y2-y1);
    else dx3=0;

  // Render scanlines (horizontal lines are the fastest rendering method)
  if (dx1 > dx2) 
  {
    for(; sy<=y1; sy++, sx1+=dx2, sx2+=dx1)
    {
      drawLine(sx1/1000, sy, sx2/1000, sy, color);
    }
    sx2 = x1*1000;
    sy = y1;
    for(; sy<=y2; sy++, sx1+=dx2, sx2+=dx3)
    {
      drawLine(sx1/1000, sy, sx2/1000, sy, color);
    }
  } 
  else 
  {
    for(; sy<=y1; sy++, sx1+=dx1, sx2+=dx2)
    {
      drawLine(sx1/1000, sy, sx2/1000, sy, color);
    }
    sx1 = x1*1000;
    sy = y1;
    for(; sy<=y2; sy++, sx1+=dx3, sx2+=dx2)
    {
      drawLine(sx1/1000, sy, sx2/1000, sy, color);
    }
  }
}

/**************************************************************************/
/*!
    @brief  Converts a 24-bit RGB color to an equivalent 16-bit RGB565 value

    @param[in]  r
                8-bit red
    @param[in]  g
                8-bit green
    @param[in]  b
                8-bit blue

    @section Example

    @code 

    // Get 16-bit equivalent of 24-bit color
    uint16_t gray = drawRGB24toRGB565(0x33, 0x33, 0x33);

    @endcode
*/
/**************************************************************************/
uint16_t drawRGB24toRGB565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

/**************************************************************************/
/*!
    @brief  Converts a 16-bit RGB565 color to a standard 32-bit BGRA32
            color (with alpha set to 0xFF)

    @param[in]  color
                16-bit rgb565 color

    @section Example

    @code 

    // First convert 24-bit color to RGB565
    uint16_t rgb565 = drawRGB24toRGB565(0xFF, 0x00, 0x00);
  
    // Convert RGB565 color back to BGRA32
    uint32_t bgra32 = drawRGB565toBGRA32(rgb565);
  
    // Display results
    printf("BGRA32: 0x%08X R: %u G: %u B: %u A: %u \r\n", 
        bgra32, 
        (bgra32 & 0x000000FF),        // Blue
        (bgra32 & 0x0000FF00) >> 8,   // Green
        (bgra32 & 0x00FF0000) >> 16,  // Red
        (bgra32 & 0xFF000000) >> 24); // Alpha

    @endcode
*/
/**************************************************************************/
uint32_t drawRGB565toBGRA32(uint16_t color)
{
  uint32_t bits = (uint32_t)color;
  uint32_t blue = bits & 0x001F;     // 5 bits blue
  uint32_t green = bits & 0x07E0;    // 6 bits green
  uint32_t red = bits & 0xF800;      // 5 bits red

  // Return shifted bits with alpha set to 0xFF
  return (red << 8) | (green << 5) | (blue << 3) | 0xFF000000;
}

/**************************************************************************/
/*! 
    @brief  Reverses a 16-bit color from BGR to RGB
*/
/**************************************************************************/
uint16_t drawBGR2RGB(uint16_t color)
{   
  uint16_t r, g, b;   
   
  b = (color>>0)  & 0x1f;   
  g = (color>>5)  & 0x3f;   
  r = (color>>11) & 0x1f;   
     
  return( (b<<11) + (g<<5) + (r<<0) );
}

/**************************************************************************/
/*!
    @brief  Draws a progress bar with rounded corners

    @param[in]  x
                Starting x location
    @param[in]  y
                Starting y location
    @param[in]  width
                Total width of the progress bar in pixels
    @param[in]  height
                Total height of the progress bar in pixels
    @param[in]  borderCorners
                The type of rounded corners to render with the progress bar border
    @param[in]  progressCorners
                The type of rounded corners to render with the inner progress bar
    @param[in]  borderColor
                16-bit color for the outer border
    @param[in]  borderFillColor
                16-bit color for the interior of the outer border
    @param[in]  progressBorderColor
                16-bit color for the progress bar's border
    @param[in]  progressFillColor
                16-bit color for the inner bar's fill
    @param[in]  progress
                Progress percentage (between 0 and 100)

    @section Example

    @code 
    #include "drivers/lcd/tft/drawing.h"

    // Draw a the progress bar (150x15 pixels large, starting at X:10, Y:195
    // with rounded corners on the top and showing 72% progress)
    drawProgressBar(10, 195, 150, 15, DRAW_ROUNDEDCORNERS_TOP, DRAW_ROUNDEDCORNERS_TOP, COLOR_DARKERGRAY, COLOR_DARKGRAY, COLOR_LIMEGREENDIM, COLOR_LIMEGREEN, 72 );

    @endcode
*/
/**************************************************************************/
void drawProgressBar ( uint16_t x, uint16_t y, uint16_t width, uint16_t height, drawRoundedCorners_t borderCorners, drawRoundedCorners_t progressCorners, uint16_t borderColor, uint16_t borderFillColor, uint16_t progressBorderColor, uint16_t progressFillColor, uint8_t progress )
{
  // Draw border with rounded corners
  drawRectangleRounded(x, y, x + width, y + height, borderColor, 5, borderCorners);
  drawRectangleRounded(x+1, y+1, x + width - 1, y + height - 1, borderFillColor, 5, borderCorners);

  // Progress bar
  if (progress > 0 && progress <= 100)
  {
    // Calculate bar size
    uint16_t bw;
    bw = (width - 6);   // bar at 100%
    if (progress != 100)
    {
      bw = (bw * progress) / 100;
    } 
    drawRectangleRounded(x + 3, y + 3, bw + x + 3, y + height - 3, progressBorderColor, 5, progressCorners);
    drawRectangleRounded(x + 4, y + 4, bw + x + 3 - 1, y + height - 4, progressFillColor, 5, progressCorners);
  }
}

/**************************************************************************/
/*!
    @brief  Draws a simple button

    @param[in]  x
                Starting x location
    @param[in]  y
                Starting y location
    @param[in]  width
                Total width of the button in pixels
    @param[in]  height
                Total height of the button in pixels
    @param[in]  fontInfo
                Pointer to the FONT_INFO used to render the button text
    @param[in]  fontHeight
                The height in pixels of the font (used for centering)
    @param[in]  borderclr
                The rgb565 border color
    @param[in]  fillclr
                The rgb565 background color
    @param[in]  fontclr
                The rgb565 font color
    @param[in]  text
                The text to render on the button

    @section Example

    @code 

    #include "drivers/lcd/tft/drawing.h"  
    #include "drivers/lcd/tft/fonts/dejavusans9.h"

    // Draw two buttons using Vera Sans Bold 9
    drawButton(20, 195, 200, 35, &dejaVuSans9ptFontInfo, 7, COLOR_GRAY_80, COLOR_GRAY_80, COLOR_WHITE, "System Settings");
    drawButton(20, 235, 200, 35, &dejaVuSans9ptFontInfo, 7, COLOR_THEME_LIMEGREEN_DARKER, COLOR_THEME_LIMEGREEN_BASE, COLOR_BLACK, "System Settings");

    @endcode
*/
/**************************************************************************/
void drawButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const FONT_INFO *fontInfo, uint16_t fontHeight, uint16_t borderclr, uint16_t fillclr, uint16_t fontclr, char* text)
{
  // Border
  drawRectangleRounded(x, y, x + width, y + height, borderclr, 5, DRAW_ROUNDEDCORNERS_ALL);
  // Fill
  drawRectangleRounded(x+2, y+2, x+width-2, y+height-2, fillclr, 5, DRAW_ROUNDEDCORNERS_ALL);

  // Render text
  if (text != NULL)
  {
    uint16_t textWidth = drawGetStringWidth(&*fontInfo, text);
    uint16_t xStart = x + (width / 2) - (textWidth / 2);
    uint16_t yStart = y + (height / 2) - (fontHeight / 2) + 1;
    drawString(xStart, yStart, fontclr, &*fontInfo, text);
  }
}

/**************************************************************************/
/*! 
    @brief  Renders a 16x16 monochrome icon using the supplied uint16_t
            array.

    @param[in]  x
                The horizontal location to start rendering from
    @param[in]  x
                The vertical location to start rendering from
    @param[in]  color
                The RGB565 color to use when rendering the icon
    @param[in]  icon
                The uint16_t array containing the 16x16 image data

    @section Example

    @code 

    #include "drivers/lcd/tft/drawing.h"  
    #include "drivers/lcd/icons16.h"

    // Renders the info icon, which has two seperate parts ... the exterior
    // and a seperate interior mask if you want to fill the contents with a
    // different color
    drawIcon16(132, 202, COLOR_BLUE, icons16_info);
    drawIcon16(132, 202, COLOR_WHITE, icons16_info_interior);

    @endcode
*/
/**************************************************************************/
void drawIcon16(uint16_t x, uint16_t y, uint16_t color, uint16_t icon[])
{
  int i;
  for (i = 0; i<16; i++)
  {
    if (icon[i] & (0X8000)) drawPixel(x, y+i, color);
    if (icon[i] & (0X4000)) drawPixel(x+1, y+i, color);
    if (icon[i] & (0X2000)) drawPixel(x+2, y+i, color);
    if (icon[i] & (0X1000)) drawPixel(x+3, y+i, color);
    if (icon[i] & (0X0800)) drawPixel(x+4, y+i, color);
    if (icon[i] & (0X0400)) drawPixel(x+5, y+i, color);
    if (icon[i] & (0X0200)) drawPixel(x+6, y+i, color);
    if (icon[i] & (0X0100)) drawPixel(x+7, y+i, color);
    if (icon[i] & (0X0080)) drawPixel(x+8, y+i, color);
    if (icon[i] & (0x0040)) drawPixel(x+9, y+i, color);
    if (icon[i] & (0X0020)) drawPixel(x+10, y+i, color);
    if (icon[i] & (0X0010)) drawPixel(x+11, y+i, color);
    if (icon[i] & (0X0008)) drawPixel(x+12, y+i, color);
    if (icon[i] & (0X0004)) drawPixel(x+13, y+i, color);
    if (icon[i] & (0X0002)) drawPixel(x+14, y+i, color);
    if (icon[i] & (0X0001)) drawPixel(x+15, y+i, color);
  }
}

#ifdef CFG_SDCARD
/**************************************************************************/
/*!
    @brief  Loads a 24-bit Windows bitmap image from an SD card and
            renders it

    @section Example

    @code 

    #include "drivers/lcd/tft/drawing.h"

    // Draw image.bmp (from the root folder) starting at pixel 0,0
    bmp_error_t error = drawBitmapImage(0, 0, "/image.bmp");

    if (error)
    {
      switch (error)
      {
        case BMP_ERROR_SDINITFAIL:
          break;
        case BMP_ERROR_FILENOTFOUND:
          break;
        case BMP_ERROR_NOTABITMAP:
          // First two bytes of image not 'BM'
          break;
        case BMP_ERROR_INVALIDBITDEPTH:
          // Image is not 24-bits
          break;
        case BMP_ERROR_COMPRESSEDDATA:
          // Image contains compressed data
          break;
        case BMP_ERROR_INVALIDDIMENSIONS:
          // Width or Height is > LCD size
          break;
        case BMP_ERROR_PREMATUREEOF:
          // EOF unexpectedly reached in pixel data
          break;
      }
    }
        
    @endcode
*/
/**************************************************************************/
bmp_error_t drawBitmapImage(uint16_t x, uint16_t y, char *filename)
{
  return bmpDrawBitmap(x, y, filename);
}

#endif
