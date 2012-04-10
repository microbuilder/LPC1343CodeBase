/**************************************************************************/
/*! 
    @file     drawing.h
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
#ifndef __DRAWING_H__
#define __DRAWING_H__

#include "projectconfig.h"
#include "lcd.h"
#include "colors.h"
#include "theme.h"

#if CFG_TFTLCD_USEAAFONTS
  #include "aafonts.h"
#else
  #include "fonts.h"
#endif

#if CFG_TFTLCD_INCLUDESMALLFONTS
  #include "drivers/displays/smallfonts.h"
#endif

typedef enum
{
  DRAW_CORNERS_NONE        = 0x00,
  DRAW_CORNERS_TOPLEFT     = 0x01,
  DRAW_CORNERS_TOPRIGHT    = 0x02,
  DRAW_CORNERS_BOTTOMLEFT  = 0x04,
  DRAW_CORNERS_BOTTOMRIGHT = 0x08,
  DRAW_CORNERS_ALL         = 0x0F, // 0x01 + 0x02 + 0x04 + 0x08
  DRAW_CORNERS_TOP         = 0x03, // 0x01 + 0x02
  DRAW_CORNERS_BOTTOM      = 0x0C, // 0x04 + 0x08
  DRAW_CORNERS_LEFT        = 0x05, // 0x01 + 0x04
  DRAW_CORNERS_RIGHT       = 0x0A  // 0x02 + 0x08
} drawCorners_t;

typedef enum
{
  DRAW_DIRECTION_LEFT,
  DRAW_DIRECTION_RIGHT,
  DRAW_DIRECTION_UP,
  DRAW_DIRECTION_DOWN
} drawDirection_t;

void      drawTestPattern      ( void );
void      drawPixel            ( uint16_t x, uint16_t y, uint16_t color );
void      drawFill             ( uint16_t color );
void      drawLine             ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color );
void      drawLineDotted       ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t space, uint16_t solid, uint16_t color );
void      drawCircle           ( uint16_t xCenter, uint16_t yCenter, uint16_t radius, uint16_t color );
void      drawCircleFilled     ( uint16_t xCenter, uint16_t yCenter, uint16_t radius, uint16_t color );
void      drawCorner           ( uint16_t xCenter, uint16_t yCenter, uint16_t r, drawCorners_t corner, uint16_t color );
void      drawCornerFilled     ( uint16_t xCenter, uint16_t yCenter, uint16_t radius, drawCorners_t position, uint16_t color );
void      drawArrow            ( uint16_t x, uint16_t y, uint16_t size, drawDirection_t, uint16_t color );
void      drawRectangle        ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color );
void      drawRectangleFilled  ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color );
void      drawRoundedRectangle ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint16_t radius, drawCorners_t corners );
void      drawRoundedRectangleFilled ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint16_t radius, drawCorners_t corners );
void      drawGradient         ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t startColor, uint16_t endColor );
void      drawTriangle         ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color );
void      drawTriangleFilled   ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color );
void      drawIcon16           ( uint16_t x, uint16_t y, uint16_t color, uint16_t icon[] );

#if CFG_TFTLCD_INCLUDESMALLFONTS
void      drawStringSmall      ( uint16_t x, uint16_t y, uint16_t color, char* text, struct FONT_DEF font );
#endif

#endif
