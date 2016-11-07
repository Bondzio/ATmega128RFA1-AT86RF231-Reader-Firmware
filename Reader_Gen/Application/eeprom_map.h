/************************************************************************************************************************************/
// A brief description of eeprom layout and reader related variables.
//
//
//
//=======================================================================================================
//  | Address       |   data					|   Definition											|
//  |---------------|---------------------------|-------------------------------------------------------|
//  |0x00~0x07      | Reader Mac Address		|														|
//  |---------------|---------------------------|-------------------------------------------------------|
//  |0x08~0x1D      |		RFU					|														|   
//  |---------------|---------------------------|-------------------------------------------------------|
//  |0x1E~21        |   Device Type				|Type of device (washer/dryer, etc) connected to reader	|
//  |---------------|---------------------------|-------------------------------------------------------|
//  |0x22~23        |  Reader State				|Current state of the reader. Setup or not              |
//  |---------------|---------------------------|-------------------------------------------------------|
//	|0x24~0x56		| Machine Setup				|Setup config. for machine connected to reader          |
//	|---------------|---------------------------|-------------------------------------------------------|
//	|0x57~0x5F		| Reader Validation Code	|Validation code for reader mac address setup           |
//	|---------------|---------------------------|-------------------------------------------------------|
//  |---------------|---------------------------|-------------------------------------------------------|
//  |0x61~0xB9      |Reader specific setup		|Card reader related setup options						|
//  |---------------|---------------------------|-------------------------------------------------------|
//	|	---> (0x6C) |	Machine Label			|Name used to attach reader to machine uniquely			|
//	|---------------|---------------------------|-------------------------------------------------------|
//	|	---> (0x77)	|	Machine Description		|Description of machine reader is attached to			|
//	|---------------|---------------------------|-------------------------------------------------------|
//	|0xBA~0xFF		|	RFU						|														|
//	|---------------|---------------------------|-------------------------------------------------------|
//	|0x100~0x500	|Machine transactions		|Transaction storage for when server is offline			|
//	|---------------|---------------------------|-------------------------------------------------------|														




#include "application.h"

#define SET_READER_FLAG				0
#define CLR_READER_FLAG				1
#define READER_SETUP_DONE           0x00
#define READER_SETUP_NEEDED		    0x01
#define VIRGIN_READER               0x01
#define NON_VIRGIN_READER           0x00
#define VALIDATE_READER_SETUP       0x01
#define VALID_READER_SETUP          0x00

//Reader State Def

#define READER_NEW              0x07//0x0F //0xc7
//============================================BIT MASK===============================================//

//------------------------------------NUMBER OF BYTES PER FIELD--------------------------------------//
//setup info only apply to MDC
//field start addresses must be recalculated for new field size

#define MACHINE_SETUP_MAX_BYTE			50  //maximum number of bytes per machine setup

#if( DEVICE_CONNECTED == MDC_MACHINE)
	#define MACHINE_SETUP_NUM_BYTE			12  // number of bytes needed to store washer setup in eeprom 
	#define DRYER_SETUP_NUM_BYTE			7   //number of additional bytes needed dryer specific setup
	#define QUANTUM_SETUP_NUM_BYTE			5   //number of additional bytes needed Quantum specific setup
#elif(DEVICE_CONNECTED == MAYTAG_MACHINE)
	#define MACHINE_SETUP_NUM_BYTE			12  // number of bytes needed to store washer setup in eeprom 
	#define DRYER_SETUP_NUM_BYTE			7   //number of additional bytes needed dryer specific setup
#elif(DEVICE_CONNECTED == MARS_BA)
	#define MACHINE_SETUP_NUM_BYTE			12  // number of bytes needed to store Bill Acceptor setup in eeprom 
#endif

#define VALIDATION_CODE_NUM_BYTE		8   //number of bytes needed to store validation code
#define OFFLINE_TRANSACTION_NUM_BYTE	21	//number of bytes per offline transaction
#define READER_STATE_FLAG_SIZE			2	//number of bytes for reader state flag
#define MACHINE_NAME_SIZE				50	//number of bytes for reader name field
#define MACHINE_LABEL_SIZE				10	//number of bytes for reader label field
#define MACHINE_TYPE_SIZE				2	//number of bytes for reader label field
//-----------------------------------ADDRESS OF SETUP INFO------------------------------------------// 

#define READER_SETUP_ADDR					(u8*)0x61//(u8*)0x08
#define MACHINE_TYPE_ADDR                   (u8*)0x1E//(u8*)0x20
#define READER_STATE_ADDR                   (u8*)0x22

#define MACHINE_SETUP_START_ADDR            (u8*)0x24
#define VALIDATION_CODE_START_ADDR          ((u8*)(MACHINE_SETUP_START_ADDR+MACHINE_SETUP_MAX_BYTE+1))

#define MACHINE_SETUP_ADDR                  MACHINE_SETUP_START_ADDR
#define MACHINE_SETUP_ADDR_DRYER            ((u8*)(MACHINE_SETUP_ADDR+MACHINE_SETUP_NUM_BYTE))
#define MACHINE_SETUP_ADDR_QUANTUM			((u8*)(MACHINE_SETUP_ADDR_DRYER+DRYER_SETUP_NUM_BYTE))

#define VALIDATION_CODE_ADDR                VALIDATION_CODE_START_ADDR

#define MACHINE_LABEL_ADDR					(u8*)0x6C
#define MACHINE_DESCRIPTION_ADDR			(u8*)0x77

//-----------------------------------RECORD STORAGE ADDRESS----------------------------------------//

#define OFFLINE_HEADER_NUM_BYTE				24
#define OFFLINE_TRANSACTION_RECORD_ADDR		(u8*)0x100
#define OFFLINE_TRANSACTION_ADDR_START		((u8*)(OFFLINE_TRANSACTION_RECORD_ADDR + OFFLINE_HEADER_NUM_BYTE))
#define OFFLINE_TRANSACTION_ADDR_END		(u8*)0x500

#define OFFLINE_TRANSACTION_RECORD_START	(u8*)0x101
#define MAX_NUM_OFFLINE_TRANS				(((OFFLINE_TRANSACTION_ADDR_END - OFFLINE_TRANSACTION_ADDR_START)/ OFFLINE_TRANSACTION_NUM_BYTE))

//=================================================================================================//
