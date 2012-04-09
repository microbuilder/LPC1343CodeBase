/**************************************************************************/
/*! 
    @file     cmd_buton.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_button in the 'core/cmd'
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
  #include "drivers/displays/tft/touchscreen.h"
  #include "drivers/displays/tft/controls/button.h"

/**************************************************************************/
/*! 
    Displays a button on the LCD with or without text.

    Ex.: "btn 10 10 220 35 1 This is a test" will display a 220x35 pixel
    button in the 'pressed' state (arg 5 = 1) starting at pixel 10, 10 with
    'This is a test' rendered on the button.
*/
/**************************************************************************/
void cmd_button(uint8_t argc, char **argv)
{
  int32_t x, y, w, h, fontcolor;
  
  // ToDo: Validate data!

  // Convert supplied parameters
  getNumber (argv[0], &x);
  getNumber (argv[1], &y);
  getNumber (argv[2], &w);
  getNumber (argv[3], &h);
  getNumber (argv[4], &fontcolor);

  if (argc == 5)
  {
    // Render the button with no text
    buttonRender(x, y, w, h, (uint16_t)fontcolor, NULL, themeGetDefault());
  }
  else
  {
    // Get text contents
    uint8_t i, len, *data_ptr, data[50];
    data_ptr = data;
    for (i = 0; i < argc - 5; i++)
    {
      len = strlen(argv[i + 5]);
      strcpy((char *)data_ptr, (char *)argv[i + 5]);
      data_ptr += len;
      *data_ptr++ = ' ';
    }
    *data_ptr++ = '\0';

    // Render the button with text
    buttonRender(x, y, w, h, (uint16_t)fontcolor, (char *)&data, themeGetDefault());
  }
}

#endif  
