/* Copyright (c) 2008  ATMEL Corporation
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
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, data, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
/*    
  $Id: sleep.c,v 1.6 2009/01/16 01:40:04 bleverett Exp $

  $Log
*/
#include "mac.h"
#include "system.h"
#include "sleep.h"
#include "radio.h"
#include "serial.h"
#include <avr/sleep.h>

#if defined(DOXYGEN)
#redefine SLEEP 1
#endif

#if ((NODETYPE == ENDDEVICE) || defined(DOXYGEN)) && (SLEEP == 1)

/**
   @defgroup sleep Sleep functions

   Functions to support the sleeping of the node.  This code only runs
   on the end nodes.

*/


/// @todo Move this declaration to twi.h, when it exists.
void twiInit(void);

/**
   @brief Put the node to sleep.  This function must put all the parts of the
   hardware to sleep, and wake them up.

   @todo Make the sleep time maximum each time it sleeps, to minimize
   the number of wakeups.

   @ingroup sleep
*/
void nodeSleep(u16 seconds)
{
    // Check to see if we really should sleep
    if (!macConfig.sleeping)
        // Just return, rather than sleeping
        return;

    // ************** Power down the other board peripherals
    LED_OFF(1);

    // ************** Power down the radio
    // wait for radio to be done
    u8 state = BUSY_TX_ARET;
    while (state == BUSY_TX_ARET ||
           state == BUSY_RX_AACK)
        state = radioGetTrxState();

    // Now put radio to sleep
    radioEnterSleepMode();

    // ************** Power down the on-chip modules

    // Disable ADC
    ADCSRA &= 1 << ADEN;
    // Turn off comparator
    ACSR |= (1 << ACD);

    PRR0 = 0xaf;  // shutdown all but timer2
    PRR1 = 0xff;  // shutdown all of these peripherals

    // Turn of BOD
    AVR_ENTER_CRITICAL_REGION();
#define BODS  6      
#define BODSE 5
    MCUCR  |= (1 << BODSE) | (1<< BODS);
    MCUCR  &= ~(1 << BODSE);
    AVR_LEAVE_CRITICAL_REGION();
   
    // ************** Set the timer to wake up
    // Set TIMER2 Asyncronous Mode.
    ASSR |= (1 << AS2);
    // Set TIMER2 Prescaler to 1024.
    TCCR2B |= ((1 << CS22)|(1 << CS21)|(1 << CS20));
    // Wait for TCNT2 write to finish.
    while(ASSR & (1 << TCR2BUB))
        ;
    // Set TIMER2 output compare register from user.
    OCR2A = 32; // 1 sec.
    // Wait for OCR2 write to finish.
    while(ASSR & (1 << OCR2AUB))
        ;

    // Sleep as many times as needed to sleep for the full time
    while (seconds--)
    {
        // Reset TIMER2 timer counter value.
        TCNT2 = 0;
        // Wait for TCNT2 write to finish before entering sleep.
        while(ASSR & (1 << TCN2UB))
            ;
        // Enable TIMER2 output compare interrupt.
        TIMSK2 |= (1 << OCIE2A);

        // ************** Go to sleep
        AVR_ENTER_CRITICAL_REGION();
        set_sleep_mode(SLEEP_MODE_PWR_SAVE);
        sleep_enable();
        sei();
        sleep_cpu();
        sleep_disable();
        AVR_LEAVE_CRITICAL_REGION();
    }

    // ************** Awake now, wake up everything
    PRR0 = 0;
    PRR1 = 0;

    if (DEBUG)
        serial_init(NULL);

    // Bring SPI port back up (must re-init after sleep)
    hal_spi_init();

    // Wake up radio.
    radioLeaveSleepMode();
}


/**
    @brief TIMER2 Real Time Clock Sleep ISR function.
*/
ISR(TIMER2_COMPA_vect)
{
    // Disable TIMER2 output compare interrupt.
    TIMSK2 &= ~(1 << OCIE2A);
}


#else  // if (NODETYPE != COORD)
// For coord, provide dummy function so that code compiles
void nodeSleep(u16 seconds) {}

#endif // if (NODETYPE != COORD)
