/**************************************************************************/
/*! 
    @file     pn532_bus_i2c.c
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include <string.h>

#include "pn532.h"
#include "pn532_bus.h"

#ifdef PN532_BUS_I2C

#include "core/systick/systick.h"
#include "core/gpio/gpio.h"
#include "core/i2c/i2c.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

/* ======================================================================
   PRIVATE FUNCTIONS                                                      
   ====================================================================== */

/**************************************************************************/
/*! 
    @brief  Writes an 8 bit value over I2C

    @note   Possible error messages are:

            - PN532_ERROR_I2C_NACK
*/
/**************************************************************************/
pn532_error_t pn532_bus_i2c_WriteData (const byte_t * pbtData, const size_t szData)
{
  uint32_t i2cState;

  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  // Send the specified bytes
  I2CWriteLength = szData+1;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = PN532_I2C_ADDRESS;         // I2C device address
  for ( i = 0; i < szData; i++ )
  {
    I2CMasterBuffer[i+1] = pbtData[i];
  }
  i2cState = i2cEngine();

  // Check if we got an ACK
  if ((i2cState == I2CSTATE_NACK) || (i2cState == I2CSTATE_SLA_NACK))
  {
    // I2C slave didn't acknowledge the master transfer
    // The PN532 probably isn't connected properly or the
    // bus select pins are in the wrong state
    return PN532_ERROR_I2C_NACK;
  }

  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    @brief    Checks the 'IRQ' pin to know if the PN532 is ready to send
              a response or not
    
    @note     The IRQ bit may stay high intentionally, and this isn't
              always an error condition.  When PN532_COMMAND_INLISTPASSIVETARGET
              is sent, for example, the PN532 will wait until a card
              enters the magnetic field, and IRQ will remain high since
              there is no response ready yet.  The IRQ pin will go low
              as soon as a card enters the magentic field and the data
              has been retrieved from it.

    @returns  1 if a response is ready, 0 if the PN532 is still busy or a
              timeout occurred
*/
/**************************************************************************/
uint8_t pn532_bus_i2c_WaitForReady(void) 
{
  uint8_t busy = 1;
  // uint8_t busyTimeout = 0;

  while (busy)
  {
    // For now, we wait forever until a command is ready
    // ToDo: Add user-specified timeout
    busy = gpioGetValue(PN532_I2C_IRQPORT, PN532_I2C_IRQPIN);
    systickDelay(1);
    // busyTimeout++;
    // if (busyTimeout == PN532_I2C_READYTIMEOUT)
    // {
    //   return false;
    // }
  }

  return true;
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
pn532_error_t pn532_bus_i2c_BuildFrame(byte_t * pbtFrame, size_t * pszFrame, const byte_t * pbtData, const size_t szData)
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

/* ======================================================================
   PUBLIC FUNCTIONS                                                      
   ====================================================================== */

/**************************************************************************/
/*! 
    @brief  Initialises I2C and configures the PN532 HW
*/
/**************************************************************************/
void pn532_bus_HWInit(void)
{
  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Initialising I2C %s", CFG_PRINTF_NEWLINE);
  #endif
  i2cInit(I2CMASTER);

  // Set IRQ pin to input
  gpioSetDir(PN532_I2C_IRQPORT, PN532_I2C_IRQPIN, gpioDirection_Input);

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

            - PN532_ERROR_EXTENDEDFRAME       // Extended frames not supported
            - PN532_ERROR_BUSY                // Already busy with a command
            - PN532_ERROR_I2C_NACK            // No ACK on I2C
            - PN532_ERROR_READYSTATUSTIMEOUT  // Timeout waiting for ready bit
            - PN532_ERROR_INVALIDACK          // No ACK frame received
*/
/**************************************************************************/
pn532_error_t pn532_bus_SendCommand(const byte_t * pbtData, const size_t szData)
{
  pn532_error_t error = PN532_ERROR_NONE;
  pn532_pcb_t *pn532 = pn532GetPCB();
    
  // Check if we're busy
  if (pn532->state == PN532_STATE_BUSY)
  {
    return PN532_ERROR_BUSY;
  }

  // Flag the stack as busy
  pn532->state = PN532_STATE_BUSY;

  // --------------------------------------------------------------------
  // Send the command frame
  // --------------------------------------------------------------------
  byte_t abtFrame[PN532_BUFFER_LEN] = { 0x00, 0x00, 0xff };
  size_t szFrame = 0;

  // Build the frame
  pn532_bus_i2c_BuildFrame (abtFrame, &szFrame, pbtData, szData);

  // Keep track of the last command that was sent
  pn532->lastCommand = pbtData[0];

  // Output the frame data for debugging if requested
  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Sending  (%02d): ", szFrame);
  pn532PrintHex(abtFrame, szFrame);
  #endif

  // Send data to the PN532
  error = pn532_bus_i2c_WriteData(abtFrame, szFrame);

  if (error == PN532_ERROR_I2C_NACK)
  {
    // Most likely error is PN532_ERROR_I2C_NACK
    // meaning no I2C ACK received from the PN532
    #ifdef PN532_DEBUGMODE
    PN532_DEBUG ("No ACK received on I2C bus%s", CFG_PRINTF_NEWLINE);
    #endif
    pn532->state = PN532_STATE_READY;
    return error;
  }

  // --------------------------------------------------------------------
  // Wait for the IRQ/Ready flag
  // --------------------------------------------------------------------
  if (!(pn532_bus_i2c_WaitForReady()))
  {
    pn532->state = PN532_STATE_READY;
    #ifdef PN532_DEBUGMODE
    PN532_DEBUG ("Timed out waiting for IRQ/Ready%s", CFG_PRINTF_NEWLINE);
    #endif
    return PN532_ERROR_READYSTATUSTIMEOUT;
  }

  // --------------------------------------------------------------------
  // Read the ACK frame
  // --------------------------------------------------------------------
  uint32_t i;
  // Clear buffer
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }
  I2CWriteLength = 0;
  I2CReadLength = 7;  // ACK + Ready bit = 7
  I2CMasterBuffer[0] = PN532_I2C_ADDRESS | PN532_I2C_READBIT;
  i2cEngine();

  // Make sure the received ACK matches the prototype
  const byte_t abtAck[6] = { 0x00, 0x00, 0xff, 0x00, 0xff, 0x00 };
  byte_t abtRxBuf[6];
  // memcpy(abtRxBuf, I2CSlaveBuffer+1, 6);
  for ( i = 0; i < 6; i++ )
  {
    abtRxBuf[i] = I2CSlaveBuffer[i+1];
  }
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

  // --------------------------------------------------------------------
  // Wait for the post-ACK IRQ/Ready flag
  // --------------------------------------------------------------------
  if (!(pn532_bus_i2c_WaitForReady()))
  {
    pn532->state = PN532_STATE_READY;
    #ifdef PN532_DEBUGMODE
    PN532_DEBUG ("Timed out waiting for IRQ/Ready%s", CFG_PRINTF_NEWLINE);
    #endif
    return PN532_ERROR_READYSTATUSTIMEOUT;
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

  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }
  I2CWriteLength = 0;
  I2CReadLength = I2C_BUFSIZE;
  I2CMasterBuffer[0] = PN532_I2C_ADDRESS | PN532_I2C_READBIT;
  i2cEngine();

  // Display the raw response data for debugging if requested
  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Received (%02d): ", I2C_BUFSIZE-1);
  pn532PrintHex(pbtResponse, I2C_BUFSIZE-1);
  #endif

  // Use the full I2C buffer size for now (until we're sure we have a good frame)
  *pszRxLen = I2C_BUFSIZE - 1;

  // Fill the response buffer from I2C (skipping the leading 'ready' bit when using I2C)
  // memcpy(pbtResponse, I2CSlaveBuffer+1, I2C_BUFSIZE-1);
  for ( i = 0; i < I2C_BUFSIZE-1; i++ )
  {
    pbtResponse[i] = I2CSlaveBuffer[i+1];
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
  }

  // Figure out how large the response really is
  // Response Frame Len = pbtResponse[3] + 7 (00 00 FF LEN LCS TFI [DATA] DCS)
  *pszRxLen = pbtResponse[3] + 7;

  pn532->state = PN532_STATE_READY;
  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    @brief      Sends the wakeup sequence to the PN532.

    @note   Possible error message are:

            - PN532_ERROR_BUSY
            - PN532_ERROR_I2C_NACK            // No I2C ACK
            - PN532_ERROR_READYSTATUSTIMEOUT  // Timed out waiting for ready bit
*/
/**************************************************************************/
pn532_error_t pn532_bus_Wakeup(void)
{
  pn532_error_t error = PN532_ERROR_NONE;
  byte_t abtWakeUp[] = { 0x55,0x55,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00 };
  uint32_t i;

  pn532_pcb_t *pn532 = pn532GetPCB();

  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Sending Wakeup Sequence%s", CFG_PRINTF_NEWLINE);
  #endif
  error = pn532_bus_i2c_WriteData(abtWakeUp,sizeof(abtWakeUp));
  systickDelay(100);

  // Wait for the IRQ/Ready flag to indicate a response is ready
  if (!(pn532_bus_i2c_WaitForReady()))
  {
    error = PN532_ERROR_READYSTATUSTIMEOUT;
  }

  // Read and discard the ACK frame
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }
  I2CWriteLength = 0;
  I2CReadLength = 7;  // ACK + Ready bit = 7
  I2CMasterBuffer[0] = PN532_I2C_ADDRESS | PN532_I2C_READBIT;
  i2cEngine();
  systickDelay(1);

  // Wait for the IRQ/Ready flag to indicate a response is ready
  if (!(pn532_bus_i2c_WaitForReady()))
  {
    error = PN532_ERROR_READYSTATUSTIMEOUT;
  }

  pn532->state = PN532_STATE_READY;
  return error;
}

#endif  // #ifdef PN532_BUS_I2C