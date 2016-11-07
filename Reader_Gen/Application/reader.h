/*
 * reader.h
 *
 * Created: 6/15/2011 9:56:05 PM
 *  Author: Paul
 */ 


#ifndef READER_H_
#define READER_H_

#include "rum_types.h"
#include "eeprom_map.h"


typedef struct{
    bool FirstRun       : 1;
    bool ReaderSetup    : 1;
    bool ValidateSetup  : 1;
	bool EnableOfflineTransaction	: 1;
	bool MaxNumTransReached			: 1;
	bool OfflineTransactionExist	: 1;
    bool Busy			: 1;
	bool ReaderNameSetup            : 1; 
	bool ReaderLabelSetup			: 1;  
	bool showModifiedVendPrice		: 1;
	u16   : 6;
} __attribute__((packed)) structReaderFlag;
structReaderFlag ReaderStateFlag;

typedef struct{
    bool sendingOfflineTransactions     : 1;
	bool waitingForBalance				: 1;
	u8   : 6;
} __attribute__((packed)) structTimeoutFlag;
structTimeoutFlag timeout;

typedef struct {
	
	bool deviceIsOffline	: 1; // 0 - running, 1 - offline
	u8						: 7; //remaining bit flags
} __attribute__((packed)) structDeviceFlag;

structDeviceFlag deviceFlag;

typedef struct  
{
	u8 deviceType[2];
	structDeviceFlag flags;
	
}__attribute__((packed)) structDeviceStatus;

structDeviceStatus deviceStatus;

typedef struct {
	
	u32 customerId;
    u16 manufacturerId;
	u16 locationId;
	u8	maxOfflineTransaction;
	u8  numOfSavedTransactions;
//	bool  EnableTransactionColl;		//Allow transactions to be collected
//	bool	MaxNumTransReached;
	u8 machineDescription[MACHINE_NAME_SIZE+1];
	u8 machineLabel[MACHINE_LABEL_SIZE+1];
	
}	__attribute__((packed)) structReaderSetup;

structReaderSetup ReaderSetup;

typedef struct{
    u8  Type;
	unsigned long	ID;
    u16 Pin;
	u16	Value;
	u8 	CardState;
}	__attribute__((packed)) structCurrentAccount;

structCurrentAccount CurrentAccount;

typedef struct  
{
	u8 hours;
	u8 minutes;
	u8 seconds;
}__attribute__((packed)) structTime;

typedef struct
{
	u8 month;
	u8 day;
	u16 year;
}__attribute__((packed)) structDate;

typedef struct {
	
	u16 LocationId;
	u8 MachineId[2];	//machine type involved in transaction, i.e. washer, dryer
	u16 ManufactureId;	//gen2, MDC, etc.
	u8 CycleType;		//machine cycle type
	u16 vendPrice;		//cost of cycle at transaction time
	u32 CardId;			//serial number of card involved in transaction
	u8 Date[4];			//date of transaction based on time received from server. BCD (mm,dd,yyyy).
	u8 Time[3];			//number of seconds from 12AM; 12:00:00AM == 0 seconds, 11:59:59PM == 86399 seconds.
	u8 isOffline;
}  __attribute__((packed)) structTransaction;

typedef struct
{
	u32 cardId;
	u8 cashValue;
	structDate date;			//date of transaction based on time received from server. BCD (mm,dd,yyyy).
	structTime time;			//time of transaction.
} __attribute__((packed))structBATransaction;

void initReader(void);
void getDate(structDate *date);
void getTime(structTime *time);
u8 storeOfflineTransaction(u32 cardNum);
//u8 transmitStoredTransaction(void);
u8 sendStoredTransaction(void);
bool sendCCTransaction(structTransaction *transRecord);

static inline void timeoutWaitingForOLTReponse(void)
{
	
	timeout.sendingOfflineTransactions = true;
}

#endif /* READER_H_ */