/**************************************************************************/
/*! 
    @file     cmd_tbl.h
    @author   K. Townsend (microBuilder.eu)

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

#ifndef __CMD_TBL_H__ 
#define __CMD_TBL_H__

#define CMD_COUNT (sizeof(cmd_tbl)/sizeof(cmd_t))

#include <stdio.h>

#ifdef CFG_INTERFACE_UART
#include "core/uart/uart.h"
#endif

// Function prototypes for the command table
void cmd_help(uint8_t argc, char **argv);         // handled by core/cmd/cmd.c
void cmd_sysinfo(uint8_t argc, char **argv);
void cmd_reset(uint8_t argc, char **argv);

#ifdef CFG_TFTLCD
void cmd_backlight(uint8_t argc, char **argv);
void cmd_button(uint8_t argc, char **argv);
void cmd_circle(uint8_t argc, char **argv);
void cmd_clear(uint8_t argc, char **argv);
void cmd_line(uint8_t argc, char **argv);
void cmd_rectangle(uint8_t argc, char **argv);
void cmd_rectangleround(uint8_t argc, char **argv);
void cmd_pixel(uint8_t argc, char **argv);
void cmd_progress(uint8_t argc, char **argv);
void cmd_getpixel(uint8_t argc, char **argv);
void cmd_calibrate(uint8_t argc, char **argv);
void cmd_orientation(uint8_t argc, char **argv);
void cmd_roundedcorner(uint8_t argc, char **argv);
void cmd_text(uint8_t argc, char **argv);
void cmd_textw(uint8_t argc, char **argv);
void cmd_triangle(uint8_t argc, char **argv);
void cmd_tsthreshhold(uint8_t argc, char **argv);
void cmd_tswait(uint8_t argc, char **argv);
#ifdef CFG_SDCARD
void cmd_bmp(uint8_t argc, char **argv);
#endif
#endif

#ifdef CFG_CHIBI
void cmd_chibi_addr(uint8_t argc, char **argv);
void cmd_chibi_tx(uint8_t argc, char **argv);
#endif

#ifdef CFG_I2CEEPROM
void cmd_i2ceeprom_read(uint8_t argc, char **argv);
void cmd_i2ceeprom_write(uint8_t argc, char **argv);
void cmd_uart(uint8_t argc, char **argv);
#endif

#ifdef CFG_LM75B
void cmd_lm75b_gettemp(uint8_t argc, char **argv);
#endif

#ifdef CFG_SDCARD
void cmd_sd_dir(uint8_t argc, char **argv);
#endif

#define CMD_NOPARAMS "This command has no parameters"

/**************************************************************************/
/*! 
    Command list for the command-line interpreter and the name of the
    corresponding method that handles the command.

    Note that a trailing ',' is required on the last entry, which will
    cause a NULL entry to be appended to the end of the table.
*/
/**************************************************************************/
cmd_t cmd_tbl[] = 
{
  // command name, min args, max args, hidden, function name, command description, syntax
  { "?",    0,  0,  0, cmd_help              , "Help"                           , CMD_NOPARAMS },
  { "V",    0,  0,  0, cmd_sysinfo           , "System Info"                    , CMD_NOPARAMS },
  { "Z",    0,  0,  0, cmd_reset             , "Reset"                          , CMD_NOPARAMS },

  #ifdef CFG_I2CEEPROM
  { "e",    1,  1,  0, cmd_i2ceeprom_read    , "EEPROM Read"                    , "'e <addr>'" },
  { "w",    2,  2,  0, cmd_i2ceeprom_write   , "EEPROM Write"                   , "'w <addr> <val>'" },
  { "U",    0,  1,  0, cmd_uart              , "UART baud rate"                 , "'U [<val>]'" },
  #endif

  #ifdef CFG_TFTLCD
  { "b",    7,  99, 0, cmd_button            , "Button"                         , "'b <x> <y> <w> <h> <brdrclr> <fillclr> <fontclr> [<txt>]'" },
  #ifdef CFG_SDCARD
  { "B",    3,  3,  0, cmd_bmp               , "Bitmap (SD Card)"               , "'B <x> <y> <file>'" },
  #endif
  { "c",    4,  6,  0, cmd_circle            , "Circle"                         , "'c <x> <y> <radius> <color> [<filled[0|1]> <bcolor>]'" },
  { "C",    0,  0,  0, cmd_calibrate         , "Calibrate Touch Screen"         , CMD_NOPARAMS },
  { "F",    0,  1,  0, cmd_clear             , "Fill"                           , "'F [<color>]'" },
  { "g",    2,  2,  0, cmd_getpixel          , "Get Pixel"                      , "'g <x> <y>'" },
  { "l",    5,  7,  0, cmd_line              , "Line"                           , "'l <x1> <y1> <x2> <y2> <color> [<empty> <solid>]'" },
  { "L",    1,  1,  0, cmd_backlight         , "Backlight"                      , "'L <0|1>'" },
  { "n",    5,  5,  0, cmd_roundedcorner     , "Rounded Corner"                 , "'n <x> <y> <radius> <corner> <color>'" },
  { "o",    0,  1,  0, cmd_orientation       , "LCD Orientation"                , "'o [<0|1>]'" },
  { "p",    3,  3,  0, cmd_pixel             , "Draw Pixel"                     , "'p <x> <y> <color>'" },
  { "P",    9,  9,  0, cmd_progress          , "Progress Bar"                   , "'P <x> <y> <w> <h> <%> <bclr> <bfillclr> <pbrdclr> <pfillclr>'" },
  { "r",    5,  7,  0, cmd_rectangle         , "Rectangle"                      , "'r <x1> <y1> <x2> <y2> <color> [<filled[0|1]> <bcolor>]'" },
  { "R",    7,  7,  0, cmd_rectangleround    , "Rounded Rectangle"              , "'R <x1> <y1> <x2> <y2> <color> <radius> <corners>'" },
  { "s",    2, 99,  0, cmd_textw             , "Text Width"                     , "'s <font#> <msg>'" },
  { "t",    5, 99,  0, cmd_text              , "Text"                           , "'t <x> <y> <color> <font#> <msg>'" },
  { "v",    7,  8,  0, cmd_triangle          , "Triangle"                       , "'v <x1> <y1> <x2> <y2> <x3> <y3> <color> [<filled[0|1]>]'" },
  { "W",    0,  1,  0, cmd_tswait            , "Wait for Touch"                 , "'W [<ms>]'" },
  { "x",    0,  1,  0, cmd_tsthreshhold      , "Touch Threshold"                , "'x [<0..254>]'" },
  #endif

  #ifdef CFG_CHIBI
  { "A",    0,  1,  0, cmd_chibi_addr        , "Get/Set Node Address"           , "'A [<0x0..0xFFFE>]'" },
  { "S",    2, 99,  0, cmd_chibi_tx          , "Send Message"                   , "'S <destaddr> <msg>'" },
  #endif

  #ifdef CFG_LM75B
  { "m",    0,  0,  0, cmd_lm75b_gettemp     , "Temp (C)"                       , CMD_NOPARAMS },
  #endif

  #ifdef CFG_SDCARD
  { "d",    0,  1,  0,  cmd_sd_dir           , "Dir (SD Card)"                  , "'d [<path>]'" },
  #endif
};

#endif