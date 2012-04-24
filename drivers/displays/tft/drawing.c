/**************************************************************************/
/*! 
    @file     drawing.c
    @author   K. Townsend (microBuilder.eu)

    drawLine and drawCircle adapted from a tutorial by Leonard McMillan:
    http://www.cs.unc.edu/~mcmillan/

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
      bit = (bit >> 7);                                   // Shift current row bit right (results in 0x01 for black, and 0x00 for white)
      if (bit)
      {
        drawPixel(x + xoffset, y + yoffset, color);
      }
    }
  }
}
#endif

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
  if ((x < lcdGetWidth()) && (y < lcdGetHeight()))
  {
    lcdDrawPixel(x, y, color);
  }
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

    #include "drivers/displays/fonts/smallfonts.h"
    
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
  lcdProperties_t properties;

  // Get the LCD properties (to check for HW acceleration in the driver)
  properties = lcdGetProperties();

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
  if ((y0 == y1) && (empty == 0) && properties.fastHLine)
  {
    lcdDrawHLine(x0, x1, y0, color);
    return;
  }

  // Check if we can use the optimised vertical line method.
  // This can make a huge difference in performance, but may
  // not work properly on every LCD controller:
  if ((x0 == x1) && (empty == 0) && properties.fastVLine)
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
  drawPixel(xCenter, yCenter+radius, color);
  drawPixel(xCenter, yCenter-radius, color);
  drawPixel(xCenter+radius, yCenter, color);
  drawPixel(xCenter-radius, yCenter, color);
  drawCorner(xCenter, yCenter, radius, DRAW_CORNERS_ALL, color);
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
    @brief  Draws a single 1-pixel wide corner

    @note   Code courtesy Adafruit's excellent GFX lib:
            https://github.com/adafruit/Adafruit-GFX-Library

    @param[in]  xCenter
                The horizontal center of the circle
    @param[in]  yCenter
                The vertical center of the circle
    @param[in]  corner
                The drawCorners_t representing the corner(s) to draw
    @param[in]  color
                Color used when drawing
    
    @section EXAMPLE

    @code

    // Draw a top-left corner with a 10 pixel radius, centered at 20, 20
    drawCorner(20, 20, 10, DRAW_CORNER_TOPLEFT, COLOR_GRAY_128);

    @endcode
*/
/**************************************************************************/
void drawCorner (uint16_t xCenter, uint16_t yCenter, uint16_t r, drawCorners_t corner, uint16_t color) 
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) 
  {
    if (f >= 0) 
    {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (corner & DRAW_CORNERS_BOTTOMRIGHT) 
    {
      drawPixel(xCenter + x, yCenter + y, color);
      drawPixel(xCenter + y, yCenter + x, color);
    } 
    if (corner & DRAW_CORNERS_TOPRIGHT) 
    {
      drawPixel(xCenter + x, yCenter - y, color);
      drawPixel(xCenter + y, yCenter - x, color);
    }
    if (corner & DRAW_CORNERS_BOTTOMLEFT) 
    {
      drawPixel(xCenter - y, yCenter + x, color);
      drawPixel(xCenter - x, yCenter + y, color);
    }
    if (corner & DRAW_CORNERS_TOPLEFT) 
    {
      drawPixel(xCenter - y, yCenter - x, color);
      drawPixel(xCenter - x, yCenter - y, color);
    }
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
void drawCornerFilled (uint16_t xCenter, uint16_t yCenter, uint16_t radius, drawCorners_t position, uint16_t color)
{
  int16_t f = 1 - radius;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * radius;
  int16_t x = 0;
  int16_t y = radius;
  int16_t xc_px, yc_my, xc_mx, xc_py, yc_mx, xc_my;
  int16_t lcdWidth = lcdGetWidth();


  if ((position & DRAW_CORNERS_TOPRIGHT) || (position & DRAW_CORNERS_TOPLEFT))
  {
    if (xCenter < lcdWidth) drawLine(xCenter, yCenter-radius < 0 ? 0 : yCenter-radius, xCenter, yCenter, color);
  }
  if ((position & DRAW_CORNERS_BOTTOMRIGHT) || (position & DRAW_CORNERS_BOTTOMLEFT))
  {
    if (xCenter < lcdWidth) drawLine(xCenter, yCenter-radius < 0 ? 0 : yCenter, xCenter, (yCenter-radius) + (2*radius), color);
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


    if (position & DRAW_CORNERS_TOPRIGHT)
    {
        if ((xc_px < lcdWidth) && (xc_px >= 0)) drawLine(xc_px, yc_my, xc_px, yCenter, color);
        if ((xc_py < lcdWidth) && (xc_py >= 0)) drawLine(xc_py, yc_mx, xc_py, yCenter, color);
    }
    if (position & DRAW_CORNERS_BOTTOMRIGHT)
    {
        if ((xc_px < lcdWidth) && (xc_px >= 0)) drawLine(xc_px, yCenter, xc_px, yc_my + 2*y, color);
        if ((xc_py < lcdWidth) && (xc_py >= 0)) drawLine(xc_py, yCenter, xc_py, yc_mx + 2*x, color);
    }
    if (position & DRAW_CORNERS_TOPLEFT)
    {
        if ((xc_mx < lcdWidth) && (xc_mx >= 0)) drawLine(xc_mx, yc_my, xc_mx, yCenter, color);
        if ((xc_my < lcdWidth) && (xc_my >= 0)) drawLine(xc_my, yc_mx, xc_my, yCenter, color);
    }
    if (position & DRAW_CORNERS_BOTTOMLEFT)
    {
        if ((xc_mx < lcdWidth) && (xc_mx >= 0)) drawLine(xc_mx, yCenter, xc_mx, yc_my + 2*y, color);
        if ((xc_my < lcdWidth) && (xc_my >= 0)) drawLine(xc_my, yCenter, xc_my, yc_mx + 2*x, color);
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
    @brief  Draws a rectangle with rounded corners

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

    @section    EXAMPLE
    @code

    drawRoundedRectangle ( 10, 10, 200, 200, COLOR_BLACK, 10, DRAW_CORNERS_ALL );

    @endcode

*/
/**************************************************************************/
void drawRoundedRectangle ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint16_t radius, drawCorners_t corners )
{
  int height;
  uint16_t y;

  if (corners == DRAW_CORNERS_NONE)
  {
    drawRectangle(x0, y0, x1, y1, color);
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

  switch (corners)
  {
    case DRAW_CORNERS_ALL:
      drawCorner(x0 + radius, y0 + radius, radius, DRAW_CORNERS_TOPLEFT, color);
      drawCorner(x1 - radius, y0 + radius, radius, DRAW_CORNERS_TOPRIGHT, color);
      drawCorner(x0 + radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMLEFT, color);
      drawCorner(x1 - radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMRIGHT, color);
      if (radius*2+1 < height)
      {
        drawLine(x0, y0+radius, x0, y1-radius, color);
        drawLine(x1, y0+radius, x1, y1-radius, color);
      }
      drawLine(x0+radius, y0, x1-radius, y0, color);
      drawLine(x0+radius, y1, x1-radius, y1, color);
      break;
    case DRAW_CORNERS_TOP:
      drawCorner(x0 + radius, y0 + radius, radius, DRAW_CORNERS_TOPLEFT, color);
      drawCorner(x1 - radius, y0 + radius, radius, DRAW_CORNERS_TOPRIGHT, color);
      drawLine(x0, y0+radius, x0, y1, color);
      drawLine(x0, y1, x1, y1, color);
      drawLine(x1, y1, x1, y0+radius, color);
      drawLine(x0+radius, y0, x1-radius, y0, color);
      break;
    case DRAW_CORNERS_BOTTOM:
      drawCorner(x0 + radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMLEFT, color);
      drawCorner(x1 - radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMRIGHT, color);
      drawLine(x0, y0, x1, y0, color);
      drawLine(x1, y0, x1, y1-radius, color );
      drawLine(x1-radius, y1, x0+radius, y1, color);
      drawLine(x0, y1-radius, x0, y0, color);
      break;
    case DRAW_CORNERS_LEFT:
      drawCorner(x0 + radius, y0 + radius, radius, DRAW_CORNERS_TOPLEFT, color);
      drawCorner(x0 + radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMLEFT, color);
      if (radius*2+1 < height)
      {
        drawLine(x0, y0+radius, x0, y1-radius, color);
      }
      drawLine(x1, y0, x1, y1, color);
      drawLine(x0+radius, y0, x1, y0, color);
      drawLine(x0+radius, y1, x1, y1, color);
      break;
    case DRAW_CORNERS_RIGHT:
      drawCorner(x1 - radius, y0 + radius, radius, DRAW_CORNERS_TOPRIGHT, color);
      drawCorner(x1 - radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMRIGHT, color);
      if (radius*2+1 < height)
      {
        drawLine(x1, y0+radius, x1, y1-radius, color);
      }
      drawLine(x0, y0, x0, y1, color);
      drawLine(x0, y0, x1-radius, y0, color);
      drawLine(x0, y1, x1-radius, y1, color);
      break;
    default:
      break;
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
void drawRoundedRectangleFilled ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint16_t radius, drawCorners_t corners )
{
  int height;
  uint16_t y;

  if (corners == DRAW_CORNERS_NONE)
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
    case DRAW_CORNERS_ALL:
      drawCornerFilled(x0 + radius, y0 + radius, radius, DRAW_CORNERS_TOPLEFT, color);
      drawCornerFilled(x1 - radius, y0 + radius, radius, DRAW_CORNERS_TOPRIGHT, color);
      drawCornerFilled(x0 + radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMLEFT, color);
      drawCornerFilled(x1 - radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMRIGHT, color);
      if (radius*2+1 < height)
      {
        drawRectangleFilled(x0, y0 + radius, x0 + radius, y1 - radius, color);
        drawRectangleFilled(x1 - radius, y0 + radius, x1, y1 - radius, color);
      }
      break;
    case DRAW_CORNERS_TOP:
      drawCornerFilled(x0 + radius, y0 + radius, radius, DRAW_CORNERS_TOPLEFT, color);
      drawCornerFilled(x1 - radius, y0 + radius, radius, DRAW_CORNERS_TOPRIGHT, color);
      drawRectangleFilled(x0, y0 + radius, x0 + radius, y1, color); 
      drawRectangleFilled(x1 - radius, y0 + radius, x1, y1, color); 
      break;
    case DRAW_CORNERS_BOTTOM:
      drawCornerFilled(x0 + radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMLEFT, color);
      drawCornerFilled(x1 - radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMRIGHT, color);
      drawRectangleFilled(x0, y0, x0 + radius, y1 - radius, color);
      drawRectangleFilled(x1 - radius, y0, x1, y1 - radius, color);
      break;
    case DRAW_CORNERS_LEFT:
      drawCornerFilled(x0 + radius, y0 + radius, radius, DRAW_CORNERS_TOPLEFT, color);
      drawCornerFilled(x0 + radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMLEFT, color);
      if (radius*2+1 < height)
      {
        drawRectangleFilled(x0, y0 + radius, x0 + radius, y1 - radius, color);
      }
      drawRectangleFilled(x1 - radius, y0, x1, y1, color);
      break;
    case DRAW_CORNERS_RIGHT:
      drawCornerFilled(x1 - radius, y0 + radius, radius, DRAW_CORNERS_TOPRIGHT, color);
      drawCornerFilled(x1 - radius, y1 - radius, radius, DRAW_CORNERS_BOTTOMRIGHT, color);
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
    @brief  Draws a gradient-filled rectangle

    @param[in]  x0
                Starting x co-ordinate
    @param[in]  y0
                Starting y co-ordinate
    @param[in]  x1
                Ending x co-ordinate
    @param[in]  y1
                Ending y co-ordinate
    @param[in]  startColor
                The color at the start of the gradient
    @param[in]  endColor
                The color at the end of the gradient

    @section EXAMPLE

    @code

    #include "drivers/displays/tft/drawing.h"
    #include "drivers/displays/tft/aafonts.h"
    #include "drivers/displays/tft/aafonts/aa2/DejaVuSansCondensed14_AA2.h"

    // Draw a gradient-filled rectangle with anti-aliased text inside it

    uint16_t btnWidth, btnHeight, btnX, btnY;
    uint16_t fntX, fntY;

    btnWidth = 200;
    btnHeight = 20;
    btnX = 10;
    btnY = 30;

    lcdFillRGB(0xFFFF);

    drawRectangle(btnX-1, btnY-1, btnX+btnWidth+1, btnY+btnHeight+1, COLOR_GRAY_80);
    drawGradient(btnX, btnY, btnX+btnWidth, btnY+btnHeight, COLOR_WHITE, COLOR_GRAY_128);

    // Center text vertically and horizontally
    fntY = btnY + ((btnHeight - DejaVuSansCondensed14_AA2.fontHeight) / 2);
    fntX = btnX + ((btnWidth - aafontsGetStringWidth(&DejaVuSansCondensed14_AA2, "Click to continue"))/2);
    aafontsDrawString(fntX, fntY, COLORTABLE_AA2_BLACKONWHITE, &DejaVuSansCondensed14_AA2, "Click to continue");

    @endcode
*/
/**************************************************************************/
void drawGradient ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t startColor, uint16_t endColor)
{
  int height;
  uint16_t x, y;
  uint8_t r, g, b;
  int16_t rDelta, gDelta, bDelta;

  // Clear gradient steps, etc.
  r = g = b = 0;
  rDelta = gDelta = bDelta = 0;

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

  // Calculate global r/g/b changes between start and end colors
  rDelta = ((endColor >> 11) & 0x1F) - ((startColor >> 11) & 0x1F);
  gDelta = ((endColor >> 5) & 0x3F) - ((startColor >> 5) & 0x3F);
  bDelta = (endColor & 0x1F) - (startColor & 0x1F);

  // Calculate interpolation deltas to 2 decimal places (fixed point)
  rDelta = (rDelta * 100) / height;
  gDelta = (gDelta * 100) / height;
  bDelta = (bDelta * 100) / height;

  // Draw individual lines
  for (height = y0; y1 > height - 1; ++height)
  {
    // Calculate new rgb values based on: start color + (line number * interpolation delta)
    r = ((startColor >> 11) & 0x1F) + ((rDelta * (height - y0)) / 100);
    g = ((startColor >> 5) & 0x3F) + ((gDelta * (height - y0)) / 100);
    b = (startColor & 0x1F) + ((bDelta * (height - y0)) / 100);
    drawLine(x0, height, x1, height, ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F));
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

    #include "drivers/displays/tft/drawing.h"  
    #include "drivers/displays/icons16.h"

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
