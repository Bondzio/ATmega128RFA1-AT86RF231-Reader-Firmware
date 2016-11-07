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
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, data, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
/** \file
 *  \brief Timer 2 functions used for SPI and TWI timeout.
 */

#include "mac.h"

#ifndef COORDNODE
//#if (NODETYPE == ENDDEVICE)

#include "timer_enddevice.h"

//#ifdef CMC
#include <avr/wdt.h>
//#endif


/** \brief Function to start Timer 2.
 *
 * \param ucTimerFlagsA is setting for TCCR2A register.
 * \param ucTimerFlagsB is setting for TCCR2B register.
 * \param ucTimerCmp is the OCR2A output compare value.
 */
void Timer2_set ( uchar ucTimerFlagsA, uchar ucTimerFlagsB, \
                uchar ucTimerCmp )
{
    TCCR2A = ucTimerFlagsA;
    TCCR2B = ucTimerFlagsB;
    OCR2A = ucTimerCmp;
    TIMER2_RESET;
    TIFR2 |= (1 << OCF2A); // clear flag for AT90USB647
} 


/** \brief Function to stop Timer 2.
 */
void Timer2_stop ( void )
{
    TCCR2A = 0;
    TCCR2B = 0;
    OCR2A = 255;
    TIMER2_RESET;
    TIFR2 |= (1 << OCF2A); // clear flag for AT90USB647
}


//#ifdef CMC

volatile uchar GenericTimerFlag = 0;        //Global flag 
volatile uchar WDTFlag = 0;                 //Global flag for WatchDog Timer

/** \brief GenericTimer Initialization of for General Use.
 */
void GenericTimer_Init ( void )
{
	TIMSK3 &= ~0x2F;     		//Disable TC3 interrupts
	TCCR3A = 0x00;				//Normal mode               	
	TCCR3B = 0x02; 				//prescale the timer to be clock source / 8 to make it
                                //exactly 1 us for every timer step
	TCCR3C = 0x00;				//Normal mode               	

	TIFR3 |= 0x2F;				//clear interrupt flags for TC3
	TIMSK3 |= 0x01;        		//set Timer/Counter3 Overflow Interrupt Enable                                 
}

/** \brief Shutdown GenericTimer.
 */
void GenericTimer_Stop ( void )
{
	TIMSK3 &= ~0x2F;     		//Disable TC3 interrupts

	TCCR3A = 0x00;               	
	TCCR3B = 0x00; 				//No clock source / Timer Stopped
	TCCR3C = 0x00;               	

	TIFR3 |= 0x2F;				//clear interrupt flags for TC3
}

/** \brief GenericTimer Overflow Interrupt Routine.
 */
ISR ( TIMER3_OVF_vect )
{   
    GenericTimerFlag = 2;           //Set TC3 Flag = 2 (overflow)
}

/** \brief Delay us function using timer.
 */
void Timer_delay_us ( u16 uiDelay )
{
    TCNT3 = 0xFFFF - (2*uiDelay);
    GenericTimer_Init ();
    GenericTimerFlag = 0;           // Clear Timer Flag
    while ( !GenericTimerFlag );       // Wait for overflow
    GenericTimer_Stop ();
}

/** \brief Delay ms function using timer.
 */
void Timer_delay_ms ( u16 uiDelay )
{
    u16 i;

    for(i = 0; i < uiDelay; i++)
    {
        Timer_delay_us ( 1000 );
    }
}

/** \brief Watchdog Timer Interrupt Service Routine.
 */
ISR ( WDT_vect )
{    
    WDTFlag = 1;
}

/** \brief Turn off Watchdog timer.
 */
void WDT_Off ( void )
{
    wdt_reset();
    wdt_disable();    
}

/** \brief Start Watchdog Timer.
 */
void WDT_Start ( void )
{
    WDTFlag = 0;        
    wdt_intr_enable(WDTO_1S);        //Set Watchdog Timer to 1 second    
}

//#endif // CMC

#endif // (NODETYPE == ENDDEVICE)
