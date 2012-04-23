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
