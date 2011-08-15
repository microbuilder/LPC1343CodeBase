/**************************************************************************/
/*! 
    @file     pn532_mifare_classic.h
*/
/**************************************************************************/

#ifndef __PN532_MIFARE_CLASSIC_H__
#define __PN532_MIFARE_CLASSIC_H__

#include "projectconfig.h"
#include "pn532_mifare.h"

pn532_error_t pn532_mifareclassic_WaitForPassiveTarget (byte_t * pbtCUID, size_t * szCUIDLen);
pn532_error_t pn532_mifareclassic_AuthenticateBlock (byte_t * pbtCUID, size_t szCUIDLen, uint32_t uiBlockNumber, uint8_t uiKeyType, byte_t * pbtKeys);
pn532_error_t pn532_mifareclassic_ReadDataBlock (uint8_t uiBlockNumber, byte_t * pbtData);

#endif
