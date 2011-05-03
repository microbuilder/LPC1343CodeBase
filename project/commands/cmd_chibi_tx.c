/**************************************************************************/
/*! 
    @file     cmd_chibi_tx.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_chibi_tx in the 'core/cmd'
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

#ifdef CFG_CHIBI
  #include "drivers/chibi/chb.h"
  #include "drivers/chibi/chb_drvr.h"

/**************************************************************************/
/*! 
    Sends text or data via Chibi
    
*/
/**************************************************************************/
void cmd_chibi_tx(uint8_t argc, char **argv)
{
  uint8_t i, len, *data_ptr, data[50];
  uint16_t addr;

  // Try to convert supplied address to an integer
  int32_t addr32;
  getNumber (argv[0], &addr32);
  
  // Check for invalid values (getNumber may complain about this as well)
  if (addr32 <= 0 || addr32 > 0xFFFF)
  {
    printf("Invalid Address: 1-65534 or 0x0001-0xFFFE required.%s", CFG_PRINTF_NEWLINE);
    return;
  }

  // Address seems to be OK
  addr = (uint16_t)addr32;

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

  // Send message
  chb_write(addr, data, data_ptr - data);
}

#endif
