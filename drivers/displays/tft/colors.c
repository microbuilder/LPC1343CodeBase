/**************************************************************************/
/*! 
    @file     colors.c
    @author   K. Townsend (microBuilder.eu)
    
    Various helper functions to work with RGB565 colors, including 
    color conversion, color blending, and a basic set of predefined
    color constants (see colors.h).
	
    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, Kevin Townsend
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

#include "colors.h"

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*                                                                        */
/* ----------------------- Public Methods ------------------------------- */
/*                                                                        */
/**************************************************************************/

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
    uint16_t gray = colorsRGB24toRGB565(0x33, 0x33, 0x33);

    @endcode
*/
/**************************************************************************/
uint16_t colorsRGB24toRGB565(uint8_t r, uint8_t g, uint8_t b)
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
    uint16_t rgb565 = colorsRGB24toRGB565(0xFF, 0x00, 0x00);
  
    // Convert RGB565 color back to BGRA32
    uint32_t bgra32 = colorsRGB565toBGRA32(rgb565);
  
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
uint32_t colorsRGB565toBGRA32(uint16_t color)
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
    @brief  Reverses a 16-bit color from BGR to RGB or vice verse
*/
/**************************************************************************/
uint16_t colorsBGR2RGB(uint16_t color)
{   
  uint16_t r, g, b;   
   
  b = (color>>0)  & 0x1f;   
  g = (color>>5)  & 0x3f;   
  r = (color>>11) & 0x1f;   
     
  return( (b<<11) + (g<<5) + (r<<0) );
}

/**************************************************************************/
/*!
    @brief  Adjusts the supplied color to have the specified intensity 
            (0..100).  100 will leave the color as is at full intensity,
            50 will reduce the color intensity by half, and 0 will return
            black.

            This function is useful for anti-aliasing and sub-pixel
            rendering since colors are returned as a percentage of
            the original value, depending on the amount of space they
            take up in the sub-pixel array.

    @param[in]  color
                Base color (rgb565)
    @param[in]  intensity
                Color intensity relative to the source (0..100)

    @section Example

    @code

    #include "drivers/displays/tft/drawing.h"
    #include "drivers/displays/tft/colors.h"

    uint16_t newColor;

    // Draw a pure red rectangle
    drawRectangleFilled(10, 10, 200, 100, COLOR_RED);

    // Draw a rectangle at 50% intensity red
    newColor = colorsDim(COLOR_RED, 50);
    drawRectangleFilled(20, 20, 190, 90, newColor);

    // Draw a rectangle at 25% intensity red
    newColor = colorsDim(COLOR_RED, 25);
    drawRectangleFilled(30, 30, 180, 80, newColor);

    // Draw a rectangle at 0% intensity red
    newColor = colorsDim(COLOR_RED, 0);
    drawRectangleFilled(40, 40, 170, 70, newColor);

    @endcode
*/
/**************************************************************************/
uint16_t colorsDim(uint16_t color, uint8_t intensity)
{
  uint16_t r, g, b;               // Individual component colors

  // Add intensity adjusted forecolor
  r = ((((color >> 11) & 0x1F) * intensity) / 100) & 0x1F;
  g = ((((color >> 5) & 0x3F) * intensity) / 100) & 0x3F;
  b = (((color & 0x1F)  * intensity) / 100) & 0x1F;

  return (r << 11) | (g << 6) | b;
}

/**************************************************************************/
/*!
    @brief  Returns an alpha-blended color based on the supplied bg color,
            fore color, and intensity value (0..100).

            This function is used when alpha-blending anti-aliased fonts
            with an existing background image, amongst other things, and
            can be used to create images that appear to be 'faded' or
            semi-transparent, though at the expense of slow updates since
            reading pixels from most LCD controllers is an extremely
            expensive operation.

    @param[in]  bgColor
                Background color (rgb565)
    @param[in]  foreColor
                Forground color (rgb565)
    @param[in]  fadePercent
                Visibility of the background color in percent (0..100).
                The higher the number, the more visible the back color
                becomes.  100% signifies that the back color is entirely
                visible (only the BG color is shown), 0% signifies
                that only the fore color is shown, and 25% would
                indicate that the background is visible at approximately
                25% intensity (combined with 75% of the fore color).

    @section Example

    @code

    #include "drivers/displays/tft/drawing.h"
    #include "drivers/displays/tft/colors.h"

    uint16_t bg = COLOR_GREEN;
    uint16_t fore = COLOR_WHITE;

    // Calculate the intermediate color with 25% bg blending
    uint16_t result = colorsAlphaBlend(bg, fore, 25);

    drawRectangleFilled(10, 10, 50, 50, bg);
    drawRectangleFilled(60, 10, 100, 50, fore);
    drawRectangleFilled(35, 60, 75, 100, result);

    @endcode
*/
/**************************************************************************/
uint16_t colorsAlphaBlend(uint16_t bgColor, uint16_t foreColor, uint8_t fadePercent)
{
  uint16_t br, bg, bb;              // Background component colors
  uint16_t fr, fg, fb;              // Foreground component colors
  uint16_t newr, newg, newb;        // Blended component colors

  if (fadePercent > 100)
  {
    fadePercent = 100;
  }

  // Short cut if the color is full intensity
  if (fadePercent == 100)
    return bgColor;

  // Note: This algorithm can definately be optimised!

  // Break out component colors
  br = ((bgColor >> 11) & 0x1F);
  fr = ((foreColor >> 11) & 0x1F);
  bg = ((bgColor >> 5) & 0x3F);
  fg = ((foreColor >> 5) & 0x3F);
  bb = (bgColor & 0x1F);
  fb = (foreColor & 0x1F);

  // Z = intensity * bgcolor + (100 - intensity) * forecolor
  newr = (fadePercent * br + (100 - fadePercent) * fr) / 100;
  newg = (fadePercent * bg + (100 - fadePercent) * fg) / 200;   // Need to use 5-bit green for accurate colors :(
  newb = (fadePercent * bb + (100 - fadePercent) * fb) / 100;

  return (newr << 11) | (newg << 6) | newb;
}
