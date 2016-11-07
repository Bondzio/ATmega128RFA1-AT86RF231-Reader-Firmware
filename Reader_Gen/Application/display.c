/******************************************************************************************************************
 *
 * File Name:  display.c
 *             This is file contains the machine display functions.
 *					
 *
 * Author:     Paul Bastien
 * Date:       07/30/11
 *
 ********************************************************************************************************************
 *
 *                          Copyright (C) 2011
 *  All rights reserved. No part of this program or publication
 *  may be reproduced, transmitted, transcribed, stored in a
 *  retrieval system, or translated to any language or computer
 *  language, in any form or by any means, electronic, mechanical,
 *  magnetic, optical, chemical, manual, or otherwise, without prior
 *  written permission from:
 * 		Paul Bastien
 *		
 *	
 *
 ********************************************************************************************************************
 *
 *
 * Revision   r001 original version
 * History:  VER      SCR #    Author         Date       Description
 *           -----    -----    ------         ----       -----------
 *
 ***************************************************************************************************************************
 */ 
#include "mac.h"
#include "application.h"
#include "machine.h"
#include "display.h"
#include "SpeedQueen.h"

#include <util/delay.h>

u8 msgTable[] = {
                        LED_n, LED_E, LED_E, LED_d, 
                        LED_S, LED_E, LED_r, LED_BLANK,
                        LED_C, LED_A, LED_r, LED_d,
                        LED_F, LED_U, LED_L, LED_L,
                        LED_P, LED_U, LED_L, LED_L,
                        LED_Y, LED_E, LED_A, LED_r,
                        LED_n, LED_N, LED_o, LED_n,
                        LED_d, LED_A, LED_t, LED_E,
                        LED_H, LED_o, LED_u, LED_r,
                        LED_n, LED_N, LED_i, LED_n,
                        LED_S, LED_E, LED_t, LED_C,
                        LED_BLANK, LED_n, LED_o, LED_BLANK,
                        LED_u, LED_t, LED_d, LED_c,
                        LED_d, LED_o, LED_n, LED_E,
                        LED_b, LED_u, LED_S, LED_Y,
                        LED_BLANK, LED_r, LED_S, LED_t,
                        LED_n, LED_o, LED_S, LED_u,                 
                        LED_U, LED_C, LED_F, LED_L,
                        LED_BLANK, LED_S, LED_u, LED_BLANK,
                        LED_BLANK, LED_d, LED_c, LED_BLANK,
                        LED_P, LED_A, LED_U, LED_S,
                        LED_P, LED_A, LED_S, LED_S,
                        LED_F, LED_A, LED_I, LED_L,
                        LED_BLANK, LED_S, LED_t, LED_BLANK,
                        LED_BLANK, LED_b, LED_A, LED_L,
                        LED_BLANK, LED_BLANK, LED_BLANK, LED_BLANK

};                      // defined messages

u8 msgNumber[] = {
                           LED_0,
                           LED_1,
                           LED_2,
                           LED_3,
                           LED_4,
                           LED_5,
                           LED_6,
                           LED_7,
                           LED_8,
                           LED_9,
                           LED_A,
                           LED_b,
                           LED_C,
                           LED_d,
                           LED_E,
                           LED_F,
                           0        // NUM_BLANK = 0x10
};


#if (DEVICE_CONNECTED == MDC_MACHINE)
/**********************************************************************************************
void mdcDisplayCommand(u8 led1, u8 led2,u8 led3, u8 led4, u8 duration) 

Parameter:  u8 led1, u8 led2,u8 led3, u8 led4, u8 duration
Description: This function will be used to send text to the MDC 4 position 7 segment display.
Led1 to led4 are limited to values defined in "display.h"
***********************************************************************************************
*/

void displayMsg( u8 msg ) 
{
	if (msg == CARD_ERR_MSG)
	{
		displaySQCardError();
	}
	else if (msg == SETUP_MSG)
	{
		sendSQDisplayCommand(LED_BLANK,LED_S,LED_U,LED_BLANK,5);
	}
	else if (msg == BUSY_MSG)
	{
		sendSQDisplayCommand(LED_b,LED_U,LED_S,LED_Y, 5);
	}
	else if (msg == SERVICE_MSG)
	{
		sendSQDisplayCommand(LED_S,LED_E,LED_r,LED_BLANK,5);
	}
	else if (msg == BLANK_MSG)
	{
		sendSQDisplayCommand(LED_BLANK,LED_BLANK,LED_BLANK,LED_BLANK,0);
	}

}

/*****************************************************************
This function will display the current version of reader firmware
on the MDC display for a minimum of 2 seconds.

*/

void displayVersion(void)
{	
	displaySQReaderVersion();
}
#endif
                                                         