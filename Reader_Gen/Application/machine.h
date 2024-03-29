
#ifndef MACHINE_H
#define MACHINE_H

#include "rum_types.h"

void SetDefaultMachineSetup(void);

void WaitForMachine(void);

#define MACHINE_WAIT            0x00
#define MACHINE_TIME_OUT        0x01
#define MACHINE_WAIT_TIMEOUT    10000   //In ms, the time period to wait for a machine to power up

#if (DEVICE_CONNECTED == MDC_MACHINE)

#include "SpeedQueen.h"

void getSQAuditData(void);

//Machine status

#define STX 	0x02
#define ACK		0x06
#define INV		0x09
#define NAK		0x15

//MDC Command and data codes
#define STATUS_REQUEST						0x10
#define MACHINE_STATUS_PACKET				0x11
#define VEND_PRICE_PACKET					0x12

#define PROGRAMMING_DATA					0x20
#define PROGRAMMING_DATA_TOPLOAD			0x21
#define PROGRAMMING_DATA_FRONTLOAD			0x22
#define PROGRAMMING_DATA_WASHER_EXTRACTOR	0x24
#define PROGRAMMING_DATA_TUMBLER			0x28
#define PROGRAMMING_DATA_DRYER				0x29

#define AUDIT_DATA_REQUEST			0x30
#define AUDIT_DATA_TOPLOAD			0x31
#define AUDIT_DATA_FRONTLOAD		0x32
#define AUDIT_DATA_WASHER_EXTRACTOR	0x34
#define AUDIT_DATA_TUMBLER			0x38
#define AUDIT_DATA_DRYER			0x39

#define CASH_CARD_INSERTED			0x40
#define CARD_REMOVED				0x41
#define	CASH_CARD_TOPOFF			0x42

#define DEDUCT_VEND_COMMAND			0x46
#define DEDUCT_TOPOFF_COMMAND		0x47	//Deduct TopOff command
#define MACHINE_START_COMMAND		0x48
#define ADD_TIME_COMMAND			0x49
#define CARD_ERROR_PACKET			0x50
#define DISPLAY_REQUEST				0x60
#define AUDIO_BEEP_REQUEST			0x61
#define MANUAL_MODE_ENTRY			0x62
#define PRODUCTION_TEST_CYCLE		0x63

//temp display msg def
#define DISPLAY_REQUEST_CARD        0x70
#define DISPLAY_REQUEST_ERROR       0x71
#define DISPLAY_REQUEST_TIME        0x72
#define DISPLAY_REQUEST_OUT         0x73

#define CASH_CARD_REMOVED			0x99

//MDC packet data length
#define MACHINE_START_PACKET_SIZE	1
#define AUDIT_DATA_REQUEST_SIZE		1
#define ADD_TIME_PACKET_SIZE		1
#define CARD_REMOVED_SIZE			1
#define CARD_ERROR_PACKET_SIZE		2
#define AUDIO_BEEP_REQUEST_SIZE		2
#define STATUS_REQUEST_SIZE			2
#define CASH_CARD_REMOVED_SIZE		5
#define VEND_PRICE_SIZE				4
#define CASH_CARD_SIZE				5
#define DISPLAY_REQUEST_SIZE		6
#define CASH_CARD_TOPOFF_SIZE		6
#define MACHINE_STATUS_SIZE			11
#define PROGRAMMING_DATA_SIZE		43
#define	AUDIT_DATA_PACKET_SIZE		1
#define	MDC_MAX_PACKET_SIZE			50
#define QTL_PROGRAMMING_DATA_SIZE	45
#define QFL_PROGRAMMING_DATA_SIZE	34
#define QWE_PROGRAMMING_DATA_SIZE	38
#define QDT_PROGRAMMING_DATA_SIZE	33

//Quantum packet data length
#define QUANTUM_VEND_PRICE_SIZE		7

//Quantum commands
#define QUANTUM_VEND_PRICE		0x18

//Machine Type
#define INVALID_MACHINE			0x00
#define TOP_LOAD_WASHER			0x01
#define FRONT_LOAD_WASHER		0x02
#define WASHER_EXTRACTOR		0x04
#define TUMBLER					0x08
#define DRYER					0x09

//Washer keypad data
#define WASHER_START			0x02
#define WASHER_DELICATE			0x04
#define WASHER_NORMAL			0x08
#define WASHER_PERM_PRESS		0x40
#define WASHER_HEAVY			0x80

//Dryer/Tumbler keypad data
#define DRYER_MEDIUM_TEMP		0x01
#define DRYER_DELICATE			0x02
#define DRYER_START				0x04
#define DRYER_HIGH_TEMP			0x10
#define DRYER_LOW_TEMP			0x20

//Machine cycle type
#define HEAVY_HIGH_TEMP			0
#define NORMAL_MEDIUM_TEMP		1
#define PERM_PRESS_LOW_TEMP		2
#define DELICATE				3

//Machine command to reader
#define NORMAL_STATE			0x00
#define DEDUCT_CYCLE_VEND		0x46
#define DEDUCT_TOPOFF_VEND		0x47


//Machine status
#define MACHINE_READY_MODE		0x02
#define MACHINE_START_MODE		0x08
#define MACHINE_RUN_MODE		0x10
#define MACHINE_END_MODE		0x20
#define MACHINE_ERROR_MODE		0x40
#define MACHINE_MANUAL_MODE		0x80

//Card Status
#define CARD_OUT				0x00
#define CARD_IN					0x80

#define NUMRETRIES				8

extern u16 ucCardBalance;
extern u16 cardId;
extern double vendPrice;
extern u8 ucTimerCount;
u8 ucCardStatus;

u8 ucMachineWait;

typedef struct{

	u8 	MachineType[2];
	//u8 	KeypadData[2];
	u8 	CycleType;
	u8 	CmdToReader;
	u8 	MachineStatus[2];
	u8 	RemainingCycleMinutes;
	u8 	RemainingCycleSeconds;
	u8  CurrentKey[2];
	u8  CycleModifier[2];
	
} __attribute__((packed)) structMachineStatus;

structMachineStatus MdcStatus;

typedef struct{
	u8 KeyPressWaitTime;
	u8 ProductByte[3];
	u8 VendPrice[2];
	u8 VendPrice1[2];
	u8 VendPrice2[2];
	u8 VendPrice3[2];
	u8 VendPrice4[2];
	u8 VendPrice5[2];
	u8 VendPrice6[2];
	u8 VendPrice7[2];
	u8 VendPrice8[2];
	u8 HeavyCycle_vendPrice[2];
	u8 HeavyCycle_option;
	u8 HeavyCycle_warmRinseOption;
	u8 HeavyCycle_extraWashTime;
	u8 HeavyCycle_extraRiseTime;
	u8 MediumCycle_vendPrice[2];
	u8 MediumCycle_option;
	u8 NormalCycle_washAgitateTime;
	u8 NormalCycle_rinseAgitateTime;
	u8 NormalCycle_extraRinseAgitateTime;
	u8 NormalCycle_finalSpinTime;
	u8 PermPressCycle_washAgitateTime;
	u8 PermPressCycle_rinseAgitateTime;
	u8 PermPressCycle_extraRinseAgitateTime;
	u8 PermPressCycle_finalSpinTime;
	u8 DelicateCycle_washAgitateTime;
	u8 DelicateCycle_rinseAgitateTime;
	u8 DelicateCycle_extraRinseAgitateTime;
	u8 DelicateCycle_finalSpinTime;
	u8 Coin1[2];
	u8 Coin2[2];
	u8 StartPulse[2];
	u8 CycleLength;
	u8 ControlConfig;
	u8 DefaultCycle;
	u8 CoolDownTime;
	u8 Coin1TopoffTime;
	u8 Coin2TopoffTime;
	u8 HighTempSetting;
	u8 MediumTempSetting;
	u8 LowTempSetting;
	u8 DelicateTempSetting;
	u8 ModifiedCycleVendPrice[2];
	u8 TopoffOn;
	u8 TopoffTime;
	u8 CycleChangeResponse;
	u8 TieredDryerPricing;
	u8 AudioSetting;
	u8 CardReaderDisplayControl;
	u8 CycleModifierExtraWashTime;

} __attribute__((packed)) structMachineProg;

structMachineProg MachineSetupData;

typedef struct{
	u8 MachineType[2];
	u8 ControlSVN;
	u8 VendPrice[2];
	u8 Coin1Value[2];
	u8 Coin2Value[2];
	u8 NumStartPulse[2];
	u8 CycleConfig;
	u8 ControlConfig;
	u8 Coin1Counter[2];
	u8 Coin2Counter[2];
	u8 NumCycles[2];
	u8 AvgFillTime[2];
	u8 AvgDrainTime[2];
	u8 CycleTime;
	u8 CoolDownTime;
	u8 Coin1TopoffTime;
	u8 Coin2TopoffTime;
	u8 HighTempSetting;
	u8 MedTempSetting;
	u8 LowTempSetting;
	u8 DeliTempSetting;
	u8 Coin1TopoffCounter[2];
	u8 Coin2TopoffCounter[2];
	u8 NumTopoffStartPulses[2];

} __attribute__((packed)) structMachineAudit;

structMachineAudit MdcAuditData;

u8 sendMdcPacket(u8 *ucDataBuf);

/*
#elif (DEVICE_CONNECTED == MAYTAG_MACHINE)

u8 ucCardStatus;

typedef struct{
	u8  KeyPressWaitTime;
	u8 VendPrice[2];
	u8 Coin1[2];
	u8 Coin2[2];
	u8 StartPulse[2];
	u8 CycleLength;
	u8 ControlConfig;
	u8 DefaultCycle;
	u8 CoolDownTime;
	u8 Coin1TopoffTime;
	u8 Coin2TopoffTime;
	u8 HighTempSetting;
	u8 MediumTempSetting;
	u8 LowTempSetting;
	u8 DelicateTempSetting;

} __attribute__((packed)) structMachineProg;

void sendMTStartMessage( void );

//Message Identifiers
#define ACK     0x01
#define NAK     0x02
#define ENQ     0x05
#define SOH     0x81

// Message Type
#define START_MESSAGE   0x06
#define STACK_MESSAGE   0x07
//Card Status

#define CARD_OUT				0x00
#define CARD_IN					0x80
#define CARD_REMOVED				0x41

extern u8 startMsg[];
*/
#endif //Machine type

u8 SerialSendMachineData(u8 ucCommandType);
u8 SerialGetMachineData(void);
//void MachineStatus(void);
void InitTimerZero(void);
void StartTimerZero(void);
void StopTimerZero(void);
void SetMachineSetupData(void);
void SaveMachineSetupData(void);

void ProcessRadioData(void);

#endif // MACHINE_H
