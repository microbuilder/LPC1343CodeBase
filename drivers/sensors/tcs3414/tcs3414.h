/**************************************************************************/
/*! 
    @file     tcs3414.h
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

#ifndef _TCS3414_H_
#define _TCS3414_H_

#include "projectconfig.h"
#include "core/i2c/i2c.h"

#define TCS3414_ADDRESS                           (0x72)    // 0111001 shifted left 1 bit = 0x72 (ADDR = GND or floating)
#define TCS3414_READBIT                           (0x01)

#define TCS3414_COMMAND_BIT                       (0x80)    // Must be 1
#define TCS3414_WORD_BIT                          (0x20)    // 1 = read/write word (rather than byte)

#define TCS3414_REGISTER_CONTROL                  (0x00)
#define TCS3414_REGISTER_TIMING                   (0x01)
#define TCS3414_REGISTER_INTERRUPT                (0x02)
#define TCS3414_REGISTER_INTSOURCE                (0x03)
#define TCS3414_REGISTER_PARTNO_REVID             (0x04)
#define TCS3414_REGISTER_GAIN                     (0x07)
#define TCS3414_REGISTER_LOWTHRESHOLD_LOWBYTE     (0x08)
#define TCS3414_REGISTER_LOWTHRESHOLD_HIGHBYTE    (0x09)
#define TCS3414_REGISTER_HIGHTHRESHOLD_LOWBYTE    (0x0A)
#define TCS3414_REGISTER_HIGHTHRESHOLD_HIGHBYTE   (0x0B)
#define TCS3414_REGISTER_GREENLOW                 (0x10)
#define TCS3414_REGISTER_GREENHIGH                (0x11)
#define TCS3414_REGISTER_REDLOW                   (0x12)
#define TCS3414_REGISTER_REDHIGH                  (0x13)
#define TCS3414_REGISTER_BLUELOW                  (0x14)
#define TCS3414_REGISTER_BLUEHIGH                 (0x15)
#define TCS3414_REGISTER_CLEARLOW                 (0x16)
#define TCS3414_REGISTER_CLEARHIGH                (0x17)

#define TCS3414_CONTROL_POWERON                   (0x03)
#define TCS3414_CONTROL_POWEROFF                  (0x00)

#define TCS3414_GAIN_GAINMASK                     (0x30)
#define TCS3414_GAIN_PRESCALARMASK                (0x07)

/**************************************************************************/
/*! 
    The Gain setting (bit [5:4] in the GAIN register) multiplies the output
    by the specified amount, allowing you to adjust it's sensitivity and
    dynamic range.
*/
/**************************************************************************/
typedef enum
{
  tcs3414Gain_1 = 0x00,
  tcs3414Gain_4 = 0x10,
  tcs3414Gain_16 = 0x20,
  tcs3414Gain_64 = 0x30
} 
tcs3414Gain_t;

/**************************************************************************/
/*! 
    The Prescalar (bits [2:0] in the GAIN register) divides down the
    output by the specified amount, allowing you to adjust it's
    sensitivity and dynamic range.
*/
/**************************************************************************/
typedef enum
{
  tcs3414Prescalar_1 = 0x00,
  tcs3414Prescalar_2 = 0x01,
  tcs3414Prescalar_4 = 0x02,
  tcs3414Prescalar_8 = 0x03,
  tcs3414Prescalar_16 = 0x04,
  tcs3414Prescalar_32 = 0x05,
  tcs3414Prescalar_64 = 0x06
} 
tcs3414Prescalar_t;

/**************************************************************************/
/*! 
    Possible I2C error messages
*/
/**************************************************************************/
typedef enum
{
  TCS3414_ERROR_OK = 0,               // Everything executed normally
  TCS3414_ERROR_I2CINIT,              // Unable to initialise I2C
  TCS3414_ERROR_I2CBUSY,              // I2C already in use
  TCS3414_ERROR_LAST
}
tcs3414Error_e;

tcs3414Error_e tcs3414Init(void);
tcs3414Error_e tcs3414SetSensitivity(tcs3414Gain_t gain, tcs3414Prescalar_t prescalar);
tcs3414Error_e tcs3414GetRGBL (uint16_t *red, uint16_t *green, uint16_t *blue, uint16_t *clear);
uint32_t       tcs3414CalculateCCT (uint16_t red, uint16_t green, uint16_t blue);

#endif


