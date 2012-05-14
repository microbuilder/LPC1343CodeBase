/**************************************************************************/
/*! 
    @file     main.c
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2011, microBuilder SARL
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
#include "projectconfig.h"

#include "core/gpio/gpio.h"

/**************************************************************************/
/*! 
    As soon as the chip comes out of reset and the startup code has
    finished executing, sets up GPIO pin 2.1 as an ouput and high.

    This can be used to determine the startup delay, by measuring the
    time between reset be deasserted and the GPIO pin going high, minus
    the GPIO overhead of a few clock cycles.

    HW Setup: Set channel one of the oscilliscope to the reset pin, and
              channel two of the oscilliscope to GPIO pin 2.1.

              Set a trigger on the rising edge of the reset pin, and
              measure the delay between the rising edge of reset and the
              rising edge of GPIO 2.1.
*/
/**************************************************************************/
int main(void)
{
  /* Enable AHB clock to the GPIO domain. */
  SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_GPIO);

  /* Set 2.1 to output and high */
  GPIO_GPIO2DIR  |= (1 << 1);    // pin 1 = Output
  GPIO_GPIO2DATA |= (1 << 1);    // pin 1 = High

  while(1)
  {
  }

  return 0;
}
