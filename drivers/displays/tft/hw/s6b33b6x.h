/**************************************************************************/
/*!
    @file     s6b33b6x.h
    @author   K. Townsend (www.adafruit.com)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, Adafruit Industries
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
#ifndef __S6B33B6X_H__
#define __S6B33B6X_H__

#include "projectconfig.h"
#include "drivers/displays/tft/lcd.h"
#include "core/gpio/gpio.h"

/*=========================================================================
    SPI modes
    -----------------------------------------------------------------------
    The display supports both 3-pin and 4-pin SPI modes

    3-PIN MODE      Saves one GPIO pin (D/C) by adding the D/C bit before
                    every transfer, meaning that 9 bits are sent every frame
                    instead of 8.  You have slightly slower performance
                    but use less pins. Requires 3 GPIO pins (SCK, SID, RST)

    4-PIN MODE      Uses a normal SPI interface with 8-bits per transfer,
                    plus a dedicated D/C pin to indicate if this is a
                    command or data byte. Requires 4 GPIO pins (SCK, SID,
                    RST, DC).

    To select one of the SPI modes, make sure the pins are correctly
    set on the display and uncomment the appropriate define below.
    -----------------------------------------------------------------------*/
    // #define S6B33B6X_BUS_SPI3
    #define S6B33B6X_BUS_SPI4
/*=========================================================================*/

// Control pins
#define S6B33B6X_SCK_PORT                (2) // SCK
#define S6B33B6X_SCK_PIN                 (1)
#define S6B33B6X_SID_PORT                (2) // DATAIN/MOSI
#define S6B33B6X_SID_PIN                 (2)
#define S6B33B6X_CS_PORT                 (2) // CS
#define S6B33B6X_CS_PIN                  (3)
#define S6B33B6X_RST_PORT                (2) // RST
#define S6B33B6X_RST_PIN                 (4)
#define S6B33B6X_DC_PORT                 (2) // D/I (only required for 4-pin SPI)
#define S6B33B6X_DC_PIN                  (5)

// These registers allow fast single cycle clear+set of bits (see section 8.5.1 of LPC1343 UM)
#define S6B33B6X_GPIO2DATA_SCK          (*(pREG32 (GPIO_GPIO2_BASE + ((1 << S6B33B6X_SCK_PIN) << 2))))
#define S6B33B6X_GPIO2DATA_SID          (*(pREG32 (GPIO_GPIO2_BASE + ((1 << S6B33B6X_SID_PIN) << 2))))
#define S6B33B6X_GPIO2DATA_CS           (*(pREG32 (GPIO_GPIO2_BASE + ((1 << S6B33B6X_CS_PIN) << 2))))
#define S6B33B6X_GPIO2DATA_RST          (*(pREG32 (GPIO_GPIO2_BASE + ((1 << S6B33B6X_RST_PIN) << 2))))
#define S6B33B6X_GPIO2DATA_DC           (*(pREG32 (GPIO_GPIO2_BASE + ((1 << S6B33B6X_DC_PIN) << 2))))

// Macros for control line state
#define CLR_SCK                         do { S6B33B6X_GPIO2DATA_SCK = (0); } while (0)
#define SET_SCK                         do { S6B33B6X_GPIO2DATA_SCK = (1 << S6B33B6X_SCK_PIN); } while (0)
#define CLR_SID                         do { S6B33B6X_GPIO2DATA_SID = (0); } while (0)
#define SET_SID                         do { S6B33B6X_GPIO2DATA_SID = (1 << S6B33B6X_SID_PIN); } while (0)
#define CLR_CS                          do { S6B33B6X_GPIO2DATA_CS  = (0); } while (0)
#define SET_CS                          do { S6B33B6X_GPIO2DATA_CS  = (1 << S6B33B6X_CS_PIN); } while (0)
#define CLR_RST                         do { S6B33B6X_GPIO2DATA_RST = (0); } while (0)
#define SET_RST                         do { S6B33B6X_GPIO2DATA_RST = (1 << S6B33B6X_RST_PIN); } while (0)
#define CLR_DC                          do { S6B33B6X_GPIO2DATA_DC  = (0); } while (0)
#define SET_DC                          do { S6B33B6X_GPIO2DATA_DC  = (1 << S6B33B6X_DC_PIN); } while (0)

// S6B33B6X Commands
enum
{
  S6B33B6X_CMD_NOP                      = 0x00,
  S6B33B6X_CMD_OSCILLATIONMODESET       = 0x02,
  S6B33B6X_CMD_DRIVEROUTPUTMODESET      = 0x10,
  S6B33B6X_CMD_MONITORSIGNALCONTROL     = 0x18,
  S6B33B6X_CMD_DCDCCONTROL              = 0x20,
  S6B33B6X_CMD_DCDCCLOCKDIVISIONSET     = 0x24,
  S6B33B6X_CMD_DCDCAMPONOFFSET          = 0x26,
  S6B33B6X_CMD_TEMPCOMPENSATIONSET      = 0x28,
  S6B33B6X_CMD_CONTRASTCONTROL          = 0x2A,
  S6B33B6X_CMD_STANDBYMODEOFF           = 0x2C,
  S6B33B6X_CMD_STANDBYMODEON            = 0x2D,
  S6B33B6X_CMD_ADDRESSINGMODESET        = 0x30,
  S6B33B6X_CMD_ROWVECTORMODESET         = 0x32,
  S6B33B6X_CMD_NBLOCKINVERSIONSET       = 0x34,
  S6B33B6X_CMD_DRIVINGMODESET           = 0x36,
  S6B33B6X_CMD_ENTRYMODESET             = 0x40,
  S6B33B6X_CMD_ROWADDRESSAREASET        = 0x42,
  S6B33B6X_CMD_COLUMNADDRESSAREASET     = 0x43,
  S6B33B6X_CMD_RAMSKIPAREASET           = 0x45,
  S6B33B6X_CMD_DISPLAYOFF               = 0x50,
  S6B33B6X_CMD_DISPLAYON                = 0x51,
  S6B33B6X_CMD_SPECIFIEDDISPLAYPATTERN  = 0x53,
  S6B33B6X_CMD_PARTIALDISPLAYMODESET    = 0x55,
  S6B33B6X_CMD_PARTIALDISPLAYSTARTLINE  = 0x56,
  S6B33B6X_CMD_PARTIALDISPLAYENDLINE    = 0x57,
  S6B33B6X_CMD_OTPMODEOFF               = 0xEA,
  S6B33B6X_CMD_OTPMODEON                = 0xEB,
  S6B33B6X_CMD_OFFSETVOLUMESET          = 0xED,
  S6B33B6X_CMD_OTPWRITEENABLE           = 0xEF
};

void s6b33b6xSetCursor(uint8_t x, uint8_t y);
void s6b33b6xSendByte(uint8_t byte, uint8_t command);

#endif
