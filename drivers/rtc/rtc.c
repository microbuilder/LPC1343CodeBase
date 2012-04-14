/**************************************************************************/
/*! 
    @file     rtc.c
    @author   K. Townsend (microBuilder.eu)
	
    @brief    Generic time and RTC-related helper functions

    @section DESCRIPTION

    Time and RTC-related helper functions.

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
#include "rtc.h"

/**************************************************************************/
/*! 
    @brief  Converts the supplied date and time to seconds since the
            epoch (00h00, 1 Jan 1970).

    @section EXAMPLE

    @code
    
    long epochTime;
    // 19 Jan 2012 at 13:42:27 ... DST unnknown (-1)
    epochTime = rtcToEpochTime(2012, 0, 19, 13, 42, 27, -1);

    // Should return 1326980547 (number of seconds since epoch)

    @endcode
*/
/**************************************************************************/
long rtcToEpochTime(int year, int month, int mday, int hour, int min, int sec, int isdst)
{
  // This is purely a convenience function since it uses mktime beneath the hood
  struct tm t;
  time_t time;

  if (year > 1900) year -= 1900;

  t.tm_year = year;
  t.tm_mon = month;           // Month, 0 - jan
  t.tm_mday = mday;           // Day of the month
  t.tm_hour = hour;     
  t.tm_min = min;
  t.tm_sec = sec;
  t.tm_isdst = isdst;         // Is DST on? 1 = yes, 0 = no, -1 = unknown
  time = mktime(&t);

  // Return seconds since the epoch (00h00, 1 Jan 1970)
  return (long)time;
}

void rtcFunction(void)
{
}
