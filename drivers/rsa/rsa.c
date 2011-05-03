/**************************************************************************/
/*! 
    @file     rsa.c
    @author   Kyle Loudon
              modified: microBuilder.eu
    @date     4 January, 2010
    @version  1.0

    Basic RSA-encryption using 64-bit math (32-bit keys).

    Based on the examples from "Mastering Algorithms with C" by
    Kyle Loudon (O'Reilly, 1999).

    For details on how to generate a valid RSA key pair, see:
    http://www.microbuilder.eu/Tutorials/SoftwareDevelopment/RSAEncryption.aspx

    @warning  Most versions of libc used for embedded systems do not
              include support for 64-bit integers with printf, etc. (to
              keep the compiled code size and memory usage as small as
              possible).  Unless you have explicitly added long long
              support for printf, you should not try to display 64-bit
              values with it (%lld, etc.).  Using 32-bit values (changing
              the definition of huge_t to uint32_t) will avoid this issue
              entirely, though at the expense of weaker encryption.
*/
/**************************************************************************/

#include "rsa.h"

huge_t modexp(huge_t a, huge_t b, huge_t n) 
{
  huge_t y;
  y = 1;

  /*  Compute pow(a, b) % n using the binary square and multiply method. */
  while (b != 0) 
  {
    /*  For each 1 in b, accumulate y. */
    if (b & 1)
    {
      y = (y * a) % n;
    }
    
    /* Square a for each bit in b. */
    a = (a * a) % n;
    
    /*  Prepare for the next bit in b. */
    b = b >> 1;
  }

  return y;
}

void rsaTest()
{
  huge_t      rsaOrig, rsaDecrypted, rsaEncrypted;
  rsaPubKey_t publicKey;
  rsaPriKey_t privateKey;
  int         i;

  printf("Encrypting with RSA %s", CFG_PRINTF_NEWLINE);

  #if CFG_RSA_BITS == 64
  // Values based on 64-bit math (huge_t = uint64_t)
  // which will result in more secure encryption, but also
  // increases the size of the encrypted text
  publicKey.e = 21;
  publicKey.n = 16484947;
  privateKey.d = 15689981;
  privateKey.n = 16484947;
  #endif

  #if CFG_RSA_BITS == 32
  // Alternative values with 32-bit math (huge_t = uint32_t)
  // or when smaller encrypted text is desired
  publicKey.e = 17;
  publicKey.n = 209;
  privateKey.d = 53;
  privateKey.n = 209;
  #endif
  
  #if CFG_RSA_BITS == 64
  printf("Starting RSA encryption/decryption test %s", CFG_PRINTF_NEWLINE);
  #endif
  #if CFG_RSA_BITS == 32
  printf("d=%u, e=%u, n=%u %s", (unsigned int)privateKey.d, (unsigned int)publicKey.e, (unsigned int)publicKey.n, CFG_PRINTF_NEWLINE);  
  #endif

  for (i = 0; i < 128; i++) 
  {  
    rsaOrig = i;
    rsaEncrypt(rsaOrig, &rsaEncrypted, publicKey);
    rsaDecrypt(rsaEncrypted, &rsaDecrypted, privateKey);
  
    if (rsaOrig == rsaDecrypted)
    {
      #if CFG_RSA_BITS == 64
      printf("Encrypted and decrypted %d %s", i, CFG_PRINTF_NEWLINE);
      #endif
      #if CFG_RSA_BITS == 32
      printf("In=%5u, Encrypted=%5u, Out=%5u (OK) %s", (unsigned int)rsaOrig, (unsigned int)rsaEncrypted, (unsigned int)rsaDecrypted, CFG_PRINTF_NEWLINE);
      #endif
    }
    else
    {
      #if CFG_RSA_BITS == 64
      printf("Failed to decrypt %d %s", i, CFG_PRINTF_NEWLINE);
      #endif
      #if CFG_RSA_BITS == 32
      printf("In=%5u, Encrypted=%5u, Out=%5u (ERROR) %s", (unsigned int)rsaOrig, (unsigned int)rsaEncrypted, (unsigned int)rsaDecrypted, CFG_PRINTF_NEWLINE);
      #endif
    }
  }
}

void rsaEncrypt(huge_t plaintext, huge_t *ciphertext, rsaPubKey_t pubkey) 
{
  *ciphertext = modexp(plaintext, pubkey.e, pubkey.n);

  return;
}

void rsaDecrypt(huge_t ciphertext, huge_t *plaintext, rsaPriKey_t prikey) 
{
  *plaintext = modexp(ciphertext, prikey.d, prikey.n);

  return;
}
