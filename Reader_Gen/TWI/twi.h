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
 *  \brief Header file for twi.c.
 */
#ifndef _TWI_H_
#define _TWI_H_

//#if (NODETYPE == ENDDEVICE)

#include "config.h"
#include "rum_types.h"


#ifdef TWI

#define puchar u8 *


#define QT_CHANGE_PORT  PORTE//PORTB
//#define READ_ISTAT   DDRE //DDRB
#define READ_ISTAT  PINE

//#define QT_CHANGE_PPIN  PINE //PINB
#define ISTAT_PIN   0 //5


#define ADDR_SELECTED 1

#if ADDR_SELECTED == 1
//#define TWI_R 0xD5
//#define TWI_W 0xD4
#define TWI_R 0x39
#define TWI_W 0x38
#else
//#define TWI_R 0x51
//#define TWI_W 0x50
#endif

/** \brief Defines number of attempts to send command. 
 */
#define TWI_LL_START_TRIES 10

/** \brief Number of times to try to get ACK on a write. 
 */
#define TWI_LL_ACK_TRIES    1//8

/** \brief Bitwise structure for the TWI-like Port.
 *
 * Define the pin positions for the TWI I/O port in this structure.
 * This structure assumes that the clock and data pins are in the same I/O port.
 * This definition has the clock pin on the second port bit and the data pin on the
 * third port bit.  You may move these around to reflect your actual hardware
 * implementation.  
 */
typedef struct
{
/*
    uchar unused_pin:6; ///< unused pins
    uchar clock_pin:1;  ///< Clock pin on port pin 1
    uchar data_pin:1; ///< data  pin on port pin 2
*/


//Reader_GEN
   
    u8 clock_pin:1;  ///< Clock pin on port pin 1
    u8 data_pin	:1;   ///< data  pin on port pin 2
	u8			:6;
//	u8 unused_pin:1; ///< unused pins


} TWI_PINS ;

/** \brief Hardware registers for PORT, DDR and PIN addresses.
 *
 * You <b> must </b> instantiate one of these in your 'c' files.
 * This structure is set up such that the input register is followed by the
 * data direction register, followed by the output register.
 * It assumes that they are in this order in the hardware addresses for the
 * port chosen.  If you have a different order for these register addresses, 
 * you will want to modify this structure accordingly.
 *
 * \remarks Example:  \n
 * <tt> TWI_PORT *pTWI = (TWI_PORT *)0x33 ; </tt> \n
 * This definition assigns a pointer to this hardware structure at address 0x33.
 * With the structure below, the pin input register is at address 0x33, the port
 * data direction register is at address 0x34, and the port output register is at
 * address 0x35.  You will want to make these match your processor's I/O port
 * addresses.
 */
typedef struct
{
    TWI_PINS pin_reg,        ///< pin input register
             dir_reg,        ///< data direction register
             port_reg;       ///< port output register
} TWI_PORT;

void  Twi_ack_nack ( u8 ucAck ) ;
u8 Twi_write ( u8 ucData ) ;
u8 Twi_read ( void ) ;
void  Twi_clock_high ( void );
void  Twi_clock_low ( void );
void  Twi_clock_cycle ( void );
void  Twi_clock_cycles ( u8 ucCount );
void  Twi_data_high ( void );
void  Twi_data_low ( void );
u8 Twi_data ( void );
void  Twi_start ( void );
void  Twi_stop ( void );
u8 Twi_command_send ( puchar pucInsBuf, u8 numBytes );
u8 Twi_receive_data ( puchar pucRecBuf, u8 ucLen );
u8 Twi_send_and_receive ( puchar pucCommandBuffer, u8 ucCommandSize, puchar pucResultBuffer, \
                       u8 ucResultSize );
void  Twi_delay ( u8 ucDelay );
u8 Twi_send_only ( u8 ucData );

void Twi_Init(void);

#endif // TWI

//#endif // (NODETYPE == ENDDEVICE)

#endif // _TWI_H
