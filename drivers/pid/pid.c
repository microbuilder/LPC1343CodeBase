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
        //jei nurodytame indexe buvo tuscia komanda, prasukam iki netuscios (arba iki pabaigos)
        if(0 == getCurrentPidCommand().type) {
            Command c = nextPidCommand();
            if(0 != c.type) {
                printf("STARTING program at line %d", currentCommand);
                executeCommand(c, systickGetSecondsActive());
            }
        }
    }
}

void stopPidProgram() {
    currentCommand = MAX_PID_PROGRAM_LENGHT;
}

void setupPrimary() {
    pidProgram[10] = (Command){COMMAND_TYPE_RISE, 60, 0};
    pidProgram[20] = (Command){COMMAND_TYPE_HOLD, 60, 3600};
    pidProgram[30] = (Command){COMMAND_TYPE_RISE, 110, 0};
    pidProgram[40] = (Command){COMMAND_TYPE_HOLD, 110, 900};
    pidProgram[50] = (Command){COMMAND_TYPE_RISE, 160, 0};
    pidProgram[60] = (Command){COMMAND_TYPE_HOLD, 160, 900};
    pidProgram[70] = (Command){COMMAND_TYPE_RISE, 210, 0};
    pidProgram[80] = (Command){COMMAND_TYPE_HOLD, 210, 900};
    pidProgram[90] = (Command){COMMAND_TYPE_RISE, 260, 0};
    pidProgram[100] = (Command){COMMAND_TYPE_HOLD, 260, 900};
    pidProgram[110] = (Command){COMMAND_TYPE_RISE, 310, 0};
    pidProgram[120] = (Command){COMMAND_TYPE_HOLD, 310, 900};
    pidProgram[130] = (Command){COMMAND_TYPE_RISE, 360, 0};
    pidProgram[140] = (Command){COMMAND_TYPE_HOLD, 360, 900};
    pidProgram[150] = (Command){COMMAND_TYPE_RISE, 400, 0};
    pidProgram[160] = (Command){COMMAND_TYPE_HOLD, 400, 900};
    pidProgram[180] = (Command){COMMAND_TYPE_RISE, 470, 0};
    pidProgram[190] = (Command){COMMAND_TYPE_HOLD, 470, 900};
    pidProgram[200] = (Command){COMMAND_TYPE_RISE, 540, 0};
    pidProgram[210] = (Command){COMMAND_TYPE_HOLD, 540, 900};
    pidProgram[220] = (Command){COMMAND_TYPE_RISE, 610, 0};
    pidProgram[230] = (Command){COMMAND_TYPE_HOLD, 610, 900};
    pidProgram[240] = (Command){COMMAND_TYPE_RISE, 680, 0};
    pidProgram[250] = (Command){COMMAND_TYPE_HOLD, 680, 900};
    pidProgram[260] = (Command){COMMAND_TYPE_RISE, 750, 0};
    pidProgram[270] = (Command){COMMAND_TYPE_HOLD, 750, 900};
    pidProgram[280] = (Command){COMMAND_TYPE_RISE, 830, 0};
    pidProgram[290] = (Command){COMMAND_TYPE_HOLD, 830, 900};
    pidProgram[300] = (Command){COMMAND_TYPE_RISE, 900, 0};
    pidProgram[310] = (Command){COMMAND_TYPE_HOLD, 900, 900};
    pidProgram[320] = (Command){COMMAND_TYPE_RISE, 960, 0};
    pidProgram[330] = (Command){COMMAND_TYPE_RISE, 600, 0};
    pidProgram[340] = (Command){COMMAND_TYPE_HOLD, 600, 600};
    pidProgram[350] = (Command){COMMAND_TYPE_RISE, 0, 0};
}