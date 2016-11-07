/*
 * display.h
 *
 * Created: 7/30/2011 1:08:00 PM
 *  Author: Paul Bastien
 *	Copyright (C) 2011
 */ 

//#include "application.h"

#ifndef DISPLAY_H_
#define DISPLAY_H_


void displayMsg( u8 msg );
void displayVersion(void);

//led defines
#define LED_0     0x7e
#define LED_1     0x0c
#define LED_2     0xb6
#define LED_3     0x9e
#define LED_4     0xcc
#define LED_5     0xda
#define LED_6     0xfa
#define LED_7     0x0e
#define LED_8     0xfe
#define LED_9     0xde
#define LED_A     0xee
#define LED_B     0xfe
#define LED_b     0xf8
#define LED_C     0x72
#define LED_c     0xb0
#define LED_d     0xbc
#define LED_E     0xf2
#define LED_F     0xe2
#define LED_H     0xec 
#define LED_I     0x0c
#define LED_i     0x08
#define LED_L     0x70
#define LED_N     0x88
#define LED_n     0xa8
#define LED_o     0xb8
#define LED_P     0xe6
#define LED_r     0xa0
#define LED_S     0xda
#define LED_t     0xf0
#define LED_U     0x7c
#define LED_u     0x38
#define LED_Y     0xdc
#define LED_BLANK 0x00
#define LED_DASH  0X80 
#define NUM_BLANK 0x10           // index into msgNumber[] for blank


#define VERSION								LED_r, '__APP_MAJOR__',  '__APP_MINOR__', '__APP_REVISION__', 5		//version number to be displayed for 4 sec.
#define NOSETUP								('n', 'o',  'S', 'u', 1)		//No setup displayed for 1 sec.
#define USER_TRANS_FULL						('U', 'C', 'F', 'L', 1)			//user transaction is full. Collection is required
#define DATA_COLLECTION						(BLANK,d,c,BLANK,2)				//data Collection display dc

// message types

#define CARD_ERR_MSG	1
#define SETUP_MSG		2
#define SERVICE_MSG		3
#define BUSY_MSG		4
#define BLANK_MSG		5

// message table lookup index
enum{need = 0, ser, card};
	
//external types
extern u8 msgNumber[];
	
#endif /* DISPLAY_H_ */