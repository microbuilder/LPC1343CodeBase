/**************************************************************************/
/*! 
    @file     pn532_mifare_ultralight.c
*/
/**************************************************************************/

/*  MIFARE ULTRALIGHT DESCRIPTION
    =============================

    MIFARE Ultralight cards typically contain 512 bits (64 bytes) of
    memory, including 4 bytes (32-bits) of OTP (One Time Programmable)
    memory where the individual bits can be written but not erased.

        MF0ICU1 Mifare Ultralight Functional Specification:
        http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf
            

    Mifare Ultralight cards have a 7-byte UID
    
    EEPROM MEMORY
    =============
    Mifare Ultralight cards have 512 bits (64 bytes) of EEPROM memory,
    including 4 byte (32 bits) of OTP memory.  Unlike Mifare Classic cards,
    there is no authentication on a per block level, although the blocks
    can be set to "read-only" mode using Lock Bytes (described below).

    EEPROM memory is organised into 16 pages of four bytes eachs, in
    the following order

    Page   Description
    ----   ------------
    0      Serial Number (4 bytes)
    1      Serial Number (4 bytes)
    2      Byte 0:    Serial Number
           Byte 1:    Internal Memory
           Byte 2..3: lock bytes
    3      One-time programmable memory (4 bytes)
    4..15  User memory (4 bytes)

    Lock Bytes (Page 2)
    -------------------
    Bytes 2 and 3 of page 2 are referred to as "Lock Bytes".  Each
    page from 0x03 and higher can individually locked by setting the
    corresponding locking bit to "1" to prevent further write access,
    effectively making the memory read only.

    For information on the lock byte mechanism, refer to section 8.5.2 of
    the datasheet (referenced above).

    OTP Bytes (Page 3)
    ------------------
    Page 3 is the OTP memory, and by default all bits on this page are
    set to 0.  These bits can be bitwise modified using the Mifare WRITE
    command, and individual bits can be set to 1, but can not be changed
    back to 0.

    Data Pages (Pages 4..15)
    ------------------------
    Pages 4 to 15 are can be freely read from and written to, 
    provided there is no conflict with the Lock Bytes described above.

    After production, the bytes have the following default values:

    Page    Byte Values
    ----    ----------------------
            0     1     2     3
    4       0xFF  0xFF  0xFF  0xFF
    5..15   0x00  0x00  0x00  0x00

    ACCESSING DATA BLOCKS
    =====================

    Before you can access the cards, you must following two steps:

    1.) 'Connect' to a Mifare Ultralight card and retrieve the 7 byte
        UID of the card.

    2.) Memory can be read and written directly once a passive mode
        connection has been made.  No authentication is required for
        Mifare Ultralight cards.

*/

#include <string.h>

#include "../pn532.h"
#include "../pn532_bus.h"
#include "pn532_mifare_ultralight.h"

#include "core/systick/systick.h"

/**************************************************************************/
/*! 
    Tries to detect MIFARE targets in passive mode.

    @param  pbtCUID     Pointer to the byte array where the card's 7 byte
                        UID will be stored once a card is detected
    @param  pszUIDLen   Pointer to the size of the card UID in bytes

    Response for a valid ISO14443A 106KBPS (Mifare Ultralight, etc.)
    should be in the following format.  See UM0701-02 section
    7.3.5 for more information
    
    byte            Description
    -------------   ------------------------------------------
    b0..6           Frame header and preamble
    b7              Tags Found
    b8              Tag Number (only one used in this example)
    b9..10          SENS_RES
    b11             SEL_RES
    b12             NFCID Length
    b13..NFCIDLen   NFCID                                      
    
    SENS_RES   SEL_RES     Manufacturer/Card Type    NFCID Len
    --------   -------     -----------------------   ---------
    00 44      00          NXP Mifare Ultralight     7 bytes   

    @note   Possible error messages are:

            - PN532_ERROR_WRONGCARDTYPE
*/
/**************************************************************************/
pn532_error_t pn532_mifareultralight_WaitForPassiveTarget (byte_t * pbtCUID, size_t * szCUIDLen)
{
  byte_t abtResponse[PN532_RESPONSELEN_INLISTPASSIVETARGET];
  pn532_error_t error;
  size_t szLen;

  #ifdef PN532_DEBUGMODE
    PN532_DEBUG("Waiting for an ISO14443A Card%s", CFG_PRINTF_NEWLINE);
  #endif

  /* Try to initialise a single ISO14443A tag at 106KBPS                  */
  /* Note:  To wait for a card with a known UID, append the four byte     */
  /*        UID to the end of the command.                                */ 
  byte_t abtCommand[] = { PN532_COMMAND_INLISTPASSIVETARGET, 0x01, PN532_MODULATION_ISO14443A_106KBPS};
  error = pn532Write(abtCommand, sizeof(abtCommand));
  if (error) 
    return error;

  /* Wait until we get a valid response or a timeout                      */
  do
  {
    systickDelay(25);
    error = pn532Read(abtResponse, &szLen);
  } while (error == PN532_ERROR_RESPONSEBUFFEREMPTY);
  if (error) 
    return error;

  /* Check SENS_RES to make sure this is a Mifare Ultralight card         */
  /*          Mifare Ultralight = 00 44                                   */
  if (abtResponse[10] == 0x44)
  {
    /* Card appears to be Mifare Ultralight */
    *szCUIDLen = abtResponse[12];
    for (uint8_t i=0; i < *szCUIDLen; i++) 
    {
      pbtCUID[i] = abtResponse[13+i];
    }
    #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Card Found: %s", CFG_PRINTF_NEWLINE);
      PN532_DEBUG("      ATQA: ");
      pn532PrintHex(abtResponse+9, 2);
      PN532_DEBUG("      SAK: %02x%s", abtResponse[11], CFG_PRINTF_NEWLINE);
      PN532_DEBUG("      UID: ");
      pn532PrintHex(pbtCUID, *szCUIDLen);
    #endif
  }
  else
  {
    /* Card is ISO14443A but doesn't appear to be Mifare Ultralight       */
    /*    Mifare Classic       = 0x0002, 0x0004, 0x0008                   */
    /*    Mifare DESFire       = 0x0344                                   */
    /*    Innovision Jewel     = 0x0C00                                   */
    #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Wrong Card Type (Expected ATQA 00 44) %s%s", CFG_PRINTF_NEWLINE, CFG_PRINTF_NEWLINE);
      PN532_DEBUG("  ATQA       : ");
      pn532PrintHex(abtResponse+9, 2);
      PN532_DEBUG("  SAK        : %02x%s", abtResponse[11], CFG_PRINTF_NEWLINE);
      PN532_DEBUG("  UID Length : %d%s", abtResponse[12], CFG_PRINTF_NEWLINE);
      PN532_DEBUG("  UID        : ");
      size_t pos;
      for (pos=0; pos < abtResponse[12]; pos++) 
      {
        printf("%02x ", abtResponse[13 + pos]);
      }
      printf("%s%s", CFG_PRINTF_NEWLINE, CFG_PRINTF_NEWLINE);
    #endif
    return PN532_ERROR_WRONGCARDTYPE;
  }

  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    Tries to read an entire 4-byte page at the specified address.

    @param  page        The page number (0..63 in most cases)
    @param  pbtBuffer   Pointer to the byte array that will hold the
                        retrieved data (if any)

    @note   Possible error messages are:

            - PN532_ERROR_ADDRESSOUTOFRANGE
            - PN532_ERROR_BLOCKREADFAILED
*/
/**************************************************************************/
pn532_error_t pn532_mifareultralight_ReadPage (uint8_t page, byte_t * pbtBuffer)
{
  pn532_error_t error;
  byte_t abtCommand[4];
  byte_t abtResponse[PN532_RESPONSELEN_INDATAEXCHANGE];
  size_t szLen;

  if (page >= 64)
  {
    return PN532_ERROR_ADDRESSOUTOFRANGE;
  }

  #ifdef PN532_DEBUGMODE
    PN532_DEBUG("Reading page %03d%s", page, CFG_PRINTF_NEWLINE);
  #endif

  /* Prepare the command */
  abtCommand[0] = PN532_COMMAND_INDATAEXCHANGE;
  abtCommand[1] = 1;                            /* Card number */
  abtCommand[2] = PN532_MIFARE_CMD_READ;        /* Mifare Read command = 0x30 */
  abtCommand[3] = page;                         /* Page Number (0..63 in most cases) */
  
  /* Send the commands */
  error = pn532Write(abtCommand, sizeof(abtCommand));
  if (error)
  {
    /* Bus error, etc. */
    #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Read failed%s", CFG_PRINTF_NEWLINE);
    #endif
    return error;
  }

  /* Read the response */
  memset(abtResponse, 0, PN532_RESPONSELEN_INDATAEXCHANGE);
  do
  {
    systickDelay(50);
    error = pn532Read(abtResponse, &szLen);
  }
  while (error == PN532_ERROR_RESPONSEBUFFEREMPTY);
  if (error)
  {
    #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Read failed%s", CFG_PRINTF_NEWLINE);
    #endif
    return error;
  }  

  /* Make sure we have a valid response (should be 26 bytes long) */
  if (szLen == 26)
  {
    /* Copy the 4 data bytes to the output buffer         */
    /* Block content starts at byte 9 of a valid response */
    /* Note that the command actually reads 16 byte or 4  */
    /* pages at a time ... we simply discard the last 12  */
    /* bytes                                              */
    memcpy (pbtBuffer, abtResponse+8, 4);
  }
  else
  {
    #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Unexpected response reading block %d.  Bad key?%s", page, CFG_PRINTF_NEWLINE);
    #endif
    return PN532_ERROR_BLOCKREADFAILED;
  }

  /* Display data for debug if requested */
  #ifdef PN532_DEBUGMODE
    PN532_DEBUG("Page %02d: ", page, CFG_PRINTF_NEWLINE);
    pn532PrintHexVerbose(pbtBuffer, 4);
  #endif

  // Return OK signal
  return PN532_ERROR_NONE;
}

//static  bool
//read_card (void)
//{
//  uint32_t page;
//  bool    bFailure = false;
//  uint32_t uiReadedPages = 0;
//
//  printf ("Reading %d pages |", uiBlocks + 1);
//
//  for (page = 0; page <= uiBlocks; page += 4) {
//    // Try to read out the data block
//    if (nfc_initiator_mifare_cmd (pnd, MC_READ, page, &mp)) {
//      memcpy (mtDump.amb[page / 4].mbd.abtData, mp.mpd.abtData, 16);
//    } else {
//      bFailure = true;
//      break;
//    }
//
//    print_success_or_failure (bFailure, &uiReadedPages);
//    print_success_or_failure (bFailure, &uiReadedPages);
//    print_success_or_failure (bFailure, &uiReadedPages);
//    print_success_or_failure (bFailure, &uiReadedPages);
//  }
//  printf ("|\n");
//  printf ("Done, %d of %d pages readed.\n", uiReadedPages, uiBlocks + 1);
//  fflush (stdout);
//
//  return (!bFailure);
//}
