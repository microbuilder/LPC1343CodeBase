/**************************************************************************/
/*! 
    @file     commands.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Common helper-functions for all commands in the 'core/cmd'
              command-line interpretter.

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2010, microBuilder SARL
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

#include <stdio.h>
#include <string.h>   // memset
#include <ctype.h>    // isdigit, isspace, etc.

#include "core/cmd/cmd.h"
#include "commands.h"

/**************************************************************************/
/*!
    @brief  Attempts to convert the supplied decimal or hexadecimal
          string to the matching 32-bit value.  All hexadecimal values
          must be preceded by either '0x' or '0X' to be properly parsed.

    @param[in]  s
                Input string
    @param[out] result
                Signed 32-bit integer to hold the conversion results

    @section Example

    @code
    char *hex = "0xABCD";
    char *dec = "1234";

    // Convert supplied values to integers
    int32_t hexValue, decValue;
    getNumber (hex, &hexValue);
    getNumber (dec, &decValue);

    @endcode
*/
/**************************************************************************/
int getNumber (char *s, int32_t *result)
{
  int32_t value;
  uint32_t mustBeHex = FALSE;
  uint32_t sgn = 1;
  const unsigned char hexToDec [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255, 255, 255, 255, 255, 255, 255, 10, 11, 12, 13, 14, 15};

  if (!s)
    return 0;

  // Check if this is a hexadecimal value
  if ((strlen (s) > 2) && (!strncmp (s, "0x", 2) || !strncmp (s, "0X", 2)))
  {
    mustBeHex = TRUE;
    s += 2;
  }

  // Check for negative sign
  if (!mustBeHex && *s && (*s == '-'))
  {
    sgn = -1;
    s++;
  }

  // Try to convert value
  for (value = 0; *s; s++)
  {
    if (mustBeHex && isxdigit ((uint8_t)*s))
      value = (value << 4) | hexToDec [toupper((uint8_t)*s) - '0'];
    else if (isdigit ((uint8_t)*s))
      value = (value * 10) + ((uint8_t)*s - '0');
    else
    {
      printf ("Malformed number. Must be decimal number, or hex value preceeded by '0x'%s", CFG_PRINTF_NEWLINE);
      return 0;
    }
  }

  // Set number to negative value if required
  if (!mustBeHex)
    value *= sgn;

  *result = value;

  return 1;
}



