/**************************************************************************/
/*! 
    @file     pn532_mifare.h
*/
/**************************************************************************/

#ifndef __PN532_MIFARE_H__
#define __PN532_MIFARE_H__

#include "projectconfig.h"

// These may need to be enlarged for multi card support
#define PN532_RESPONSELEN_INLISTPASSIVETARGET (32)
#define PN532_RESPONSELEN_INDATAEXCHANGE      (32)  

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

#endif
