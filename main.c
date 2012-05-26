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

void setupPrimary();
void setupSecondary();

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
    gpioSetDir(2, 9, gpioDirection_Input);
    gpioSetPullup(&IOCON_PIO2_9, gpioPullupMode_PullUp);
    
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

    if(gpioGetValue(2, 9)) {
        setupPrimary();
        gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, 1);
    } else {
        setupSecondary();
        gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, 0);
    }
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
    pidProgram[340] = (Command){COMMAND_TYPE_HOLD, 600, 3600};
    pidProgram[350] = (Command){COMMAND_TYPE_RISE, 0, 0};
}

void setupSecondary() {
    pidProgram[10] = (Command){COMMAND_TYPE_RISE, 200, 0};
    pidProgram[20] = (Command){COMMAND_TYPE_HOLD, 200, 900};
    pidProgram[30] = (Command){COMMAND_TYPE_RISE, 400, 0};
    pidProgram[40] = (Command){COMMAND_TYPE_HOLD, 400, 900};
    pidProgram[50] = (Command){COMMAND_TYPE_RISE, 600, 0};
    pidProgram[60] = (Command){COMMAND_TYPE_HOLD, 600, 900};
    pidProgram[70] = (Command){COMMAND_TYPE_RISE, 800, 0};
    pidProgram[80] = (Command){COMMAND_TYPE_HOLD, 800, 900};
    pidProgram[90] = (Command){COMMAND_TYPE_RISE, 960, 0};
    pidProgram[100] = (Command){COMMAND_TYPE_HOLD, 960, 900};
    pidProgram[110] = (Command){COMMAND_TYPE_RISE, 990, 0};
    pidProgram[120] = (Command){COMMAND_TYPE_HOLD, 990, 300};
    pidProgram[130] = (Command){COMMAND_TYPE_RISE, 600, 0};
    pidProgram[140] = (Command){COMMAND_TYPE_HOLD, 600, 7200};
    pidProgram[150] = (Command){COMMAND_TYPE_RISE, 0, 0};
}