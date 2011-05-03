/**************************************************************************/
/*! 
    @file     stepper.c
    @author   Based on original code by Tom Igoe
              Modified by K. Townsend (microBuilder.eu)
              
    @brief    Simple bi-polar stepper motor controller, based on the
              Arduino stepper library by Tom Igoe.  Includes simple
              position handling methods to keep track of the motor's
              relative position and the spindle's current rotation.

    @section Example

    @code 
 
    #include "sysinit.h"
    #include "core/systick/systick.h"
    #include "drivers/motor/stepper/stepper.h"

    ...

    systemInit();             // Configure cpu and mandatory peripherals

    stepperInit(200);         // Initialise driver for 200-step motor
    stepperSetSpeed(60);      // Set speed to 60 rpm (1 revolution per second)

    while (1)
    {
      stepperStep(400);       // Move forward 400 steps
      stepperStep(-200);      // Move backward 200 steps
      systickDelay(1000);     // Wait one second

      // Move 'home' after 10 loops (current position = 2000)
      if (stepperGetPosition() == 2000)
      {
        stepperMoveHome();    // Move back to the starting position
        systickDelay(1000);   // Wait one second
      }
    }

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
#include <stdlib.h>

#include "stepper.h"
#include "core/gpio/gpio.h"
#include "core/timer32/timer32.h"

static int64_t  stepperPosition = 0;          // The current position (in steps) relative to 'Home'
static uint32_t stepperStepNumber = 0;        // The current position (in steps) relative to 0°
static uint32_t stepperStepsPerRotation = 0;  // Number of steps in a full 360° rotation
static uint32_t stepperStepDelay = 0;         // Delay in CPU ticks between individual steps

/**************************************************************************/
/*! 
    Private - Cause the motor to step forward or backward one step
*/
/**************************************************************************/
void stepMotor(uint32_t thisStep)
{
  switch (thisStep)
  {
    case 0: // 1010
      gpioSetValue(STEPPER_IN1_PORT, STEPPER_IN1_PIN, 1);
      gpioSetValue(STEPPER_IN2_PORT, STEPPER_IN2_PIN, 0);
      gpioSetValue(STEPPER_IN3_PORT, STEPPER_IN3_PIN, 1);
      gpioSetValue(STEPPER_IN4_PORT, STEPPER_IN4_PIN, 0);
      break;
    case 1: // 0110
      gpioSetValue(STEPPER_IN1_PORT, STEPPER_IN1_PIN, 0);
      gpioSetValue(STEPPER_IN2_PORT, STEPPER_IN2_PIN, 1);
      gpioSetValue(STEPPER_IN3_PORT, STEPPER_IN3_PIN, 1);
      gpioSetValue(STEPPER_IN4_PORT, STEPPER_IN4_PIN, 0);
      break;
    case 2: // 0101
      gpioSetValue(STEPPER_IN1_PORT, STEPPER_IN1_PIN, 0);
      gpioSetValue(STEPPER_IN2_PORT, STEPPER_IN2_PIN, 1);
      gpioSetValue(STEPPER_IN3_PORT, STEPPER_IN3_PIN, 0);
      gpioSetValue(STEPPER_IN4_PORT, STEPPER_IN4_PIN, 1);
      break;
    case 3: // 1001
      gpioSetValue(STEPPER_IN1_PORT, STEPPER_IN1_PIN, 1);
      gpioSetValue(STEPPER_IN2_PORT, STEPPER_IN2_PIN, 0);
      gpioSetValue(STEPPER_IN3_PORT, STEPPER_IN3_PIN, 0);
      gpioSetValue(STEPPER_IN4_PORT, STEPPER_IN4_PIN, 1);
      break;     
  }
}

/**************************************************************************/
/*! 
    @brief      Initialises the GPIO pins and delay timer and sets any
                default values.

    @param[in]  steps
                The number of steps per rotation (typically 200 or 400)
*/
/**************************************************************************/
void stepperInit(uint32_t steps)
{
  // Setup motor control pins
  gpioSetDir(STEPPER_IN1_PORT, STEPPER_IN1_PIN, 1);
  gpioSetDir(STEPPER_IN2_PORT, STEPPER_IN2_PIN, 1);
  gpioSetDir(STEPPER_IN3_PORT, STEPPER_IN3_PIN, 1);
  gpioSetDir(STEPPER_IN4_PORT, STEPPER_IN4_PIN, 1);

  gpioSetValue(STEPPER_IN1_PORT, STEPPER_IN1_PIN, 0);
  gpioSetValue(STEPPER_IN2_PORT, STEPPER_IN2_PIN, 0);
  gpioSetValue(STEPPER_IN3_PORT, STEPPER_IN3_PIN, 0);
  gpioSetValue(STEPPER_IN4_PORT, STEPPER_IN4_PIN, 0);

  // Set the number of steps per rotation
  stepperStepsPerRotation = steps;

  // Set the default speed (2 rotations per second)
  stepperSetSpeed(120);
}

/**************************************************************************/
/*! 
    @brief    Gets the current position (in steps) relative to 'Home'.

    @return   The difference (in steps) of the motor's current position
              from the original 'Home' position. Value can be negative or 
              positive depending on the direction of previous movements.
*/
/**************************************************************************/
int64_t stepperGetPosition()
{
  return stepperPosition;
}

/**************************************************************************/
/*! 
    @brief    Gets the motor's current rotation (in steps) relative to
              the spindle's 'Zero' position.

    @return   The current step (0 .. steps per rotation) on the motor's
              spindle relative to 0Â°.  Value is always positive.
*/
/**************************************************************************/
uint32_t stepperGetRotation()
{
  return stepperStepNumber;
}

/**************************************************************************/
/*! 
    @brief    Sets the motor's current position to 'Home', meaning that
              any future movement will be relative to the current 
              position.
*/
/**************************************************************************/
void stepperSetHome()
{
  stepperPosition = 0;
}

/**************************************************************************/
/*! 
    @brief    Moves the motor back to the original 'Home' position.
*/
/**************************************************************************/
void stepperMoveHome()
{
  stepperStep(stepperPosition * -1);
}

/**************************************************************************/
/*! 
    @brief    Saves the spindle's current angle/position as 0Â°.  Each
              step the spindle takes will now be relative to the spindle's
              current position.
*/
/**************************************************************************/
void stepperSetZero()
{
  stepperStepNumber = 0;
}

/**************************************************************************/
/*! 
    @brief    Moves the motor to its original rotation value. For example,
              if a 200-step motor is currently rotated to step 137, it
              will move the motor forward 63 steps to end at step 0 or 0Â°.
*/
/**************************************************************************/
void stepperMoveZero()
{
  if (!stepperStepNumber)
  {
    stepperStep(stepperStepsPerRotation - stepperStepNumber);
  }
}

/**************************************************************************/
/*! 
    @brief    Sets the motor speed in rpm, meaning the number of times the
              motor will fully rotate in a one minute period.

    @param[in]  rpm
                Motor speed in revolutions per minute (RPM)

    @warning  Not all motors will function at all speeds, and some trial
              and error may be required to find an appropriate speed for
              the motor.
*/
/**************************************************************************/
void stepperSetSpeed(uint32_t rpm)
{
  uint32_t ticksOneRPM = ((CFG_CPU_CCLK/SCB_SYSAHBCLKDIV) / stepperStepsPerRotation) * 60;

  // Set stepper RPM
  stepperStepDelay = ticksOneRPM / rpm;

  // Initialise 32-bit timer 0 with the appropriate delay
  timer32Init(0, stepperStepDelay);
  timer32Enable(0);
}

/**************************************************************************/
/*! 
    @brief      Moves the motor forward or backward the specified number
                of steps.  A positive number moves the motor forward,
                while a negative number moves the motor backwards.

    @param[in]  steps
                The number of steps to move foreward (positive) or
                backward (negative)
*/
/**************************************************************************/
void stepperStep(int32_t steps)
{
  uint32_t stepsLeft = abs(steps);          // Force number to be positive

  while (stepsLeft > 0)
  {
    // Wait 1 tick between individual steps
    timer32Delay(0, 1);

    // Increment or decrement step counters (depending on direction)
    if (steps > 0)
    {
      stepperPosition++;          // Increment global position counter
      stepperStepNumber++;        // Increment single rotation counter
      if (stepperStepNumber == stepperStepsPerRotation)
      {
        stepperStepNumber = 0;
      }
    }
    else
    {
      stepperPosition--;          // Decrement global position counter
      if (stepperStepNumber == 0)
      {
        stepperStepNumber = stepperStepsPerRotation;
      }
      stepperStepNumber--;        // Decrement single rotation counter
    }

    // Decrement number of remaining steps
    stepsLeft--;

    // Step the motor one step
    stepMotor(stepperStepNumber % 4);
  }
}



