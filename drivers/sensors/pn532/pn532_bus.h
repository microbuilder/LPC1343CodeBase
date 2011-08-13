/**************************************************************************/
/*! 
    @file     pn532_bus.h
*/
/**************************************************************************/

#ifndef __PN532_BUS_H__
#define __PN532_BUS_H__

#include "projectconfig.h"
#include "pn532.h"

#define PN532_BUS_UART

#define PN532_RSTPD_PORT                      (2)
#define PN532_RSTPD_PIN                       (2)
#define PN532_SPI_CSPORT                      (0)
#define PN532_SPI_CSPIN                       (2)

#define PN532_NORMAL_FRAME__DATA_MAX_LEN      (254)
#define PN532_NORMAL_FRAME__OVERHEAD          (8)
#define PN532_EXTENDED_FRAME__DATA_MAX_LEN    (264)
#define PN532_EXTENDED_FRAME__OVERHEAD        (11)
#define PN532_BUFFER_LEN                      (PN532_EXTENDED_FRAME__DATA_MAX_LEN + PN532_EXTENDED_FRAME__OVERHEAD)
#define PN532_UART_BAUDRATE                   (115200)

// Generic interface for the different serial buses available on the PN532
void          pn532_bus_HWInit(void);
pn532_error_t pn532_bus_SendCommand(const byte_t * pbtData, const size_t szData);
pn532_error_t pn532_bus_ReadResponse(byte_t * pbtResponse, size_t * pszRxLen);
bool          pn532_bus_TransceiveBytes(const byte_t * pbtTx, const size_t szTx, byte_t * pbtRx, size_t * pszRx);
pn532_error_t pn532_bus_Wakeup(void);

#endif
