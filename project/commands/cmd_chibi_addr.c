/**************************************************************************/
/*! 
    @file     cmd_chibi_addr.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_chibi_addr in the 'core/cmd'
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

#ifdef CFG_CHIBI
  #include "drivers/chibi/chb.h"
  #include "drivers/chibi/chb_drvr.h"

/**************************************************************************/
/*! 
    Gets or sets the 16-bit sensor node address.  This value can be 
    anything between 1-65534 (0x0001-0xFFFE), and in decimal or
    hexadecimal notation.  All hexadecimal values must be preceded by
    '0x' or '0X' to be properly interpreted (ex. 0x009F).
*/
/**************************************************************************/
void cmd_chibi_addr(uint8_t argc, char **argv)
{
  if (argc > 0)
  {
    // Try to convert supplied value to an integer
    int32_t addr;
    getNumber (argv[0], &addr);
    
    // Check for invalid values (getNumber may complain about this as well)
    if (addr <= 0 || addr > 0xFFFF)
    {
      printf("Invalid Address: 1-65534 or 0x0001-0xFFFE required.%s", CFG_PRINTF_NEWLINE);
      return;
    }
    if (addr == 0xFFFF)
    {
      printf("Invalid Address: 0xFFFF reserved for broadcast.%s", CFG_PRINTF_NEWLINE);
      return;
    }

    // Write address to EEPROM and update peripheral control block
    chb_set_short_addr((uint16_t)addr);
    chb_pcb_t *pcb = chb_get_pcb();
    printf("Address set to: 0x%04X%s", pcb->src_addr, CFG_PRINTF_NEWLINE);
  }
  else
  {
    // Display the current address
    chb_pcb_t *pcb = chb_get_pcb();
    printf("0x%04X%s", pcb->src_addr, CFG_PRINTF_NEWLINE);
  }
}

#endif
