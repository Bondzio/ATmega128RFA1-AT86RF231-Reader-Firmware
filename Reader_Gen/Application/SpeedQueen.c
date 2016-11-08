/*
 * SpeedQueen.c
 *
 * Created: 7/30/2011 10:16:47 PM
 *  Author: Paul
 */ 

#include "mac.h"
#include "application.h"
#if(DEVICE_CONNECTED == MDC_MACHINE)
#include "machine.h"
#include "serial.h"
#include "SpeedQueen.h"
#include "mac_event.h"
#include "reader.h"

//Retrieve SQ setup info from eeprom
void getSQSetupData(void)
{
	u8 tmp[50] = {0};
		
		

	if( deviceStatus.deviceType[0] == PROGRAMMING_DATA_TOPLOAD  || deviceStatus.deviceType[0] == PROGRAMMING_DATA_FRONTLOAD || deviceStatus.deviceType[0] == PROGRAMMING_DATA_WASHER_EXTRACTOR )
	{	
		halGetEeprom(MACHINE_SETUP_ADDR, 16, (u8*)&tmp);
		vendPrice = (tmp[1] * 256 + tmp[2]) / 100.0;
		
		MachineSetupData.KeyPressWaitTime = tmp[0];
		MachineSetupData.VendPrice[0] = tmp[1];
		MachineSetupData.VendPrice[1] = tmp[2];
		MachineSetupData.Coin1[0] = 0;
		MachineSetupData.Coin1[1] = 25;
		MachineSetupData.Coin2[0] = 0;
		MachineSetupData.Coin2[1] = 100;
		MachineSetupData.StartPulse[0] = tmp[11];
		MachineSetupData.StartPulse[1] = tmp[12];
		MachineSetupData.CycleLength = tmp[13];
		MachineSetupData.ControlConfig = tmp[14];
		MachineSetupData.DefaultCycle = tmp[15]; 		
		
		MachineSetupData.ModifiedCycleVendPrice[0] = tmp[3]; 
		MachineSetupData.ModifiedCycleVendPrice[1] = tmp[4];
		
		if ( deviceStatus.deviceType[1] == QUANTUM_SERIES )
		{
			MachineSetupData.VendPrice1[0] = 0;
			MachineSetupData.VendPrice1[1] = 100;
			MachineSetupData.VendPrice2[0] = 0;
			MachineSetupData.VendPrice2[1] = 100;
			MachineSetupData.VendPrice3[0] = 0;
			MachineSetupData.VendPrice3[1] = 100;
			MachineSetupData.VendPrice4[0] = 0;
			MachineSetupData.VendPrice4[1] = 100;
			MachineSetupData.VendPrice5[0] = 0;
			MachineSetupData.VendPrice5[1] = 100;
			MachineSetupData.VendPrice6[0] = 0;
			MachineSetupData.VendPrice6[1] = 100;
			MachineSetupData.VendPrice7[0] = 0;
			MachineSetupData.VendPrice7[1] = 100;
			MachineSetupData.VendPrice8[0] = 0;
			MachineSetupData.VendPrice8[1] = 100;
			if ( deviceStatus.deviceType[0] == PROGRAMMING_DATA_TOPLOAD )
			{
				MachineSetupData.HeavyCycle_vendPrice[0] = 0;
				MachineSetupData.HeavyCycle_vendPrice[1] = 25;
				MachineSetupData.HeavyCycle_option = 1;
				MachineSetupData.HeavyCycle_extraWashTime = 10;
				MachineSetupData.HeavyCycle_extraRiseTime = 0;
				MachineSetupData.HeavyCycle_warmRinseOption = 0;
				MachineSetupData.NormalCycle_washAgitateTime = 10;
				MachineSetupData.NormalCycle_rinseAgitateTime = 4;
				MachineSetupData.NormalCycle_extraRinseAgitateTime = 0;
				MachineSetupData.NormalCycle_finalSpinTime = 7;
				MachineSetupData.PermPressCycle_washAgitateTime = 10;
				MachineSetupData.PermPressCycle_rinseAgitateTime = 4;
				MachineSetupData.PermPressCycle_extraRinseAgitateTime = 0;
				MachineSetupData.PermPressCycle_finalSpinTime = 6;
				MachineSetupData.DelicateCycle_washAgitateTime = 10;
				MachineSetupData.DelicateCycle_rinseAgitateTime = 4;
				MachineSetupData.DelicateCycle_extraRinseAgitateTime = 0;
				MachineSetupData.DelicateCycle_finalSpinTime = 5;
			}
			else if ( deviceStatus.deviceType[0] == PROGRAMMING_DATA_FRONTLOAD )
			{
				MachineSetupData.HeavyCycle_vendPrice[0] = 0;
				MachineSetupData.HeavyCycle_vendPrice[1] = 50;
				MachineSetupData.MediumCycle_vendPrice[0] = 0;
				MachineSetupData.MediumCycle_vendPrice[1] = 25;
				MachineSetupData.MediumCycle_option = 2;
				MachineSetupData.HeavyCycle_option = 3;
				MachineSetupData.CycleModifierExtraWashTime = 2;
			}
			else if ( deviceStatus.deviceType[0] == PROGRAMMING_DATA_WASHER_EXTRACTOR )
			{
				MachineSetupData.HeavyCycle_vendPrice[0] = 0;
				MachineSetupData.HeavyCycle_vendPrice[1] = 50;
				MachineSetupData.MediumCycle_vendPrice[0] = 0;
				MachineSetupData.MediumCycle_vendPrice[1] = 25;
				MachineSetupData.MediumCycle_option = 2;
				MachineSetupData.HeavyCycle_option = 3;
				MachineSetupData.NormalCycle_washAgitateTime = 2;
				MachineSetupData.NormalCycle_rinseAgitateTime = 4;
				MachineSetupData.NormalCycle_extraRinseAgitateTime = 0;
			}
			if(tmp[14] & 0x08)
			{
				MachineSetupData.AudioSetting = 29;	//enable audio
			}
			else
			{
				MachineSetupData.AudioSetting = 0;	//disable audio
			}
			MachineSetupData.CardReaderDisplayControl = tmp[14] & 0x20;	//let machine handle display
		}
	}	
	else if( deviceStatus.deviceType[0] == PROGRAMMING_DATA_TUMBLER  || deviceStatus.deviceType[0] == PROGRAMMING_DATA_DRYER )
	{
		halGetEeprom(MACHINE_SETUP_ADDR, 18, (u8*)&tmp);
		vendPrice = (tmp[1] * 256 + tmp[2]) / 100.0;
		
		MachineSetupData.KeyPressWaitTime = tmp[0];
		MachineSetupData.VendPrice[0] = tmp[1];
		MachineSetupData.VendPrice[1] = tmp[2];
		MachineSetupData.Coin1[0] = tmp[5];
		MachineSetupData.Coin1[1] = tmp[6];
		MachineSetupData.Coin2[0] = tmp[5];
		MachineSetupData.Coin2[1] = tmp[6];
		MachineSetupData.StartPulse[0] = tmp[7];
		MachineSetupData.StartPulse[1] = tmp[8];
		MachineSetupData.CycleLength = tmp[9];
		MachineSetupData.ControlConfig = tmp[10];
		MachineSetupData.DefaultCycle = tmp[11];
		MachineSetupData.CoolDownTime = tmp[12];
		MachineSetupData.Coin1TopoffTime = tmp[13];
		MachineSetupData.Coin2TopoffTime = tmp[13];
		MachineSetupData.HighTempSetting = tmp[14];
		MachineSetupData.MediumTempSetting = tmp[15];
		MachineSetupData.LowTempSetting = tmp[16];
		MachineSetupData.DelicateTempSetting = tmp[17];
		
		
		
		MachineSetupData.ModifiedCycleVendPrice[0] = tmp[3];
		MachineSetupData.ModifiedCycleVendPrice[1] = tmp[4];
		
		if(deviceStatus.deviceType[1] == QUANTUM_SERIES)
		{
			MachineSetupData.VendPrice1[0] = tmp[1]; //heat vend
			MachineSetupData.VendPrice1[1] = tmp[2];
			MachineSetupData.VendPrice2[0] = tmp[5];//no heat vend
			MachineSetupData.VendPrice2[1] = tmp[6];
			if (MachineSetupData.TopoffTime)
			{
				MachineSetupData.TopoffOn = 1;
			}
			
			if(tmp[10] & 0x08)
			{
				MachineSetupData.AudioSetting = 29; //enable audio
			}
			else
			{
				MachineSetupData.AudioSetting = 0; //disable audio
			}
			MachineSetupData.CardReaderDisplayControl = tmp[10] & 0x20; //let machine handle display
			
		}	
	}
	
}
void getSQAuditData(void)
{
	u8 temp[3]={0};
	u8 buf[43] = {0};
			
	temp[0] = AUDIT_DATA_PACKET_SIZE;	//Number of actual data bytes to be transferred to the MDC including command data exclude bbc byte
	temp[1] = AUDIT_DATA_REQUEST;		//MDC command to be requested 

	if(sendSQData(temp))	//Send request command to MDC.
	{
		buf[0] = 43;
		if(getSQData(buf))
		{
			
		}
		sendSQCardRemoved();
	}	
	return;
}


bool getSQMachineStatus(void)
{
	u8 temp[3];
	
	temp[0] = STATUS_REQUEST_SIZE;
	if( MdcStatus.MachineType[1] == QUANTUM_SERIES)
	{
		temp[1] = QUANTUM_STATUS_REQUEST;
	}
	else
	{
		temp[1] = MDC_STATUS_REQUEST;				
	}
	temp[2] = CurrentAccount.CardState;
		
	if( sendSQData(temp) )
	{
		if ( getSQStatusData() )
		{
			if (ReaderStateFlag.ReaderSetup == READER_SETUP_DONE)
			{
				_delay_ms(3);	 //wait for at least 1ms to allow machine time to process
				sendSQVendPrice();					
			}
			return true;			
		}
	}
	return false;
}

bool getSQStatusData(void)
{
	
	
	if( MdcStatus.MachineType[1] == QUANTUM_SERIES)
	{
		u8 buf[QUANTUM_STATUS_DATA_SIZE] = {0};
	
		buf[0] = QUANTUM_STATUS_DATA_SIZE;
		if( getSQData(buf) && buf[0] == QUANTUM_STATUS_RESPONSE)
		{
			MdcStatus.MachineType[0] 	= (buf[1] + PROGRAMMING_DATA);			
			//MdcStatus.KeypadData  		= buf[2];
			MdcStatus.CycleType   		= buf[3];
			MdcStatus.CmdToReader 		= buf[4];
			MdcStatus.MachineStatus[0] 	= buf[5];
			MdcStatus.MachineStatus[1] 	= buf[6];
			MdcStatus.RemainingCycleMinutes		= buf[9];
			MdcStatus.RemainingCycleSeconds		= buf[10];
			//MdcStatus.KeypadData[0]	  			= buf[13];
			//MdcStatus.KeypadData[1]  			= buf[14];
			MdcStatus.CurrentKey[0]				= buf[15];
			MdcStatus.CurrentKey[1]				= buf[16];
			MdcStatus.CycleModifier[0]			= buf[17];
			
			return true;
		}
	}
	else if( MdcStatus.MachineType[1] == MDC_SERIES)
	{
		u8 buf[MDC_STATUS_DATA_SIZE] = {0};
		
		buf[0] = MDC_STATUS_DATA_SIZE;
		if( getSQData(buf) && buf[0] == MDC_STATUS_RESPONSE )
		{
			MdcStatus.MachineType[0] 	= (buf[1] + PROGRAMMING_DATA);			
			//MdcStatus.KeypadData[0]		= buf[2];
			MdcStatus.CycleType   		= buf[3];
			MdcStatus.CmdToReader 		= buf[4];
			MdcStatus.MachineStatus[0] 	= buf[5];
			MdcStatus.MachineStatus[1] 	= buf[6];
			MdcStatus.RemainingCycleMinutes		= buf[9];
			MdcStatus.RemainingCycleSeconds		= buf[10];
			
			return true;
		}
	}	
	return false;	
}


bool sendSQVendPrice(void)
{
	u8 temp[8];
	
	
	if( MdcStatus.MachineType[1] == QUANTUM_SERIES)
	{
		temp[0] = QUANTUM_VEND_PRICE_SIZE;
		temp[1] = QUANTUM_VEND_PRICE;
		
		if ( MdcStatus.CycleType == HEAVY_HIGH_TEMP && ReaderStateFlag.showModifiedVendPrice == true )
		{
			temp[2] = MachineSetupData.ModifiedCycleVendPrice[0];
			temp[3] = MachineSetupData.ModifiedCycleVendPrice[1];
		}
		else
		{			
			temp[2] = MachineSetupData.VendPrice[0];
			temp[3] = MachineSetupData.VendPrice[1];
		}
		temp[4] = 0;//MachineSetupData.ModifiedCycleVendPrice[0];
		temp[5] = 0;//MachineSetupData.ModifiedCycleVendPrice[1];
		temp[6] = MachineSetupData.TopoffTime;
		temp[7] = MachineSetupData.CycleChangeResponse;
	}
	else
	{
		temp[0] = VEND_PRICE_SIZE;
		temp[1] = VEND_PRICE_PACKET;
		if ( MdcStatus.CycleType == HEAVY_HIGH_TEMP && ReaderStateFlag.showModifiedVendPrice == true )
		{
			temp[2] = MachineSetupData.ModifiedCycleVendPrice[0];
			temp[3] = MachineSetupData.ModifiedCycleVendPrice[1];
		}
		else
		{
			temp[2] = MachineSetupData.VendPrice[0];
			temp[3] = MachineSetupData.VendPrice[1];
		}
		temp[4] = MachineSetupData.TieredDryerPricing;				
	}
	return (sendSQData(temp) );	
}	
bool isQuantum(void)
{
	u8 temp[8] = {0x07, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};
		
	temp[5] = __APP_MINOR__;
	temp[6] = __APP_REVISION__;
	                                                             
	if( sendSQData(temp) )
	{
		if ( getSQInitData() )
		{
			return true;
		}
	}
	return false;		
}

bool isMDC(void)
{
	return false;
}

//retrieve initial data from Quantum machines and save it to mdcstatus structure
bool getSQInitData()
{
	u8 buf[10];
	
	buf[0] = 10;
	if ( getSQData(buf) )
	{
		//do something with the returned init data
		MachineSetupData.ProductByte[0] = buf[1];
		MachineSetupData.ProductByte[1] = buf[2];
		MachineSetupData.ProductByte[2] = buf[3];
		MachineSetupData.ControlConfig  = buf[4];
		return true;
	}
	return false;
}

	
bool sendSQData(u8 *buf)
{
	u8 n, numberOfBytes, BCC, i;
	//static u8 ucTimerCount;
	
	for(n = 0; n < NUMRETRIES; n++)
	{
		BCC = STX;
		init_rx_buf();
		numberOfBytes = (buf[0] + 1);	//number of bytes sending
		serial_putchar(STX);    // Send STX, start byte


	    for (i = 0; i < numberOfBytes; i++)
	    {
	        serial_putchar(buf[i]);
			BCC = (BCC ^ buf[i]);
		}		
	
	    serial_putchar(BCC);    // Send BCC, error checking byte
		//start timer 350ms
		//wait for response

		ucTimerCount = 0;
		sei();
		StartTimerZero();
		while(ucTimerCount < 70)
		{
			if(rx_buf_count())
				break;
            else if( mac_event_pending() ){
                macTask();  //timer has timed out handle timer task.
            }//to cut down on missed calls while waiting for machine to respond
		}
		StopTimerZero();

		if(ucTimerCount < 70)	//a byte has been received.
		{
			i = serial_getchar();
			if(i == ACK)
				return 1;	//Command received successfully
			else if(i == INV)
				return 0;	//Invalid command sent				
		}//if not time out leave.
		_delay_ms(3);
	}// Retries
return false;
}

bool getSQData(u8 *buf)
{
	
	 u8 i,n,BCC;

	
	//get machine data
	i = serial_getchar();
	if( i == STX )//start byte detected
	{	
		i = serial_getchar(); // get data field size.
		BCC = (i ^ STX);
		
		if ( i > buf[0] )	//protect against buffer overrun.
		{
			return false;
		}
		
		for(n = 0; n < i; n++)
		{			
			buf[n]= serial_getchar();
			BCC = (BCC ^ buf[n]);
		}
		
		i = serial_getchar();	// get BCC
		if(BCC == i)
		{
			serial_putchar(ACK);			
			return 1;
		}
		else
		{
			serial_putchar(NAK);	
		}		
	}
	return 0;
}

bool waitSQStartKey(void)
{
	if( MdcStatus.MachineType[1] == MDC_SERIES)
	{
		if( (MdcStatus.MachineStatus[0] & MACHINE_READY_MODE) || ((MdcStatus.MachineStatus[0] == MACHINE_RUN_MODE) && 
			(MdcStatus.MachineType[0] == PROGRAMMING_DATA_DRYER)) || ((MdcStatus.MachineStatus[0] == MACHINE_RUN_MODE) && (MdcStatus.MachineType[0] == PROGRAMMING_DATA_TUMBLER)))
							
		return true;
	}
	else if( MdcStatus.MachineType[1] == QUANTUM_SERIES)
	{
		if( (MdcStatus.MachineStatus[1] & MACHINE_READY_MODE) || ((MdcStatus.MachineStatus[1] == MACHINE_RUN_MODE) && 
			(MdcStatus.MachineType[0] == PROGRAMMING_DATA_DRYER)) || ((MdcStatus.MachineStatus[1] == MACHINE_RUN_MODE) && (MdcStatus.MachineType[0] == PROGRAMMING_DATA_TUMBLER)))
		
		return true;
	}
	
	return false;
}	

bool isSQCycleRunning(void)
{
	return ( (MdcStatus.MachineStatus[0] == MACHINE_RUN_MODE) || MdcStatus.MachineStatus[1] == MACHINE_RUN_MODE );
}

bool sendSQCashCardInserted(void)
{	
	u8 temp[6] = {0};
		
	if( MdcStatus.MachineType[1] == QUANTUM_SERIES)
	{			
		temp[0] = 3;
		temp[1] = CASH_CARD_INSERTED;
		temp[2] = CurrentAccount.Value >> 8;
		temp[3] = CurrentAccount.Value;		
	}
	else
	{			
		temp[0] = CASH_CARD_SIZE;
		temp[1] = CASH_CARD_INSERTED;
		temp[2] = CurrentAccount.Value >> 8;
		temp[3] = CurrentAccount.Value;
		temp[4] = MachineSetupData.VendPrice[0];
		temp[5] = MachineSetupData.VendPrice[1];		
	}	
	
	return (sendSQData(temp) );
}

bool sendSQCashCardInsetedForTopoff(void)
{	
	u8 temp[7] = {0};
		
	if( MdcStatus.MachineType[1] == QUANTUM_SERIES)
	{		
		temp[0] = 3;
		temp[1] = CASH_CARD_TOPOFF;
		temp[2] = CurrentAccount.Value >> 8;
		temp[3] = CurrentAccount.Value;		
	}
	else
	{	
		temp[0] = CASH_CARD_TOPOFF_SIZE;
		temp[1] = CASH_CARD_TOPOFF;
		temp[2] = CurrentAccount.Value >> 8;
		temp[3] = CurrentAccount.Value;
		temp[4] = MachineSetupData.VendPrice[0];
		temp[5] = MachineSetupData.VendPrice[1];
		temp[6] = MachineSetupData.Coin1TopoffTime;		
	}	
	
	return (sendSQData(temp) );	
}

bool sendSQCardRemoved(void)
{
	u8 temp[2] = {0};
			
	temp[0] = CARD_REMOVED_SIZE;
	temp[1] = CARD_REMOVED;
	
	return (sendSQData(temp) );
}

bool sendSQCashCardRemoved(void)
{
	u8 temp[7] = {0};
		
	if( MdcStatus.MachineType[1] == QUANTUM_SERIES)
	{	
		temp[0] = 3;
		temp[1] = CARD_REMOVED;
		temp[2] = CurrentAccount.Value >> 8;
		temp[3] = CurrentAccount.Value;		
	}
	else
	{			
		temp[0] = CASH_CARD_REMOVED_SIZE;
		temp[1] = CARD_REMOVED;
		temp[2] = CurrentAccount.Value >> 8;
		temp[3] = CurrentAccount.Value;
		temp[4] = MachineSetupData.VendPrice[0];
		temp[5] = MachineSetupData.VendPrice[1];
	}	
	
	return (sendSQData(temp) );	
}

bool sendSQStart(void)
{
	u8 temp[2] = {0};
			
	temp[0] = MACHINE_START_PACKET_SIZE;
	temp[1] = MACHINE_START_COMMAND;
	
	return (sendSQData(temp) );	
}

bool sendSQAddTime(void)
{
	u8 temp[2] = {0};
			
	temp[0] = ADD_TIME_PACKET_SIZE;
	temp[1] = ADD_TIME_COMMAND;
	
	return (sendSQData(temp) );	
}

bool sendSQCardErrorCode(u8 cardErrorCode)
{
	u8 temp[3] = {0};
			
	temp[0] = CARD_ERROR_PACKET_SIZE;
	temp[1] = CARD_ERROR_PACKET;
	temp[2] = cardErrorCode;
	
	return (sendSQData(temp) );		
}

bool sendSQAudioBeepRequest(u8 beepLength)
{
	u8 temp[3] = {0};
			
	temp[0] = AUDIO_BEEP_REQUEST_SIZE;
	temp[1] = AUDIO_BEEP_REQUEST;
	temp[2] = beepLength;
	
	return (sendSQData(temp) );		
}

/**********************************************************************************************
bool sendSQDisplayCommand(u8 led1, u8 led2,u8 led3, u8 led4, u8 duration) 

Parameter:  u8 led1, u8 led2,u8 led3, u8 led4, u8 duration
Description: This function will be used to send text to the MDC 4 position 7 segment display.
Led1 to led4 are limited to values defined in "display.h"
***********************************************************************************************
*/

bool sendSQDisplayCommand( u8 led1, u8 led2,u8 led3, u8 led4, u8 duration) 
{
	u8 temp[10]={0};
	
	temp[0] = DISPLAY_REQUEST_SIZE;	//Number of actual data bytes to be transferred to the MDC including command data exclude bbc byte
	temp[1] = DISPLAY_REQUEST;		//MDC command to be requested 
	temp[2] = led1;				//Left most digit display character 4
	temp[3] = led2;				//Display character 3
	temp[4] = led3;				//Display character 2
	temp[5] = led4;				//Display character 1
	temp[6] = duration;			//Duration of display in seconds
															
	return (sendSQData(temp) );
}

/*****************************************************************
This function will display the current version of reader firmware
on the MDC display for a minimum of 2 seconds.

*/

bool displaySQReaderVersion(void)
{	
	if( sendSQDisplayCommand(LED_r, msgNumber[__APP_MAJOR__],msgNumber[__APP_MINOR__],msgNumber[__APP_REVISION__],5) )
	{
		_delay_ms(2000);
		return true;	
	}
	return false;	
}	

bool displaySQCardError(void)
{
	if( sendSQDisplayCommand(LED_C, LED_A,LED_r,LED_d,4) )
	{
		_delay_ms(2000);
		if( sendSQDisplayCommand(LED_E, LED_r,LED_r,LED_BLANK,3) )
			_delay_ms(2000);
			
		return true;	
	}
	return false;	
}	

bool cardRemoved(u8 cardType)
{
	if( cardType == CASH_CARD ) 
	{
		if( sendSQCashCardRemoved() )
		{
			CurrentAccount.CardState = CARD_OUT;
			return true;			
		}
		
	}
	else
	{
	
		if ( sendSQCardRemoved() )
		{
			CurrentAccount.CardState = CARD_OUT;
			return true;
		}
	}		
	return false;
}			
bool cardInserted(u8 cardType)
{
	if( (cardType == REGULAR_VEND) && sendSQCashCardInserted() )
	{
		CurrentAccount.CardState = CARD_IN;
		return true;
	}
	else if ( (cardType == TOPOFF_VEND) && sendSQCashCardInsetedForTopoff() )
	{
		CurrentAccount.CardState = CARD_IN;
		return true;
	}
	return false;
}
void initMachine(void)
{
	if( isQuantum() )
	{
		//set q flag
		MdcStatus.MachineType[1] = QUANTUM_SERIES;
		
	}
	else //if (isMDC())
	{
		//set MDC flag
		MdcStatus.MachineType[1] = MDC_SERIES;
		
	}
	
	_delay_ms(2);
	MachineStatus();
	deviceStatus.deviceType[0] = MdcStatus.MachineType[0];
	deviceStatus.deviceType[1] = MdcStatus.MachineType[1];
}

bool isMachineCycleRunning()
{
	return (isSQCycleRunning() );
}
bool waitForMachineStartKey()
{
	
	return waitSQStartKey();
}

void MachineStatus(void)
{
  
	if( getSQMachineStatus() )
	{
		//set machine status ok flag;
		ucDeviceStateFlag = DEVICE_STATUS_ONLINE; //machine status has been done
	}
	
}

bool setupMachine(void)
{
	u8 temp[MDC_MAX_PACKET_SIZE] = {0};
	
	//getSQSetupData(); 
	if( deviceStatus.deviceType[1] == QUANTUM_SERIES)
	{
		if ( deviceStatus.deviceType[0] == PROGRAMMING_DATA_TOPLOAD )
		{
			temp[0] = QTL_PROGRAMMING_DATA_SIZE;
			temp[1] = deviceStatus.deviceType[0];
			temp[2] = MachineSetupData.ProductByte[0];
			temp[3] = MachineSetupData.ProductByte[1];
			temp[4] = MachineSetupData.ProductByte[2];
			temp[5] = 0;
			temp[6] = 0;
			temp[7] = MachineSetupData.VendPrice[0];
			temp[8] = MachineSetupData.VendPrice[1];
			temp[9] = MachineSetupData.VendPrice1[0];
			temp[10] = MachineSetupData.VendPrice1[1];
			temp[11] = MachineSetupData.VendPrice2[0];
			temp[12] = MachineSetupData.VendPrice2[1];
			temp[13] = MachineSetupData.VendPrice3[0];
			temp[14] = MachineSetupData.VendPrice3[0];
			temp[15] = MachineSetupData.VendPrice4[1];
			temp[16] = MachineSetupData.VendPrice4[0];
			temp[17] = MachineSetupData.VendPrice5[1];
			temp[18] = MachineSetupData.VendPrice5[0];
			temp[19] = MachineSetupData.VendPrice6[0];
			temp[20] = MachineSetupData.VendPrice6[1];
			temp[21] = MachineSetupData.VendPrice7[0];
			temp[22] = MachineSetupData.VendPrice7[1];
			temp[23] = MachineSetupData.VendPrice8[0];
			temp[24] = MachineSetupData.VendPrice8[1];
			temp[25] = MachineSetupData.HeavyCycle_vendPrice[0];
			temp[26] = MachineSetupData.HeavyCycle_vendPrice[1];
			temp[27] = MachineSetupData.HeavyCycle_option;
			temp[28] = MachineSetupData.HeavyCycle_extraWashTime;
			temp[29] = MachineSetupData.HeavyCycle_extraRiseTime;
			temp[30] = MachineSetupData.HeavyCycle_warmRinseOption;
			temp[31] = MachineSetupData.NormalCycle_washAgitateTime;
			temp[32] = MachineSetupData.NormalCycle_rinseAgitateTime;
			temp[33] = MachineSetupData.NormalCycle_rinseAgitateTime;
			temp[34] = MachineSetupData.NormalCycle_finalSpinTime;
			temp[35] = MachineSetupData.PermPressCycle_washAgitateTime;
			temp[36] = MachineSetupData.PermPressCycle_extraRinseAgitateTime;
			temp[37] = MachineSetupData.PermPressCycle_rinseAgitateTime;
			temp[38] = MachineSetupData.PermPressCycle_finalSpinTime;
			temp[39] = MachineSetupData.DelicateCycle_washAgitateTime;
			temp[40] = MachineSetupData.DelicateCycle_extraRinseAgitateTime;
			temp[41] = MachineSetupData.DelicateCycle_rinseAgitateTime;
			temp[42] = MachineSetupData.DelicateCycle_finalSpinTime;
			temp[43] = MachineSetupData.DefaultCycle;
			temp[44] = MachineSetupData.AudioSetting;
			temp[45] = MachineSetupData.CardReaderDisplayControl;
		}
		if ( deviceStatus.deviceType[0] == PROGRAMMING_DATA_FRONTLOAD )
		{
			temp[0] = QFL_PROGRAMMING_DATA_SIZE;
			temp[1] = deviceStatus.deviceType[0];
			temp[2] = MachineSetupData.ProductByte[0];
			temp[3] = MachineSetupData.ProductByte[1];
			temp[4] = MachineSetupData.ProductByte[2];
			temp[5] = 0;
			temp[6] = 0;
			temp[7] = MachineSetupData.VendPrice[0];
			temp[8] = MachineSetupData.VendPrice[1];
			temp[9] = MachineSetupData.VendPrice1[0];
			temp[10] = MachineSetupData.VendPrice1[1];
			temp[11] = MachineSetupData.VendPrice2[0];
			temp[12] = MachineSetupData.VendPrice2[1];
			temp[13] = MachineSetupData.VendPrice3[0];
			temp[14] = MachineSetupData.VendPrice3[0];
			temp[15] = MachineSetupData.VendPrice4[1];
			temp[16] = MachineSetupData.VendPrice4[0];
			temp[17] = MachineSetupData.VendPrice5[1];
			temp[18] = MachineSetupData.VendPrice5[0];
			temp[19] = MachineSetupData.VendPrice6[0];
			temp[20] = MachineSetupData.VendPrice6[1];
			temp[21] = MachineSetupData.VendPrice7[0];
			temp[22] = MachineSetupData.VendPrice7[1];
			temp[23] = MachineSetupData.VendPrice8[0];
			temp[24] = MachineSetupData.VendPrice8[1];
			temp[25] = MachineSetupData.MediumCycle_vendPrice[0];
			temp[26] = MachineSetupData.MediumCycle_vendPrice[1];
			temp[27] = MachineSetupData.HeavyCycle_vendPrice[0];
			temp[28] = MachineSetupData.HeavyCycle_vendPrice[1];
			temp[29] = MachineSetupData.MediumCycle_option;
			temp[30] = MachineSetupData.HeavyCycle_option;
			temp[31] = MachineSetupData.CycleModifierExtraWashTime;
			temp[32] = MachineSetupData.DefaultCycle;
			temp[33] = MachineSetupData.AudioSetting;
			temp[34] = MachineSetupData.CardReaderDisplayControl;
		}
		if ( deviceStatus.deviceType[0] == PROGRAMMING_DATA_WASHER_EXTRACTOR )
		{
			temp[0] = QWE_PROGRAMMING_DATA_SIZE;
			temp[1] = deviceStatus.deviceType[0];
			temp[2] = MachineSetupData.ProductByte[0];
			temp[3] = MachineSetupData.ProductByte[1];
			temp[4] = MachineSetupData.ProductByte[2];
			temp[5] = MachineSetupData.ControlConfig;
			temp[6] = 0;
			temp[7] = MachineSetupData.VendPrice[0];
			temp[8] = MachineSetupData.VendPrice[1];
			temp[9] = MachineSetupData.VendPrice1[0];
			temp[10] = MachineSetupData.VendPrice1[1];
			temp[11] = MachineSetupData.VendPrice2[0];
			temp[12] = MachineSetupData.VendPrice2[1];
			temp[13] = MachineSetupData.VendPrice3[0];
			temp[14] = MachineSetupData.VendPrice3[0];
			temp[15] = MachineSetupData.VendPrice4[1];
			temp[16] = MachineSetupData.VendPrice4[0];
			temp[17] = MachineSetupData.VendPrice5[1];
			temp[18] = MachineSetupData.VendPrice5[0];
			temp[19] = MachineSetupData.VendPrice6[0];
			temp[20] = MachineSetupData.VendPrice6[1];
			temp[21] = MachineSetupData.VendPrice7[0];
			temp[22] = MachineSetupData.VendPrice7[1];
			temp[23] = MachineSetupData.VendPrice8[0];
			temp[24] = MachineSetupData.VendPrice8[1];
			temp[25] = MachineSetupData.ModifiedCycleVendPrice[0];
			temp[26] = MachineSetupData.ModifiedCycleVendPrice[1];
			temp[27] = MachineSetupData.ModifiedCycleVendPrice[0];
			temp[28] = MachineSetupData.ModifiedCycleVendPrice[1];
			temp[29] = MachineSetupData.MediumCycle_vendPrice[0];
			temp[30] = MachineSetupData.MediumCycle_vendPrice[1];
			temp[31] = MachineSetupData.HeavyCycle_vendPrice[0];
			temp[32] = MachineSetupData.HeavyCycle_vendPrice[1];
			temp[33] = MachineSetupData.MediumCycle_option;
			temp[34] = MachineSetupData.HeavyCycle_option;
			temp[35] = MachineSetupData.CycleModifierExtraWashTime;
			temp[36] = MachineSetupData.DefaultCycle;
			temp[37] = MachineSetupData.AudioSetting;
			temp[38] = MachineSetupData.CardReaderDisplayControl;
		}
		if ( deviceStatus.deviceType[0] == PROGRAMMING_DATA_DRYER || deviceStatus.deviceType[0] == PROGRAMMING_DATA_TUMBLER )
		{
			temp[0] = QDT_PROGRAMMING_DATA_SIZE;
			temp[1] = deviceStatus.deviceType[0];
			temp[2] = MachineSetupData.ProductByte[0];
			temp[3] = MachineSetupData.ProductByte[1];
			temp[4] = MachineSetupData.ProductByte[2];
			temp[5] = MachineSetupData.ControlConfig;
			temp[6] = 0;
			temp[7] = MachineSetupData.VendPrice[0]; //heat vend price
			temp[8] = MachineSetupData.VendPrice[1];
			temp[9] = MachineSetupData.VendPrice1[0];// no heat vend price
			temp[10] = MachineSetupData.VendPrice1[1];
			temp[11] = MachineSetupData.VendPrice2[0]; //topoff vend price
			temp[12] = MachineSetupData.VendPrice2[1];
			temp[13] = MachineSetupData.CycleLength; //heat cycle time in minutes
			temp[14] = 0; //heat cycle time in seconds
			temp[15] = MachineSetupData.CycleLength; //no heat cycle time in minutes
			temp[16] = 0; //no heat cycle time in seconds
			temp[17] = MachineSetupData.CoolDownTime;
			temp[18] = MachineSetupData.CoolDownTime;
			temp[19] = MachineSetupData.CoolDownTime;
			temp[20] = MachineSetupData.CoolDownTime;
			temp[21] = MachineSetupData.TopoffOn;
			temp[22] = MachineSetupData.TopoffTime;
			temp[23] = 0; //seconds
			temp[24] = MachineSetupData.HighTempSetting;
			temp[25] = MachineSetupData.MediumTempSetting;
			temp[26] = MachineSetupData.LowTempSetting;
			temp[27] = MachineSetupData.DelicateTempSetting;
			temp[28] = MachineSetupData.DefaultCycle;
			temp[29] = MachineSetupData.AudioSetting;
			temp[30] = MachineSetupData.CardReaderDisplayControl;
			temp[31] = 0; //fahrenheit
			temp[32] = 1; //display minutes only
			temp[33] = 0; //disable reversing mode
		}		
	}
	else if( deviceStatus.deviceType[1] == MDC_SERIES /*|| MdcStatus.MachineType[1] == QUANTUM_SERIES */)
	{
		temp[0] = PROGRAMMING_DATA_SIZE;
		temp[1] = deviceStatus.deviceType[0];
		temp[2] = MachineSetupData.VendPrice[0];
		temp[3] = MachineSetupData.VendPrice[1];
		temp[4] = MachineSetupData.Coin1[0];
		temp[5] = MachineSetupData.Coin1[1];	
		temp[6] = MachineSetupData.Coin2[0];
		temp[7] = MachineSetupData.Coin2[1];	
		temp[8] = MachineSetupData.StartPulse[0];
		temp[9] = MachineSetupData.StartPulse[1];
		temp[10] = MachineSetupData.CycleLength;
	
		if(MdcStatus.MachineType[0] == PROGRAMMING_DATA_DRYER || MdcStatus.MachineType[0] == PROGRAMMING_DATA_TUMBLER)
		{
			temp[11] = MachineSetupData.CoolDownTime;
			temp[12] = MachineSetupData.Coin1TopoffTime;
			temp[13] = MachineSetupData.Coin2TopoffTime;
			temp[14] = MachineSetupData.HighTempSetting;
			temp[15] = MachineSetupData.MediumTempSetting;
			temp[16] = MachineSetupData.LowTempSetting;
			temp[17] = MachineSetupData.DelicateTempSetting;
			temp[18] = MachineSetupData.ControlConfig;
			temp[19] = MachineSetupData.DefaultCycle;
		}
		else
		{
			temp[11] = MachineSetupData.ControlConfig;
			temp[12] = MachineSetupData.DefaultCycle;
		}
	}
	return (sendSQData(temp) );		
}

bool startMachineCycle()
{
	u8 temp[2];
	
	temp[0] = MACHINE_START_PACKET_SIZE;
	temp[1] = MACHINE_START_COMMAND;
	
	return (sendSQData(temp) );	
}
#endif