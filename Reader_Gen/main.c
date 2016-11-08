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
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
/*
$Id: main.c,v 1.17 2009/01/31 00:37:59 bleverett Exp $
*/


/*
Debug Compile switches

- DEBUG_CRF
- DEBUG_BOW
- DEBUG_RADIO
- DEBUG_UART
- MACHINE_CONNECTED
- DEBUG


- VIRGIN BOARD ADDRESS (0x1111111111111111)

*/
#include <stdlib.h>
#include <stdio.h>
#include "stdboolrum.h"
#include <avr/interrupt.h>
#include <string.h>
#include "radio.h"
#include "mac.h"
#include "mac_event.h"
#include "timer.h"
#include "serial.h"
#include "mac_start.h"
#include "mac_data.h"
#include "mac_associate.h"
#include "system.h"
#include <avr/wdt.h>
#include "application.h"

// mifare includes
#include "RegCtrl.h"
#include "reg52.h"
#include "Mifare.h"
#include "Rc522Reg.h"
#include "OpCtrl.h"
#include "ErrCode.h"
#include "spi.h"

// Make the compiler accept these unused function names
#define usb_task_init()
#define cdc_task_init()
#define usb_task()
#define cdc_task()

#include "machine.h"
///bool OP;

/**
@brief Main function of the program.
*/

/*Initialize the processor and RC522*/
void mifareInit(void)
{
	Spi_initialize();
	Rc522Init();
}

int main(void){

	
	// SINGLE-CHIP SOLUTION
	cli(); //make sure interrupts are disabled before initialization.
	wdt_disable();

	// init serial for PinPointSystem LCD output
	serial_init(NULL);
	
	#ifdef DEBUG_UART
	debugMsgStrConst("\r\nUART TEST\r\n ");
	#endif

	// Init the timer system for the MAC
	timerInit();

	#if (DEVICE_CONNECTED == MDC_MACHINE)
	InitTimerZero();	//timer for MDC machine comm.
	#endif

	sei();
	
	// Init the (sample) application
	appInit();

	#if( DEBUG )
	Beeps(2);
	#endif
	
	// Initialize mifare
	mifareInit();

	// Main forever loop for the application.
	for(;;)
	{
		// Turn interrupts on if they are off.  They should always be on
		// at this point in the loop, so just turn them on every time, in
		// case interrupts were inadvertently turned off elsewhere.
		sei();

		// Task functions called from main loop.  Either add your own task loop
		// or edit the example appTask().			
		//appTask();	
		APP_TaskHandler();	
		macTask();

	}
	return 0;
}
