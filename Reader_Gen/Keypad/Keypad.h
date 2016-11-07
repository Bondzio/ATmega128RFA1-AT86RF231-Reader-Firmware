/*
 * Keypad.h
 *
 * Created: 9/3/2013 1:01:46 AM
 *  Author: Paul
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "rum_types.h"

#define KEYPAD_TIMEOUT	      5


//account entry relevant defines
#define KEYPAD_ACCOUNT_NUM_SIZE  9
#define KEYPAD_PASSCODE_SIZE     4
//-------------------------------

typedef struct
{
	uint8_t address;
	uint8_t size;
	uint8_t *buf;
}QTData_t;

typedef struct
{
   bool isAccountNumberEntered   : 1;
   bool isPasscodeEntered        : 1;
   bool keypadEntryTimedout      : 1;
   uint8_t : 5;
}Flags;
typedef struct  KeypadInfo
{
   uint8_t accountNumber[KEYPAD_ACCOUNT_NUM_SIZE];
   uint8_t passcode[KEYPAD_PASSCODE_SIZE];
   uint8_t entryTimedout;
   Flags flags;
}Keypad;

void resetKeypad(void);
void initKeypad(void);
void clearKeypadEntryTimeout(void);
void setKeypadEntryTimeout(void);
void clearKeypadEntry(void);

bool isKeyPressed(void);
bool accountNumberEntry();
bool passcodeEntry();
bool changeStateStatus(void);
bool numberEntry(uint8_t *num);

u8 getPressedKey(void);
u8 sendKeypadAccount(void);
u8 sendKeypadData(QTData_t *info);

void getKeypadData(QTData_t *info);

void qtRegDump(void);



extern Keypad keypad;
extern u8 keypadEntryTimeoutTimerID;
#endif /* KEYPAD_H_ */