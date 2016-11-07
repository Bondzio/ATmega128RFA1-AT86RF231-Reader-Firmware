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
 *  \brief Bowline TWI Functions.
 */
#ifndef COORDNODE //#if (NODETYPE == ENDDEVICE)

#ifdef TWI

#include "timer_enddevice.h"
#include <twi/twi.h>
//#include "bowline.h"
#include <util/delay.h>

#ifdef DEBUG
#include "serial.h"
#endif
/** \brief PORT for Bowline TWI.
 */
//TWI_PORT *pTWI = (TWI_PORT *) 0x20;///< TWI port is Port A
//TWI_PORT *pTWI = (TWI_PORT *) 0x23;///< TWI port is Port B
TWI_PORT *pTWI = (TWI_PORT *) 0x29;///< TWI port is Port D

/** \brief Delay loop for the bit-banged TWI-like interface.
 *
 *  We use this loop to generate an approximate 1 us delay.
 *  A hardware timer might be a better way to implement this, but
 *  to save resources, this example uses a software timer provided
 *  by avr-libc.
 *  The avr-libc _delay_us(double __us) function will link in the 
 *  floating point library if you pass a variable to this function.
 *  That is why we pass a fixed value (1) and use a loop here.
 */
void Twi_delay ( u8 ucDelay )
{/*
    u8 i;
    for ( ; ucDelay != 0; ucDelay-- )
    {
        for (i = 0; i < 1; i++)
        {
        }
    }
	*/
	for ( ;ucDelay > 0; --ucDelay)
	{
		_delay_us(1);
	}

}

void Twi_Init(void)
{
	/* pTWI->dir_reg.clock_pin = 1;///0;
	//Twi_delay (1);///
    pTWI->port_reg.clock_pin = 1;
	 // Twi_delay (1);
    pTWI->dir_reg.data_pin = 1;
    pTWI->port_reg.data_pin = 1;
   // Twi_delay (2);*/
	Twi_clock_high();
	Twi_data_high();
}

/** \brief Sets the clock pin to a high level by floating the port output.
 *
 *  See file twi.h for port structure definitions.
 *  Uses the internal port pull-up to provide the TWI clock pull-up.
 */
void Twi_clock_high ( void )
{
    ///Twi_delay (1);
    pTWI->dir_reg.clock_pin = 0;
	asm("nop");	
    pTWI->port_reg.clock_pin = 1;
    ///Twi_delay (1);
	Timer_delay_us(2);
}


/** \brief Sets the clock pin to a low level. 
 *
 *  See file twi.h for port structure definitions.
 */
void Twi_clock_low ( void )
{
    //_delay_us(1);//Twi_delay (1);
    pTWI->port_reg.clock_pin = 0;
    ///_delay_us(2);//Twi_delay (2);
    asm("nop");
	pTWI->dir_reg.clock_pin = 1;
	Timer_delay_us(2);///
	
}


/** \brief Cycles the clock pin low then high. 
 */
void Twi_clock_cycle ( void )
{
    Twi_clock_low ();
    Twi_clock_high ();
}


/** \brief Calls Twi_clock_cycle() ucCount times. 
 *
 *  \param ucCount is the number of loop to call Twi_clock_cycle 
*/
void Twi_clock_cycles ( u8 ucCount )
{
    for ( ; ucCount > 0; ucCount-- )
    {
        Twi_clock_cycle ();
    }
}


/** \brief Sets the data pin to a high level by floating the port output.
 *
 *  See file twi.h for port structure definitions.
 *  Uses the internal port pull-up to provide the TWI data pull-up.
 */
void Twi_data_high ( void )
{
    ///_delay_us(1);//Twi_delay (1);
    pTWI->dir_reg.data_pin = 0;
    asm("nop");
	pTWI->port_reg.data_pin = 1;
    Timer_delay_us(2);//Twi_delay (2);
}


/** \brief Sets the data pin to a low level. 
 *
 *  See file twi.h for port structure definitions.
 */
void Twi_data_low ( void )
{
    ///_delay_us(1);//Twi_delay (1);
    pTWI->port_reg.data_pin = 0;
    ///_delay_us(2);//Twi_delay (2);
    asm("nop");
	pTWI->dir_reg.data_pin = 1;
	Timer_delay_us(2);///
}


/** \brief Reads and returns the data pin value.  Leaves the pin high-impedance.
*/
u8 Twi_data ( void )
{
    ///Twi_delay (1);
    pTWI->dir_reg.data_pin  = 0;
    asm("nop");
	pTWI->port_reg.data_pin = 1;
    Timer_delay_us(2);//Twi_delay (4);
    return pTWI->pin_reg.data_pin;
}


/** \brief Sends a TWI start sequence.
*/
void Twi_start ( void )
{
	if (pTWI->pin_reg.clock_pin && pTWI->pin_reg.data_pin)
	{
		Twi_data_low ();
		Twi_delay (4);
		Twi_clock_low ();
		Twi_delay (4);
	}
	else
	{	
		Twi_clock_low ();
		Twi_data_high ();
		Twi_delay (4);
		Twi_clock_high ();
		Twi_delay (4);
		Twi_data_low ();
		Twi_delay (4);
		Twi_clock_low ();
		Twi_delay (4);
	}
}


/** \brief  Sends a TWI stop sequence.
*/
void Twi_stop ( void )
{
    Twi_clock_low ();
    Twi_data_low ();
    Twi_clock_high ();
    Twi_delay (4);
    Twi_data_high ();
    Twi_delay (4);
}


/** \brief Sends an ACK or NACK to the device (after a read). 
 * 
 *  \param ucAck - if TRUE means an ACK will be sent, otherwise
 *  NACK will be sent.
 */
void Twi_ack_nack ( u8 ucAck )
{
    Twi_clock_low ();
    if (ucAck) 
    {
        Twi_data_low (); //Low data line indicates an ACK
    }
    else
    {    
        Twi_data_high (); // High data line indicates an NACK
    }
    Twi_clock_high ();
    Twi_clock_low ();
}


/** \brief  Write a byte on the TWI-like bus.
*
*  \param ucData is data to be sent.
* 
* \return 0 if write success, else 'i' (the number of attempts
*  to get ACK that failed). The maximum value of i is TWI_LL_ACK_TRIES.
*/
u8 Twi_write(u8 ucData)
{
    static u8 i,x=0;

    for(i = 0; i < 8; i++) // Send 8 bits of data
    {                 
        Twi_clock_low ();
        if (ucData & 0x80) 
        {
            Twi_data_high ();
        }
        else   
        {
            Twi_data_low ();
        }
        Twi_clock_high ();
        ucData = ucData << 1;
    }
    Twi_clock_low ();

    // wait for the ack
    Twi_data_high (); // Set data line to be an input

    //Twi_delay (8);
	Timer_delay_us(8);///
    Twi_clock_high ();
	Timer_delay_us(2);	
    for ( i = 0; i < TWI_LL_ACK_TRIES; i++ )
    {   if ( Twi_data () == 0 )
        {
			break ;
        }
    }
    Twi_clock_low ();
	Twi_data_high ();
	while(x==0)
	{
		x=(pTWI->pin_reg.data_pin);
	}

    return i;
}


/** \brief Read a byte from device starting from MSB.
 */
u8 Twi_read ( void )
{
    u8 i;
    u8 ucReadByte = 0;
    Twi_data_high ();
	Timer_delay_us(4);
   for(i = 0; i < 8; i++ )/// for(i = 0x80; i; i = i >> 1)
    {
		ucReadByte <<= 1;////
		
        ///Twi_clock_cycle ();
		Twi_clock_high();
		//_delay_us(2);
        if (Twi_data ())
        { 
            ucReadByte |= 1; ///i;
        }
        Twi_clock_low ();
		//_delay_us(4);
    }
    return ucReadByte;
}


/** \brief Send a command over the bus.
 * 
 * \param pucInsBuf is pointer to a buffer containing command sent.
 * \param ucNumBytes is the number of command sent.
 *
 * \return SUCCESS or FAIL_CMDSEND (if Twi_write() fails).
 */
u8 Twi_command_send ( puchar pucInsBuf, u8 ucNumBytes )
{
    static u8 i;

    i = TWI_LL_START_TRIES;
    while (i) 
    {
        Twi_start ();
        if (Twi_write ( TWI_W ) == 0) 
        {
            break;    //send sla_w address
        }
        if (--i == 0) 
        {
            return FAIL_COMMAND_SEND;
        }
    }

    for(i = 0; i < ucNumBytes; i++) 
    {
        if (Twi_write ( pucInsBuf[i] ) != 0) 
        {
            return FAIL_COMMAND_SEND;
        }
    }

    return SUCCESS;
}


/** \brief Receive data over the bus.
 * 
 * \param pucRecBuf is a pointer to a buffer to contain data received.
 * \param ucLen is the number of bytes received.
 *
 * \return SUCCESS or FAIL_COMMAND_SEND.
 */
u8 Twi_receive_data ( puchar pucRecBuf, u8 ucLen )
{
	u8 i = 0; ///
	
    Twi_start ();
    if (Twi_write ( TWI_R ) != 0)     //send sla_r address
    {
		Twi_stop ();///
        return FAIL_COMMAND_SEND;
    }
    while ( /*((READ_ISTAT & (1 << ISTAT_PIN)) != 0) && */(i++ < ucLen) )
    {
        *pucRecBuf = Twi_read ();
        pucRecBuf =pucRecBuf+ i;///pucRecBuf++;
        if (i != ucLen)///((READ_ISTAT & (1 << ISTAT_PIN)) != 0)
        { 
            Twi_ack_nack ( TRUE );
        }
        else
        {
            Twi_ack_nack ( FALSE );
        }
		Timer_delay_us(50);///
    }
    Twi_stop ();
    return SUCCESS;
}


/** \brief Send Bowline command through TWI and receive response.
 * 
 * \param pucCommandBuffer is a pointer to command buffer.
 * \param ucCommandSize is the command size.
 * \param pucResultBuffer is a pointer to a received data buffer.
 * \param ucResultSize is the received data size.
 *
 * \return SUCCESS or FAIL_COMMAND_SEND.
 */
u8 Twi_send_and_receive ( puchar pucCommandBuffer, u8 ucCommandSize, \
                       puchar pucResultBuffer, u8 ucResultSize )
{
    u8 ucReturn;
    //send command
    if ((ucReturn = Twi_command_send( pucCommandBuffer, ucCommandSize )) != SUCCESS) 
    {
        return ucReturn;
    }
    Twi_stop ();
    //wait Istat high
    Timer2_set ( 0x00, 0x07, 250 );
    while ( (READ_ISTAT & (1 << ISTAT_PIN)) == 0 )
    {
        if (TIMER2_OCA_TEST != 0)
        {
            Timer2_stop ();
            Twi_stop();
            return FAIL_COMMAND_SEND;
        }
    }
	Timer2_stop ();
    //receive response
    if ((ucReturn = Twi_receive_data( pucResultBuffer, ucResultSize )) != SUCCESS) 
    {
        return ucReturn;
    }
    
    return SUCCESS;
}


/** \brief Send Bowline command through TWI.
 * 
 * \param ucData is data to be sent.
 *
 * \return SUCCESS or FAIL_COMMAND_SEND.
 */
u8 Twi_send_only ( u8 ucData )
{
    u8 ucReturn;
    //send command
    if ((ucReturn = Twi_command_send( &ucData, 1 )) != SUCCESS) 
    {
        return ucReturn;
    }
    return SUCCESS;
}

#endif // TWI

#endif // (NODETYPE == ENDDEVICE)
