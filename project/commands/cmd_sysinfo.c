/**************************************************************************/
/*! 
    @file     cmd_sysinfo.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_sysinfo in the 'core/cmd'
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
#include "core/systick/systick.h"
#include "core/iap/iap.h"
#include "project/commands.h"       // Generic helper functions

#ifdef CFG_CHIBI
  #include "drivers/chibi/chb.h"
  #include "drivers/chibi/chb_drvr.h"
#endif

#ifdef CFG_PRINTF_UART
  #include "core/uart/uart.h"
#endif

#ifdef CFG_LM75B
  #include "drivers/sensors/lm75b/lm75b.h"
#endif

#ifdef CFG_TFTLCD
  #include "drivers/lcd/tft/lcd.h"
#endif

#ifdef CFG_SDCARD
  #include "core/gpio/gpio.h"
#endif

/**************************************************************************/
/*! 
    'sysinfo' command handler
*/
/**************************************************************************/
void cmd_sysinfo(uint8_t argc, char **argv)
{
  IAP_return_t iap_return;

  printf("%-25s : %d.%d MHz %s", "System Clock", CFG_CPU_CCLK / 1000000, CFG_CPU_CCLK % 1000000, CFG_PRINTF_NEWLINE);
  printf("%-25s : v%d.%d.%d %s", "Firmware", CFG_FIRMWARE_VERSION_MAJOR, CFG_FIRMWARE_VERSION_MINOR, CFG_FIRMWARE_VERSION_REVISION, CFG_PRINTF_NEWLINE);

  // 128-bit MCU Serial Number
  iap_return = iapReadSerialNumber();
  if(iap_return.ReturnCode == 0)
  {
    printf("%-25s : %08X %08X %08X %08X %s", "Serial Number", iap_return.Result[0],iap_return.Result[1],iap_return.Result[2],iap_return.Result[3], CFG_PRINTF_NEWLINE);
  }

  // CLI and buffer Settings
  #ifdef CFG_INTERFACE
    printf("%-25s : %d bytes %s", "Max CLI Command", CFG_INTERFACE_MAXMSGSIZE, CFG_PRINTF_NEWLINE);
  #endif

  #ifdef CFG_PRINTF_UART
    uart_pcb_t *pcb = uartGetPCB();
    printf("%-25s : %u %s", "UART Baud Rate", (unsigned int)(pcb->baudrate), CFG_PRINTF_NEWLINE);
  #endif

  // TFT LCD Settings (if CFG_TFTLCD enabled)
  #ifdef CFG_TFTLCD
    printf("%-25s : %d %s", "LCD Width", (int)lcdGetWidth(), CFG_PRINTF_NEWLINE);
    printf("%-25s : %d %s", "LCD Height", (int)lcdGetHeight(), CFG_PRINTF_NEWLINE);
  #endif

  // Wireless Settings (if CFG_CHIBI enabled)
  #ifdef CFG_CHIBI
    chb_pcb_t *pcb = chb_get_pcb();
    printf("%-25s : %s %s", "Wireless", "AT86RF212", CFG_PRINTF_NEWLINE);
    printf("%-25s : 0x%04X %s", "802.15.4 PAN ID", CFG_CHIBI_PANID, CFG_PRINTF_NEWLINE);
    printf("%-25s : 0x%04X %s", "802.15.4 Node Address", pcb->src_addr, CFG_PRINTF_NEWLINE);
    printf("%-25s : %d %s", "802.15.4 Channel", CFG_CHIBI_CHANNEL, CFG_PRINTF_NEWLINE);
  #endif

  // System Uptime (based on systick timer)
  printf("%-25s : %u s %s", "System Uptime", (unsigned int)systickGetSecondsActive(), CFG_PRINTF_NEWLINE);

  // System Temperature (if LM75B Present)
  #ifdef CFG_LM75B
    int32_t temp = 0;
    lm75bGetTemperature(&temp);
    temp *= 125;
    printf("%-25s : %d.%d C %s", "Temperature", temp / 1000, temp % 1000, CFG_PRINTF_NEWLINE);
  #endif

  #ifdef CFG_SDCARD
    printf("%-25s : %s %s", "SD Card Present", gpioGetValue(CFG_SDCARD_CDPORT, CFG_SDCARD_CDPIN) ? "True" : "False", CFG_PRINTF_NEWLINE);
  #endif
}
