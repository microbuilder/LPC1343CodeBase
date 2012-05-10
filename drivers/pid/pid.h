/**************************************************************************/
/*! 
    @file     pid.h
    @author   Albertas Mickenas (wemakethings.net)

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

#ifndef _PID_H_
#define _PID_H_

#include "projectconfig.h"

uint16_t setPoint;

typedef struct {
    int16_t dState;
    int16_t iState;
    int16_t iMin, iMax;
    
    double iGain, pGain, dGain;
} Pid;

Pid pid;

uint16_t updatePID(Pid *pid, int16_t error, uint16_t position) ;

#define COMMAND_TYPE_RISE 1
#define COMMAND_TYPE_HOLD 2

typedef struct {
    int type;
    uint16_t temperature;
    uint32_t time;
} Command;

#define MAX_PID_PROGRAM_LENGHT 400

Command pidProgram[MAX_PID_PROGRAM_LENGHT];

void initPidProgram();
void addPidCommand(uint16_t index, Command cmd);
void deletePidCommand(uint16_t index);
void printPidProgram();
void processPidProgramStep(uint16_t temp);
void startPidProgram(uint16_t index);
void stopPidProgram();

void setupPrimary();
#endif