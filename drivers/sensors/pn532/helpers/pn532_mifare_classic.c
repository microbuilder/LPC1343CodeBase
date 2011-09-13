/**************************************************************************/
/*! 
    @file     pn532_mifare_classic.c
*/
/**************************************************************************/

/*  MIFARE CLASSIC DESCRIPTION
    ==========================

    MIFARE Classic cards come in 1K and 4K varieties.  While several
    varieties of chips exist, the two main chipsets used are described
    in the following publicly accessible documents:

        MF1S503x Mifare Classic 1K data sheet:
        http://www.nxp.com/documents/data_sheet/MF1S503x.pdf
            
        MF1S70yyX MIFARE Classic 4K data sheet:
        http://www.nxp.com/documents/data_sheet/MF1S70YYX.pdf

    Mifare Classic cards typically have a a 4-byte NUID, though you may
    find cards with 7 byte IDs as well
        
    EEPROM MEMORY
    =============
    Mifare Classic cards have either 1K or 4K of EEPROM memory. Each
    memory block can be configured with different access conditions,
    with two seperate authentication keys present in each block.

    The two main Mifare Classic card types are organised as follows:

        1K Cards: 16 sectors of 4 blocks (0..15)
        4K Cards: 32 sectors of 4 blocks (0..31) and
                  8 sectors of 16 blocks (32..39)

    4 block sectors
    ===============
    Sector  Block   Bytes                                                           Description
    ------  -----   -----                                                           -----------
                    0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15

    1       3       [-------KEY A-------]   [Access Bits]   [-------KEY A-------]   Sector Trailer 1
            2       [                            Data                           ]   Data
            1       [                            Data                           ]   Data
            0       [                            Data                           ]   Data

    0       3       [-------KEY A-------]   [Access Bits]   [-------KEY A-------]   Sector Trailer 1
            2       [                            Data                           ]   Data
            1       [                            Data                           ]   Data
            0       [                     Manufacturer Data                     ]   Manufacturer Block

    Sector Trailer (Block 3)
    ------------------------
    The sector trailer block contains the two secret keys (Key A and Key B), as well
    as the access conditions for the four blocks.  It has the following structure:

        Sector Trailer Bytes
        --------------------------------------------------------------
        0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
        [       Key A       ]   [Access Bits]   [       Key B       ]

    For more information in using Keys to access the clock contents, see
    Accessing Data Blocks further below.

    Data Blocks (Blocks 0..2)
    -------------------------
    Data blocks are 16 bytes wide and, depending on the permissions set in the
    access bits, can be read from and written to. You are free to use the 16 data
    bytes in any way you wish.  You can easily store text input, store four 32-bit
    integer values, a 16 character uri, etc.

    Data Blocks as "Value Blocks"
    -----------------------------
    An alternative to storing random data in the 16 byte-wide blocks is to
    configure them as "Value Blocks".  Value blocks allow performing electronic
    purse functions (valid commands are: read, write, increment, decrement,
    restore, transfer).
    
    Each Value block contains a single signed 32-bit value, and this value is
    stored 3 times for data integrity and security reasons.  It is stored twice
    non-inverted, and once inverted.  The last 4 bytes are used for a 1-byte
    address, which is stored 4 times (twice non-inverted, and twice inverted).
    
    Data blocks configured as "Value Blocks" have the following structure:

        Value Block Bytes
        --------------------------------------------------------------
        0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
        [   Value   ]   [   ~Value  ]   [   Value   ]   [A  ~A  A   ~A]

    Manufacturer Block (Sector 0, Block 0)
    --------------------------------------
    Sector 0 is special since it contains the Manufacturer Block. This block
    contains the manufacturer data, and is read-only.  It should be avoided
    unless you know what you are doing.

    16 block sectors
    ================
    16 block sectors are identical to 4 block sectors, but with more data blocks.  The same
    structure described in the 4 block sectors above applies.

    Sector  Block   Bytes                                                           Description
    ------  -----   -----                                                           ----------
                    0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15

    32      15      [-------KEY A-------]   [Access Bits]   [-------KEY B-------]   Sector Trailer 32
            14      [                            Data                           ]   Data
            13      [                            Data                           ]   Data
            ...
            2       [                            Data                           ]   Data
            1       [                            Data                           ]   Data
            0       [                            Data                           ]   Data

    ACCESSING DATA BLOCKS
    =====================

    Before you can access the cards, you must following two steps:

    1.) You must retrieve the 7 byte UID or the 4-byte NUID of the card.
        This can be done using pn532_mifareclassic_WaitForPassiveTarget()
        below, which will return the appropriate ID.

    2.) You must authenticate the sector you wish to access according to the
        access rules defined in the Sector Trailer block for that sector.
        This can be done using pn532_mifareclassic_AuthenticateBlock(),
        passing in the appropriate key value.
          
        Most new cards have a default Key A of 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF,
        but some common values worth trying are:

            0XFF 0XFF 0XFF 0XFF 0XFF 0XFF
            0XD3 0XF7 0XD3 0XF7 0XD3 0XF7
            0XA0 0XA1 0XA2 0XA3 0XA4 0XA5
            0XB0 0XB1 0XB2 0XB3 0XB4 0XB5
            0X4D 0X3A 0X99 0XC3 0X51 0XDD
            0X1A 0X98 0X2C 0X7E 0X45 0X9A
            0XAA 0XBB 0XCC 0XDD 0XEE 0XFF
            0X00 0X00 0X00 0X00 0X00 0X00
            0XAB 0XCD 0XEF 0X12 0X34 0X56

    3.) Once authenication has succeeded, and depending on the sector
        permissions, you can then read/write/increment/decrement the
        contents of the specific block, using one of the helper functions
        included in this module.

*/

#include <string.h>

#include "../pn532.h"
#include "../pn532_bus.h"
#include "pn532_mifare.h"
#include "pn532_mifare_classic.h"

#include "core/systick/systick.h"

/**************************************************************************/
/*! 
      Indicates whether the specified block number is the first block
      in the sector (block 0 relative to the current sector)
*/
/**************************************************************************/
bool is_first_block (uint32_t uiBlock)
{
  // Test if we are in the small or big sectors
  if (uiBlock < 128)
    return ((uiBlock) % 4 == 0);
  else
    return ((uiBlock) % 16 == 0);
}

/**************************************************************************/
/*! 
      Indicates whether the specified block number is the sector trailer
*/
/**************************************************************************/
bool is_trailer_block (uint32_t uiBlock)
{
  // Test if we are in the small or big sectors
  if (uiBlock < 128)
    return ((uiBlock + 1) % 4 == 0);
  else
    return ((uiBlock + 1) % 16 == 0);
}

/**************************************************************************/
/*! 
    Tries to detect MIFARE targets in passive mode.  This needs to be done
    before anything useful can be accomplished with a tag since you need
    the tag's unique UID to communicate with it.

    @param  pbtCUID     Pointer to the byte array where the card's UID
                        will be stored once a card is detected
    @param  pszUIDLen   Pointer to the size of the card UID in bytes

    Response for a valid ISO14443A 106KBPS (Mifare Classic, etc.)
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
    00 04      08          NXP Mifare Classic 1K     4 bytes   
    00 02      18          NXP Mifare Classic 4K     4 bytes

    @note   Possible error messages are:

            - PN532_ERROR_WRONGCARDTYPE
*/
/**************************************************************************/
pn532_error_t pn532_mifareclassic_WaitForPassiveTarget (byte_t * pbtCUID, size_t * szCUIDLen)
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

  /* Check SENSE_RES to make sure this is a Mifare Classic card           */
  /*          Classic 1K       = 00 04                                    */
  /*          Classic 4K       = 00 02                                    */
  /*          Classic Emulated = 00 08                                    */
  if ((abtResponse[10] == 0x02) || 
      (abtResponse[10] == 0x04) || 
      (abtResponse[10] == 0x08))
  {
    /* Card appears to be Mifare Classic */
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
    /* Card is ISO14443A but doesn't appear to be Mifare Classic          */
    /*    Mifare Ultralight    = 0x0044                                   */
    /*    Mifare DESFire       = 0x0344                                   */
    /*    Innovision Jewel     = 0x0C00                                   */
    #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Wrong Card Type (Expected ATQA 00 02, 00 04 or 00 08) %s%s", CFG_PRINTF_NEWLINE, CFG_PRINTF_NEWLINE);
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
    Tries to authenticate a block of memory on a MIFARE card using the
    INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
    for more information on sending MIFARE and other commands.

    @param  pbtCUID       Pointer to a byte array containing the card UID
    @param  szCUIDLen     The length (in bytes) of the card's UID (Should
                          be 4 for MIFARE Classic)
    @param  uiBlockNumber The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  uiKeyType     Which key type to use during authentication
                          (PN532_MIFARE_CMD_AUTH_A or PN532_MIFARE_CMD_AUTH_B)
    @param  pbtKeys       Pointer to a byte array containing the 6 byte
                          key value
*/
/**************************************************************************/
pn532_error_t pn532_mifareclassic_AuthenticateBlock (byte_t * pbtCUID, size_t szCUIDLen, uint32_t uiBlockNumber, uint8_t uiKeyType, byte_t * pbtKeys)
{
  pn532_error_t error;
  byte_t abtCommand[17];
  byte_t abtResponse[PN532_RESPONSELEN_INDATAEXCHANGE];
  size_t szLen;

  #ifdef PN532_DEBUGMODE
  PN532_DEBUG("Trying to authenticate card ");
  pn532PrintHex(pbtCUID, szCUIDLen);
  #endif

  /* Prepare the authentication command */
  abtCommand[0] = PN532_COMMAND_INDATAEXCHANGE;   /* Data Exchange Header */
  abtCommand[1] = 1;                              /* Max card numbers */
  abtCommand[2] = (uiKeyType) ? PN532_MIFARE_CMD_AUTH_A : PN532_MIFARE_CMD_AUTH_B;
  abtCommand[3] = uiBlockNumber;                  /* Block Number (1K = 0..63, 4K = 0..255 */
  memcpy (abtCommand+4, pbtKeys, 6);
  uint8_t i;
  for (i = 0; i < szCUIDLen; i++)
  {
    abtCommand[10+i] = pbtCUID[i];                /* 4 byte card ID */
  }
  
  /* Send the command */
  error = pn532Write(abtCommand, 10+szCUIDLen);
  if (error)
  {
    /* Problem with the serial bus, etc. */
    #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Authentification failed%s", CFG_PRINTF_NEWLINE);
    #endif
    return error;
  }

  /* Read the authentication response */
  memset(abtResponse, 0, PN532_RESPONSELEN_INDATAEXCHANGE);
  do
  {
    systickDelay(25);
    error = pn532Read(abtResponse, &szLen);
  }
  while (error == PN532_ERROR_RESPONSEBUFFEREMPTY);
  if (error)
  {
    #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Authentification failed%s", CFG_PRINTF_NEWLINE);
    #endif
    return error;
  }

  // ToDo: How to check if authentification really worked (bad key, etc.)?

  /* Output the authentification data */
  #ifdef PN532_DEBUGMODE
    PN532_DEBUG("Authenticated block %d %s", uiBlockNumber, CFG_PRINTF_NEWLINE);
  #endif

  // Return OK signal
  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    Tries to read an entire 16-byte data block at the specified block
    address.

    @param  uiBlockNumber The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  pbtData       Pointer to the byte array that will hold the
                          retrieved data (if any)

    @note   Possible error messages are:

            - PN532_ERROR_BLOCKREADFAILED
*/
/**************************************************************************/
pn532_error_t pn532_mifareclassic_ReadDataBlock (uint8_t uiBlockNumber, byte_t * pbtData)
{
  pn532_error_t error;
  byte_t abtCommand[4];
  byte_t abtResponse[PN532_RESPONSELEN_INDATAEXCHANGE];
  size_t szLen;

  #ifdef PN532_DEBUGMODE
    PN532_DEBUG("Reading 16 bytes at block %03d%s", uiBlockNumber, CFG_PRINTF_NEWLINE);
  #endif

  /* Prepare the command */
  abtCommand[0] = PN532_COMMAND_INDATAEXCHANGE;
  abtCommand[1] = 1;                            /* Card number */
  abtCommand[2] = PN532_MIFARE_CMD_READ;        /* Mifare Read command = 0x30 */
  abtCommand[3] = uiBlockNumber;                /* Block Number (0..63 for 1K, 0..255 for 4K) */
  
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
    /* Copy the 16 data bytes to the output buffer        */
    /* Block content starts at byte 9 of a valid response */
    memcpy (pbtData, abtResponse+8, 16);
  }
  else
  {
    #ifdef PN532_DEBUGMODE
      PN532_DEBUG("Unexpected response reading block %d.  Bad key?%s", uiBlockNumber, CFG_PRINTF_NEWLINE);
    #endif
    return PN532_ERROR_BLOCKREADFAILED;
  }

  /* Display data for debug if requested */
  #ifdef PN532_DEBUGMODE
    PN532_DEBUG("Block %03d: ", uiBlockNumber, CFG_PRINTF_NEWLINE);
    pn532PrintHexVerbose(pbtData, 16);
  #endif

  // Return OK signal
  return PN532_ERROR_NONE;
}
