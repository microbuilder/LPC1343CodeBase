/**************************************************************************/
/*! 
    @file     pn532_bus.h
*/
/**************************************************************************/

#ifndef __PN532_BUS_H__
#define __PN532_BUS_H__

#include "projectconfig.h"
#include "pn532.h"

// #define PN532_BUS_UART
#define PN532_BUS_I2C

#define PN532_RSTPD_PORT                      (3)
#define PN532_RSTPD_PIN                       (1)
#define PN532_I2C_IRQPORT                     (3)
#define PN532_I2C_IRQPIN                      (2)

#define PN532_NORMAL_FRAME__DATA_MAX_LEN      (254)
#define PN532_NORMAL_FRAME__OVERHEAD          (8)
#define PN532_EXTENDED_FRAME__DATA_MAX_LEN    (264)
#define PN532_EXTENDED_FRAME__OVERHEAD        (11)
#define PN532_BUFFER_LEN                      (PN532_EXTENDED_FRAME__DATA_MAX_LEN + PN532_EXTENDED_FRAME__OVERHEAD)

#define PN532_UART_BAUDRATE                   (115200)

#define PN532_I2C_ADDRESS                     (0x48)
#define PN532_I2C_READBIT                     (0x01)
#define PN532_I2C_READYTIMEOUT                (20)    // Max number of attempts to read Ready bit (see UM 5-Nov-2007 Section 6.2.4)

// Generic interface for the different serial buses available on the PN532
void          pn532_bus_HWInit(void);
pn532_error_t pn532_bus_SendCommand(const byte_t * pbtData, const size_t szData);
pn532_error_t pn532_bus_ReadResponse(byte_t * pbtResponse, size_t * pszRxLen);
pn532_error_t pn532_bus_Wakeup(void);

#endif
