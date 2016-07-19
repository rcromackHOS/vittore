/*
 * MCP7940N.c
 *
 *  Created on: May 26, 2016
 *      Author: rcromack
 */


#include "MCP7940N.h"





//-------------------------------------------------------------------------------
// The USCI_B0 data ISR is used to move received data from the I2C slave
// to the MSP430 memory. It is structured such that it can be used to receive
//-------------------------------------------------------------------------------
//#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
  /*
  if(Rx == 1)
  {                                               // Master Recieve?
      I2C_RXBUFFER = UCB0RXBUF;                     // Get RX data
      //__bic_SR_register_on_exit();          // Exit LPM0
  }

  else
  {                                               // Master Transmit
      if (TXByteCtr)                              // Check TX byte counter
      {
    	  UCB0TXBUF = I2C_TXBUFFER;                // Load TX buffer
          TXByteCtr--;                            // Decrement TX byte counter
      }
      else
      {
          UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
          IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
          //__bic_SR_register_on_exit();      // Exit LPM0
      }
  }
  */
}

//--------------------------------------------------------------------
//
void init_I2C(void)
{
      UCB0CTL1 |= UCSWRST;                      // Enable SW reset
      UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
      UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
      UCB0BR0 = 10;                             // fSCL = SMCLK/10 = ~100kHz
      UCB0BR1 = 0;
      UCB0I2CSA = RTC_ADDRESS;                  // Slave Address
      UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
      //IE2 |= UCB0RXIE + UCB0TXIE;               //Enable RX and TX interrupt
}

void Transmit(void)
{
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    __bis_SR_register(GIE);               // Enter interrupts
}

void Receive(void)
{
	while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
	UCB0CTL1 &= ~UCTR ;                     // Clear UCTR
	UCB0CTL1 |= UCTXSTT;                    // I2C start condition
	while (UCB0CTL1 & UCTXSTT);             // Start condition sent?
	UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
	__bis_SR_register(GIE);        			// Enter interrupts
}















