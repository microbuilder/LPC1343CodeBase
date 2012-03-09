/**************************************************************************/
/*! 
    @file     smallfonts.h
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
#ifndef __SMALLFONTS_H_
#define __SMALLFONTS_H_

/* Partially based on original code for the KS0108 by Stephane Rey */
/* Current version by Kevin Townsend */
/* Last Updated: 12 May 2009 */

#include "projectconfig.h"

struct FONT_DEF 
{
    uint8_t u8Width;     	/* Character width for storage         */
    uint8_t u8Height;  	/* Character height for storage        */
    uint8_t u8FirstChar;     /* The first character available       */
    uint8_t u8LastChar;      /* The last character available        */
    const uint8_t *au8FontTable;   /* Font table start address in memory  */
};

extern const struct FONT_DEF Font_System3x6;
extern const struct FONT_DEF Font_System5x8;
extern const struct FONT_DEF Font_System7x8;
extern const struct FONT_DEF Font_8x8;
extern const struct FONT_DEF Font_8x8Thin;

extern const uint8_t au8FontSystem3x6[];
extern const uint8_t au8FontSystem5x8[];
extern const uint8_t au8FontSystem7x8[];
extern const uint8_t au8Font8x8[];
extern const uint8_t au8Font8x8Thin[];

#endif
