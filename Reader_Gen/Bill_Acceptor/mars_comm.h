
//#if (DEVICE_CONNECTED == MARS_BA)
#ifndef _mars_comm_h_
#define _mars_comm_h_

#define MARS_BA_DEVICE			0xBA	//Mars bill acceptor type device
//status

#define MARS_RESPONSE_TIMEOUT       1040 //msec

u8 t_response;

typedef struct{
    u8 numDataBytes;
    u8 cmdType;
    u8 data[36];
}__attribute__((packed)) structMarsData;

typedef struct{
    u8 msgType;
    u8 billState;
    u8 billValue;
    u8 acceptorState[2];
    u8 modelNum;
    u8 revNum;
}__attribute__((packed)) structMarsStatus;

typedef struct{
    u8 billType;
    u8 acceptorState;
    u8 billPush;        
}__attribute__((packed)) structMarsSetup;

structMarsData Mars;
structMarsStatus MarsStatus;
structMarsSetup MarsSetup;

//command to BA definitions
#define MARS_SETUP          1
#define MARS_POLL			2
#define MARS_STORE_BILL		3
#define MARS_RETURN_BILL	4
#define MARS_CASH_IN_BOX	5

// message len
#define MARS_CTRL_MSG_LEN       8
#define MARS_MAX_PACKET_SIZE    11

#define MARS_MSG_SUCCESS        0x20
#define MARS_MSG_FAILURE        0x21


#define MARS_ENABLE_ALL_BILLS   0x7F
#define MARS_ENABLE_1_BILL      0x01
#define MARS_ENABLE_5_BILL      0x04
#define MARS_ENABLE_10_BILL     0x08
#define MARS_ENABLE_20_BILL     0x10

//bill denominations
#define MARS_BILL_VALUE_MASK    0x38
#define MARS_ONE_DOLLAR         0x08
#define MARS_TWO_DOLLAR         0x10
#define MARS_FIVE_DOLLAR        0x18
#define MARS_TEN_DOLLAR         0x20
#define MARS_TWENTY_DOLLAR      0x28
#define MARS_FIFTY_DOLLAR       0x30
#define MARS_HUNDRED_DOLLAR     0x38

//bill acceptor state
//byte zero
#define MARS_BILL_IDLING        0x01		//BA is waiting for bill insertion
#define MARS_ACCEPTING_BILL     0x02		//Bill is being fed into BA
#define MARS_BILL_ESCROWED      0x04		//Bill has reached escrow state
#define MARS_STCACKING_BILL     0x08		//Bill is being stacked
#define MARS_BILL_STACKED       0x10		//Bill is fully stored and secured
#define MARS_RETURNING_BILL     0x20		//In the process of returning a bill
#define MARS_BILL_RETURNED      0x40		//Bill has been fully returned
//byte one
#define MARS_POWER_UP_STATE     0x01		//BA has experienced a power up state (after reset or power failure recovery)
#define MARS_INVALID_COMMAND    0x02		//Command not supported by BA
#define MARS_FAILURE_MODE       0x04		//BA cannot recover from this state without operator involvement


#define MARS_ACCEPTOR_STATE_DEFAULT     0x1C //

//bill storage mask
#define MARS_STACK_BILL_MASK    (MARS_ACCEPTOR_STATE_DEFAULT | 0x20)
#define MARS_RETURN_BILL_MASK   (MARS_ACCEPTOR_STATE_DEFAULT | 0x40)


// Error Codes
#define BILL_STACKED			(0x00)
#define BILL_STACK_ERROR		(0x01)
#define BILL_RETURNED			(0x02)
#define BILL_STATE_UNKNOWN		(0xFF)

u8 getMarsData(void);
u8 sendMarsData(u8 ucCommandType);
u8 storeMarsBill( void );
u8 returnMarsBill( void );
u8 transmitStoredTransaction(void);

void pollMars(void);
void initMars(void);
void marsTimeout(void);
u8 getNumOfBills(void);

#endif //_mars_comm_h_
//#endif