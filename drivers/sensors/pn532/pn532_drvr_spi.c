/**************************************************************************/
/*! 
    @file   pn532_drvr_spi.c
*/
/**************************************************************************/
#include "pn532_drvr.h"

#ifdef PN532_SPI

#include <string.h>
#include "core/systick/systick.h"
#include "core/gpio/gpio.h"
#include "core/ssp/ssp.h"

#define PN532_SELECT        gpioSetValue(PN532_SPI_CSPORT, PN532_SPI_CSPIN, 0);
#define PN532_DESELECT      gpioSetValue(PN532_SPI_CSPORT, PN532_SPI_CSPIN, 1);

#define PN532_SPI_STATREAD  0x02
#define PN532_SPI_DATAWRITE 0x01
#define PN532_SPI_DATAREAD  0x03
#define PN532_SPI_READY     0x01

// The number of attempts to make while waiting for the status ready bit
// Each attempt is 1ms
#define PN532_SPI_TIMEOUT   100

/**************************************************************************/
/*! 
    @brief  Writes a single byte via SPI
*/
/**************************************************************************/
uint8_t pn532SPIWrite(unsigned char data)
{
  // Note: bits have to be reversed since SPI is LSB on the PN532
  // Thankfully the M3 has a quick HW reverse command (RBIT)
  while ((SSP_SSP0SR & SSP_SSP0SR_TNF_NOTFULL) == 0);
  SSP_SSP0DR = ((unsigned char)(RBIT(data)>>24));       // Write
  while ((SSP_SSP0SR & SSP_SSP0SR_RNE_NOTEMPTY) == 0);
  data =  ((unsigned char)(RBIT(SSP_SSP0DR)>>24));      // Read
  return data;
}

/**************************************************************************/
/*! 
    @brief  Writes a byte array via SPI
*/
/**************************************************************************/
void pn532SPIWriteArray(byte_t * pbtData, size_t len)
{
  while (len != 0)
  {
    pn532SPIWrite(*pbtData);
    pbtData++;
    len--;
  }
}

/**************************************************************************/
/*! 
    @brief  Reads a single byte via SPI
*/
/**************************************************************************/
uint8_t pn532SPIRead(void)
{
  return pn532SPIWrite(0x00);
}

/**************************************************************************/
/*! 
    @brief  Reads a byte array via SPI
*/
/**************************************************************************/
void pn532SPIReadArray(uint8_t* buff, size_t len) 
{
  size_t i;
  for (i=0; i<len; i++) 
  {
    buff[i] = pn532SPIRead();
  }
}


/**************************************************************************/
/*! 
    @brief  Sends a status request (SPI only)
*/
/**************************************************************************/
uint8_t pn532GetStatus()
{
  unsigned char res;
  
  // Send SPI status request
  PN532_SELECT;
  pn532SPIWrite(PN532_SPI_STATREAD);
  res = pn532SPIRead();
  PN532_DESELECT;
  
  return res;
}

/**************************************************************************/
/*! 
    @brief  Initialises SPI and configures the PN532
*/
/**************************************************************************/
void pn532HWInit(void)
{
  PN532_DEBUG("Initialising SPI %s", CFG_PRINTF_NEWLINE);
  sspInit(0, sspClockPolarity_Low, sspClockPhase_RisingEdge); 

  // Configure CS pin
  gpioSetDir(PN532_SPI_CSPORT, PN532_SPI_CSPIN, gpioDirection_Output);
  PN532_DESELECT;

  // Reset PN532 and wait for it to finish booting
  gpioSetDir(PN532_RSTPD_PORT, PN532_RSTPD_PIN, gpioDirection_Output);
  PN532_DEBUG("Resetting the PN532...\r\n");
  gpioSetValue(PN532_RSTPD_PORT, PN532_RSTPD_PIN, 0);
  systickDelay(400);
  gpioSetValue(PN532_RSTPD_PORT, PN532_RSTPD_PIN, 1);
  systickDelay(100);
}

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
pn532_error_t pn532BuildFrame(byte_t * pbtFrame, size_t * pszFrame, const byte_t * pbtData, const size_t szData)
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
    @brief  Sends the specified command to the PN532, automatically
            creating an appropriate frame for it

    @param  pdbData   Pointer to the byte data to send
    @param  szData    Length in bytes of the data to send

    @note   Possible error messages are:

            - PN532_ERROR_BUSY
            - PN532_ERROR_NOACK
            - PN532_ERROR_INVALIDACK
            - PN532_ERROR_SPIREADYSTATUSTIMEOUT
*/
/**************************************************************************/
pn532_error_t pn532SendCommand(const byte_t * pbtData, const size_t szData)
{
  pn532_pcb_t *pn532 = pn532GetPCB();
    
  // Check busy flag
  if (pn532->state == PN532_STATE_BUSY)
  {
    return PN532_ERROR_BUSY;
  }
  pn532->state = PN532_STATE_BUSY;

  // Every packet must start with "00 00 ff"
  byte_t  abtFrame[PN532_BUFFER_LEN] = { 0x00, 0x00, 0xff };
  size_t szFrame = 0;

  // Build the frame
  pn532BuildFrame (abtFrame, &szFrame, pbtData, szData);

  // Register the last command that was sent
  pn532->lastCommand = pbtData[0];

  // Output the frame data for debugging if requested
  PN532_DEBUG("Sending  (%02d): ", szFrame);
  pn532PrintHex(abtFrame, szFrame);

  PN532_SELECT;
  systickDelay(5);

  // Send data to the PN532
  pn532SPIWrite(PN532_SPI_DATAWRITE);
  pn532SPIWriteArray(abtFrame, szFrame);

  // Wait for READY status
  size_t t = 0;
  while (pn532GetStatus() != PN532_SPI_READY) 
  {
    if(t++>PN532_SPI_TIMEOUT) 
    {
      PN532_DEBUG("Timeout waiting for READY status%s", CFG_PRINTF_NEWLINE);
      pn532->state = PN532_STATE_READY;
      return PN532_ERROR_SPIREADYSTATUSTIMEOUT;
    }
    systickDelay(1);
  }

  // Read ACK
  PN532_SELECT;
  pn532SPIWrite(PN532_SPI_DATAREAD);
  const byte_t abtAck[6] = { 0x00, 0x00, 0xff, 0x00, 0xff, 0x00 };
  byte_t abtRxBuf[6];
  pn532SPIReadArray(abtRxBuf, 6);
  PN532_DESELECT;

  // Make sure the received ACK matches the prototype
  if (0 != (memcmp (abtRxBuf, abtAck, 6))) 
  {
    PN532_DEBUG ("Invalid ACK: ");
    pn532PrintHex(abtRxBuf, 6);
    PN532_DEBUG("%s", CFG_PRINTF_NEWLINE);
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
            - PN532_ERROR_SPIREADYSTATUSTIMEOUT
*/
/**************************************************************************/
pn532_error_t pn532ReadResponse(byte_t * pbtResponse, size_t * pszRxLen)
{
  size_t t, i;
  pn532_pcb_t *pn532 = pn532GetPCB();

  // Check if we're busy
  if (pn532->state == PN532_STATE_BUSY)
  {
    return PN532_ERROR_BUSY;
  }
  pn532->state = PN532_STATE_BUSY;
  pn532->appError = PN532_APPERROR_NONE;

  // Wait for the response ready signal
  t = 0;
  while (pn532GetStatus() != PN532_SPI_READY)
  {
    if(t++>PN532_SPI_TIMEOUT) 
    {
      PN532_DEBUG("Timeout waiting for READY status%s", CFG_PRINTF_NEWLINE);
      pn532->state = PN532_STATE_READY;
      return PN532_ERROR_SPIREADYSTATUSTIMEOUT;
    }
    systickDelay(1);
  }

  PN532_SELECT;
  systickDelay(1);

  pn532SPIWrite(PN532_SPI_DATAREAD);

  // Check preamble
  pbtResponse[0] = pn532SPIRead();
  pbtResponse[1] = pn532SPIRead();
  pbtResponse[2] = pn532SPIRead();
  const byte_t pn53x_preamble[3] = { 0x00, 0x00, 0xff };
  if (0 != (memcmp (pbtResponse, pn53x_preamble, 3))) 
  {
    PN532_DEBUG("Frame preamble + start code mismatch%s", CFG_PRINTF_NEWLINE);
    pn532->state = PN532_STATE_READY;
    return PN532_ERROR_PREAMBLEMISMATCH;
  }

  // Check the frame type
  pbtResponse[3] = pn532SPIRead();
  pbtResponse[4] = pn532SPIRead();
  pbtResponse[5] = pn532SPIRead();
  if ((0x01 == pbtResponse[3]) && (0xff == pbtResponse[4])) 
  {
    // Error frame
    PN532_DEBUG("Application level error (%02d)%s", pbtResponse[5], CFG_PRINTF_NEWLINE);
    // Set application error message ID
    pn532->appError = pbtResponse[5];
    pn532->state = PN532_STATE_READY;
    return PN532_ERROR_APPLEVELERROR;
  } 
  else if ((0xff == pbtResponse[3]) && (0xff == pbtResponse[4])) 
  {
    // Extended frame
    PN532_DEBUG("Extended frames currently unsupported%s", CFG_PRINTF_NEWLINE);
    pn532->state = PN532_STATE_READY;
    return PN532_ERROR_EXTENDEDFRAME;
  } 
  else 
  {
    // Check checksum, unless this is a response to the wakeup command
    if (pn532->lastCommand = PN532_COMMAND_SAMCONFIGURATION)
    {
      *pszRxLen = 6;
    }
    else
    {
      // Normal frame
      if (256 != (pbtResponse[3] + pbtResponse[4])) 
      {
        // TODO: Retry
        PN532_DEBUG("Length checksum mismatch%s", CFG_PRINTF_NEWLINE);
        pn532PrintHex(pbtResponse, 6);
        pn532->state = PN532_STATE_READY;
        return PN532_ERROR_LENCHECKSUMMISMATCH;
      }
      // Read payload
      size_t szPayloadLen = pbtResponse[3] - 2;
      for (i=0; i<szPayloadLen; i++) 
      {
        pbtResponse[i+6] = pn532SPIRead();
      }
      // Get checksum and postamble
      pbtResponse[i+7] = pn532SPIRead();
      pbtResponse[i+8] = pn532SPIRead();
      pbtResponse[i+9] = pn532SPIRead();
      // Set frame length
      *pszRxLen = i + 10;
    }
  }

  // Display the raw response data for debugging if requested
  PN532_DEBUG("Received (%02d): ", *pszRxLen);
  pn532PrintHex(pbtResponse, *pszRxLen);

  pn532->state = PN532_STATE_READY;
  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    @brief      Sends the wakeup sequence to the PN532.
*/
/**************************************************************************/
pn532_error_t pn532Wakeup(void)
{
  size_t szLen;
  byte_t abtWakeUp[] = { 0x01,0x55,0x55,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00 };
  // byte_t abtWakeUp[] = { 0x01,0x00,0x00,0xff,0x03,0xfd,0xd4,PN532_COMMAND_SAMCONFIGURATION,0x01,0x17,0x00 };

  pn532_pcb_t *pn532 = pn532GetPCB();

  PN532_DEBUG("Sending Wakeup Sequence%s", CFG_PRINTF_NEWLINE);

  PN532_SELECT;
  systickDelay(2);

  // Transmit wakeup sequence
  pn532SPIWriteArray(abtWakeUp, sizeof(abtWakeUp));
  systickDelay(100);

  // Register the last command that was sent
  pn532->lastCommand = PN532_COMMAND_SAMCONFIGURATION;

  byte_t response[32];
  pn532ReadResponse(response, &szLen);
  PN532_DESELECT;

  // Todo: Check for error ... currently throws a checksum error
  // that isn't really an error

  pn532->state = PN532_STATE_READY;
  return PN532_ERROR_NONE;
}

#endif  // #ifdef PN532_SPI
