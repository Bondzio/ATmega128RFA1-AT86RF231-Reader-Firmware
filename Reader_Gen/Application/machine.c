
#include "mac.h"

#ifndef COORNODE

#include "reader.h"
#include "serial.h"
#include "machine.h"
#include "avr_timer.h"
#include "timer_enddevice.h"
#include <avr/wdt.h>
#include <util/delay.h>
#include "hal_avr.h"
#include "eeprom_map.h"

#include "mac_event.h"
#include "SpeedQueen.h"

u16 calcMachineCRC(u8 *msg, u8 len);
#define MAX_ALLOWED_ARRAY_SIZE  50

#if ( DEVICE_CONNECTED == MDC_MACHINE )

u8 ucTopOffTime;
u8 ucCardErrorCode;
u16 ucCardBalance;
u16 cardId;
double vendPrice;

u8 ucTimerCount;

void WaitForMachine(void)
{
	ucMachineWait = MACHINE_TIME_OUT;
}

u8 SerialGetMachineData(void)
{
	static u8 ucMdcDataBuf[MDC_MAX_PACKET_SIZE];
	static u8 i,n,BCC;

	
	//get machine data
	i = serial_getchar();
	if( i == STX )//state byte detected
	{
		i = serial_getchar(); // get data field size.
		BCC = (i ^ STX);

		for(n = 0; n < i; n++)
		{
			
			ucMdcDataBuf[n]= serial_getchar();
			BCC = (BCC ^ ucMdcDataBuf[n]);
		}
		i = serial_getchar();	// get BCC
		if(BCC == i)
		{
			switch( ucMdcDataBuf[0])
			{
				case MACHINE_STATUS_PACKET:
				MdcStatus.MachineType[0] 	= (ucMdcDataBuf[1] + PROGRAMMING_DATA);
				MdcStatus.MachineType[1] = 1;///temp. fix later, differentiate between MDC and Quantum
				//MdcStatus.KeypadData[0]	= ucMdcDataBuf[2];
				MdcStatus.CycleType   	= ucMdcDataBuf[3];
				MdcStatus.CmdToReader 	= ucMdcDataBuf[4];
				MdcStatus.MachineStatus[0] 	= ucMdcDataBuf[5];
				MdcStatus.MachineStatus[1] 	= ucMdcDataBuf[6];
				MdcStatus.RemainingCycleMinutes		= ucMdcDataBuf[9];
				MdcStatus.RemainingCycleSeconds		= ucMdcDataBuf[10];
				
				break;
				case AUDIT_DATA_TOPLOAD:
				case AUDIT_DATA_FRONTLOAD:
				case AUDIT_DATA_WASHER_EXTRACTOR:
				case AUDIT_DATA_TUMBLER:
				case AUDIT_DATA_DRYER:
				MdcAuditData.MachineType[0] = ucMdcDataBuf[1];
				MdcAuditData.ControlSVN = ucMdcDataBuf[2];
				MdcAuditData.VendPrice[0] = ucMdcDataBuf[3];
				MdcAuditData.VendPrice[1] = ucMdcDataBuf[4];
				MdcAuditData.Coin1Value [0] = ucMdcDataBuf[5];
				MdcAuditData.Coin1Value[1] = ucMdcDataBuf[6];
				MdcAuditData.Coin2Value [0] = ucMdcDataBuf[7];
				MdcAuditData.Coin2Value[1] = ucMdcDataBuf[8];
				MdcAuditData.NumStartPulse[0] = ucMdcDataBuf[9];
				MdcAuditData.NumStartPulse [1]= ucMdcDataBuf[10];
				MdcAuditData.CycleConfig = ucMdcDataBuf[11];
				if (ucMdcDataBuf[0] == AUDIT_DATA_FRONTLOAD || ucMdcDataBuf[0] == AUDIT_DATA_TOPLOAD ||
				ucMdcDataBuf[0] == AUDIT_DATA_WASHER_EXTRACTOR)
				{
					MdcAuditData.ControlConfig = ucMdcDataBuf[12];
					MdcAuditData.Coin1Counter[0] = ucMdcDataBuf[13];
					MdcAuditData.Coin1Counter[1] = ucMdcDataBuf[14];
					MdcAuditData.Coin2Counter[0] = ucMdcDataBuf[15];
					MdcAuditData.Coin2Counter[1] = ucMdcDataBuf[16];
					MdcAuditData.NumCycles[0] = ucMdcDataBuf[17];
					MdcAuditData.NumCycles[1] = ucMdcDataBuf[18];
					MdcAuditData.NumStartPulse[0] = ucMdcDataBuf[19];
					MdcAuditData.NumStartPulse[1] = ucMdcDataBuf[20];
					MdcAuditData.AvgFillTime[0] = ucMdcDataBuf[21];
					MdcAuditData.AvgFillTime[1] = ucMdcDataBuf[22];
					MdcAuditData.AvgDrainTime[0] = ucMdcDataBuf[23];
					MdcAuditData.AvgDrainTime[1] = ucMdcDataBuf[24];
				}
				else if (ucMdcDataBuf[0] == AUDIT_DATA_TUMBLER || ucMdcDataBuf[0] == AUDIT_DATA_DRYER)
				{
					MdcAuditData.CoolDownTime = ucMdcDataBuf[12];
					MdcAuditData.Coin1TopoffTime = ucMdcDataBuf[13];
					MdcAuditData.Coin2TopoffTime = ucMdcDataBuf[14];
					MdcAuditData.HighTempSetting = ucMdcDataBuf[15];
					MdcAuditData.MedTempSetting = ucMdcDataBuf[16];
					MdcAuditData.LowTempSetting = ucMdcDataBuf[17];
					MdcAuditData.DeliTempSetting = ucMdcDataBuf[18];
					MdcAuditData.ControlConfig = ucMdcDataBuf[19];
					MdcAuditData.Coin1Counter[0] = ucMdcDataBuf[20];
					MdcAuditData.Coin1Counter[1] = ucMdcDataBuf[21];
					MdcAuditData.Coin2Counter[0] = ucMdcDataBuf[22];
					MdcAuditData.Coin2Counter[1] = ucMdcDataBuf[23];
					MdcAuditData.NumCycles[0] = ucMdcDataBuf[24];
					MdcAuditData.NumCycles[1] = ucMdcDataBuf[25];
					MdcAuditData.Coin1TopoffCounter[0] = ucMdcDataBuf[26];
					MdcAuditData.Coin1TopoffCounter[1] = ucMdcDataBuf[27];
					MdcAuditData.Coin2TopoffCounter[0] = ucMdcDataBuf[28];
					MdcAuditData.Coin2TopoffCounter[1] = ucMdcDataBuf[29];
					MdcAuditData.NumStartPulse[0] = ucMdcDataBuf[30];
					MdcAuditData.NumStartPulse[1] = ucMdcDataBuf[31];
					MdcAuditData.NumTopoffStartPulses[0] = ucMdcDataBuf[32];
					MdcAuditData.NumTopoffStartPulses[1] = ucMdcDataBuf[33];
				}
				break;
				default:
				break;
			}

			return 1;
		}
	}
	return 0;
}

void InitTimerZero(void)
{	
	TIFR0 = 2;//1<<TOV0;					//clear TOV0 / clear interrupt
	TIMSK0 = 2;//1<<TOIE0;				//enable timer0 overflow interrupt
	TCNT0 = 0;//16;
	ucTimerCount = 0;
}

void StopTimerZero(void)
{
	TCCR0B = 0;//~(1<<CS02) | ~(1<<CS01) | ~(1<<CS00);	//set prescaler to zero
}

/* set timer zero to 5 milli seconds resolution and start.*/
void StartTimerZero(void)
{
	OCR0A = 78;
	TCCR0A = 2;
	TCCR0B = (1<<CS02) | (1<<CS00);	//set prescaler to 1024
}

ISR(TIMER0_COMPA_vect) // timer0 overflow interrupt
{
	ucTimerCount++; //event to be executed every ms here
}

void SetMachineSetupData(void)
{
	//get setup data from EEPROM
	halGetEeprom(MACHINE_SETUP_ADDR, MACHINE_SETUP_NUM_BYTE, (u8*)&MachineSetupData);
	vendPrice = (MachineSetupData.VendPrice[0] * 256 + MachineSetupData.VendPrice[1]) / 100.0;
	if(MdcStatus.MachineType[0] == TUMBLER  || MdcStatus.MachineType[0] == DRYER)
	halGetEeprom(MACHINE_SETUP_ADDR_DRYER, DRYER_SETUP_NUM_BYTE, &MachineSetupData.CoolDownTime);
	if(MdcStatus.MachineType[1] == QUANTUM_SERIES)
	halGetEeprom(MACHINE_SETUP_ADDR_QUANTUM, QUANTUM_SETUP_NUM_BYTE, &MachineSetupData.CoolDownTime);
}

void SaveMachineSetupData(void)
{
	/*	halPutEeprom((u8*)3, 11, &MachineSetupData)
	if(MdcStatus.MachineType == TUMBLER  || MdcStatus.MachineType == DRYER)
	halPutEeprom((u8*)11, 7, &(MachineSetupData + 11))*/
}

void SetDefaultMachineSetup(void)
{
	//Default settings
	MachineSetupData.KeyPressWaitTime = 25;
	MachineSetupData.VendPrice[0] = 0;
	MachineSetupData.VendPrice[1] = 100;
	MachineSetupData.Coin1[0] = 0;
	MachineSetupData.Coin1[1] = 25;
	MachineSetupData.Coin2[0] = 0;
	MachineSetupData.Coin2[1] = 100;
	MachineSetupData.StartPulse[0] = 0;
	MachineSetupData.StartPulse[1] = 100;
	MachineSetupData.CycleLength = 1;
	MachineSetupData.ControlConfig = 0;//8;
	MachineSetupData.DefaultCycle = 2; //Normal for washer

	//Dryer specific settings
	MachineSetupData.CoolDownTime = 3;
	MachineSetupData.Coin1TopoffTime = 15;
	MachineSetupData.Coin2TopoffTime = 60;
	MachineSetupData.HighTempSetting = 0;
	MachineSetupData.MediumTempSetting = 1;
	MachineSetupData.LowTempSetting = 3;
	MachineSetupData.DelicateTempSetting = 9;
}

u8 sendMdcPacket(u8 *ucDataBuf)
{
	u8 buf_size,i,n;
	u8 BCC = 0;
	
	if (ucDataBuf[0] == 0)
	return 0;
	
	for(n = 0; n < NUMRETRIES; n++)
	{
		BCC = STX;
		init_rx_buf();
		buf_size = (ucDataBuf[0] + 1);	//number of bytes sending
		serial_putchar(STX);    // Send STX, start byte


		for (i = 0; i < buf_size; i++)
		{
			serial_putchar(ucDataBuf[i]);
			BCC = (BCC ^ ucDataBuf[i]);
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

	return 0;
}

#endif // (MDC_MACHINE)

u16 calcMachineCRC(u8 *msg, u8 len)
{
	u8 i;                            //loop counter 1
	u8 j;                            //loop counter 2
	u8 flag;                         //flag
	u16 crc;                         //CRC
	u8 tempMsg[MAX_ALLOWED_ARRAY_SIZE];//temp array
	
	crc = 0;                            //clear crc

	for(i=0;i<len; i++)                //save into temp array
	tempMsg[i] = msg[i];
	
	for(i=0;i<len; i++)                 //loop through msg array
	{
		for(j=0;j<8; j++)                //loop through bits
		{
			flag = crc & 0x0001;          //set or reset flag
			crc >>= 1;                    //shift crc
			if (flag ^ (tempMsg[i] & 0x01)) crc ^= 0xA001;//crc calculation
			tempMsg[i] >>= 1;                //shift message
		}
	}
	return (crc);                       //return crc
}//end of calcMachine

#endif // (NODETYPE == ENDDEVICE)
