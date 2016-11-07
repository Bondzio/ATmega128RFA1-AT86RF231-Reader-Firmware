/*
 * Keypad.c
 *
 * Created: 9/3/2013 12:40:53 AM
 *  Author: Paul
 */ 


#include <avr/io.h>
#include <string.h>
#include <stdbool.h>

#include "Keypad.h"
//#include <twi/twi.h>
#include <util/delay.h>
#include "rum_types.h"
#include "TWI/i2cbitbang.h"

#ifdef ENABLE_KEYPAD

//Port pin defines
#define KEYPAD_POWER_PIN	   PG2
#define KEYPAD_CHANGE_PIN	   PB5
#define KEYPAD_LED_PIN		   PB4
//-------------------------------
//register addresses
#define QTOUCH_ADDR			   0x38
#define RESET_QTOUCH		   0x01
#define RESET_ADDR			0x07
#define QT_STATUS_ADDR     2  //start address of status bytes
//---------------------

#define KEY_PRESS_DETECTED 0x01

//--------------------------MACRO's-----------------
#define CHANGE_STATE_STATUS() (bool)((PINB & (1<<KEYPAD_CHANGE_PIN)))
#define NO_CHANGE 1
#define CHANGED   0
//--------------------------------------------------

enum RegAddresses
{
   ChipID, FirmwareVersion, DetectionStatus,KeyStatus1, KeyStatus2, Calibration = 6, nReset, LowPowerMode,
   TowardsTouchDriftComp, AwayFromTouchDriftComp, DetectionIntegration, TouchRecalDelay, DriftHoldTime, ChargeTime=15,
   Key0DetectionThreshold, Key1DetectionThreshold, Key2DetectionThreshold, Key3DetectionThreshold, Key4DetectionThreshold,
   Key5DetectionThreshold, Key6DetectionThreshold, Ke70DetectionThreshold, Key8DetectionThreshold, Key9DetectionThreshold, 
   Key10DetectionThreshold, Key11DetectionThreshold, Key0Control, Key1Control, Key2Control, Key3Control, Key4Control,
   Key5Control, Key6Control, Key7Control, Key8Control, Key9Control, Key10Control, Key11Control,
   Key0PulseScale, Key1PulseScale, Key2PulseScale, Key3PulseScale, Key4PulseScale, Key5PulseScale, Key6PulseScale,
   Key7PulseScale, Key8PulseScale, Key9PulseScale, Key10PulseScale, Key11PulseScale
   
};

typedef struct
{
	uint8_t chipId;
	uint8_t firmwareVersion;
	uint8_t detectionStatus;
	uint16_t keyStatus;
	uint8_t sliderPosition; //not used with this application
	uint8_t calibrateCommand;
	uint8_t resetCommand;
	uint8_t lowPowerMode;
	uint8_t towardsDriftCompensation;
	uint8_t awayDriftCompensation;
	uint8_t detectionIntegrator;
	uint8_t touchRecalDelay;
	uint8_t driftHoldTime;
	uint8_t sliderOptions;
	uint8_t chargeTime;
}QTInfo_t;

typedef struct
{
	uint8_t key_0;
	uint8_t key_1;
	uint8_t key_2;
	uint8_t key_3;
	uint8_t key_4;
	uint8_t key_5;
	uint8_t key_6;
	uint8_t key_7;
	uint8_t key_8;
	uint8_t key_9;
	uint8_t key_10;
	uint8_t key_11;
}QTKeyThreshold_t;
typedef struct
{
	uint8_t key_0;
	uint8_t key_1;
	uint8_t key_2;
	uint8_t key_3;
	uint8_t key_4;
	uint8_t key_5;
	uint8_t key_6;
	uint8_t key_7;
	uint8_t key_8;
	uint8_t key_9;
	uint8_t key_10;
	uint8_t key_11;	
}QTKeyControl_t;

typedef struct
{
	uint8_t key_0;
	uint8_t key_1;
	uint8_t key_2;
	uint8_t key_3;
	uint8_t key_4;
	uint8_t key_5;
	uint8_t key_6;
	uint8_t key_7;
	uint8_t key_8;
	uint8_t key_9;
	uint8_t key_10;
	uint8_t key_11;	
}QTKeyPulse_t;

typedef struct
{
	uint8_t key_0;
	uint8_t key_1;
	uint8_t key_2;
	uint8_t key_3;
	uint8_t key_4;
	uint8_t key_5;
	uint8_t key_6;
	uint8_t key_7;
	uint8_t key_8;
	uint8_t key_9;
	uint8_t key_10;
	uint8_t key_11;	
}QTKeySignal_t;

typedef struct
{
	uint8_t key_0;
	uint8_t key_1;
	uint8_t key_2;
	uint8_t key_3;
	uint8_t key_4;
	uint8_t key_5;
	uint8_t key_6;
	uint8_t key_7;
	uint8_t key_8;
	uint8_t key_9;
	uint8_t key_10;
	uint8_t key_11;	
}QTKeyReference_t;

typedef struct
{
	QTInfo_t			deviceInfo;
	QTKeyThreshold_t	detectThreshold;
	QTKeyControl_t	keyControl;
	QTKeyPulse_t		pulseScale;
	QTKeySignal_t		signal;
	QTKeyReference_t	data;
}AT42QT2120_t;



static AT42QT2120_t touchDevice;
Keypad keypad;

enum RegAddresses regAddress;

u8 keypadEntryTimeoutTimerID;


void resetKeypad(void)
{
	i2c_start();
	i2c_tx(QTOUCH_ADDR,XFER_WRITE);
	i2c_tx(RESET_ADDR, XFER_DATA);
	i2c_tx(RESET_QTOUCH, XFER_DATA);
	i2c_stop();
}

void initKeypad(void)
{
   QTData_t tmp;
   uint8_t tmpBuf[12] = {10,50,50,50,50,50,50};
   
   
   
	//volatile uint8_t v;

     //----------------------------
   //increase ports B and G drive strength to 8mA
 //  DPDS0 |= 3;
   DPDS1 |= 3;
   //---------------------------
   //set change pin as input with pull-up
   //DDRB &= ~(1<<KEYPAD_CHANGE_PIN);
  // DDRB |= (1<<KEYPAD_CHANGE_PIN);
   PORTB |= (1<<KEYPAD_CHANGE_PIN);
   //----------------------------
 

	PORTG |= (1<<KEYPAD_POWER_PIN); //turn PG2 on to power AT42QT
	I2C_BITBANG_INIT();
	_delay_us(50);
   while(CHANGE_STATE_STATUS() == NO_CHANGE); //change pin is held high for some time after reset, wait for low state before comms.
   _delay_us(50);
	resetKeypad();
   _delay_ms(500);
/*
// 	while(CHANGE_STATE_STATUS() == NO_CHANGE) //wait for reset condition to clear, about 200ms. implimentation to be removed, don't want blocking 
// 	{
// 		TOGGLE_KEYPAD_LED();
// 		_delay_ms(10);
// 		//time out 
// 		//break; 
// 	}
// 	
// 	while(CHANGE_STATE_STATUS() == CHANGED) //wait for reset condition to clear, about 200ms. implementation to be removed, don't want blocking 
// 	{
// 		TOGGLE_KEYPAD_LED();
// 		_delay_ms(10);
// 		//time out 
// 		//break;
// 	}
*/
   tmp.address = Key0Control;
   tmp.size = 12;
   tmp.buf = &touchDevice.keyControl.key_0;
   
   touchDevice.keyControl.key_0 = 4;
   touchDevice.keyControl.key_1 = 4;
   touchDevice.keyControl.key_2 = 8;
   touchDevice.keyControl.key_3 = 8;
   touchDevice.keyControl.key_4 = 4;
   touchDevice.keyControl.key_5 = 4;
   touchDevice.keyControl.key_6 = 8;
   touchDevice.keyControl.key_7 = 8;
   touchDevice.keyControl.key_8 = 12;
   touchDevice.keyControl.key_9 = 12;
   touchDevice.keyControl.key_10 =12;
   touchDevice.keyControl.key_11 = 12;
   sendKeypadData(&tmp);
   _delay_us(50);
  
   touchDevice.detectThreshold.key_0 = 25;  
   touchDevice.detectThreshold.key_1 = 25;
   touchDevice.detectThreshold.key_2 = 25;
   touchDevice.detectThreshold.key_3 = 25;
   touchDevice.detectThreshold.key_4 = 25;
   touchDevice.detectThreshold.key_5 = 25;
   touchDevice.detectThreshold.key_6 = 25;
   touchDevice.detectThreshold.key_7 = 25;
   touchDevice.detectThreshold.key_8 = 25;
   touchDevice.detectThreshold.key_9 = 25;
   touchDevice.detectThreshold.key_10 = 25;
   touchDevice.detectThreshold.key_11 = 25;
   tmp.address = Key0DetectionThreshold;
   tmp.size = 12;
   tmp.buf = &touchDevice.detectThreshold.key_0;
   sendKeypadData(&tmp);
   _delay_us(50);
   /*
   touchDevice.pulseScale.key_0 = 0x21;
	touchDevice.pulseScale.key_1 = 0x21;
   touchDevice.pulseScale.key_2 = 0x21;
   touchDevice.pulseScale.key_3 = 0x21;
   touchDevice.pulseScale.key_4 = 0x21;
   touchDevice.pulseScale.key_5 = 0x21;
   touchDevice.pulseScale.key_6 = 0x21;
   touchDevice.pulseScale.key_7 = 0x21;
   touchDevice.pulseScale.key_8 = 0x21;
   touchDevice.pulseScale.key_9 = 0x21;
   touchDevice.pulseScale.key_10 = 0x21;
   touchDevice.pulseScale.key_11 = 0x21;
   tmp.address = Key0PulseScale;
   tmp.size = 12;
   tmp.buf = &touchDevice.pulseScale.key_0;
   sendKeypadData(&tmp);
   _delay_us(50);
   */

   tmp.address = DetectionIntegration;
   tmp.size = 1;
   tmp.buf = tmpBuf;
   sendKeypadData(&tmp);
   //get device info
	qtRegDump();
}


bool isKeyPressed(void)
{
	if (CHANGE_STATE_STATUS() == CHANGED)
	{
      return 1;
/*
// 		QTData_t tmp;
//       //uint16_t x;
//       
//       TOGGLE_KEYPAD_LED();
// 		tmp.address = QT_STATUS_ADDR;
//       tmp.size = 4;
//       tmp.buf = &touchDevice.deviceInfo.detectionStatus;
// 		getKeypadData(&tmp);
*/

	}
   return 0;
}
u8 getPressedKey(void)
{//store pressed keys until we encounter a * or #
	//return single key press
	 uint8_t returnVal = 0;
    	QTData_t tmp;
      //uint16_t x;
      uint16_t key;// = touchDevice.deviceInfo.keyStatus;
      
      TOGGLE_KEYPAD_LED();
		tmp.address = QT_STATUS_ADDR;
      tmp.size = 4;
      tmp.buf = &touchDevice.deviceInfo.detectionStatus;
		getKeypadData(&tmp);
    if (touchDevice.deviceInfo.detectionStatus == 1)
    {    
      
       key = touchDevice.deviceInfo.keyStatus;
    
	    if(key & 0x001)
	    {
   	    returnVal = '*';
	    }
	    else if (key & 0x002)
	    {
          returnVal =  '3';
	    }
	    else if (key & 0x004)
	    {
          returnVal =  '2';
	    }
	    else if (key & 0x008)
	    {
          returnVal =  '1';
	    }
	    else if (key & 0x010)
	    {
          returnVal =  '0';
	    }
	    else if (key & 0x020)
	    {
          returnVal =  '6';
	    }
	    else if (key & 0x040)
	    {
          returnVal =  '5';
	    }
	    else if (key & 0x080)
	    {
          returnVal =  '4';
	    }
	    else if (key & 0x100)
	    {
          returnVal =  '7';
	    }
	    else if (key & 0x200)
	    {
          returnVal =  '8';
	    }
	    else if (key & 0x400)
	    {
          returnVal =  '9';
	    }
	    else if (key & 0x800)
	    {
          returnVal =  '#';
	    }
    }	
	return returnVal;
}
//caller is responsible for providing adequate storage space for account number
bool numberEntry(uint8_t *num)
{
   static char keys[12] = {0};
   static uint8_t i = 0;
   uint8_t returnVal = 0;
	
   if(isKeyPressed())
   {
      if( ((keys[i] = getPressedKey()) != 0) /*&& i < MAX_DIGIT_ENTRY*/)
      {      
         if (keys[i] == '#')
         {
            keys[i] = 0;
            memcpy(num, keys, sizeof(keys));
            returnVal = 1;
         }
         else
         {
            i++;
         }            
                  
      }         
      if (i==12 || keys[i] == '*')
      {
         i=0;
         keys[i] = 0;
      }      
   }      
	return returnVal;
}
bool accountNumberEntry()
{
   if (keypad.flags.isAccountNumberEntered == false)
   {
      keypad.flags.isAccountNumberEntered = numberEntry(keypad.accountNumber);
   }
   return keypad.flags.isAccountNumberEntered;
}
bool passcodeEntry()
{
   if (keypad.flags.isPasscodeEntered == false)
   {
      keypad.flags.isPasscodeEntered = numberEntry(keypad.passcode);
   }
   return keypad.flags.isPasscodeEntered;
}
void clearKeypadEntry(void)
{
   keypad.flags.isPasscodeEntered = false;
   keypad.flags.isAccountNumberEntered = false;
   memset(keypad.accountNumber,0,sizeof(keypad.accountNumber));
   memset(keypad.accountNumber,0,sizeof(keypad.accountNumber));
}
void clearKeypadEntryTimeout(void)
{
	return;
}
void setKeypadEntryTimeout(void)
{
	return;
}
u8 sendKeypadAccount(void)
{
	return 1;
}
u8 sendKeypadData(QTData_t *info)
{
   uint8_t i = info->size;
	
   if (info->address > 0x5 && info->address < 0x33)
   {   
      i2c_start();
      _delay_us(50);
	   i2c_tx(QTOUCH_ADDR,XFER_WRITE);
	   i2c_tx(info->address, XFER_DATA);
      do 
	   {
		   if( (i2c_tx(info->buf[0], XFER_DATA)))
         {
            break;
         }            
		   info->buf++;
		   _delay_us(100);
	   } while (--i);
	   i2c_stop();	
   }	
   return (i == 0 ? 1:0);
}
void getKeypadData(QTData_t *info)
{
	uint8_t i = info->size;
	
	i2c_start();
   _delay_us(50);
	i2c_tx(QTOUCH_ADDR,XFER_WRITE);
	i2c_tx(info->address, XFER_DATA);
   _delay_us(50);
	i2c_stop();
	_delay_us(50);
	i2c_start();
	i2c_tx(QTOUCH_ADDR,XFER_READ);
   _delay_us(100);
	do 
	{
		info->buf[0] = i2c_rx(i == 1 ? 0:1); //nack last byte else ack
		info->buf++;
		_delay_us(100);
	} while (--i);
	i2c_stop();	//we've successfully received all the bytes, let's stop  
}
void qtRegDump(void)
{
	QTData_t tmp;
	
	tmp.address = 0;	//read all registers starting at zero
	tmp.size = 0x63;
	tmp.buf = &touchDevice.deviceInfo.chipId; //point to start of struct
	
	getKeypadData(&tmp);
}
#endif // KEYPAD_ENABLED