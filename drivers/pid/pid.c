/**************************************************************************/
/*! 
    @file     pid.c
    @author   Albertas Mickenas (wemakethings.net)
    
    @brief    Generig PID controller.

    @section Example

    @code

    @endcode

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
#include "core/systick/systick.h"
#include "pid.h"
//#include "core/timer32/timer32.h"

uint16_t updatePID(Pid *pid, int16_t error, uint16_t position) {
    int16_t pTerm, iTerm, dTerm;
    
//    printf("iState: %d, iMax: %d, iMin: %d %s", pid->iState, pid->iMax, pid->iMin, CFG_PRINTF_NEWLINE);
    
    pTerm = pid->pGain * (double)error;
    
    
    if(pTerm > 10) {
        pid->iState = 0;
    } else {
        pid->iState += error;
    }
    
    if(pid->iState > pid->iMax) {
        pid->iState = pid->iMax;
    }
    if(pid->iState < pid->iMin) {
        pid->iState = pid->iMin;
    }
    
    iTerm = pid->iGain * (double) pid->iState;
    
    dTerm = pid->dGain * (double)(pid->dState - position);
    pid->dState = position;
//    printf("pTerm: %d, iTerm: %d, iState: %d %s", pTerm, iTerm, pid->iState, CFG_PRINTF_NEWLINE);
    int16_t controlValue = pTerm + dTerm + iTerm;
    
    printf("%d, %d, %d, %d, %d, %d, %d, %s", position, error, controlValue, pTerm, iTerm, dTerm, pid->iState, CFG_PRINTF_NEWLINE);
    
    return controlValue;
}

uint16_t currentCommand = MAX_PID_PROGRAM_LENGHT;
Command EMPTY_COMMAND = {0, 0, 0};

void initPidProgram() {
    int i;
    for(i = 0; i < MAX_PID_PROGRAM_LENGHT; i++) {
        pidProgram[i].type = 0;
    }
}

void printPidCommand(Command cmd) {
    if(cmd.type == COMMAND_TYPE_RISE) {
        printf("RISE to %d%s", cmd.temperature, CFG_PRINTF_NEWLINE);
    } else if(cmd.type == COMMAND_TYPE_HOLD) {
        printf("HOLD on %d for %d seconds%s", cmd.temperature, cmd.time, CFG_PRINTF_NEWLINE);
    }
}

void addPidCommand(uint16_t index, Command cmd) {
    printf("Adding command ");
    printPidCommand(cmd);
    pidProgram[index] = cmd;
}

void deletePidCommand(uint16_t index) {
    if(index > 0 && index < MAX_PID_PROGRAM_LENGHT){
        pidProgram[index].type = 0;
    }
}

void printPidProgram() {
    int i;
    for(i = 0; i < MAX_PID_PROGRAM_LENGHT; i++) {
        if(0 != pidProgram[i].type) {
            if(i == currentCommand) {
                printf("*");
            } else {
                printf(" ");
            }
            
            printf("%d: ", i);
            printPidCommand(pidProgram[i]);
        }
    }
}

Command nextPidCommand() {
    currentCommand++;
    //prasukam tuscias komandas
    while(currentCommand < MAX_PID_PROGRAM_LENGHT && pidProgram[currentCommand].type == 0) {
        currentCommand++;
    }
    if(currentCommand < MAX_PID_PROGRAM_LENGHT) {
        return pidProgram[currentCommand];
    } else {
        currentCommand = MAX_PID_PROGRAM_LENGHT;
        setPoint = 0;
    }
    return EMPTY_COMMAND;
}

Command getCurrentPidCommand() {
    if(currentCommand < MAX_PID_PROGRAM_LENGHT) {
        return pidProgram[currentCommand];
    }
    return EMPTY_COMMAND;
}

uint32_t commandStartedSeconds;

BOOL shouldProgramAdvance(uint16_t temp, uint32_t sysSeconds) {
    if(currentCommand >= MAX_PID_PROGRAM_LENGHT) {
        return FALSE;
    }
    
    if(COMMAND_TYPE_HOLD == getCurrentPidCommand().type) {
        if((sysSeconds - commandStartedSeconds) >= getCurrentPidCommand().time) {
            return TRUE;
        }
    } else if(COMMAND_TYPE_RISE == getCurrentPidCommand().type) {
        if(temp > 900 && temp > getCurrentPidCommand().temperature - 3) {
            return TRUE;
        }
        if(temp >= getCurrentPidCommand().temperature) {
            return TRUE;
        }
    }    
    return FALSE;
}

void executeCommand(Command c, uint32_t sysSeconds) {
    if(COMMAND_TYPE_HOLD == c.type) {
        printf("Processing command: ");
        printPidCommand(c);
        commandStartedSeconds = sysSeconds;
        setPoint = c.temperature;
    } else if(COMMAND_TYPE_RISE == c.type) {
        printf("Processing command: ");
        printPidCommand(c);
        setPoint = c.temperature;
    } else {
        printf("Program ENDED.");
    }
}

void processPidProgramStep(uint16_t temp) {
    if(shouldProgramAdvance(temp, systickGetSecondsActive())) {
        Command c = nextPidCommand();
        executeCommand(c, systickGetSecondsActive());
    }
}

void startPidProgram(uint16_t index) {
    if(index < MAX_PID_PROGRAM_LENGHT) {
        currentCommand = index;
        Command c = getCurrentPidCommand();
        
        //jei nurodytame indexe buvo tuscia komanda, prasukam iki netuscios (arba iki pabaigos)
        if(0 == c.type) {
            c = nextPidCommand();
            if(0 == c.type) {
                printf("Could not start program - no command after index %d%s", index, CFG_PRINTF_NEWLINE);
                return;
            }
        }
        printf("STARTING program at line %d%s", currentCommand, CFG_PRINTF_NEWLINE);
        printPidCommand(c);
        executeCommand(c, systickGetSecondsActive());
    }
}

BOOL isPidProgramRunning() {
    return currentCommand < MAX_PID_PROGRAM_LENGHT;
}

void stopPidProgram() {
    currentCommand = MAX_PID_PROGRAM_LENGHT;
}