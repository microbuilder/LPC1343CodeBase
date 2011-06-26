/**************************************************************************/
/*! 
    @file     cmd_uart.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_uart in the 'core/cmd'
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
  #include "core/uart/uart.h"

/**************************************************************************/
/*! 
    Gets or sets the UART speed from EEPROM.
*/
/**************************************************************************/
void cmd_uart(uint8_t argc, char **argv)
{
  if (argc > 0)
  {
    // Try to convert supplied value to an integer
    int32_t speed;
    getNumber (argv[0], &speed);
    
    // Check for invalid values (getNumber may complain about this as well)
    if (speed < 9600 || speed  > 115200)
    {
      printf("Invalid baud rate: 9600-115200 required.%s", CFG_PRINTF_NEWLINE);
      return;
    }

    // Write baud rate to EEPROM and reinitialise UART if using it
    printf("Setting UART to: %d%s", (int)speed, CFG_PRINTF_NEWLINE);
    eepromWriteU32(CFG_EEPROM_UART_SPEED, speed);
    #ifdef CFG_PRINTF_UART
    uartInit(speed);
    #endif
  }
  else
  {
    // Display the current baud rate
    #ifdef CFG_PRINTF_UART
      uart_pcb_t *pcb = uartGetPCB();
      printf("%u%s", (unsigned int)(pcb->baudrate), CFG_PRINTF_NEWLINE);
    #else
      // Try to get UART from EEPROM
      uint32_t uartEEPROM = eepromReadU32(CFG_EEPROM_UART_SPEED);
      if ((uartEEPROM < 9600) || (uartEEPROM > 115200))
      {
        printf("UART not set in EEPROM%s", CFG_PRINTF_NEWLINE);
      }
      else
      {
        printf("%u%s", (unsigned int)uartEEPROM, CFG_PRINTF_NEWLINE);
      }
    #endif
  }
}

#endif
