/**************************************************************************/
/*! 
    @file     as1115.h
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

#ifndef _AS1115_H_
#define _AS1115_H_

#include "projectconfig.h"
#include "core/i2c/i2c.h"

#define AS1115_ADDRESS                (0x00 << 1)
#define AS1115_SUBADDRESS             (0x03 << 1)
#define AS1115_READBIT                (0x01)

// Individual digit registers
#define AS1115_DIGIT0                 (0x01)
#define AS1115_DIGIT1                 (0x02)
#define AS1115_DIGIT2                 (0x03)
#define AS1115_DIGIT3                 (0x04)
#define AS1115_DIGIT4                 (0x05)
#define AS1115_DIGIT5                 (0x06)
#define AS1115_DIGIT6                 (0x07)
#define AS1115_DIGIT7                 (0x08)

// Command registers
#define AS1115_DECODEMODE             (0x09)
#define AS1115_INTENSITY              (0x0A)
#define AS1115_SCANLIMIT              (0x0B)
#define AS1115_SHUTDOWN               (0x0C)
#define AS1115_SELFADDR               (0x2D)
#define AS1115_FEATURE                (0x0E)
#define AS1115_DISPTEST               (0x0F)

// Bit definitions for AS1115_FEATURE
#define AS1115_FEATURE_CLOCKENABLE    (0x01)   // 0 = Internal oscillator, 1 = Use pin CLK for sys clock input
#define AS1115_FEATURE_RESETREGISTERS (0x02)   // 0 = reset disabled, 1 = reset all ctrl registers to default state except function register
#define AS1115_FEATURE_DECODESEL      (0x04)   // 0 = Enable BCD decoding, 1 = HEX decoding
#define AS1115_FEATURE_BLINKENABLE    (0x10)   // 0 = Disable blinking, 1 = Enable blinking
#define AS1115_FEATURE_BLINKFREQSEL   (0x20)   // 0 = 0.5s+0.5s on/off, 1 = 1s+1s on/off
#define AS1115_FEATURE_SYNC           (0x40)   // Syncs blinking on rising edge of pin LD/CS 
#define AS1115_FEATURE_BLINKSTART     (0x80)   // 0 = blinking starts off, 1 = blinking starts on 

typedef enum
{
  AS1115_ERROR_OK = 0,                          // Everything executed normally
  AS1115_ERROR_I2CINIT,                         // Unable to initialise I2C
  AS1115_ERROR_I2CBUSY,                         // I2C already in use
  AS1115_ERROR_UNEXPECTEDRESPONSE,              // Didn't get 0x80 after AS1115_DISPTEST
  AS1115_ERROR_LAST
}
as1115Error_t;

as1115Error_t as1115Init(void);
as1115Error_t as1115Test(void);
as1115Error_t as1115SetDecodeMode(uint8_t x);
as1115Error_t as1115SetBrightness(uint8_t x);
as1115Error_t as1115SetFeature(uint8_t feature);
as1115Error_t as1115WriteBuffer(uint8_t *buffer);

#endif


