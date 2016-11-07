/*
 * SpeedQueen.h
 *
 * Created: 5/6/2013 6:51:00 AM
 *  Author: Paul
 */ 


#ifndef SPEEDQUEEN_H_
#define SPEEDQUEEN_H_

#include "display.h"

#define MDC_SERIES		1
#define QUANTUM_SERIES	2

#define MDC_STATUS_REQUEST			0x10
#define QUANTUM_STATUS_REQUEST		0x16

#define MDC_STATUS_RESPONSE			0x11
#define QUANTUM_STATUS_RESPONSE		0x17

#define MDC_STATUS_DATA_SIZE		11
#define QUANTUM_STATUS_DATA_SIZE	18

#define REGULAR_VEND	1
#define TOPOFF_VEND		2

#define CASH_CARD		1
#define REMOVE_CARD		2

bool isQuantum(void);
bool isMDC(void);
bool sendSQData(u8 *buf);
bool getSQData(u8 *buf);
bool getSQInitData(void);
void getSQSetupData(void);
bool getSQMachineStatus(void);
bool getSQStatusData(void);
bool sendSQVendPrice(void);
bool waitSQStartKey(void);
bool isSQCycleRunning(void);
bool sendSQCashCardInserted(void);
bool sendSQCashCardInsetedForTopoff(void);
bool sendSQCashCardRemoved(void);
bool sendSQCardRemoved(void);
bool sendSQStart(void);
bool sendSQAddTime(void);
bool sendSQCardErrorCode(u8 cardErrorCode);
bool sendSQAudioBeepRequest(u8 beepLength);
bool sendSQDisplayCommand( u8 led1, u8 led2,u8 led3, u8 led4, u8 duration);
bool displaySQReaderVersion(void);
bool displaySQCardError(void);
bool cardRemoved(u8 cardType);
bool cardInserted(u8 cardType);

bool setupMachine(void);
bool startMachineCycle(void);

void initMachine(void);
bool waitForMachineStartKey();
bool isMachineCycleRunning();
void MachineStatus(void);

#endif /* SPEEDQUEEN_H_ */