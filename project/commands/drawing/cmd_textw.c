/**************************************************************************/
/*! 
    @file     cmd_textw.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_textw in the 'core/cmd'
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
  #include "drivers/lcd/tft/lcd.h"    
  #include "drivers/lcd/tft/drawing.h"  
  #include "drivers/lcd/tft/fonts/dejavusans9.h"

  // Only include this w/UART since there isn't enough space otherwise!
  #ifdef CFG_PRINTF_UART
  #include "drivers/lcd/tft/fonts/dejavusansmono8.h"
  #include "drivers/lcd/tft/fonts/dejavusansbold9.h"
  #endif

/**************************************************************************/
/*! 
    Returns the width of the supplied text in pixels.
*/
/**************************************************************************/
void cmd_textw(uint8_t argc, char **argv)
{
  int32_t font;
  uint8_t i, len;
  char *data_ptr, data[80];
  
  // Convert supplied parameters
  getNumber (argv[0], &font);

  // Get message contents
  data_ptr = data;
  for (i=0; i<argc-1; i++)
  {
    len = strlen(argv[i+1]);
    strcpy((char *)data_ptr, (char *)argv[i+1]);
    data_ptr += len;
    *data_ptr++ = ' ';
  }
  *data_ptr++ = '\0';

  // Only include this w/UART since there isn't enough space otherwise!
  #ifdef CFG_PRINTF_UART
    switch (font)
    {
      case 1:   // DejaVu Sans Mono 8
        printf("%d %s", drawGetStringWidth(&dejaVuSansMono8ptFontInfo, data), CFG_PRINTF_NEWLINE);
        break;
      case 2:   // DejaVu Sans Bold 9
        printf("%d %s", drawGetStringWidth(&dejaVuSansBold9ptFontInfo, data), CFG_PRINTF_NEWLINE);
        break;
      default:  // DejaVu Sans 9        
        printf("%d %s", drawGetStringWidth(&dejaVuSans9ptFontInfo, data), CFG_PRINTF_NEWLINE);
        break;
    }
  #else
    // Always use DejaVu Sans 9 by default
    printf("%d %s", drawGetStringWidth(&dejaVuSans9ptFontInfo, data), CFG_PRINTF_NEWLINE);
  #endif


  return;
}

#endif  
