/**************************************************************************/
/*! 
    @file     lm75b.h
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

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

#ifndef _LM75B_H_
#define _LM75B_H_

#include "projectconfig.h"
#include "core/i2c/i2c.h"

#define LM75B_ADDRESS (0x90) // 100 1000 shifted left 1 bit = 0x90
#define LM75B_READBIT (0x01)

#define LM75B_REGISTER_TEMPERATURE      (0x00)
#define LM75B_REGISTER_CONFIGURATION    (0x01)

#define LM75B_CONFIG_SHUTDOWN_MASK      (0x01)
#define LM75B_CONFIG_SHUTDOWN_POWERON   (0x00)
#define LM75B_CONFIG_SHUTDOWN_SHUTDOWN  (0x01)

typedef enum
{
  LM75B_ERROR_OK = 0,               // Everything executed normally
  LM75B_ERROR_I2CINIT,              // Unable to initialise I2C
  LM75B_ERROR_I2CBUSY,              // I2C already in use
  LM75B_ERROR_LAST
}
lm75bError_e;

lm75bError_e lm75bInit(void);
lm75bError_e lm75bGetTemperature (int32_t *temp);
lm75bError_e lm75bConfigWrite (uint8_t configValue);

#endif


