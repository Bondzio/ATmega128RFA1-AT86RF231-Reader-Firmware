/*
 * i2cbitbang.h
 *
 * Created: 3/22/2012 10:38:04 PM
 *  Author: rocendob
 */ 


#ifndef I2CBITBANG_H_
#define I2CBITBANG_H_
//-------------------------------------------------------------------
//clock generation should be handled by a timer for accuracy. This is a temporary hack, only works at 100K with optimization set to Os.
#define I2C_SPEED    100000
#define I2C_DELAY_TIME  (uint8_t)(F_CPU / (I2C_SPEED*20))
//-------------------------------------------------------------------

#define BB_SDAPORT	PORTB
#define BB_SCLPORT	PORTB
#define BB_SDAPIN  6
#define BB_SCLPIN  7

// I2C Xfer modes
#define XFER_DATA 	0x01
#define XFER_READ	   0x02
#define XFER_WRITE	0x03

#define I2C_BITBANG_INIT()  DDRB |= 0xD0, PORTB |= 0xF0
#define TOGGLE_KEYPAD_LED() PINB |= 0x10



//unsigned char geti2cPinVal(unsigned char i2cPin);
void i2cBusInit(void);
//void i2c_delay(void);
unsigned char i2c_rx(char ack);
unsigned char i2c_tx(unsigned char data, unsigned char xfer);

void i2c_start(void);
void i2c_stop(void);

#endif /* I2CBITBANG_H_ */