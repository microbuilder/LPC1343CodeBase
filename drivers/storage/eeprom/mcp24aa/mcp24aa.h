/**************************************************************************/
/*! 
    @file     mcp24aa.h
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

#ifndef _MCP24AA_H_
#define _MCP24AA_H_

#include "projectconfig.h"

#define MCP24AA_ADDR    0xA0          // 10100000
#define MCP24AA_RW      0x01
#define MCP24AA_READBIT 0x01
#define MCP24AA_MAXADDR 0xFFF         // 4K = 4096

typedef enum
{
  MCP24AA_ERROR_OK = 0,               // Everything executed normally
  MCP24AA_ERROR_I2CINIT,              // Unable to initialise I2C
  MCP24AA_ERROR_I2CBUSY,              // I2C already in use
  MCP24AA_ERROR_ADDRERR,              // Address out of range
  MCP24AA_ERROR_BUFFEROVERFLOW,       // Max 8 bytes can be read/written in one operation
  MCP24AA_ERROR_LAST
}
mcp24aaError_e;

mcp24aaError_e mcp24aaInit (void);
mcp24aaError_e mcp24aaReadBuffer (uint16_t address, uint8_t *buffer, uint32_t bufferLength);
mcp24aaError_e mcp24aaWriteBuffer (uint16_t address, uint8_t *buffer, uint32_t bufferLength);
mcp24aaError_e mcp24aaReadByte (uint16_t address, uint8_t *buffer);
mcp24aaError_e mcp24aaWriteByte (uint16_t address, uint8_t value);


#endif
