/**************************************************************************/
/*! 
    @file     pn532.h
*/
/**************************************************************************/

#ifndef __PN532_H__
#define __PN532_H__

#include "projectconfig.h"

#define PN532_DEBUG(fmt, args...)             printf(fmt, ##args) 

typedef enum pn532_state_e
{
  PN532_STATE_SLEEP,
  PN532_STATE_READY,
  PN532_STATE_BUSY
}
pn532_state_t;

/* Error messages generate by the stack (not to be confused with app level errors from the PN532) */
typedef enum pn532_error_e
{
  PN532_ERROR_NONE                    = 0x00,
  PN532_ERROR_UNABLETOINIT            = 0x01,   // Unable to initialise or wakeup the device
  PN532_ERROR_APPLEVELERROR           = 0x02,   // Application level error detected
  PN532_ERROR_BUSY                    = 0x03,   // Busy executing a previous command
  PN532_ERROR_NOACK                   = 0x04,   // No ack message received
  PN532_ERROR_INVALIDACK              = 0x05,   // Ack != 00 00 FF 00 FF 00
  PN532_ERROR_PREAMBLEMISMATCH        = 0x06,   // Frame preamble + start code mismatch
  PN532_ERROR_EXTENDEDFRAME           = 0x07,   // Extended frames currently unsupported
  PN532_ERROR_LENCHECKSUMMISMATCH     = 0x08,
  PN532_ERROR_RESPONSEBUFFEREMPTY     = 0x09,   // No response data received
  PN532_ERROR_SPIREADYSTATUSTIMEOUT   = 0x0A    // Timeout waiting for 'ready' status (SPI only)

} pn532_error_t;

typedef enum pn532_modulation_e
{
  PN532_MODULATION_ISO14443A_106KBPS  = 0x00,
  PN532_MODULATION_FELICA_212KBPS     = 0x01,
  PN532_MODULATION_FELICA_424KBPS     = 0x02,
  PN532_MODULATION_ISO14443B_106KBPS  = 0x03,
  PN532_MODULATION_JEWEL_106KBPS      = 0x04
} pn532_modulation_t;

/* PN532 Protocol control block */
typedef struct
{
  BOOL                initialised;
  pn532_state_t       state;
  pn532_modulation_t  modulation;
  uint32_t            lastCommand;
  uint32_t            appError;
} pn532_pcb_t;

void          pn532Init();
pn532_pcb_t * pn532GetPCB();
pn532_error_t pn532SetModulation(pn532_modulation_t mod);
pn532_error_t pn532Write(byte_t *abtCommand, size_t szLen);
pn532_error_t pn532Read(byte_t *abtResponse, size_t * pszLen);
void          pn532PrintHex(const byte_t * pbtData, const size_t szBytes);

#endif
