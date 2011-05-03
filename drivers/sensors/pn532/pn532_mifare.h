/**************************************************************************/
/*! 
    @file     pn532_mifare.h
*/
/**************************************************************************/

#ifndef __PN532_MIFARE_H__
#define __PN532_MIFARE_H__

#include "projectconfig.h"

typedef enum pn532_mifare_cmd_e
{
  PN532_MIFARE_CMD_AUTH_A     = 0x60,
  PN532_MIFARE_CMD_AUTH_B     = 0x61,
  PN532_MIFARE_CMD_READ       = 0x30,
  PN532_MIFARE_CMD_WRITE      = 0xA0,
  PN532_MIFARE_CMD_TRANSFER   = 0xB0,
  PN532_MIFARE_CMD_DECREMENT  = 0xC0,
  PN532_MIFARE_CMD_INCREMENT  = 0xC1,
  PN532_MIFARE_CMD_STORE      = 0xC2
} 
pn532_mifare_cmd_t;

typedef struct 
{
  byte_t  abtKey[6];
  byte_t  abtUid[4];
} 
pn532_mifare_param_auth_t;

typedef struct 
{
  byte_t  abtData[16];
} 
pn532_mifare_param_data_t;

typedef struct 
{
  byte_t  abtValue[4];
} 
pn532_mifare_param_value_t;

typedef union 
{
  pn532_mifare_param_auth_t   mpa;
  pn532_mifare_param_data_t   mpd;
  pn532_mifare_param_value_t  mpv;
} 
pn532_mifare_param_t;

// MIFARE Classic
typedef struct 
{
  byte_t  abtUID[4];
  byte_t  btBCC;
  byte_t  btUnknown;
  byte_t  abtATQA[2];
  byte_t  abtUnknown[8];
} 
pn532_mifare_classic_block_manufacturer_t;

typedef struct 
{
  byte_t  abtData[16];
} 
pn532_mifare_classic_block_data_t;

typedef struct 
{
  byte_t  abtKeyA[6];
  byte_t  abtAccessBits[4];
  byte_t  abtKeyB[6];
} 
pn532_mifare_classic_block_trailer_t;

typedef union 
{
  pn532_mifare_classic_block_manufacturer_t mbm;
  pn532_mifare_classic_block_data_t         mbd;
  pn532_mifare_classic_block_trailer_t      mbt;
} 
pn532_mifare_classic_block_t;

typedef struct 
{
  pn532_mifare_classic_block_t amb[256];
} 
pn532_mifare_classic_tag_t;

// MIFARE Ultralight
typedef struct 
{
  byte_t  sn0[3];
  byte_t  btBCC0;
  byte_t  sn1[4];
  byte_t  btBCC1;
  byte_t  internal;
  byte_t  lock[2];
  byte_t  otp[4];
} 
pn532_mifareul_block_manufacturer_t;

typedef struct 
{
  byte_t  abtData[16];
} 
pn532_mifareul_block_data_t;

typedef union 
{
  pn532_mifareul_block_manufacturer_t mbm;
  pn532_mifareul_block_data_t         mbd;
} 
pn532_mifareul_block_t;

typedef struct 
{
  pn532_mifareul_block_t amb[4];
} 
pn532_mifareul_tag_t;

#endif
