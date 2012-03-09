/**************************************************************************/
/*! 
    @file     samsung_20T202DA2JA.h
    @author   Original source : Limor Fried/ladyada (Adafruit Industries)
              LPC1343 port    : K. Townsend

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012 Adafruit Industries
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
#ifndef __SAMSUNG_20T202DA2JA__
#define __SAMSUNG_20T202DA2JA__

#include "projectconfig.h"

// Pin Definitions
#define SAMSUNGVFD_SIO_PORT               (2)    // Data
#define SAMSUNGVFD_SIO_PIN                (1)
#define SAMSUNGVFD_STB_PORT               (2)    // Strobe
#define SAMSUNGVFD_STB_PIN                (2)
#define SAMSUNGVFD_SCK_PORT               (2)    // Clock
#define SAMSUNGVFD_SCK_PIN                (3)

// Commands
#define SAMSUNGVFD_CLEARDISPLAY           0x01
#define SAMSUNGVFD_RETURNHOME             0x02
#define SAMSUNGVFD_ENTRYMODESET           0x04
#define SAMSUNGVFD_DISPLAYCONTROL         0x08
#define SAMSUNGVFD_CURSORSHIFT            0x10
#define SAMSUNGVFD_FUNCTIONSET            0x30
#define SAMSUNGVFD_SETCGRAMADDR           0x40
#define SAMSUNGVFD_SETDDRAMADDR           0x80

// flags for display entry mode
#define SAMSUNGVFD_ENTRYRIGHT             0x00
#define SAMSUNGVFD_ENTRYLEFT              0x02
#define SAMSUNGVFD_ENTRYSHIFTINCREMENT    0x01
#define SAMSUNGVFD_ENTRYSHIFTDECREMENT    0x00

// flags for display on/off control
#define SAMSUNGVFD_DISPLAYON              0x04
#define SAMSUNGVFD_DISPLAYOFF             0x00
#define SAMSUNGVFD_CURSORON               0x02
#define SAMSUNGVFD_CURSOROFF              0x00
#define SAMSUNGVFD_BLINKON                0x01
#define SAMSUNGVFD_BLINKOFF               0x00

// flags for display/cursor shift
#define SAMSUNGVFD_DISPLAYMOVE            0x08
#define SAMSUNGVFD_CURSORMOVE             0x00
#define SAMSUNGVFD_MOVERIGHT              0x04
#define SAMSUNGVFD_MOVELEFT               0x00

// flags for function set
#define SAMSUNGVFD_2LINE                  0x08
#define SAMSUNGVFD_1LINE                  0x00
#define SAMSUNGVFD_BRIGHTNESS25           0x03
#define SAMSUNGVFD_BRIGHTNESS50           0x02
#define SAMSUNGVFD_BRIGHTNESS75           0x01
#define SAMSUNGVFD_BRIGHTNESS100          0x00

#define SAMSUNGVFD_SPICOMMAND             0xF8
#define SAMSUNGVFD_SPIDATA                0xFA

// Initialisation/Config Prototypes
void samsungvfdInit ( uint8_t brightness );
void samsungvfdWrite ( uint8_t value ); 
void samsungvfdWriteString( const char * str );
void samsungvfdSetBrightness ( uint8_t brightness );
void samsungvfdNoDisplay ( void );
void samsungvfdDisplay ( void );
void samsungvfdNoCursor ( void ); 
void samsungvfdCursor ( void ); 
void samsungvfdNoBlink ( void );
void samsungvfdBlink ( void );
void samsungvfdScrollDisplayLeft ( void );
void samsungvfdScrollDisplayRight ( void );
void samsungvfdLeftToRight ( void );
void samsungvfdRightToLeft ( void );
void samsungvfdAutoscroll ( void );
void samsungvfdNoAutoscroll ( void );
void samsungvfdClear ( void );
void samsungvfdHome ( void );
void samsungvfdSetCursor ( uint8_t row, uint8_t col );

#endif
