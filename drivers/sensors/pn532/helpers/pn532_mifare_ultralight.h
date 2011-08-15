/**************************************************************************/
/*! 
    @file     pn532_mifare_ultralight.h
*/
/**************************************************************************/

#ifndef __PN532_MIFARE_ULTRALIGHT_H__
#define __PN532_MIFARE_ULTRALIGHT_H__

#include "projectconfig.h"
#include "pn532_mifare.h"

pn532_error_t pn532_mifareultralight_WaitForPassiveTarget (byte_t * pbtCUID, size_t * szCUIDLen);
pn532_error_t pn532_mifareultralight_ReadPage (uint8_t page, byte_t * pbtBuffer);

#endif
