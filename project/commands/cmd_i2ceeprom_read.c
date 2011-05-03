/**************************************************************************/
/*! 
    @file     cmd_i2ceeprom_read.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_i2ceeprom_read in the 'core/cmd'
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

#ifdef CFG_I2CEEPROM
  #include "drivers/eeprom/eeprom.h"

/**************************************************************************/
/*! 
    Reads a single byte at the supplied EEPROM address
*/
/**************************************************************************/
void cmd_i2ceeprom_read(uint8_t argc, char **argv)
{
  uint16_t addr;
  uint8_t value;

  // Try to convert supplied address to an integer
  int32_t addr32;
  getNumber (argv[0], &addr32);
  
  // Check for invalid values (getNumber may complain about this as well)
  if (addr32 < 0 || eepromCheckAddress(addr32))
  {
    printf("Address out of range %s", CFG_PRINTF_NEWLINE);
    return;
  }

  // Address seems to be OK
  addr = (uint16_t)addr32;
  value = eepromReadU8(addr);

  printf("0x%02X%s", value, CFG_PRINTF_NEWLINE);
}

#endif
