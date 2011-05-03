/**************************************************************************/
/*! 
    @file     ILI9328.h
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
#ifndef __ILI9328_H__
#define __ILI9328_H__

#include "projectconfig.h"

#include "drivers/lcd/tft/lcd.h"
#include "core/gpio/gpio.h"

// Control pins
#define ILI9328_CS_PORT           1     // CS (LCD Pin 7)
#define ILI9328_CS_PIN            8
#define ILI9328_CD_PORT           1     // CS/RS (LCD Pin 8)
#define ILI9328_CD_PIN            9
#define ILI9328_WR_PORT           1     // WR (LCD Pin 9)
#define ILI9328_WR_PIN            10
#define ILI9328_RD_PORT           1     // RD (LCD Pin 10)
#define ILI9328_RD_PIN            11

// These combined pin definitions are for optimisation purposes.
// If the pin values above are modified the bit equivalents
// below will also need to be updated
#define ILI9328_CS_CD_PINS        0x300   // 8 + 9
#define ILI9328_RD_WR_PINS        0xC00   // 11 + 10
#define ILI9328_WR_CS_PINS        0x500   // 10 + 8
#define ILI9328_CD_RD_WR_PINS     0xE00   // 9 + 11 + 10
#define ILI9328_CS_CD_RD_WR_PINS  0xF00   // 8 + 9 + 11 + 10

// Backlight and Reset pins
#define ILI9328_RES_PORT          3     // LCD Reset  (LCD Pin 31)
#define ILI9328_RES_PIN           3
#define ILI9328_BL_PORT           2     // Backlight Enable (LCD Pin 16)
#define ILI9328_BL_PIN            9

// Data pins
// Note: data pins must be consecutive and on the same port
#define ILI9328_DATA_PORT         2     // 8-Pin Data Port
#define ILI9328_DATA_PIN1         1
#define ILI9328_DATA_PIN2         2
#define ILI9328_DATA_PIN3         3
#define ILI9328_DATA_PIN4         4
#define ILI9328_DATA_PIN5         5
#define ILI9328_DATA_PIN6         6
#define ILI9328_DATA_PIN7         7
#define ILI9328_DATA_PIN8         8
#define ILI9328_DATA_MASK         0x000001FE
#define ILI9328_DATA_OFFSET       1    // Offset = PIN1

// Placed here to try to keep all pin specific values in header file
#define ILI9328_DISABLEPULLUPS() do { gpioSetPullup(&IOCON_PIO2_1, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_2, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_3, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_4, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_5, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_6, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_7, gpioPullupMode_Inactive); \
                                      gpioSetPullup(&IOCON_PIO2_8, gpioPullupMode_Inactive); } while (0)

// These registers allow fast single operation clear+set of bits (see section 8.5.1 of LPC1343 UM)
#define ILI9328_GPIO2DATA_DATA        (*(pREG32 (GPIO_GPIO2_BASE + (ILI9328_DATA_MASK << 2))))
#define ILI9328_GPIO1DATA_WR          (*(pREG32 (GPIO_GPIO1_BASE + ((1 << ILI9328_WR_PIN) << 2))))
#define ILI9328_GPIO1DATA_CD          (*(pREG32 (GPIO_GPIO1_BASE + ((1 << ILI9328_CD_PIN) << 2))))
#define ILI9328_GPIO1DATA_CS          (*(pREG32 (GPIO_GPIO1_BASE + ((1 << ILI9328_CS_PIN) << 2))))
#define ILI9328_GPIO1DATA_RD          (*(pREG32 (GPIO_GPIO1_BASE + ((1 << ILI9328_RD_PIN) << 2))))
#define ILI9328_GPIO3DATA_RES         (*(pREG32 (GPIO_GPIO3_BASE + ((1 << ILI9328_RES_PIN) << 2))))
#define ILI9328_GPIO1DATA_CS_CD       (*(pREG32 (GPIO_GPIO1_BASE + ((ILI9328_CS_CD_PINS) << 2))))
#define ILI9328_GPIO1DATA_RD_WR       (*(pREG32 (GPIO_GPIO1_BASE + ((ILI9328_RD_WR_PINS) << 2))))
#define ILI9328_GPIO1DATA_WR_CS       (*(pREG32 (GPIO_GPIO1_BASE + ((ILI9328_WR_CS_PINS) << 2))))
#define ILI9328_GPIO1DATA_CD_RD_WR    (*(pREG32 (GPIO_GPIO1_BASE + ((ILI9328_CD_RD_WR_PINS) << 2))))
#define ILI9328_GPIO1DATA_CS_CD_RD_WR (*(pREG32 (GPIO_GPIO1_BASE + ((ILI9328_CS_CD_RD_WR_PINS) << 2))))

// Macros to set data bus direction to input/output
#define ILI9328_GPIO2DATA_SETINPUT  GPIO_GPIO2DIR &= ~ILI9328_DATA_MASK
#define ILI9328_GPIO2DATA_SETOUTPUT GPIO_GPIO2DIR |= ILI9328_DATA_MASK

// Macros for control line state
#define CLR_CD          ILI9328_GPIO1DATA_CD = (0)
#define SET_CD          ILI9328_GPIO1DATA_CD = (1 << ILI9328_CD_PIN)
#define CLR_CS          ILI9328_GPIO1DATA_CS = (0)
#define SET_CS          ILI9328_GPIO1DATA_CS = (1 << ILI9328_CS_PIN)
#define CLR_WR          ILI9328_GPIO1DATA_WR = (0)
#define SET_WR          ILI9328_GPIO1DATA_WR = (1 << ILI9328_WR_PIN)
#define CLR_RD          ILI9328_GPIO1DATA_RD = (0)
#define SET_RD          ILI9328_GPIO1DATA_RD = (1 << ILI9328_RD_PIN)
#define CLR_RESET       ILI9328_GPIO3DATA_RES = (0)
#define SET_RESET       ILI9328_GPIO3DATA_RES = (1 << ILI9328_RES_PIN)

// These 'combined' macros are defined to improve code performance by
// reducing the number of instructions in heavily used functions
#define CLR_CS_CD           ILI9328_GPIO1DATA_CS_CD = (0);
#define SET_RD_WR           ILI9328_GPIO1DATA_RD_WR = (ILI9328_RD_WR_PINS);
#define SET_WR_CS           ILI9328_GPIO1DATA_WR_CS = (ILI9328_WR_CS_PINS);
#define SET_CD_RD_WR        ILI9328_GPIO1DATA_CD_RD_WR = (ILI9328_CD_RD_WR_PINS);
#define CLR_CS_CD_SET_RD_WR ILI9328_GPIO1DATA_CS_CD_RD_WR = (ILI9328_RD_WR_PINS);
#define CLR_CS_SET_CD_RD_WR ILI9328_GPIO1DATA_CS_CD_RD_WR = (ILI9328_CD_RD_WR_PINS);

enum
{
  ILI9328_COMMANDS_DRIVERCODEREAD                 = 0x0000,
  ILI9328_COMMANDS_DRIVEROUTPUTCONTROL1           = 0x0001,
  ILI9328_COMMANDS_LCDDRIVINGCONTROL              = 0x0002,
  ILI9328_COMMANDS_ENTRYMODE                      = 0x0003,
  ILI9328_COMMANDS_RESIZECONTROL                  = 0x0004,
  ILI9328_COMMANDS_DISPLAYCONTROL1                = 0x0007,
  ILI9328_COMMANDS_DISPLAYCONTROL2                = 0x0008,
  ILI9328_COMMANDS_DISPLAYCONTROL3                = 0x0009,
  ILI9328_COMMANDS_DISPLAYCONTROL4                = 0x000A,
  ILI9328_COMMANDS_RGBDISPLAYINTERFACECONTROL1    = 0x000C,
  ILI9328_COMMANDS_FRAMEMAKERPOSITION             = 0x000D,
  ILI9328_COMMANDS_RGBDISPLAYINTERFACECONTROL2    = 0x000F,
  ILI9328_COMMANDS_POWERCONTROL1                  = 0x0010,
  ILI9328_COMMANDS_POWERCONTROL2                  = 0x0011,
  ILI9328_COMMANDS_POWERCONTROL3                  = 0x0012,
  ILI9328_COMMANDS_POWERCONTROL4                  = 0x0013,
  ILI9328_COMMANDS_HORIZONTALGRAMADDRESSSET       = 0x0020,
  ILI9328_COMMANDS_VERTICALGRAMADDRESSSET         = 0x0021,
  ILI9328_COMMANDS_WRITEDATATOGRAM                = 0x0022,
  ILI9328_COMMANDS_POWERCONTROL7                  = 0x0029,
  ILI9328_COMMANDS_FRAMERATEANDCOLORCONTROL       = 0x002B,
  ILI9328_COMMANDS_GAMMACONTROL1                  = 0x0030,
  ILI9328_COMMANDS_GAMMACONTROL2                  = 0x0031,
  ILI9328_COMMANDS_GAMMACONTROL3                  = 0x0032,
  ILI9328_COMMANDS_GAMMACONTROL4                  = 0x0035,
  ILI9328_COMMANDS_GAMMACONTROL5                  = 0x0036,
  ILI9328_COMMANDS_GAMMACONTROL6                  = 0x0037,
  ILI9328_COMMANDS_GAMMACONTROL7                  = 0x0038,
  ILI9328_COMMANDS_GAMMACONTROL8                  = 0x0039,
  ILI9328_COMMANDS_GAMMACONTROL9                  = 0x003C,
  ILI9328_COMMANDS_GAMMACONTROL10                 = 0x003D,
  ILI9328_COMMANDS_HORIZONTALADDRESSSTARTPOSITION = 0x0050,
  ILI9328_COMMANDS_HORIZONTALADDRESSENDPOSITION   = 0x0051,
  ILI9328_COMMANDS_VERTICALADDRESSSTARTPOSITION   = 0x0052,
  ILI9328_COMMANDS_VERTICALADDRESSENDPOSITION     = 0x0053,
  ILI9328_COMMANDS_DRIVEROUTPUTCONTROL2           = 0x0060,
  ILI9328_COMMANDS_BASEIMAGEDISPLAYCONTROL        = 0x0061,
  ILI9328_COMMANDS_VERTICALSCROLLCONTROL          = 0x006A,
  ILI9328_COMMANDS_PARTIALIMAGE1DISPLAYPOSITION   = 0x0080,
  ILI9328_COMMANDS_PARTIALIMAGE1AREASTARTLINE     = 0x0081,
  ILI9328_COMMANDS_PARTIALIMAGE1AREAENDLINE       = 0x0082,
  ILI9328_COMMANDS_PARTIALIMAGE2DISPLAYPOSITION   = 0x0083,
  ILI9328_COMMANDS_PARTIALIMAGE2AREASTARTLINE     = 0x0084,
  ILI9328_COMMANDS_PARTIALIMAGE2AREAENDLINE       = 0x0085,
  ILI9328_COMMANDS_PANELINTERFACECONTROL1         = 0x0090,
  ILI9328_COMMANDS_PANELINTERFACECONTROL2         = 0x0092,
  ILI9328_COMMANDS_PANELINTERFACECONTROL4         = 0x0095,
  ILI9328_COMMANDS_OTPVCMPROGRAMMINGCONTROL       = 0x00A1,
  ILI9328_COMMANDS_OTPVCMSTATUSANDENABLE          = 0x00A2,
  ILI9328_COMMANDS_OTPPROGRAMMINGIDKEY            = 0x00A5
};

#endif
