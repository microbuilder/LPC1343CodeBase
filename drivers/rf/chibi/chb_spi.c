/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/
/*!
    \file 
    \ingroup


*/
/**************************************************************************/
#include "chb.h"
#include "chb_spi.h"
#include "core/ssp/ssp.h"

/**************************************************************************/
/*!

*/
/**************************************************************************/
void chb_spi_init()
{
    // initialise spi, high between frames and transition on trailing edge
    sspInit(0, sspClockPolarity_High, sspClockPhase_FallingEdge);

    // set the slave select to idle
    CHB_SPI_DISABLE();
}

/**************************************************************************/
/*!
    This function both reads and writes data. For write operations, include data
    to be written as argument. For read ops, use dummy data as arg. Returned
    data is read byte val.
*/
/**************************************************************************/
U8 chb_xfer_byte(U8 data)
{
    /* Move on only if NOT busy and TX FIFO not full */
    while ((SSP_SSP0SR & (SSP_SSP0SR_TNF_MASK | SSP_SSP0SR_BSY_MASK)) != SSP_SSP0SR_TNF_NOTFULL);
    SSP_SSP0DR = data;
  
    /* Wait until the busy bit is cleared and receive buffer is not empty */
    while ((SSP_SSP0SR & (SSP_SSP0SR_BSY_MASK | SSP_SSP0SR_RNE_MASK)) != SSP_SSP0SR_RNE_NOTEMPTY);

    // Read the queue
    return SSP_SSP0DR;
}
