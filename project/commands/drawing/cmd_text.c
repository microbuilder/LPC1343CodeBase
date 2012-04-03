/**************************************************************************/
/*! 
    @file     cmd_text.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_text in the 'core/cmd'
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
#include <string.h>

#include "projectconfig.h"
#include "core/cmd/cmd.h"
#include "project/commands.h"       // Generic helper functions

#ifdef CFG_TFTLCD    
  #include "drivers/displays/tft/lcd.h"    
  #include "drivers/displays/tft/drawing.h"  

/**************************************************************************/
/*! 
    Displays the supplied text on the LCD.
*/
/**************************************************************************/
void cmd_text(uint8_t argc, char **argv)
{
  int32_t x, y, bgcolor, fontcolor;
  int32_t font;
  uint8_t i, len;
  char *data_ptr, data[80];
  
  // Convert supplied parameters
  getNumber (argv[0], &x);
  getNumber (argv[1], &y);
  getNumber (argv[2], &bgcolor);
  getNumber (argv[3], &fontcolor);
  getNumber (argv[4], &font);

  // Get message contents
  data_ptr = data;
  for (i=0; i<argc-5; i++)
  {
    len = strlen(argv[i+5]);
    strcpy((char *)data_ptr, (char *)argv[i+5]);
    data_ptr += len;
    *data_ptr++ = ' ';
  }
  *data_ptr++ = '\0';

  #if CFG_TFTLCD_USEAAFONTS
    uint16_t ctable[4];
    // Calculate 4 color lookup table using the appropriate fore and bg colors
    // This should really be optimized out into theme.h!
    aafontsCalculateColorTable(bgcolor, (uint16_t)fontcolor, &ctable[0], 4);
    switch (font)
    {
      default:  // Only enough space for the default font for now
        aafontsDrawString((uint16_t)x, (uint16_t)y, ctable, &THEME_FONT, (char *)&data);
        break;
    }
  #else
    switch (font)
    {
      default:  // Only enough space for the default font for now
        fontsDrawString((uint16_t)x, (uint16_t)y, (uint16_t)fontcolor, &THEME_FONT, (char *)&data);
        break;
    }
  #endif
}

#endif  
