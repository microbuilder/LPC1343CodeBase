/**************************************************************************/
/*! 
    @file   pn532_bus_uart.c
*/
/**************************************************************************/
#include <string.h>

#include "pn532.h"
#include "pn532_bus.h"

#ifdef PN532_BUS_UART

#include "core/systick/systick.h"
#include "core/gpio/gpio.h"
#include "core/uart/uart.h"

/**************************************************************************/
/*! 
    @brief  Builds a standard PN532 frame using the supplied data

    @param  pbtFrame  Pointer to the field that will hold the frame data
    @param  pszFrame  Pointer to the field that will hold the frame length
    @param  pbtData   Pointer to the data to insert in a frame
    @param  swData    Length of the data to insert in bytes

    @note   Possible error messages are:

            - PN532_ERROR_EXTENDEDFRAME
*/
/**************************************************************************/
pn532_error_t pn532_bus_BuildFrame(byte_t * pbtFrame, size_t * pszFrame, const byte_t * pbtData, const size_t szData)
{
  if (szData > PN532_NORMAL_FRAME__DATA_MAX_LEN) 
  {
    // Extended frames currently unsupported
    return PN532_ERROR_EXTENDEDFRAME;
  }

  // LEN - Packet length = data length (len) + checksum (1) + end of stream marker (1)
  pbtFrame[3] = szData + 1;
  // LCS - Packet length checksum
  pbtFrame[4] = 256 - (szData + 1);
  // TFI
  pbtFrame[5] = 0xD4;
  // DATA - Copy the PN53X command into the packet buffer
  memcpy (pbtFrame + 6, pbtData, szData);

  // DCS - Calculate data payload checksum
  byte_t btDCS = (256 - 0xD4);
  size_t szPos;
  for (szPos = 0; szPos < szData; szPos++) 
  {
    btDCS -= pbtData[szPos];
  }
  pbtFrame[6 + szData] = btDCS;

  // 0x00 - End of stream marker
  pbtFrame[szData + 7] = 0x00;

  (*pszFrame) = szData + PN532_NORMAL_FRAME__OVERHEAD;

  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    @brief  Initialises UART and configures the PN532
*/
/**************************************************************************/
void pn532_bus_HWInit(void)
{
  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Initialising UART (%d)%s", PN532_UART_BAUDRATE, CFG_PRINTF_NEWLINE);
  #endif
  uartInit(PN532_UART_BAUDRATE);

  // Set reset pin as output and reset device
  gpioSetDir(PN532_RSTPD_PORT, PN532_RSTPD_PIN, gpioDirection_Output);
  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Resetting the PN532...\r\n");
  #endif
  gpioSetValue(PN532_RSTPD_PORT, PN532_RSTPD_PIN, 0);
  systickDelay(400);
  gpioSetValue(PN532_RSTPD_PORT, PN532_RSTPD_PIN, 1);

  // Wait for the PN532 to finish booting
  systickDelay(100);
}

/**************************************************************************/
/*! 
    @brief  Sends the specified command to the PN532, automatically
            creating an appropriate frame for it

    @param  pdbData   Pointer to the byte data to send
    @param  szData    Length in bytes of the data to send

    @note   Possible error messages are:

            - PN532_ERROR_BUSY
            - PN532_ERROR_NOACK
            - PN532_ERROR_INVALIDACK
*/
/**************************************************************************/
pn532_error_t pn532_bus_SendCommand(const byte_t * pbtData, const size_t szData)
{
  pn532_pcb_t *pn532 = pn532GetPCB();
    
  // Check if we're busy
  if (pn532->state == PN532_STATE_BUSY)
  {
    return PN532_ERROR_BUSY;
  }

  // Flag the stack as busy
  pn532->state = PN532_STATE_BUSY;

  // Every packet must start with "00 00 ff"
  byte_t  abtFrame[PN532_BUFFER_LEN] = { 0x00, 0x00, 0xff };
  size_t szFrame = 0;

  // Build the frame
  pn532_bus_BuildFrame (abtFrame, &szFrame, pbtData, szData);

  // Keep track of the last command that was sent
  pn532->lastCommand = pbtData[0];

  // Output the frame data for debugging if requested
  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Sending  (%02d): ", szFrame);
  pn532PrintHex(abtFrame, szFrame);
  #endif

  // Send data to the PN532
  uartSend (abtFrame, szFrame);

  // Wait for ACK
  byte_t abtRxBuf[6];
  uart_pcb_t *uart = uartGetPCB();
  systickDelay(10);   // FIXME: How long should we wait for ACK?
  if (uart->rxfifo.len < 6) 
  {
    // Unable to read ACK
    #ifdef PN532_DEBUGMODE
    PN532_DEBUG ("Unable to read ACK%s", CFG_PRINTF_NEWLINE);
    #endif
    pn532->state = PN532_STATE_READY;
    return PN532_ERROR_NOACK;
  }

  // Read ACK ... this will also remove it from the buffer
  const byte_t abtAck[6] = { 0x00, 0x00, 0xff, 0x00, 0xff, 0x00 };
  abtRxBuf[0] = uartRxBufferRead();
  abtRxBuf[1] = uartRxBufferRead();
  abtRxBuf[2] = uartRxBufferRead();
  abtRxBuf[3] = uartRxBufferRead();
  abtRxBuf[4] = uartRxBufferRead();
  abtRxBuf[5] = uartRxBufferRead();

  // Make sure the received ACK matches the prototype
  if (0 != (memcmp (abtRxBuf, abtAck, 6))) 
  {
    #ifdef PN532_DEBUGMODE
    PN532_DEBUG ("Invalid ACK: ");
    pn532PrintHex(abtRxBuf, 6);
    PN532_DEBUG("%s", CFG_PRINTF_NEWLINE);
    #endif
    pn532->state = PN532_STATE_READY;
    return PN532_ERROR_INVALIDACK;
  }

  pn532->state = PN532_STATE_READY;
  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    @brief  Reads a response from the PN532

    @note   Possible error message are:

            - PN532_ERROR_BUSY
            - PN532_ERROR_RESPONSEBUFFEREMPTY
            - PN532_ERROR_PREAMBLEMISMATCH
            - PN532_ERROR_APPLEVELERROR
            - PN532_ERROR_EXTENDEDFRAME
            - PN532_ERROR_LENCHECKSUMMISMATCH
*/
/**************************************************************************/
pn532_error_t pn532_bus_ReadResponse(byte_t * pbtResponse, size_t * pszRxLen)
{
  pn532_pcb_t *pn532 = pn532GetPCB();

  // Check if we're busy
  if (pn532->state == PN532_STATE_BUSY)
  {
    return PN532_ERROR_BUSY;
  }

  // Flag the stack as busy
  pn532->state = PN532_STATE_BUSY;

  // Reset the app error flag
  pn532->appError = PN532_APPERROR_NONE;

  // Read response from uart
  if (!uartRxBufferReadArray(pbtResponse, pszRxLen)) 
  {
    pn532->state = PN532_STATE_READY;
    return PN532_ERROR_RESPONSEBUFFEREMPTY;
  }

  // Display the raw response data for debugging if requested
  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Received (%02d): ", *pszRxLen);
  pn532PrintHex(pbtResponse, *pszRxLen);
  #endif
  
  // Check preamble
  const byte_t pn53x_preamble[3] = { 0x00, 0x00, 0xff };
  if (0 != (memcmp (pbtResponse, pn53x_preamble, 3))) 
  {
    #ifdef PN532_DEBUGMODE
    PN532_DEBUG("Frame preamble + start code mismatch%s", CFG_PRINTF_NEWLINE);
    #endif
    pn532->state = PN532_STATE_READY;
    return PN532_ERROR_PREAMBLEMISMATCH;
  }

  // Check the frame type
  if ((0x01 == pbtResponse[3]) && (0xff == pbtResponse[4])) 
  {
    // Error frame
    #ifdef PN532_DEBUGMODE
    PN532_DEBUG("Application level error (0x%02x)%s", pbtResponse[5], CFG_PRINTF_NEWLINE);
    #endif
    // Set application error message ID
    pn532->appError = pbtResponse[5];
    pn532->state = PN532_STATE_READY;
    return PN532_ERROR_APPLEVELERROR;
  } 
  else if ((0xff == pbtResponse[3]) && (0xff == pbtResponse[4])) 
  {
    // Extended frame
    #ifdef PN532_DEBUGMODE
    PN532_DEBUG("Extended frames currently unsupported%s", CFG_PRINTF_NEWLINE);
    #endif
    pn532->state = PN532_STATE_READY;
    return PN532_ERROR_EXTENDEDFRAME;
  } 
  else 
  {
    // Normal frame
    if (256 != (pbtResponse[3] + pbtResponse[4])) 
    {
      // TODO: Retry
      #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Length checksum mismatch%s", CFG_PRINTF_NEWLINE);
      #endif
      pn532->state = PN532_STATE_READY;
      return PN532_ERROR_LENCHECKSUMMISMATCH;
    }
    // size_t szPayloadLen = abtRx[3] - 2;
  }

  pn532->state = PN532_STATE_READY;
  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    @brief      Sends the wakeup sequence to the PN532.
*/
/**************************************************************************/
pn532_error_t pn532_bus_Wakeup(void)
{
  size_t szLen;
  byte_t abtWakeUp[] = { 0x55,0x55,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00 };

  pn532_pcb_t *pn532 = pn532GetPCB();

  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Sending Wakeup Sequence%s", CFG_PRINTF_NEWLINE);
  #endif
  uartSend(abtWakeUp,sizeof(abtWakeUp));
  systickDelay(100);

  byte_t response[32];
  pn532_bus_ReadResponse(response, &szLen);

  // Todo: Check for error ... currently throws a checksum error
  // that isn't really an error

  pn532->state = PN532_STATE_READY;
  return PN532_ERROR_NONE;
}

#endif  // #ifdef PN532_BUS_UART