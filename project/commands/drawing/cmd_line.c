/**************************************************************************/
/*! 
    @file     cmd_line.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_line in the 'core/cmd'
              command-line interpretter.

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
#include <stdio.h>

#include "projectconfig.h"
#include "core/cmd/cmd.h"
#include "project/commands.h"       // Generic helper functions

#ifdef CFG_TFTLCD    
  #include "drivers/lcd/tft/lcd.h"    
  #include "drivers/lcd/tft/drawing.h"  

/**************************************************************************/
/*! 
    Displays a line on the LCD.
*/
/**************************************************************************/
void cmd_line(uint8_t argc, char **argv)
{
  int32_t x1, y1, x2, y2, c, empty, solid;

  // Convert supplied parameters
  getNumber (argv[0], &x1);
  getNumber (argv[1], &y1);
  getNumber (argv[2], &x2);
  getNumber (argv[3], &y2);
  getNumber (argv[4], &c);
  if (argc > 5)
  {
    getNumber (argv[5], &empty);
    getNumber (argv[6], &solid);
  }
  else
  {
    empty = 0;
    solid = 1;
  }

  // ToDo: Validate data!
  if (c < 0 || c > 0xFFFF)
  {
    printf("Invalid Color%s", CFG_PRINTF_NEWLINE);
    return;
  }

  drawLineDotted(x1, y1, x2, y2, empty, solid, (uint16_t)c);
}

#endif  
