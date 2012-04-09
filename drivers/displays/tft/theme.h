/**************************************************************************/
/*! 
    @file     theme.h
    @author   K. Townsend (microBuilder.eu)

    @brief    Common UI definitions for the color scheme, fonts, etc.

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
#ifndef __THEME_H__
#define __THEME_H__

#include "projectconfig.h"

#include "colors.h"

#if CFG_TFTLCD_USEAAFONTS
  #include "drivers/displays/tft/aafonts/aa2/DejaVuSansCondensedBold14_AA2.h"
#else
  #include "drivers/displays/tft/fonts/dejavusans9.h"
#endif

// Generic Color Palettes (provided as a starting point for custom themes)
#define THEME_COLOR_LIMEGREEN_BASE          (uint16_t)(0xD7F0)    // 211 255 130
#define THEME_COLOR_LIMEGREEN_DARKER        (uint16_t)(0x8DE8)    // 137 188  69
#define THEME_COLOR_LIMEGREEN_LIGHTER       (uint16_t)(0xEFF9)    // 238 255 207
#define THEME_COLOR_LIMEGREEN_SHADOW        (uint16_t)(0x73EC)    // 119 127 103
#define THEME_COLOR_LIMEGREEN_ACCENT        (uint16_t)(0xAE6D)    // 169 204 104

#define THEME_COLOR_VIOLET_BASE             (uint16_t)(0x8AEF)    // 143  94 124
#define THEME_COLOR_VIOLET_DARKER           (uint16_t)(0x4187)    //  66  49  59
#define THEME_COLOR_VIOLET_LIGHTER          (uint16_t)(0xC475)    // 194 142 174
#define THEME_COLOR_VIOLET_SHADOW           (uint16_t)(0x40E6)    //  66  29  52
#define THEME_COLOR_VIOLET_ACCENT           (uint16_t)(0xC992)    // 204  50 144

#define THEME_COLOR_EARTHY_BASE             (uint16_t)(0x6269)    //  97  79  73
#define THEME_COLOR_EARTHY_DARKER           (uint16_t)(0x3103)    //  48  35  31
#define THEME_COLOR_EARTHY_LIGHTER          (uint16_t)(0x8C30)    // 140 135 129
#define THEME_COLOR_EARTHY_SHADOW           (uint16_t)(0xAB29)    // 173 102  79
#define THEME_COLOR_EARTHY_ACCENT           (uint16_t)(0xFE77)    // 250 204 188

#define THEME_COLOR_SKYBLUE_BASE            (uint16_t)(0x95BF)    // 150 180 255
#define THEME_COLOR_SKYBLUE_DARKER          (uint16_t)(0x73B0)    // 113 118 131
#define THEME_COLOR_SKYBLUE_LIGHTER         (uint16_t)(0xE75F)    // 227 235 255
#define THEME_COLOR_SKYBLUE_SHADOW          (uint16_t)(0x4ACF)    //  75  90 127
#define THEME_COLOR_SKYBLUE_ACCENT          (uint16_t)(0xB5F9)    // 182 188 204

// Themes can be used to centrally change the color scheme for your app
// All controls use the colors defined in the supplied theme_t by default
typedef struct theme_s
{
  uint16_t colorBackground;                 // Fill color for the entire LCD
  uint16_t colorBorder;                     // Border color for frames, buttons, windows, etc.
  uint16_t colorBorderDarker;               // Slightly darker border color for frames when needed
  uint16_t colorText;                       // Default text color (may be overridden in certain functions)
  uint16_t colorTextAlt;                    // Alternate text color (may be overridden in certain functions)
  uint16_t colorFill;                       // Fill color for window and control backgrounds
  uint16_t colorFillAlt;                    // Slightly lighter or darker fill for window and control backgrounds
} theme_t;

// Default font
#if CFG_TFTLCD_USEAAFONTS
  #define THEME_FONT                        DejaVuSansCondensedBold14_AA2
#else
  #define THEME_FONT                        dejaVuSans9ptFontInfo
#endif

theme_t themeGetDefault(void);

#endif
