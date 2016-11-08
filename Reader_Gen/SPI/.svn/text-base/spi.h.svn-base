/** \file
 *  \brief Header file for spi.c.
 */

#ifndef _SPI_H_
#define _SPI_H_

#ifndef COORNODE
//#include "config.h"
#ifdef SPI
#include <avr/io.h>

   // ATmega128rfa1
   // PB0: SSN
   // PB1: SCK
   // PB2: MOSI
   // PB3: MISO
 
   #define Spi_sck_high  PORTB |= (1 << PB1)
   #define Spi_sck_low   PORTB &= ~(1 << PB1)
   
  // #define Spi_ss_high   PORTB |= (1 << PB0)
  // #define Spi_ss_low    PORTB &= ~(1 << PB0)
   
   #define Spi_ss_high   PORTE |= (1 << PE1)
   #define Spi_ss_low    PORTE &= ~(1 << PE1)  
   
   //#define Spi_timeout   ((F_CPU / 1000 / 1024) * 45)
   #define Spi_timeout   ((F_CPU / 1000 / 1024) * 60)

void  Spi_initialize();
uint8_t Spi_send_and_receive(uint8_t ucData);
uint8_t Spi_wait_and_receive(uint8_t* pucCommandBuffer, uint8_t ucCommandSize, uint8_t* pucResultBuffer, uint8_t ucResultSize );
void Spi_send_only(uint8_t ucData);

void Spi_send ( uint8_t ucData );
uint8_t Spi_receive (void);

#endif // SPI

#endif  // NODETYPE
#endif // _SPI_H_
