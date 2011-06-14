/**************************************************************************/
/*! 
    @file     isl12022m.h
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

#ifndef _ISL12022M_H_
#define _ISL12022M_H_

#include "projectconfig.h"
#include "core/i2c/i2c.h"

#define ISL12022M_RTC_ADDRESS       (0xDE)    // 1101111 shifted left 1 bit = 0xDE
#define ISL12022M_SRAM_ADDRESS      (0xAE)    // 1010111 shifted left 1 bit = 0xAE
#define ISL12022M_READBIT           (0x01)

#define ISL12022M_HR_MILITARY       (1<<7)    // 1 = 24-Hour, 0 = 12-Hour
#define ISL12022M_HR_AMPM           (1<<5)    // If 24-hour time enabled, 0 = AM, 1 = PM

#define ISL12022M_STATUS_BUSY       (1<<7)
#define ISL12022M_STATUS_OSCFAIL    (1<<6)
#define ISL12022M_STATUS_DSTADJUST  (1<<5)
#define ISL12022M_STATUS_ALARM      (1<<4)
#define ISL12022M_STATUS_LOWVDD     (1<<3)
#define ISL12022M_STATUS_LOWBATT85  (1<<2)
#define ISL12022M_STATUS_LOWBATT75  (1<<1)
#define ISL12022M_STATUS_RTCFAIL    (1<<0)

#define ISL12022M_INT_AUTORESET     (1<<7)
#define ISL12022M_INT_WRITEENABLE   (1<<6)
#define ISL12022M_INT_INTALARM      (1<<5)
#define ISL12022M_INT_FOBATB        (1<<4)

#define ISL12022M_BETA_TEMPENABLE   (1<<7)   // Temperature Sensor Enabled Bit

enum
{
  // RTC Registers
  ISL12022M_REG_RTC_SC               = 0x00,
  ISL12022M_REG_RTC_MN               = 0x01,
  ISL12022M_REG_RTC_HR               = 0x02,
  ISL12022M_REG_RTC_DT               = 0x03,
  ISL12022M_REG_RTC_MO               = 0x04,
  ISL12022M_REG_RTC_YR               = 0x05,
  ISL12022M_REG_RTC_DW               = 0x06,
  // Control and Status Registers
  ISL12022M_REG_CSR_SR               = 0x07,
  ISL12022M_REG_CSR_INT              = 0x08,
  ISL12022M_REG_CSR_PWR_VDD          = 0x09,
  ISL12022M_REG_CSR_PWR_VBAT         = 0x0A,
  ISL12022M_REG_CSR_ITRO             = 0x0B,
  ISL12022M_REG_CSR_ALPHA            = 0x0C,
  ISL12022M_REG_CSR_BETA             = 0x0D,
  ISL12022M_REG_CSR_FATR             = 0x0E,
  ISL12022M_REG_CSR_FDTR             = 0x0F,
  // Alarm Registers
  ISL12022M_REG_ALARM_SCA0           = 0x10,
  ISL12022M_REG_ALARM_MNA0           = 0x11,
  ISL12022M_REG_ALARM_HRA0           = 0x12,
  ISL12022M_REG_ALARM_DTA0           = 0x13,
  ISL12022M_REG_ALARM_MOA0           = 0x14,
  ISL12022M_REG_ALARM_DWA0           = 0x15,
  //
  ISL12022M_REG_TSV2B_VSC            = 0x16,
  ISL12022M_REG_TSV2B_VMN            = 0x17,
  ISL12022M_REG_TSV2B_VHR            = 0x18,
  ISL12022M_REG_TSV2B_VDT            = 0x19,
  ISL12022M_REG_TSV2B_VMO            = 0x1A,
  ISL12022M_REG_TSV2B_TSB2V          = 0x1B,
  ISL12022M_REG_TSV2B_BMN            = 0x1C,
  ISL12022M_REG_TSV2B_BHR            = 0x1D,
  ISL12022M_REG_TSV2B_BDT            = 0x1E,
  ISL12022M_REG_TSV2B_BMO            = 0x1F,
  // DST Control Registers
  ISL12022M_REG_DSTCR_DSTMOFD        = 0x20,
  ISL12022M_REG_DSTCR_DSTDWFD        = 0x21,
  ISL12022M_REG_DSTCR_DSTDTFD        = 0x22,
  ISL12022M_REG_DSTCR_DSTHRFD        = 0x23,
  ISL12022M_REG_DSTCR_DSTMORV        = 0x24,
  ISL12022M_REG_DSTCR_DSTDWRV        = 0x25,
  ISL12022M_REG_DSTCR_DSTDTRV        = 0x26,
  ISL12022M_REG_DSTCR_DSTHRRV        = 0x27,
  // Temperature Registers
  ISL12022M_REG_TEMP_TKOL            = 0x28,
  ISL12022M_REG_TEMP_TKOM            = 0x29,
  // NPPM Registers
  ISL12022M_REG_NPPM_NPPML           = 0x2A,
  ISL12022M_REG_NPPM_NPPMH           = 0x2B,
  // XTO Register
  ISL12022M_REG_XT0_XT0              = 0x2C,
  // Alpha Hot Register
  ISL12022M_REG_ALPHAH_ALPHAH        = 0x2D,
  // General Purpose Memory (SRAM) Registers
  ISL12022M_REG_GPM_GPM1             = 0x2E,
  ISL12022M_REG_GPM_GPM2             = 0x2F
};

typedef enum
{
  ISL12022M_ERROR_OK = 0,               // Everything executed normally
  ISL12022M_ERROR_I2C_INIT,             // Unable to initialise I2C
  ISL12022M_ERROR_I2C_BUSY,             // I2C already in use
  ISL12022M_ERROR_I2C_BUFFEROVERFLOW,   // I2C Buffer is too small
  ISL12022M_ERROR_RTC_LOWBATT,          // RTC battery low ... value may not be accurate
  ISL12022M_ERROR_LAST
}
isl12022mError_t;

// ISL12022M time + status placeholder
typedef struct
{
  uint8_t interrupt;
  uint8_t status;
  uint8_t dayofweek;
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} 
isl12022mTime_t;

isl12022mError_t isl12022mInit(void);
isl12022mError_t isl12022mGetTime(isl12022mTime_t *time);
isl12022mError_t isl12022mSetTime(uint8_t dayofweek, uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
isl12022mError_t isl12022mGetTemp(uint8_t *celsius);

#endif
