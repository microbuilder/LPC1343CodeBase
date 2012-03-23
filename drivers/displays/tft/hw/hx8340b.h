/**************************************************************************/
/*! 
    @file     hx8340b.h
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
#ifndef __HX8340B_H__
#define __HX8340B_H__

#include "projectconfig.h"
#include "drivers/displays/tft/lcd.h"

#define HX8340B_ICVERSION_N
// #define HX8340B_ICVERSION_T

#if defined HX8340B_ICVERSION_N && defined HX8340B_ICVERSION_T
  #error "Only one IC version can be set for HX8340B (HX8340B_ICVERSION_N or HX8340B_ICVERSION_T)"
#endif
#if !defined HX8340B_ICVERSION_N && !defined HX8340B_ICVERSION_T
  #error "HX8340B_ICVERSION_N or HX8340B_ICVERSION_T must be defined for HW8340 LCDs"
#endif

/**************************************************************************
    BTL221722-276L CONNECTOR (HX8340B_ICVERSION_N)
    -----------------------------------------------------------------------
    Pin   Function        Notes
    ===   ==============  ===============================
      1   VSS
      2   NC
      3   LED A/+         Vf 3.3V
      4   VDD             2.8-3.3V
      5   CS
      6   SDI             Serial Data
      7   SCL             Serial Clock
      8   RS              Not used
      9   RESET
     10   VDD             2.8-3.3V
     11   LED K1
     12   LED K2
     13   LED K3
     14   VSS

 **************************************************************************/

// Control pins
#define HX8340B_GPIODATAREG     (*(pREG32 (0x50023FFC)))   // GPIO2DATA
#define HX8340B_PORT            (2)
#define HX8340B_SDI_PIN         (2)
#define HX8340B_SCL_PIN         (3)
#define HX8340B_CS_PIN          (4)
#define HX8340B_RES_PIN         (5)
#define HX8340B_BL_PIN          (6)

// Macros for control line state
#define CLR_SDI     do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_SDI_PIN); } while(0)
#define SET_SDI     do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_SDI_PIN); HX8340B_GPIODATAREG |= (1<<HX8340B_SDI_PIN); } while(0)
#define CLR_SCL     do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_SCL_PIN); } while(0)
#define SET_SCL     do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_SCL_PIN); HX8340B_GPIODATAREG |= (1<<HX8340B_SCL_PIN); } while(0)
#define CLR_CS      do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_CS_PIN); } while(0)
#define SET_CS      do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_CS_PIN); HX8340B_GPIODATAREG |= (1<<HX8340B_CS_PIN); } while(0)
#define CLR_RES     do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_RES_PIN); } while(0)
#define SET_RES     do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_RES_PIN); HX8340B_GPIODATAREG |= (1<<HX8340B_RES_PIN); } while(0)
#define CLR_BL      do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_BL_PIN); } while(0)
#define SET_BL      do { HX8340B_GPIODATAREG &= ~(1<<HX8340B_BL_PIN); HX8340B_GPIODATAREG |= (1<<HX8340B_BL_PIN); } while(0)

// HX8340-B(N) Commands (used by BTL221722-276L)
#define HX8340B_N_NOP                     (0x00)
#define HX8340B_N_SWRESET                 (0x01)
#define HX8340B_N_RDDIDIF                 (0x04)
#define HX8340B_N_RDDST                   (0x09)
#define HX8340B_N_RDDPM                   (0x0A)
#define HX8340B_N_RDDMADCTL               (0x0B)
#define HX8340B_N_RDDCOLMOD               (0x0C)
#define HX8340B_N_RDDIM                   (0x0D)
#define HX8340B_N_RDDSM                   (0x0E)
#define HX8340B_N_RDDSDR                  (0x0F)
#define HX8340B_N_SLPIN                   (0x10)
#define HX8340B_N_SPLOUT                  (0x11)
#define HX8340B_N_PTLON                   (0x12)
#define HX8340B_N_NORON                   (0x13)
#define HX8340B_N_INVOFF                  (0x20)
#define HX8340B_N_INVON                   (0x21)
#define HX8340B_N_GAMSET                  (0x26)
#define HX8340B_N_DISPOFF                 (0x28)
#define HX8340B_N_DISPON                  (0x29)
#define HX8340B_N_CASET                   (0x2A)
#define HX8340B_N_PASET                   (0x2B)
#define HX8340B_N_RAMWR                   (0x2C)
#define HX8340B_N_RAMRD                   (0x2E)
#define HX8340B_N_RGBSET                  (0x2D)
#define HX8340B_N_PLTAR                   (0x30)
#define HX8340B_N_VSCRDEF                 (0x33)
#define HX8340B_N_TEOFF                   (0x34)
#define HX8340B_N_TEON                    (0x35)
#define HX8340B_N_MADCTL                  (0x36)
#define HX8340B_N_VSCRSADD                (0x37)
#define HX8340B_N_IDMOFF                  (0x38)
#define HX8340B_N_IDMON                   (0x39)
#define HX8340B_N_COLMOD                  (0x3A)
#define HX8340B_N_RDID1                   (0xDA)
#define HX8340B_N_RDID2                   (0xDB)
#define HX8340B_N_RDID3                   (0xDC)
#define HX8340B_N_SETOSC                  (0xB0)
#define HX8340B_N_SETPWCTR1               (0xB1)
#define HX8340B_N_SETPWCTR2               (0xB2)
#define HX8340B_N_SETPWCTR3               (0xB3)
#define HX8340B_N_SETPWCTR4               (0xB4)
#define HX8340B_N_SETPWCTR5               (0xB5)
#define HX8340B_N_SETDISCTRL              (0xB6)
#define HX8340B_N_SETFRMCTRL              (0xB7)
#define HX8340B_N_SETDISCYCCTRL           (0xB8)
#define HX8340B_N_SETINVCTRL              (0xB9)
#define HX8340B_N_RGBBPCTR                (0xBA)
#define HX8340B_N_SETRGBIF                (0xBB)
#define HX8340B_N_SETDODC                 (0xBC)
#define HX8340B_N_SETINTMODE              (0xBD)
#define HX8340B_N_SETPANEL                (0xBE)
#define HX8340B_N_SETOTP                  (0xC7)
#define HX8340B_N_SETONOFF                (0xC0)
#define HX8340B_N_SETEXTCMD               (0xC1)
#define HX8340B_N_SETGAMMAP               (0xC2)
#define HX8340B_N_SETGAMMAN               (0xC3)

// HX8340-B(T) Commands
#define HX8340B_T_DISPLAYMODECONTROL      (0x01)
#define HX8340B_T_COLADDR_START2          (0x02)
#define HX8340B_T_COLADDR_START1          (0x03)
#define HX8340B_T_COLADDR_END2            (0x04)
#define HX8340B_T_COLADDR_END1            (0x05)
#define HX8340B_T_ROWADDR_START2          (0x06)
#define HX8340B_T_ROWADDR_START1          (0x07)
#define HX8340B_T_ROWADDR_END2            (0x08)
#define HX8340B_T_ROWADDR_END1            (0x09)
#define HX8340B_T_MEMACCESSCONTROL        (0x16)
#define HX8340B_T_COLMOD                  (0x17)
#define HX8340B_T_OSCCONTROLA             (0x18)
#define HX8340B_T_POWERCONTROL1           (0x19)
#define HX8340B_T_POWERCONTROL3           (0x1A)
#define HX8340B_T_POWERCONTROL4           (0x1B)
#define HX8340B_T_POWERCONTROL5           (0x1C)
#define HX8340B_T_POWERCONTROL6           (0x1D)
#define HX8340B_T_POWERCONTROL7           (0x1E)
#define HX8340B_T_POWERCONTROL8           (0x1F)
#define HX8340B_T_POWERCONTROL9           (0x20)
#define HX8340B_T_OSCCONTROLB             (0x21)
#define HX8340B_T_SRAMWRITECONTROL        (0x22)
#define HX8340B_T_VCOMCONTROL1            (0x23)
#define HX8340B_T_VCOMCONTROL2            (0x24)
#define HX8340B_T_VCOMCONTROL3            (0x25)
#define HX8340B_T_DISPLAYCONTROL1         (0x26)
#define HX8340B_T_DISPLAYCONTROL2         (0x27)
#define HX8340B_T_FRAMERATECONTROL1       (0x28)
#define HX8340B_T_FRAMERATECONTROL2       (0x29)
#define HX8340B_T_FRAMERATECONTROL3       (0x2A)
#define HX8340B_T_FRAMERATECONTROL4       (0x2B)
#define HX8340B_T_DISPLAYINVERSION        (0x2F)
#define HX8340B_T_RGBINTERFACECONTROL1    (0x31)
#define HX8340B_T_RGBINTERFACECONTROL2    (0x32)
#define HX8340B_T_RGBINTERFACECONTROL3    (0x33)
#define HX8340B_T_RGBINTERFACECONTROL4    (0x34)
#define HX8340B_T_OUTPUTSDIRECTION        (0x35)
#define HX8340B_T_INTERFACEMODE           (0x36)
#define HX8340B_T_NWNBPANELSET            (0x37)
#define HX8340B_T_OTPCONTROL1             (0x38)
#define HX8340B_T_OTPCONTROL2             (0x39)
#define HX8340B_T_OTPCONTROL3             (0x3A)
#define HX8340B_T_POWERCONTROL_INT1       (0x60)
#define HX8340B_T_POWERCONTROL_INT2       (0x61)
#define HX8340B_T_SOURCECONTROL_INT1      (0x62)
#define HX8340B_T_SOURCECONTROL_INT2      (0x63)
#define HX8340B_T_SOURCEOPCONTROL         (0x73)
#define HX8340B_T_HIMAXID                 (0x93)

#endif
