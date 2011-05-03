/**************************************************************************/
/*! 
    @file     analogjoystick.h
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

#ifndef _ANALOGJOYSTICK_H_
#define _ANALOGJOYSTICK_H_

#include "projectconfig.h"

#define JOYSTICK_VERT_ADCPORT     (2)     // ADC2
#define JOYSTICK_HORIZ_ADCPORT    (3)     // ADC3
#define JOYSTICK_SEL_PORT         (0)     // Use UART CTS for SEL input (P0.7)
#define JOYSTICK_SEL_PIN          (7)

#define JOYSTICK_SEL_FUNC_GPIO  do {IOCON_PIO0_7 &= ~IOCON_PIO0_7_FUNC_MASK; IOCON_PIO0_7 |= IOCON_PIO0_7_FUNC_GPIO;} while (0)
#define JOYSTICK_VERT_FUNC_ADC  do {IOCON_JTAG_TMS_PIO1_0 &= ~IOCON_JTAG_TMS_PIO1_0_FUNC_MASK; IOCON_JTAG_TMS_PIO1_0 |= IOCON_JTAG_TMS_PIO1_0_FUNC_AD1;} while (0)
#define JOYSTICK_HORIZ_FUNC_ADC do {IOCON_JTAG_nTRST_PIO1_2 &= ~IOCON_JTAG_nTRST_PIO1_2_FUNC_MASK; IOCON_JTAG_nTRST_PIO1_2 |= IOCON_JTAG_nTRST_PIO1_2_FUNC_AD3;} while (0)

typedef enum
{
  JOYSTICK_DIR_NONE = 0,
  JOYSTICK_DIR_NORTH = 1,
  JOYSTICK_DIR_NORTHEAST = 2,
  JOYSTICK_DIR_EAST = 3,
  JOYSTICK_DIR_SOUTHEAST = 4,
  JOYSTICK_DIR_SOUTH = 5,
  JOYSTICK_DIR_SOUTHWEST = 6,
  JOYSTICK_DIR_WEST = 7,
  JOYSTICK_DIR_NORTHWEST = 8
} joystick_direction_t;

void joystickInit(void);
void joystickGetValues(uint32_t *horizontal, uint32_t *vertical, bool *select);
void joystickGetDirection(joystick_direction_t *direction);
void joystickGetRotation(uint32_t *rotation);

#endif
