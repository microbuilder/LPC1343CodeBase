/**************************************************************************/
/*! 
    @file     cmd_bmp.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_bmp in the 'core/cmd'
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

#if defined CFG_TFTLCD && defined CFG_SDCARD
  #include "drivers/lcd/tft/lcd.h"    
  #include "drivers/lcd/tft/drawing.h"  

/**************************************************************************/
/*! 
    Displays a bitmap image on the LCD.
*/
/**************************************************************************/
void cmd_bmp(uint8_t argc, char **argv)
{
  int32_t x, y;
  char* filename;

  // Convert supplied parameters
  getNumber (argv[0], &x);
  getNumber (argv[1], &y);
  filename = argv[2];

  // Render image
  bmp_error_t error;
  error = drawBitmapImage(x, y, filename);

  switch (error)
  {
    case BMP_ERROR_SDINITFAIL:
      printf("SD Init Failed%s", CFG_PRINTF_NEWLINE);
      break;
    case BMP_ERROR_FILENOTFOUND:
      printf("File Not Found: '%s'%s", filename, CFG_PRINTF_NEWLINE);
      break;
    case BMP_ERROR_UNABLETOCREATEFILE:
      printf("Unable to create file: '%s'%s", filename, CFG_PRINTF_NEWLINE);
      break;
    case BMP_ERROR_NOTABITMAP:
      printf("Not a Bitmap: '%s'%s", filename, CFG_PRINTF_NEWLINE);
      break;
    case BMP_ERROR_INVALIDBITDEPTH:
      printf("Not a 24-Bit Image%s", CFG_PRINTF_NEWLINE);
      break;
    case BMP_ERROR_INVALIDDIMENSIONS:
      printf("Image Exceeds %d x %d Pixels%s", lcdGetWidth(), lcdGetHeight(), CFG_PRINTF_NEWLINE);
      break;
    case BMP_ERROR_COMPRESSEDDATA:
      printf("Compressed Images Unsupported%s", CFG_PRINTF_NEWLINE);
      break;
    case BMP_ERROR_PREMATUREEOF:
      printf("Premature EOF%s", CFG_PRINTF_NEWLINE);
	  break;
    case BMP_ERROR_NONE:
	  break;
  }
}

#endif  
