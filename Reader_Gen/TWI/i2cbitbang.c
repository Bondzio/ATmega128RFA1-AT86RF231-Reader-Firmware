

#include <avr/io.h>
#include "i2cbitbang.h"

#define i2cSetSDA() BB_SDAPORT |= (1<<BB_SDAPIN)
#define i2cClrSDA()	BB_SDAPORT &= ~(1<<BB_SDAPIN)
#define i2cSetSCL()	BB_SCLPORT |= (1<<BB_SCLPIN)
#define i2cClrSCL()	BB_SCLPORT &= ~(1<<BB_SCLPIN)


inline unsigned char geti2cPinVal(unsigned char i2cPin) {
    volatile uint8_t value;
	//DDRB &= ~(1<<i2cPin); //make pin input
 	if (PINB & (1<<i2cPin)) {
		value = 1;
	} else {
		value = 0;
	}
	//DDRB |= (1<<i2cPin); //make pin output
	return value; 
}


inline void i2c_delay(void) 
{
  int i=0;
   while (i++ < I2C_DELAY_TIME) {
      asm volatile ("nop");
   } 


/* 
 asm volatile ("nop");
 asm volatile ("nop");
 asm volatile ("nop");
 asm volatile ("nop");
*/ 
}


void i2c_start(void)
 { 
   i2cSetSDA();
  // i2c_delay();
   i2cSetSCL(); 
   i2c_delay();
   i2cClrSDA(); 
   i2c_delay();
   i2cClrSCL(); 
  // i2c_delay();
 }

 
 
void i2c_stop(void)
 {
   i2cClrSDA(); 
   i2c_delay();
   i2cSetSCL(); 
   i2c_delay();
   i2cSetSDA();
   i2c_delay();
 }
 
 

unsigned char i2c_rx(char ack)
 {
 
   volatile uint8_t x=0;
   volatile uint8_t d=0;
 
	DDRB &= ~(1<<BB_SDAPIN); //make pin input

// i2c_delay();
   for(x=0; x<8; x++) {

      d <<= 1;
		i2cSetSCL();
	   i2c_delay();
       while(!(PINB &(1<<BB_SCLPIN)));
       
       if(PINB &(1<<BB_SDAPIN)){
	       d |= 1;
         }
         	
		
       i2cClrSCL();
       i2c_delay();
   }
   
   DDRB |= (1<<BB_SDAPIN); //make pin output
   if(ack) {
       i2cClrSDA();
   } else {
       i2cSetSDA();
   }
   
   i2cSetSCL();   
   i2c_delay();            
   i2cClrSCL(); 
   i2cClrSDA();
   
   return d;
 }

 
 
unsigned char i2c_tx(unsigned char data, unsigned char xfer)
 {
   
 volatile unsigned char x;
 volatile unsigned char b;
 volatile unsigned char d;
 

  if (xfer == XFER_DATA){
       d = data;
  } else if (xfer == XFER_READ){
      d = data | 0x01;
  } else if (xfer == XFER_WRITE) {
      d = data & 0xFE;
  } else {
      d = data;
  }
  
  
  for(x=0; x<8; x++) {
     
      if(d&0x80) {
        i2cSetSDA();
      } else {
	    i2cClrSDA(); 
      }
        i2c_delay();
        i2cSetSCL(); 
        d <<= 1;
	    i2c_delay();
        i2cClrSCL(); 
    }
   
   i2c_delay();
   i2cSetSDA();
   i2c_delay();
   //i2cSetSCL(); 
   //make SCL line input with pullup
   DDRB &= ~(1<<BB_SCLPIN);
   PORTB |= (1<<BB_SCLPIN);
   //i2c_delay();
   while(!(PINB &(1<<BB_SCLPIN)))
   ;
   //return SCL line to output low
   DDRB |= (1<<BB_SCLPIN);
   PORTB &= ~(1<<BB_SCLPIN);
   b = !(PINB &(1<<BB_SDAPIN)); 
   i2cClrSCL(); 
   i2cSetSDA();//should be set after reading ack bit.
  

   return b;
 }
 
 