#include "mac.h"
#include "application.h"
#if (DEVICE_CONNECTED == MARS_BA)

//#include "application.h"
#include "atmega128rfa1.h"

#include "serial.h"
//#include "machine.h"
#include "avr_timer.h"
#include "timer_enddevice.h"
#include <avr/wdt.h>
#include <util/delay.h>
#include "hal_avr.h"
#include "eeprom_map.h"

#include "mac_event.h"

#include "mars_comm.h"
#include "machine.h"


#define STX     0x02
#define ETX     0x03

static u8 ucAckMsg;

//  send data to the Mars bill acceptor.
u8 sendMarsData( u8 ucCommandType )
{
    u8 ucData[MARS_CTRL_MSG_LEN]={0},i,check_sum=0;


    switch (ucCommandType)	//Format command
	{
		case MARS_SETUP:
            ucData[0] = MARS_CTRL_MSG_LEN;
//            ucData[1] = 0x10; 
            ucData[2] = MarsSetup.billType;        
            ucData[3] = (MarsSetup.acceptorState & MARS_ACCEPTOR_STATE_DEFAULT);
            ucData[4] = MarsSetup.billPush;
            break;
        case MARS_POLL:
            ucData[0] = MARS_CTRL_MSG_LEN;
/*            if( ucAckMsg | 0x10)//resend message
                ucData[1] = (0x10 | (MarsStatus.msgType & 0x01));
            else
                ucData[1] = (0x10 | ucAckMsg);
*/
            ucData[2] = MarsSetup.billType;        
            ucData[3] = (MarsSetup.acceptorState & MARS_ACCEPTOR_STATE_DEFAULT);
            ucData[4] = MarsSetup.billPush;
            break;
        case MARS_STORE_BILL:
            ucData[0] = MARS_CTRL_MSG_LEN;
//            ucData[1] = 0x10;
            ucData[2] = MarsSetup.billType;        
            ucData[3] = (MarsSetup.acceptorState | MARS_STACK_BILL_MASK);
            ucData[4] = MarsSetup.billPush;
            break;
        case MARS_RETURN_BILL:
            ucData[0] = MARS_CTRL_MSG_LEN;
//            ucData[1] = 0x10;
            ucData[2] = MarsSetup.billType;        
            ucData[3] = (MarsSetup.acceptorState | MARS_RETURN_BILL_MASK);
            ucData[4] = MarsSetup.billPush;            
            break;
        case MARS_CASH_IN_BOX:
            ucData[0] = MARS_CTRL_MSG_LEN;
            ucData[2] = 0;        
            ucData[3] = 0;
            ucData[4] = 1;            
		
			break;
		default:
            break;
    }
    
    //command not understood, return
    if (ucData[0] == 0)
		return 0;

    if( ucAckMsg )    
        ucData[1] = 0x10 + ((MarsStatus.msgType & 0x01) ^ 0x01);
    else
        ucData[1] = 0x10 + (MarsStatus.msgType & 0x01); //resend message

    init_rx_buf();

    serial_putchar(STX);
    
    for (i = 0; i < (MARS_CTRL_MSG_LEN - 3); i++)
    {
        serial_putchar(ucData[i]);
		check_sum = (check_sum ^ ucData[i]);
	}		
    
    serial_putchar(ETX);
    serial_putchar(check_sum);    // Send check_sum, error checking byte

    //wait up to timeout for response from bill acceptor.
    t_response  = 0;
    i=macSetAlarm(MARS_RESPONSE_TIMEOUT, marsTimeout);
    while( (t_response == 0) && (rx_buf_count() == 0) )
    {
        if( mac_event_pending() )
            macTask();  //timer has timed out handle timer task.
    }
    if( t_response == 0 )
    {
		macTimerEnd(i);
        if( getMarsData() )
        {    //message successfully received, no need to repeat.
            ucAckMsg = 1;
            return 1;
        }
        else
            ucAckMsg = 0;
    }	
		    
    return 0;
}

//  retrieve data from the Mars bill acceptor and place it in the buffer.
u8 getMarsData( void )
{
	u8 ucMarsDataBuf[MARS_MAX_PACKET_SIZE] = {0};
	u8 i,n,check_sum;

	
	//get machine data
	i = serial_getchar();
	if( i == STX )//state byte detected
	{	
		i = serial_getchar();
		check_sum = i;

		for(n = 0; n < (i-4); n++)
		{
            ucMarsDataBuf[n]= serial_getchar();
			check_sum = (check_sum ^ ucMarsDataBuf[n]);
            //debug only
            //Led3_toggle();
		}
		
        if( (i = serial_getchar()) == ETX)
        {
            i = serial_getchar();	// get check_sum

    		if(check_sum == i)
    		{
    			/*switch( ucMarsDataBuf[0])
    			{
    				case MARS_MSG_SUCCESS:*/
    					MarsStatus.msgType 	    = ucMarsDataBuf[0];
    					MarsStatus.billState  	= ucMarsDataBuf[1];
                        switch( (ucMarsDataBuf[3] & MARS_BILL_VALUE_MASK) )
                        { 
                            case MARS_ONE_DOLLAR:
                                MarsStatus.billValue = 1;
                                break;
                            case MARS_FIVE_DOLLAR:
                                MarsStatus.billValue = 5;
                                break;
                            case MARS_TEN_DOLLAR:
                                MarsStatus.billValue = 10;
                                break;
                            case MARS_TWENTY_DOLLAR:
                                MarsStatus.billValue = 20;
                                break;
                            case MARS_FIFTY_DOLLAR:
                                MarsStatus.billValue = 50;
                            default:
                                MarsStatus.billValue = 0;
                                break;
                        }
                       
    					MarsStatus.acceptorState[0]   	= ucMarsDataBuf[2];
    					MarsStatus.acceptorState[1]     = (ucMarsDataBuf[3] & 0x07);
                        MarsStatus.acceptorState[1]     =   (ucMarsDataBuf[4] << 4);
				        MarsStatus.modelNum     = ucMarsDataBuf[5];
    					MarsStatus.revNum       = ucMarsDataBuf[6];
                       /* break;
    				case MARS_MSG_FAILURE:
					    //debug only
                        Led2_toggle();
    					break;
    				default:
    					break;
    			}*/

    			return 1;
    		}
        }
	}
	return 0;
}

void initMars( void )
{
    MarsSetup.billType = MARS_ENABLE_1_BILL | MARS_ENABLE_5_BILL | MARS_ENABLE_10_BILL | MARS_ENABLE_20_BILL;        
    MarsSetup.acceptorState = MARS_ACCEPTOR_STATE_DEFAULT;
    MarsSetup.billPush = 0;

    MarsStatus.msgType = 0;
    MarsStatus.billState = 0;
    MarsStatus.billValue = 0;
    MarsStatus.acceptorState[0] = 0;
    MarsStatus.acceptorState[1] = 0;
    MarsStatus.modelNum = 0;
    MarsStatus.revNum = 0;

    sendMarsData(MARS_SETUP);
    
}

u8 storeMarsBill( void )
{
	u8 status;
    u8 tmp = 0xff;

	status = BILL_STATE_UNKNOWN;
	
   if( sendMarsData(MARS_STORE_BILL))
   {    //wait for bill to be fully stored
        ucDeviceStateFlag = DEVICE_STATUS_NEEDED;
		pollMars();

        while( (MarsStatus.billState & MARS_STCACKING_BILL) || (MarsStatus.billState & MARS_BILL_ESCROWED) )
        {   
            if( ucDeviceStateFlag == DEVICE_STATUS_NEEDED) 
            {
                pollMars();
		        tmp = macSetAlarm(DEVICE_POLL_PERIOD,SetDeviceState);
                ucDeviceStateFlag = DEVICE_STATUS_DONE;                 
            }
			if( mac_event_pending() )
            macTask();  //timer has timed out handle timer task.           
        }
		if( MarsStatus.billState & MARS_BILL_STACKED )
        {
            macTimerEnd(tmp);
            status = BILL_STACKED;
        }
   }
   ucDeviceStateFlag = DEVICE_STATUS_NEEDED;
   return status;    
}

u8 returnMarsBill( void )
{
	u8 status, tmp=0xff;
	
	status = BILL_STATE_UNKNOWN;
	sendMarsData(MARS_RETURN_BILL);
	ucDeviceStateFlag = DEVICE_STATUS_NEEDED;
	pollMars();
   
	//wait for bill to be fully returned
	while( (MarsStatus.billState & MARS_RETURNING_BILL) || (MarsStatus.billState & MARS_BILL_ESCROWED))
    {   
        if( ucDeviceStateFlag == DEVICE_STATUS_NEEDED) 
        {
            pollMars();
		    tmp = macSetAlarm(DEVICE_POLL_PERIOD,SetDeviceState);
            ucDeviceStateFlag = DEVICE_STATUS_DONE;                 
        }   
		if( mac_event_pending() )
           macTask();  //timer has timed out handle timer task.        
    }
	if( MarsStatus.billState & MARS_BILL_RETURNED )
    {
        macTimerEnd(tmp);
        status = BILL_RETURNED;
    }
	ucDeviceStateFlag = DEVICE_STATUS_NEEDED;
   return status;    
}

//poll Mars bill acceptor
void pollMars( void )
{
   sendMarsData(MARS_POLL);
/*   if(MarsStatus.AcceptorState[1] & MARS_POWER_UP_STATE)
		storeMarsBill();*/
}

void marsTimeout(void)
{
    t_response = 1;
}

u8 getNumOfBills(void)
{
	//static u8 numBill=0;
	/*if (sendMarsData(MARS_CASH_IN_BOX)== 1)
	{
		return MarsStatus.AcceptorState[0];
	}
	else
		return 0;
	*/	
	return (sendMarsData(MARS_CASH_IN_BOX)== 1) ? MarsStatus.acceptorState[0] : 0;
	
	
}
//place holder, this feature is not yet available in the BA
u8 transmitStoredTransaction(void)
{
	return 1;
}

#endif  //MARS_BA  CONNECTED
