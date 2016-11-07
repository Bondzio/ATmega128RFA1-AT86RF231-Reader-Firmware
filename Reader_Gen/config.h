/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      This file contains the system configuration definition.
 *
 * \par Application note:
 *      AVR280: USB Host CDC Demonstration
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Name: coin_op $
 * $Revision: 1.2 $
 * $RCSfile: config.h,v $
 * $Date: 2009/01/30 23:29:02 $  \n
 * $Id: config.h,v 1.2 2009/01/30 23:29:02 bleverett Exp $
 ******************************************************************************/
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

/**
   @addtogroup usb
   @{
*/

//_____ I N C L U D E S ____________________________________________________


#include "compiler.h" //!< Compiler definitions

#ifdef AVRGCC
   #include <avr/io.h>
#else
   #include "lib_mcu/mcu.h" //!< Register declaration
#endif

//! CPU core frequency in kHz
#define FOSC 16000

#ifndef SUCCESS
#define SUCCESS                         (0)
#endif
// -------- END Generic Configuration -------------------------------------

// UART Sample configuration, if we have one ... __________________________

#ifndef AVRGCC
   #define uart_usb_putchar putchar
#endif

#define NB_MS_BEFORE_FLUSH			50

// ADC Sample configuration, if we have one ... ___________________________

#ifndef COORDNODE
/** \brief Fail to send a command error code definition.
 */
#ifndef FAIL_COMMAND_SEND
#define FAIL_COMMAND_SEND 0xA0
#endif

/** \brief General fail error code definition.
 */
#ifndef FAILED
#define FAILED 0xA1
#endif

/** \brief Unknown command error code definition.
 */
#ifndef UNKNOWN_COMMAND
#define UNKNOWN_COMMAND 0xA2
#endif

/** \brief command not supported error code definition.
 */
#ifndef COMMAND_NOT_SUPPORTED
#define COMMAND_NOT_SUPPORTED 0xA3
#endif

#ifndef READ_TOO_MUCH_DATA
#define READ_TOO_MUCH_DATA 0xA4
#endif

/** \brief Timeout value of TX data command.
 *  Timeout value = TX_DATA_TIMEOUT * 151 us
 *  Timeout value must be less than timeout value of 
 *  Timer2 used in Twi_send_and_receive.
 */
#define TX_DATA_TIMEOUT      0xA0

#else

/** \brief Timeout value when waiting for result.
 *  Timeout value = MAX_TIMEOUT_OVERFLOW * 32.7 ms
 *  Timeout value must be longer than the time required 
 *  by each command to be processed and return result
 */
#define MAX_TIMEOUT_OVERFLOW 10

#define WAIT_TIME_OUT        0xB4
#define CAN_NOT_SEND_CMD     0xB5

#endif
/** @}  */

#endif // _CONFIG_H_

