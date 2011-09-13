/**************************************************************************/
/*! 
    @file     w25q16bv.h
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

#ifndef _W25Q16BV_H_
#define _W25Q16BV_H_

#include "projectconfig.h"

#define W25Q16BV_MAXADDRESS      0x1FFFFF
#define W25Q16BV_PAGESIZE        256    // 256 bytes per programmable page
#define W25Q16BV_PAGES           8192   // 2,097,152 Bytes / 256 bytes per page
#define W25Q16BV_SECTORSIZE      4096   // 1 erase sector = 4096 bytes
#define W25Q16BV_SECTORS         512    // 2,097,152 Bytes / 4096 bytes per sector
#define W25Q16BV_MANUFACTURERID  0xEF   // Used to validate read data
#define W25Q16BV_DEVICEID        0x14   // Used to validate read data

#define W25Q16BV_STAT1_BUSY      0x01   // Erase/Write in Progress
#define W25Q16BV_STAT1_WRTEN     0x02   // Write Enable Latch
#define W25Q16BV_STAT2_QUADENBL  0x02   // Quad Enable
#define W25Q16BV_STAT2_SSPNDSTAT 0x80   // Suspend Status

/**************************************************************************/
/*! 
    W25Q16BV Commands
*/
/**************************************************************************/
typedef enum
{
  // Erase/Program Instructions
  W25Q16BV_CMD_WRITEENABLE    = 0x06,   // Write Enabled
  W25Q16BV_CMD_WRITEDISABLE   = 0x04,   // Write Disabled
  W25Q16BV_CMD_READSTAT1      = 0x05,   // Read Status Register 1
  W25Q16BV_CMD_READSTAT2      = 0x35,   // Read Status Register 2
  W25Q16BV_CMD_WRITESTAT      = 0x01,   // Write Status Register
  W25Q16BV_CMD_PAGEPROG       = 0x02,   // Page Program
  W25Q16BV_CMD_QUADPAGEPROG   = 0x32,   // Quad Page Program
  W25Q16BV_CMD_SECTERASE4     = 0x20,   // Sector Erase (4KB)
  W25Q16BV_CMD_BLOCKERASE32   = 0x52,   // Block Erase (32KB)
  W25Q16BV_CMD_BLOCKERASE64   = 0xD8,   // Block Erase (64KB)
  W25Q16BV_CMD_CHIPERASE      = 0x60,   // Chip Erase
  W25Q16BV_CMD_ERASESUSPEND   = 0x75,   // Erase Suspend
  W25Q16BV_CMD_ERASERESUME    = 0x7A,   // Erase Resume
  W25Q16BV_CMD_POWERDOWN      = 0xB9,   // Power Down
  W25Q16BV_CMD_CRMR           = 0xFF,   // Continuous Read Mode Reset
  // Read Instructions
  W25Q16BV_CMD_READDATA       = 0x03,   // Read Data
  W25Q16BV_CMD_FREAD          = 0x0B,   // Fast Read
  W25Q16BV_CMD_FREADDUALOUT   = 0x3B,   // Fast Read Dual Output
  W25Q16BV_CMD_FREADDUALIO    = 0xBB,   // Fast Read Dual I/O
  W25Q16BV_CMD_FREADQUADOUT   = 0x6B,   // Fast Read Quad Output
  W25Q16BV_CMD_FREADQUADIO    = 0xEB,   // Fast Read Quad I/O
  W25Q16BV_CMD_WREADQUADIO    = 0xE7,   // Word Read Quad I/O
  W25Q16BV_CMD_OWREADQUADIO   = 0xE3,   // Octal Word Read Quad I/O
  // ID/Security Instructions
  W25Q16BV_CMD_RPWRDDEVID     = 0xAB,   // Release Power Down/Device ID
  W25Q16BV_CMD_MANUFDEVID     = 0x90,   // Manufacturer/Device ID
  W25Q16BV_CMD_MANUFDEVID2    = 0x92,   // Manufacturer/Device ID by Dual I/O
  W25Q16BV_CMD_MANUFDEVID4    = 0x94,   // Manufacturer/Device ID by Quad I/O
  W25Q16BV_CMD_JEDECID        = 0x9F,   // JEDEC ID
  W25Q16BV_CMD_READUNIQUEID   = 0x4B    // Read Unique ID
} w25q16bv_Commands_e;

#endif
