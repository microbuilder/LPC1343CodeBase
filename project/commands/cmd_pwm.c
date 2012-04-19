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
#include "core/pwm/pwm.h"

#ifdef CFG_PRINTF_UART
  #include "core/uart/uart.h"
#endif

/**************************************************************************/
/*! 
    PWM command handler
*/
/**************************************************************************/
uint8_t pwmStarted = 0;

void cmd_pwm(uint8_t argc, char **argv) {
    int32_t frequencyTicks = 65535;
    int32_t dutyCycle = 25;
    
    if(argc > 0) {
        getNumber (argv[0], &dutyCycle);
        if(dutyCycle < 1 || dutyCycle > 100) {
            printf("Invalid duty cycle. Duty cycle must be [1 .. 65535]%s", CFG_PRINTF_NEWLINE);
            return;
        }

        if(argc > 1) {
            getNumber (argv[1], &frequencyTicks);
            if(frequencyTicks < 0 || frequencyTicks > 0xffff) {
                printf("Invalid frequency. Frequency must be [1 .. 65535]%s", CFG_PRINTF_NEWLINE);
                return;
            }
        } else {
            frequencyTicks = 65535;
        }
    } else {
        dutyCycle = 25;

    }

    if(! pwmStarted) {
        printf("Initializing PWM%s", CFG_PRINTF_NEWLINE);
        pwmInit();
    }
    
    printf("Setting frequency ticks to %u%s", (uint16_t) frequencyTicks, CFG_PRINTF_NEWLINE);
    pwmSetFrequencyInTicks(frequencyTicks);
    printf("Setting duty cycle to %u%s", (uint16_t) dutyCycle, CFG_PRINTF_NEWLINE);
    pwmSetDutyCycle(dutyCycle);
    if(! pwmStarted) {
        pwmStart();
        pwmStarted = 1;
    }
}