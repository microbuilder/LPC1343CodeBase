/**************************************************************************/
/*! 
    @file     pn532_mifare.c
*/
/**************************************************************************/
#include <string.h>

#include "pn532.h"
#include "pn532_mifare.h"

bool pn532MifareCmd(const pn532_mifare_cmd_t mc, const uint8_t ui8Block, pn532_mifare_param_t * pmp)
{
  byte_t  abtRx[265];
  size_t  szRx = sizeof(abtRx);
  size_t  szParamLen;
  byte_t  abtCmd[265];
  bool    bEasyFraming;

  abtCmd[0] = mc;               // The MIFARE Classic command
  abtCmd[1] = ui8Block;         // The block address (1K=0x00..0x39, 4K=0x00..0xff)

  switch (mc) {
    // Read and store command have no parameter
  case PN532_MIFARE_CMD_READ:
  case PN532_MIFARE_CMD_STORE:
    szParamLen = 0;
    break;

    // Authenticate command
  case PN532_MIFARE_CMD_AUTH_A:
  case PN532_MIFARE_CMD_AUTH_B:
    szParamLen = sizeof (pn532_mifare_param_auth_t);
    break;

    // Data command
  case PN532_MIFARE_CMD_WRITE:
    szParamLen = sizeof (pn532_mifare_param_data_t);
    break;

    // Value command
  case PN532_MIFARE_CMD_DECREMENT:
  case PN532_MIFARE_CMD_INCREMENT:
  case PN532_MIFARE_CMD_TRANSFER:
    szParamLen = sizeof (pn532_mifare_param_value_t);
    break;

    // Please fix your code, you never should reach this statement
  default:
    return false;
    break;
  }

  // When available, copy the parameter bytes
  if (szParamLen)
    memcpy (abtCmd + 2, (byte_t *) pmp, szParamLen);

//  bEasyFraming = pnd->bEasyFraming;
//  if (!nfc_configure (pnd, NDO_EASY_FRAMING, true)) {
//    nfc_perror (pnd, "nfc_configure");
//    return false;
//  }
//
//  // Fire the mifare command
//  if (!nfc_initiator_transceive_bytes (pnd, abtCmd, 2 + szParamLen, abtRx, &szRx)) {
//    if (pnd->iLastError == EINVRXFRAM) {
//      // "Invalid received frame" AKA EINVRXFRAM,  usual means we are
//      // authenticated on a sector but the requested MIFARE cmd (read, write)
//      // is not permitted by current acces bytes;
//      // So there is nothing to do here.
//    } else {
//      nfc_perror (pnd, "nfc_initiator_transceive_bytes");
//    }
//    nfc_configure (pnd, NDO_EASY_FRAMING, bEasyFraming);
//    return false;
//  }
//  if (!nfc_configure (pnd, NDO_EASY_FRAMING, bEasyFraming)) {
//    nfc_perror (pnd, "nfc_configure");
//    return false;
//  }
//
//  // When we have executed a read command, copy the received bytes into the param
//  if (mc == MC_READ) {
//    if (szRx == 16) {
//      memcpy (pmp->mpd.abtData, abtRx, 16);
//    } else {
//      return false;
//    }
//  }

  // Command succesfully executed
  return true;
}
