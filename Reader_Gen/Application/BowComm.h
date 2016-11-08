/*
* BowComm.h
*
* This file contains the interface to the BOW.
*
* Created: 11/22/2013 6:42:46 AM
*  Author: Paul
*/


#ifndef BOWCOMM_H_
#define BOWCOMM_H_

#include "mac.h"
#include "hal_avr.h"
#include "application.h"
#include "reader.h"
#include "system.h"
#include "UART/serial.h"
#include "avr_timer.h"
#include "KeyPad/Keypad.h"
#include <stdlib.h>
#include <avr/wdt.h>

//Reader/BOW command types

#define FIRMWARE_VERSION		1
#define REQUEST_SETUP			2
#define REQUEST_VALIDATION		3
#define REQUEST_MACHINE_LABEL	4
#define REQUEST_MACHINE_DESCRIPTION		5
#define REQUEST_NUMBER_OF_OLT			6
#define REQUEST_KEYPAD_ACCOUNT      7

//BOW error codes
#define TRANSACTION_SUCCESSFUL	6
#define INVALID_MANU_CODE		11
#define MACHINE_SETUP_NOT_FOUND	12
#define INVALID_SETUP			13
#define INVALID_CYCLE_PRICE		14

#define ACCOUNT_NOT_FOUND		21
#define INVALID_PIN				22

#define LAUNDRY_ROOM_NOT_FOUND	31
#define READER_NOT_FOUND		32
#define INVALID_MACHINE_LABEL	33
#define MACHINE_LABEL_NOT_FOUND	34

#define SERIAL_NUM_NOT_FOUND	41
#define CARD_NOT_ASSIGNED		42

#define USER_NOT_FOUND			51
#define INSUFFICIENT_FUNDS		52
#define USER_NOT_AUTHORIZED		53


bool sendReaderFirmwareVersion(void);
bool sendSetupRequest(void);
bool sendValidationRequest(void);
bool sendMachineDescription(void);
bool sendMachineLabel(void);
bool sendNumberOfOLT(void);
bool getReaderSetupInfo(void);
bool getKeypadAccountInfo(void);

u8 extractIncomingMessage(char *dataBuffer);
bool constructOutgoingString(u8 strType, char *tempStr);

#endif /* BOWCOMM_H_ */