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
 *  \brief Header file for application.c.
 */
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#define INIT_READER 				0x01
#define SCANNING					0x02	
#define ENABLE_SCAN					0x03
#define SCAN_ENABLE					0x04
#define ZONE                        0x04 // start of data zones for mifare Ultra light
#define ZONE_SIZE                   0x04 // mifare Ultra light
#define DISABLE_SCAN				0x05
#define TIME_OUT					0x06
#define WAIT_FOR_SERVER				0x07
#define WAIT_FOR_SELECTION  		0x08
#define SETUP_PACKAGE     			0x09
#define SETUP_REQUEST       		0x0A
#define SETUP_RESPONSE				0x0B
#define CARD_DETECTED				0x0C
#define SCARD_REMOVED				0x0D
#define START_CYCLE					0x0F
#define CYCLE_STARTED				0x10

#define UPDATE_READER_FIRMWARE      0x12
#define VALIDATE_READER				0x13
#define VALIDATE_SETUP				0x14
#define GET_FIRMWARE_VERSION	    0x15
#define RESET_EEPROM                0x16
#define ADD_CARD_VALUE              0x17
#define RETURN_BILL                 0x18
#define WAIT_FOR_BILL				0x19
#define BOW_ERROR					0x1A // error message received from BOW
#define READER_NAME_SETUP			0x1B	//assign name to reader to allow id of physical machine
#define READER_NAME_REQUEST			0x1C	//retrieve reader name
#define READER_LABEL_SETUP			0x1D
#define READER_LABEL_REQUEST		0x1E
#define SHOW_BALANCE_REQUEST		0x1F
#define APP_INIT					0x20
#define IDLE						0x27
#define SEND_OFFLINE_TRANSACTIONS	0x28
	
// bootloader definitions	
#define EEPROM_RESET_VALUE 			0xFFFFFFFFFFFFFFFF 
#define BOOT_INIT_ADDR				0xFF0
#define CHANNEL_ADDR   				0xFF2 
#define PANID_ADDR 					0xFF4      
#define SRCADDR_ADDR				0xFF6 
#define DESTADDR_ADDR				0xFF8 


/* This is the EEPROM address that 
   contains the actual size of the 
   loaded application image */
#define	  APP_IMAGE_SIZE    0x0FFC

/* This is the EEPROM address that 
   contains the uint16_t CRC of the 
   application image */
#define   APP_CRC			0x0FFA 

//#define lowNibble 0
//#define highNibble 1

//! Maximum number of retry when sending command/result.
#define MAX_RETRY_TO_SEND_DATA 3
//! Serial send and receive data buffer size.
#define SERIAL_RECEIVE_MAX_BUFFER 192

// Wait time for BOW response to card present request, in sec.
#define SERVER_RESPONSE_TIMEOUT    60
// Wait time for BOW response to setup validation request, in seconds.
#define SETUP_VALIDATION_TIMEOUT	60 
// Wait time for BOW response to setup request, in seconds.
#define SETUP_REQUEST_TIMEOUT	120 

// Wait time to toggle led1, in msec
#define FAST_HEARTBEAT              250
#define SLOW_HEARTBEAT              500

#define HEARTBEAT   SLOW_HEARTBEAT

// device connected to the reader
#define MDC_MACHINE       1
#define MAYTAG_MACHINE    2

#define DEVICE_CONNECTED  MDC_MACHINE
 
#if(DEVICE_CONNECTED == MDC_MACHINE) 

    #define DEVICE_POLL_PERIOD          300     //poll period in msec

#elif(DEVICE_CONNECTED == MAYTAG_MACHINE)

    #define DEVICE_POLL_PERIOD          250     //poll period in msec

#endif

u8 ucDeviceStateFlag;
u8 OP;

//Machine State def
#define DEVICE_STATUS_DONE		0x00
#define DEVICE_STATUS_NEEDED	0x01
#define	DEVICE_STATUS_ONLINE	0x02

u8 ucNumberOfRetry;
u8 ucSendDataBuffer[SERIAL_RECEIVE_MAX_BUFFER];
u8 ucSendDataSize;

void appStartScan(void);
void doPing(void);

void String_to_hexa_convert ( u8 ucDataLength, u8 * pucDataBuffer );



void Beeps(u8 nBeeps);
//void Led1_toggle (void);
void CommTimeOut();
void HeartBeat();
void KeypressTimeOut();
void SetupTimeout();

void ProcessServerResponse(void);
void DevicePoll(void);
void SetDeviceState(void);
void SetBillReturn(void);

char * padLeft(char * string, u8 paddedLength, const char * pad) ;

char * GetParam(char *ptrBuffer, char *tmpstr, char *EndChar);

#if (NODETYPE == COORD)

typedef enum{
	BLANK             = 0,
	TAKE_DATA         = 1,
	END_OF_VALID_DATA = 2,
	END_OF_TAKE_DATA  = 3,
    TAKE_ENDNODE_SHORT_ADDRESS = 4

} serial_data_parse_state_t;

typedef enum{
	START           = 0,
	WAIT_FOR_RESULT = 1
} send_cmd_state_t;

#define SOP 0x73 // 'S'
#define EOP 0x70 // 'P'

//! Command for coordinator.
#define GET_VERSION       0x01
#define GET_NODES     	  0x02
//! Serial receive data buffer.
u8 ucSerialReceiveDataBuffer[SERIAL_RECEIVE_MAX_BUFFER];
u8 ucSerialReceiveDataIndex;
u16  ENDNODE_SHORTADDRESS;

//! Serial Data Parser State.
serial_data_parse_state_t ucSerialDataParseState;
u8 ucParseDataStart;
send_cmd_state_t ucSendCmdState;

void Serial_variable_initialize(void);
void Serial_receive_data_take ( void );
void String_to_hexa_convert ( u8 ucDataLength, u8 * pucDataBuffer );
void Serial_send_data (u8 ucDataLength, u8* pucDataBuffer);
void Hexa_to_string_convert ( u8 ucDataLength, u8* pucDataBuffer );
void Serial_send_board_version ( void );


u8 AppCheckCoordinatorCommand (u8 * ucSerialReceiveDataBuffer);


#elif (NODETYPE != COORD)


//app version # macros

/* String literal representation of the current application version. */

#if(DEVICE_CONNECTED == MDC_MACHINE)
    #define __APP_VERSION_STRING__ "0.4.4 MDC"
/* app_version
    Numerical representation of the current app version.

    In the numerical representation, the major number is multiplied by
    10000, the minor number by 100, and all three parts are then
    added.  It is intended to provide a monotonically increasing
    numerical value that can easily be used in numerical checks.
 */
#define __APP_VERSION__        0044UL

//    String literal representation of the release date. */
#define __APP_DATE_STRING__    "20161115"

/* YYYYMMDD
    Numerical representation of the release date. */
#define __APP_DATE_            20161115UL

/* 
    Application major version number. */
#define __APP_MAJOR__          0

/* Application minor version number. */
#define __APP_MINOR__          4

/* Application revision number. */
#define __APP_REVISION__       4

#elif(DEVICE_CONNECTED == MAYTAG_MACHINE)
    #define __APP_VERSION_STRING__ "0.0.1 GEN2"
#endif




#endif // (NODETYPE == COORD)




#endif // _APPLICATION_H_

