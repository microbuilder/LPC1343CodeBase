/**************************************************************************/
/*! 
    @file     touchscreen.h
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
#ifndef __TOUCHSCREEN_H__
#define __TOUCHSCREEN_H__

#include "projectconfig.h"

#define TS_XP_PORT        (1)
#define TS_XP_PIN         (0)
#define TS_XP_FUNC_GPIO   do {IOCON_JTAG_TMS_PIO1_0 &= ~(IOCON_JTAG_TMS_PIO1_0_FUNC_MASK | IOCON_JTAG_TMS_PIO1_0_ADMODE_MASK); IOCON_JTAG_TMS_PIO1_0 |= IOCON_JTAG_TMS_PIO1_0_FUNC_GPIO;} while (0)
#define TS_XP_FUNC_ADC    do {IOCON_JTAG_TMS_PIO1_0 &= ~(IOCON_JTAG_TMS_PIO1_0_FUNC_MASK | IOCON_JTAG_TMS_PIO1_0_ADMODE_MASK); IOCON_JTAG_TMS_PIO1_0 |= IOCON_JTAG_TMS_PIO1_0_FUNC_AD1;} while (0)
  
#define TS_XM_PORT        (1)
#define TS_XM_PIN         (1)
#define TS_XM_FUNC_GPIO   do {IOCON_JTAG_TDO_PIO1_1 &= ~(IOCON_JTAG_TDO_PIO1_1_FUNC_MASK | IOCON_JTAG_TDO_PIO1_1_ADMODE_MASK); IOCON_JTAG_TDO_PIO1_1 |= IOCON_JTAG_TDO_PIO1_1_FUNC_GPIO;} while (0)
#define TS_XM_FUNC_ADC    do {IOCON_JTAG_TDO_PIO1_1 &= ~(IOCON_JTAG_TDO_PIO1_1_FUNC_MASK | IOCON_JTAG_TDO_PIO1_1_ADMODE_MASK); IOCON_JTAG_TDO_PIO1_1 |= IOCON_JTAG_TDO_PIO1_1_FUNC_AD2;} while (0)
  
#define TS_YP_PORT        (0)
#define TS_YP_PIN         (11)
#define TS_YP_FUNC_GPIO   do {IOCON_JTAG_TDI_PIO0_11 &= ~(IOCON_JTAG_TDI_PIO0_11_FUNC_MASK | IOCON_JTAG_TDI_PIO0_11_ADMODE_MASK); IOCON_JTAG_TDI_PIO0_11 |= IOCON_JTAG_TDI_PIO0_11_FUNC_GPIO;} while (0)
#define TS_YP_FUNC_ADC    do {IOCON_JTAG_TDI_PIO0_11 &= ~(IOCON_JTAG_TDI_PIO0_11_FUNC_MASK | IOCON_JTAG_TDI_PIO0_11_ADMODE_MASK); IOCON_JTAG_TDI_PIO0_11 |= IOCON_JTAG_TDI_PIO0_11_FUNC_AD0;} while (0)
  
#define TS_YM_PORT        (1)
#define TS_YM_PIN         (2)
#define TS_YM_FUNC_GPIO   do {IOCON_JTAG_nTRST_PIO1_2 &= ~(IOCON_JTAG_nTRST_PIO1_2_FUNC_MASK | IOCON_JTAG_nTRST_PIO1_2_ADMODE_MASK); IOCON_JTAG_nTRST_PIO1_2 |= IOCON_JTAG_nTRST_PIO1_2_FUNC_GPIO;} while (0)
#define TS_YM_FUNC_ADC    do {IOCON_JTAG_nTRST_PIO1_2 &= ~(IOCON_JTAG_nTRST_PIO1_2_FUNC_MASK | IOCON_JTAG_nTRST_PIO1_2_ADMODE_MASK); IOCON_JTAG_nTRST_PIO1_2 |= IOCON_JTAG_nTRST_PIO1_2_FUNC_AD3;} while (0)
  
#define TS_YP_ADC_CHANNEL   (0)   // ADC0.0
#define TS_XP_ADC_CHANNEL   (1)   // ADC0.1
#define TS_XM_ADC_CHANNEL   (2)   // ADC0.2
#define TS_YM_ADC_CHANNEL   (3)   // ADC0.3

typedef struct Point 
{
  int32_t x;
  int32_t y;
} tsPoint_t;

typedef struct Matrix 
{
  int32_t An,
          Bn,
          Cn,
          Dn,
          En,
          Fn,
          Divider ;
} tsMatrix_t;

typedef struct
{
  uint32_t xraw;  // Touch screen x
  uint32_t yraw;  // Touch screen Y
  uint16_t xlcd;  // LCD co-ordinate X
  uint16_t ylcd;  // LCD co-ordinate Y
  uint32_t z1;
  uint32_t z2;
  bool valid;     // Whether this is a valid reading or not
} tsTouchData_t;

typedef enum
{
  TS_ERROR_NONE         = 0,
  TS_ERROR_TIMEOUT      = -1,   // Timeout occured before a valid reading
  TS_ERROR_XYMISMATCH   = -2    // Unable to get a stable X/Y value
} tsTouchError_t;

// Method Prototypes
void           tsInit ( void );
tsTouchError_t tsRead(tsTouchData_t* data);
void           tsCalibrate ( void );
tsTouchError_t tsWaitForEvent(tsTouchData_t* data, uint32_t timeoutMS);
int            tsSetThreshhold(uint8_t value);
uint8_t        tsGetThreshhold(void);

#endif
