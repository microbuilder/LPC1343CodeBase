/**************************************************************************/
/*! 
    @file     analogjoystick.c
    @author   K. Townsend (microBuilder.eu)

    @section DESCRIPTION

    Driver for a simple four-way analog joystick.
    
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
#include "analogjoystick.h"
#include "core/adc/adc.h"
#include "core/gpio/gpio.h"

static bool _joystickInitialised = false;

/**************************************************************************/
/*! 
    @brief  Initialises the analog joystick, setting the appropriate
            pins to analog and digital input.
*/
/**************************************************************************/
void joystickInit(void)
{
  // Make sure SEL is set to GPIO and input
  JOYSTICK_SEL_FUNC_GPIO;
  gpioSetDir (JOYSTICK_SEL_PORT, JOYSTICK_SEL_PIN, 0);

  // Make sure that ADC is initialised
  adcInit();

  // Set HORIZ/VERT pins to ADC
  JOYSTICK_VERT_FUNC_ADC;
  JOYSTICK_HORIZ_FUNC_ADC;

  _joystickInitialised = true;
}

/**************************************************************************/
/*! 
    @brief  Gets the raw ADC values for the horizontal and vertical
            axis, as well as the current state of the select button.

    @param[out] horizontal
                pointer to the uint32_t variable that will hold the
                results of the horizontal analog conversion
    @param[out] vertical
                pointer to the uint32_t variable that will hold the
                results of the vertical analog conversion
    @param[out] select
                pointer to the bool variable that will hold the
                current state of the digital select button
*/
/**************************************************************************/
void joystickGetValues(uint32_t *horizontal, uint32_t *vertical, bool *select)
{
  if (!_joystickInitialised) joystickInit();

  // Get current ADC values
  *horizontal = adcRead(JOYSTICK_HORIZ_ADCPORT);
  *vertical = adcRead(JOYSTICK_VERT_ADCPORT);
  *select = gpioGetValue(JOYSTICK_SEL_PORT, JOYSTICK_SEL_PIN);
}

/**************************************************************************/
/*! 
    @brief  Gets the rough direction that the joystick is currently
            positioned at

    @param[out] direction
                The joystick_direction_t (enum) that will be used to
                identify the joysticks approximate current direction.
*/
/**************************************************************************/
void joystickGetDirection(joystick_direction_t *direction)
{
  if (!_joystickInitialised) joystickInit();

  uint32_t hor, ver;
  bool sel;

  // Get current joystick position
  joystickGetValues(&hor, &ver, &sel);

  // ToDo: Process values
  direction = JOYSTICK_DIR_NONE;
}

/**************************************************************************/
/*! 
    @brief  Get the approximate rotation of the joystick from 0-359° in
            a clock-wise direction.

    @param[out] rotation
                Pointer to the uint32_t variable that will hold the
                rotation value.
*/
/**************************************************************************/
void joystickGetRotation(uint32_t *rotation)
{
  if (!_joystickInitialised) joystickInit();

  // ToDo
}
