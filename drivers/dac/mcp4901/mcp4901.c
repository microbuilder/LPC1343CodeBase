/**************************************************************************/
/*!
    @file     mcp4901.c
    @author   Tauon

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, Tauon
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
#include "mcp4901.h"

#define DAC_BUF  0
#define DAC_GA   1
#define DAC_SHDN 1
#define DAC_BITS8

uint16_t settings;

// Control pins
#define MCP4901_GPIODATAREG     (*(pREG32 (0x50023FFC)))   // GPIO2DATA
#define MCP4901_PORT            (2)

#define MCP4901_LDAC            (0)
#define MCP4901_CS_PIN          (6)
#define MCP4901_SCL_PIN         (7)
#define MCP4901_SDI_PIN         (8)

// Macros for control line state
#define CLR_SDI     do { MCP4901_GPIODATAREG &= ~(1<<MCP4901_SDI_PIN); } while(0)
#define SET_SDI     do { MCP4901_GPIODATAREG &= ~(1<<MCP4901_SDI_PIN); MCP4901_GPIODATAREG |= (1<<MCP4901_SDI_PIN); } while(0)
#define CLR_SCL     do { MCP4901_GPIODATAREG &= ~(1<<MCP4901_SCL_PIN); } while(0)
#define SET_SCL     do { MCP4901_GPIODATAREG &= ~(1<<MCP4901_SCL_PIN); MCP4901_GPIODATAREG |= (1<<MCP4901_SCL_PIN); } while(0)
#define CLR_CS      do { MCP4901_GPIODATAREG &= ~(1<<MCP4901_CS_PIN); } while(0)
#define SET_CS      do { MCP4901_GPIODATAREG &= ~(1<<MCP4901_CS_PIN); MCP4901_GPIODATAREG |= (1<<MCP4901_CS_PIN); } while(0)

/*************************************************/

void mcpWriteData(uint8_t data)
{
  uint8_t i = 0;
  for (i=0; i<8; i++)
  {
    if (data & 0x80)
    {
    	SET_SDI;
    }
    else
    {

    	CLR_SDI;
    }
    data <<= 1;
    CLR_SCL;
    SET_SCL;
  }
}

/*************************************************/
void mcpWriteData16(uint16_t data)
{
	mcpWriteData((data>>8) & 0xFF);
	mcpWriteData(data & 0xFF);
}

void  mcp4901Init()
{
	settings = (DAC_BUF << 2 | DAC_GA << 1 | DAC_SHDN) << 12;
	gpioSetDir(MCP4901_PORT, MCP4901_LDAC   , gpioDirection_Output);
	gpioSetDir(MCP4901_PORT, MCP4901_CS_PIN , gpioDirection_Output);
	gpioSetDir(MCP4901_PORT, MCP4901_SCL_PIN, gpioDirection_Output);
	gpioSetDir(MCP4901_PORT, MCP4901_SDI_PIN, gpioDirection_Output);
}
void mcp4901ChangeSettings(bool BUFFER, bool GAIN, bool SHUTDOWN)
{
	settings = (BUFFER << 2 | GAIN << 1 | (!SHUTDOWN)) << 12;
}
void mcp4901LDAC(void)
{
	gpioSetValue(MCP4901_PORT,MCP4901_LDAC,0);
	gpioSetValue(MCP4901_PORT,MCP4901_LDAC,1);
}
void mcp4901SetVoltage( uint8_t output)
{
	uint16_t data;
	#ifdef DAC_BITS8
	data = settings  | (output << 4);
	#endif

	#ifdef DAC_BITS10
	data = settings | (output << 2);
	#endif

	#ifdef DAC_BITS12
	data = settings | output;
	#endif
	CLR_CS;
	mcpWriteData16(data);
	SET_CS;
}


