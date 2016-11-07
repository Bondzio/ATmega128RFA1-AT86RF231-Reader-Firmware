/* Copyright (c) 2009  ATMEL Corporation
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
/** \file
 *  \brief Timer 2 functions: Used to wait for result after sending command.
 */

#ifdef COORDNODE

#include "timer_coordinator.h"

/** \brief Function to start Timer 2.
 *
 * \param ucTimerFlagsA is setting for TCCR2A register.
 * \param ucTimerFlagsB is setting for TCCR2B register.
 */
void Timer2_set ( u8 ucTimerFlagsA, u8 ucTimerFlagsB )
{
    TIMER2_RESET;
    TIFR2 |= (1 << TOV2); // clear flag for AT90USB647
    TIMSK2 |= (1 << TOIE2);
    TIMER2_FLAG_CLEAR;
    TCCR2A = ucTimerFlagsA;
    TCCR2B = ucTimerFlagsB;
} 

/** \brief Function to stop Timer 2.
 */
void Timer2_stop ( void )
{
    TCCR2A = 0;
    TCCR2B = 0;
    TIMER2_RESET;
    TIFR2 |= (1 << TOV2); // clear flag for AT90USB647
    TIMSK2 |= (1 << TOIE2);
    TIMER2_FLAG_CLEAR;
}

/** \brief Interrupt overflow routine of Timer 2.
 */
ISR ( TIMER2_OVF_vect )
{
    TIMER2_FLAG_INCREMENT;
}

#endif // COORDNODE
