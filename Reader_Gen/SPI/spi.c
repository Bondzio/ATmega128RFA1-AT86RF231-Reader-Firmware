/** \file
 *  \brief Implementation of SPI functions.
 */

#ifdef SPI

//#include "ll_timers.h"
#include "spi.h"
#include "stdint.h"
#include <avr/io.h>


/** \brief This function initializes SPI peripheral.
 */
void Spi_initialize()
{
    volatile uint8_t ucIOReg;
   
   // ATmega128rfa1
   // PB0: SSN
   // PB1: SCK
   // PB2: MOSI
   // PB3: MISO
   
    // Set MOSI, SS, SCK, as output, and MISO as input
    DDRB |= (1 << PB2) | (1 << PB1) | (1 << PB0);
	 DDRB &= ~(1<<PB3);
    
	// This ensures PB0 (Bowline) is never selected
	PORTB |= (1 << PB0);
	
	// Set SS as output, all others input
    DDRE = (1 << PE1); 

    // Set SS high and SCK low
    Spi_ss_high;
    Spi_sck_low;

    //  Enable SPI, Master, mode 0, set clock rate
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);    // fck/64

    // clear SPIF bit in SPSR
    ucIOReg = SPSR;
    ucIOReg = SPDR;
}


/** \brief Send and receive data on SPI.
 *
 * \param ucData is data which will be sent using SPI.
 * \return SPDR is data received from SPI slave.
 */
uint8_t Spi_send_and_receive ( uint8_t ucData )
{
	
    /* Start transmission */
    SPDR  = ucData ;     // Send Character

    /* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)))
    {
        // Wait until Char is sent
    }

    /* Return the received byte*/
    return SPDR;
}


/** \brief Send Data over SPI, Wait, and Receive Data.
 *
 * \param pucCommandBuffer is pointer to command.
 * \param ucCommandSize is command size.
 * \param pucResultBuffer is pointer to received data.
 * \param ucResultSize is number of bytes received.
 * \return SUCCESS or FAIL_COMMAND_SEND
 */
uint8_t Spi_wait_and_receive(uint8_t* pucCommandBuffer, uint8_t ucCommandSize, uint8_t* pucResultBuffer, uint8_t ucResultSize)
{
	/*
   uchar i;
   Spi_ss_low;

   for (i = 0; i < ucCommandSize; i++)
   {
      Spi_send_and_receive(*pucCommandBuffer);
      pucCommandBuffer++;
   }

   // parameter 1 (TCCR1A): 0x00 Normal Port Operation,
   //            				 OCnA, OCnB, and OCnC disconnected
   //
   // parameter 2 (TCCR1B): 0x05 F_CPU/1024 (From Prescaler)
   //
   // parameter 3 (OCR1A):  match value
   Timer_set(0, 5, Spi_timeout);

   // Wait for ISTAT going high.
   while (!(PIND & (1 << ISTAT)))
   {
     if (!Timer_test)
         continue;

      Timer_stop();
      Spi_ss_high;
      return SPI_TX_TIMEOUT;
   }
   Timer_stop();

   i = 0;
   while (PIND & (1 << ISTAT))
   {
      *pucResultBuffer = Spi_send_and_receive(0xFF);
      pucResultBuffer++;
      if (++i > ucResultSize)
      {
         Spi_ss_high;
         return SPI_RX_OVERFLOW;
      }
    }
    Spi_ss_high;
	*/
    return 0x00; //SUCCESS;
}


/** \brief Send only a command to SPI without waiting for response.
 *
 * \param ucData is data to be sent.
 * \return ucData1 is received data from SPI
 */
void Spi_send_only ( uint8_t ucData )
{
   Spi_send_and_receive(ucData);
}


void Spi_send ( uint8_t ucData )
{
	
    /* Start transmission */
    SPDR  = ucData ;     // Send Character

    /* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)))
    {
        // Wait until Char is sent
    }

}


uint8_t Spi_receive ()
{
	
    /* Start transmission */
    SPDR  = 0xff;     // Send dummy byte

    /* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)))
    {
        // Wait until Char is sent
    }

    /* Return the received byte*/
    return SPDR;
}



#endif
