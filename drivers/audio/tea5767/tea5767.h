/**************************************************************************/
/*! 
    @file     tea5767.h
    @author   K. Townsend

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, K. Townsend.
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

#ifndef _TEA5767_H_
#define _TEA5767_H_

#include "projectconfig.h"

#define TEA5767_FMBANDSTART_US_EUROPE            (87500000) // 87.5 MHz to 108 MHz
#define TEA5767_FMBANDSTART_JAPAN                (76000000) // 76 MHz to 91 MHz plus TV audio at 108 MHz

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define TEA5767_ADDRESS                      (0xC0)  // 1100000x
    #define TEA5767_READ                         (0x01)
/*=========================================================================*/

/*=========================================================================
    WRITE BYTE 1
    MUTE | SEARCHMODE | PLL13 | PLL12 | PLL11 | PLL10 | PLL9 | PLL8
    -----------------------------------------------------------------------*/
    #define TEA5767_WBYTE1_MUTE                  (1<<7)  // 1 = mute, 0 = output enabled (mute enabled after reset)
    #define TEA5767_WBYTE1_SEARCHMODE            (1<<6)  // 1 = Search mode enabled
/*=========================================================================*/

/*=========================================================================
    WRITE BYTE 2
    PLL7 | PLL6 | PLL5 | PLL4 | PLL3 | PLL2 | PLL1 | PLL0    
    -----------------------------------------------------------------------*/
    
/*=========================================================================*/

/*=========================================================================
    WRITE BYTE 3    
    SUD | SSL1 | SSL0 | HLSI | MS | MR | ML | SWP1
    -----------------------------------------------------------------------*/
    #define TEA5767_WBYTE3_SEARCHUPDOWN          (1<<7)  // 1 = search up, 0 = search down
    #define TEA5767_WBYTE3_SEARCHSTOPLEVEL1      (1<<6)  // 10 = mid level (ADC = 7), 11 = high level (ADC = 10)
    #define TEA5767_WBYTE3_SEARCHSTOPLEVEL0      (1<<5)  // 00 = invalid, 01 = low level (ADC = 5)  
    #define TEA5767_WBYTE3_SEARCHSTOPLEVELMASK   (3<<5)
    #define TEA5767_WBYTE3_HLSI                  (1<<4)  // 1 = high side LO injection, 0 = low side LO injection
    #define TEA5767_WBYTE3_MONOTOSTEREO          (1<<3)  // 1 = force mono, 0 = stereo on
    #define TEA5767_WBYTE3_MUTERIGHT             (1<<2)  // 1 = mute right audio, 0 = enabled
    #define TEA5767_WBYTE3_MUTELEFT              (1<<1)  // 1 = mute left audio, 0 = enabled
    /*---------------------------------------------------------------------*/
    #define TEA5767_WBYTE3_SEARCHSTOPLEVEL_LOW   (0x20)  // ADC output = 5  (bit: 01)
    #define TEA5767_WBYTE3_SEARCHSTOPLEVEL_MID   (0x40)  // ADC output = 7  (bit: 10)
    #define TEA5767_WBYTE3_SEARCHSTOPLEVEL_HIGH  (0x60)  // ADC output = 10 (bit: 11)
/*=========================================================================*/

/*=========================================================================
    WRITE BYTE 4
    SWP2 | STBY | BL | XTAL | SMUTE | HCC | SNC | SI
    -----------------------------------------------------------------------*/
    #define TEA5767_WBYTE4_STANDBY               (1<<6)  // 1 = standby mode
    #define TEA5767_WBYTE4_BANDLIMITS            (1<<5)  // 1 = Japanese FM band, 0 = US/Europe
    #define TEA5767_WBYTE4_XTAL                  (1<<4)  // Combined with PLLREF in byte 5 (set to 1 for 32.768kHz crystal)
    #define TEA5767_WBYTE4_SOFTMUTE              (1<<3)  // 1 = soft mute enabled
    #define TEA5767_WBYTE4_HIGHCUTCONTROL        (1<<2)  // 1 = HCC enabled
    #define TEA5767_WBYTE4_STEREONOISECANCEL     (1<<1)  // 1 = stereo noise cancelling enabled
/*=========================================================================*/

/*=========================================================================
    WRITE BYTE 5
    PLLREF | DTC | - | - | - | - | - | -
    -----------------------------------------------------------------------*/
    #define TEA5767_WBYTE5_PLLREF                (1<<7)  // 1 = 6.5MHz PLL ref freq. enabled (set to 0 for 32.768kHz crystal)
    #define TEA5767_WBYTE5_DEEMPHASISTIMECONST   (1<<6)  // 1 = DTC is 75µs, 0 = 50µs
/*=========================================================================*/


/*=========================================================================
    READ BYTE 1
    RF | BLF | PLL13 | PLL12 | PLL11 | PLL 10 | PLL9 | PLL8
    -----------------------------------------------------------------------*/
    #define TEA5767_RBYTE1_READYFLAG             (1<<7)  // 1 = station found or band-limit reached, 0 = no station found
    #define TEA5767_RBYTE1_BANDLIMITFLAG         (1<<6)  // 1 = band limit has been reached, 0 = band limit not reached
/*=========================================================================*/

/*=========================================================================
    READ BYTE 2
    PLL7 | PLL6 | PLL5 | PLL4 | PLL3 | PLL2 | PLL1 | PLL0
    -----------------------------------------------------------------------*/

/*=========================================================================*/

/*=========================================================================
    READ BYTE 3
    STEREO | IF6 | IF5 | IF4 | IF3 | IF2 | IF1 | IF0
    -----------------------------------------------------------------------*/
    #define TEA5767_RBYTE3_STEREOINDICATOR       (1<<7)  // 1 = stereo reception, 0 = mono reception
/*=========================================================================*/

/*=========================================================================
    READ BYTE 4
    LEV3 | LEV2 | LEV1 | LEV0 | CI3 | CI2 | CI1 | -
    -----------------------------------------------------------------------*/
    #define TEA5767_RBYTE4_ADCLEVELOUTPUTMASK    (0xF0)  // ADC output level
    #define TEA5767_RBYTE4_CHIPIDMASK            (0x0F)  // These bits must be set to 0!
/*=========================================================================*/

/*=========================================================================
    READ BYTE 5
    - | - | - | - | - | - | - | -
    -----------------------------------------------------------------------*/

/*=========================================================================*/

uint32_t  tea5767Init( void );
void      tea5767SetFrequency( uint32_t );
uint32_t  tea5767GetFrequency( void );
void      tea5767Scan( uint8_t );
void      tea5767Mute( bool );

#endif