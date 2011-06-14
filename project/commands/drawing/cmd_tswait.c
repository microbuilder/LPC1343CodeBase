/**************************************************************************/
/*! 
    @file     cmd_tswait.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_tswait in the 'core/cmd'
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
  #include "drivers/lcd/tft/touchscreen.h"

/**************************************************************************/
/*! 
    Waits for a touch screen event and returns the co-ordinates
*/
/**************************************************************************/
void cmd_tswait(uint8_t argc, char **argv)
{
  tsTouchData_t data;
  int32_t delay;
  int32_t error = 0;

  if (argc == 1)
  {
    getNumber (argv[0], &delay);
  }
  else
  {
    delay = 0;
  }

  // Validate delay
  if (delay < 0)
  {
    printf("Invalid timeout%s", CFG_PRINTF_NEWLINE);
    return;
  }

  // Blocking delay until a valid touch event occurs
  error = tsWaitForEvent(&data, delay > 0 ? (uint32_t)delay : 0);

  if (error == TS_ERROR_NONE)
  {
    // A valid touch event occurred ... parse data
    printf("%d,%d%s",(int)data.xlcd, (int)data.ylcd, CFG_PRINTF_NEWLINE);
  }
  else
  {
    // Display error code
    printf("%d%s", (int)error, CFG_PRINTF_NEWLINE);
  }

  return;
}

#endif
