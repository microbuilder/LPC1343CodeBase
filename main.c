/**************************************************************************/
/*! 
    @file     main.c
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, microBuilder SARL
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
#include "projectconfig.h"
#include "sysinit.h"

#include "core/gpio/gpio.h"
#include "core/timer32/timer32.h"
#include "core/systick/systick.h"
#include "core/gpio/gpio.h"
#include "core/adc/adc.h"
#include "drivers/pid/pid.h"

#ifdef CFG_INTERFACE
  #include "core/cmd/cmd.h"
#endif


uint16_t controlLoopTotal = 10; //sekundziu trunka sildytuvo veikimo ciklas
int16_t controlValue = 0;

uint16_t temp;
int16_t error;
BOOL controlLoopDone = TRUE;

#define RING_BUFFER_SIZE (6)

uint16_t ringBuffer[RING_BUFFER_SIZE] = {0, 0, 0, 0, 0, 0};
uint8_t ringIndex = 0;

void heaterOn() {
    gpioSetValue(2, 1, 1);
}

void heaterOff() {
    gpioSetValue(2, 1, 0);
}

uint8_t waitasec = FALSE;
void controlAction() {
    if(timer32GetCount(0) < controlLoopTotal) {
        if((controlValue > 0) && timer32GetCount(0) <= controlValue) {
            heaterOn();
//            printf("Heater ON%s", CFG_PRINTF_NEWLINE);
        } else {
            heaterOff();
//            printf("Heater OFF%s", CFG_PRINTF_NEWLINE);
        }
    } else {
        if(waitasec) {
            timer32Disable(0);
            controlLoopDone = TRUE;
            waitasec = FALSE;
        } else {
            heaterOff();
            waitasec = TRUE;
        }
    }
}

void heaterSetup() {
    gpioSetDir(2, 1, gpioDirection_Output);
    gpioSetValue(2, 1, 0);
}

uint16_t getTemperature() {
    uint32_t adcOversampled = 0;
    uint16_t adc0Value;
    uint16_t j;
    for(j = 0; j < 4096; j++){
        adc0Value = adcReadSingle(0);
        adcOversampled += adc0Value;
    }
    adcOversampled = adcOversampled >> 6;
    double voltage = (double)(((double)adcOversampled / 65535.0F) * 3300.0F);
    
    return (uint16_t)(voltage / 3.0);
    
}

void recordTemp(uint16_t t) {
    ringBuffer[ringIndex] = t;
    ringIndex++;
    if(ringIndex >= RING_BUFFER_SIZE) {
        ringIndex = 0;
    }     
}

uint16_t getAverageTemp() {
    uint8_t i;
    uint16_t sum = 0;
    for(i = 0; i < RING_BUFFER_SIZE; i++){
        sum += ringBuffer[i];
    }
    return sum / RING_BUFFER_SIZE;
}

int main(void)
{
  // Configure cpu and mandatory peripherals
    systemInit();
    
    heaterSetup();
    timer32Init(0, TIMER32_CCLK_1S);
    timer32SetIntHandler(controlAction);
    
    pid.pGain = (double) ((double) 350 / 1000.0);
    pid.iGain = (double) 30 / 1000.0;
    pid.dGain = (double) 400 / 1000.0;
    
    pid.iState = 0;
    pid.iMax = 5.0 / pid.iGain;
    pid.iMin = 5.0 / pid.iGain * -1;
    
    
    uint32_t currentSecond, lastSecond;
    currentSecond = lastSecond = 0;

    setPoint = 0;
    temp = getTemperature();
    recordTemp(temp);

    setupPrimary();
    startPidProgram(0);

    error = setPoint - temp;
    controlValue = updatePID(&pid, error, temp);

  while (1)
  {
//    currentSecond = systickGetSecondsActive();
//    if (currentSecond != lastSecond)
//    {
//      lastSecond = currentSecond;
//      gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, lastSecond % 2);
//
//    }
      
      if(controlLoopDone) {
          timer32ResetCounter(0);
          temp = getTemperature();
          recordTemp(temp);

          processPidProgramStep(getAverageTemp());

          printf("%d, ", getAverageTemp());
          error = setPoint - temp;
          controlValue = updatePID(&pid, error, temp);
          controlLoopDone = FALSE;
          
          timer32Enable(0);
      }
      
    #ifdef CFG_INTERFACE 
      cmdPoll(); 
    #endif
  }

  return 0;
}
