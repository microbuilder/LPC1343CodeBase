/**************************************************************************/
/*! 
    @file     stepper.h
    @author   K. Townsend (microBuilder.eu)

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

#ifndef _STEPPER_H_
#define _STEPPER_H_

#include "projectconfig.h"

#define STEPPER_IN1_PORT   (3)
#define STEPPER_IN1_PIN    (0)
#define STEPPER_IN2_PORT   (3)
#define STEPPER_IN2_PIN    (1)
#define STEPPER_IN3_PORT   (3)
#define STEPPER_IN3_PIN    (2)
#define STEPPER_IN4_PORT   (3)
#define STEPPER_IN4_PIN    (3)

void     stepperInit( uint32_t steps );
void     stepperSetSpeed( uint32_t rpm );
int64_t  stepperGetPosition();
uint32_t stepperGetRotation();
void     stepperMoveHome();
void     stepperSetHome();
void     stepperMoveZero();
void     stepperSetZero();
void     stepperStep( int32_t steps );

#endif