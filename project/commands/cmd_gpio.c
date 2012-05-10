/**************************************************************************/
/*! 
    @file     cmd_pio.c
    @author   Albertas Mickenas (wemakethings.net)

    @brief    GPIO controll via command line.

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
#include "core/gpio/gpio.h"
#include "drivers/pid/pid.h"

#ifdef CFG_PRINTF_UART
  #include "core/uart/uart.h"
#endif

void cmd_gpioSetOutput(uint8_t argc, char **argv) {
    int32_t port;
    int32_t pin;
    
    getNumber (argv[0], &port);
    getNumber (argv[1], &pin);
    
    gpioSetDir (port, pin, gpioDirection_Output);
}

void cmd_gpioSetInput(uint8_t argc, char **argv) {
    int32_t port;
    int32_t pin;
    
    getNumber (argv[0], &port);
    getNumber (argv[1], &pin);
    
    gpioSetDir (port, pin, gpioDirection_Input);
}

void cmd_gpioSet(uint8_t argc, char **argv) {
    int32_t port;
    int32_t pin;
    int32_t value;
    
    getNumber (argv[0], &port);
    getNumber (argv[1], &pin);
    getNumber (argv[2], &value);
    
    gpioSetDir (port, pin, gpioDirection_Output);
    gpioSetValue(port, pin, value);
}

void cmd_setTemperature(uint8_t argc, char **argv) {
    int32_t sp;
    getNumber(argv[0], &sp);
    setPoint = (uint16_t) sp;
}

void cmd_pid(uint8_t argc, char **argv) {
    int32_t p, i, d;
    getNumber(argv[0], &p);
    getNumber(argv[1], &i);
    getNumber(argv[2], &d);
    pid.pGain = (double) ((double) p / 1000.0);
    pid.iGain = (double) i / 1000.0;
    pid.dGain = (double) d / 1000.0;
    
    pid.iState = 0;
    pid.iMax = 5.0 / pid.iGain;
    pid.iMin = 5.0 / pid.iGain * -1;
}

void cmd_list(uint8_t argc, char **argv) {
    printPidProgram();
}

void cmd_addCommand(uint8_t argc, char **argv) {
    char *rise = "rise";
    char *hold = "hold";
    Command c;
    int32_t index, temp, time;
    
    getNumber(argv[0], &index);
    
    printf("index: %d ", index);
    if(!strcmp(argv[1], "rise")) {
        printf(" rise ");
        c.type = COMMAND_TYPE_RISE;
        getNumber(argv[2], &temp);
        c.temperature = temp;
        c.time = 0;
    } else if(!strcmp(argv[1], "hold")) {
        printf(" hold ");
        c.type = COMMAND_TYPE_HOLD;
        getNumber(argv[2], &temp);
        c.temperature = temp;
        getNumber(argv[3], &time);
        c.time = time;
    }
    printf(" adding PID command...");
    addPidCommand(index, c);
}

void cmd_delCommand(uint8_t argc, char **argv) {
    int32_t index;
    getNumber(argv[0], &index);
    deletePidCommand(index);
}

void cmd_start(uint8_t argc, char **argv) {
    int32_t index = 0;
    if(argc > 0) {
        getNumber(argv[0], &index);        
    }
    startPidProgram(index);
}

void cmd_stop(uint8_t argc, char **argv) {
    stopPidProgram();
}
