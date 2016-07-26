/*****< RTC.c >************************************************************/
/*                                                                            */
/*  Hardware - Real Time CLock Source 								  */
/*                                                                            */
/*  Author:  Tom Nixon                                                        */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/13/16  Tom Nixon       Initial creation.                              */
/******************************************************************************/


#ifndef _RTC_C_
#define _RTC_C_

#include <msp430.h>        /* Compiler specific Chip header.                  */
#include "Common.h"
#include "Hardware.h"
#include "WatchdogTimerControl.h"
#include "RTC.h"
#include "timeDate.h"
#include "Common.h"
#include "config.h"

static void KillI2C(void);

// Global variables
unsigned int	RtcCountdown;
unsigned int	RtcTimeout;
unsigned char 	RtcRxBuffer[8];

// local variables
unsigned char TXByteCtr;
unsigned char TxIdx;
unsigned char TxData[5];

unsigned char RXByteCtr;
unsigned char RxIdx;
unsigned char LastByte;
unsigned char Write;

//--------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
// DESCRIPTION:		initialize the RTC, check the status of the RTC IC, and it's saved data
//					validate it's alive, load new data if the time is lost, make sure
//					it counts the new data
//
// RETURN/UPDATES:	void
//
//---------------------------------------------------------------------------------------------
void InitializeRTC()
{
	now = datetime(0, 0, 12, 1, 1, 2016);

	if (RTC_timeIsSet() == 0)
	{
		RTC_adjust(now);
		setStateCode(10);
	}

    RtcCountdown = 200;			// 2 seconds delay to make sure seconds are incrementing
    while (RtcCountdown)
    {
    	WdtKeepAlive();
    }

    if (ReadRTCRegisters(0, 2) == 1)
    {
    	if (bcd2bin(RtcRxBuffer[0] & 0x7f) != (now.second + 2))
    		setStateCode(11);
    }
    else
    	setStateCode(11);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Pull the time from the RTC IC
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void pollTime()
{
	if (RtcCountdown == 0)
	{
		RtcCountdown = 100;
		now = RTC_now();
	}
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Pull data from the RTC, make sure it's valid, and we get an Ack
//
// RETURN/UPDATES:	1 - A time is saved in the RTC IC
//					0 - No data is pulled
//---------------------------------------------------------------------------------------------
int RTC_timeIsSet(void)
{
    ReadRTCRegisters(0, 7);

    if (bcd2bin(RtcRxBuffer[6]) >= 16)
    	return 1;

	return 0;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Load the argument date and time into the RTC IC
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void RTC_adjust(dateTimeStruct dt)
{
	WriteRTCRegister(0, bin2bcd( dt.second ) | 0x80);
	WriteRTCRegister(1, bin2bcd( dt.minute ));
	WriteRTCRegister(2, bin2bcd( dt.hour )); // (0100 0000) 0x40 = 12 Hr clock
	WriteRTCRegister(3, bin2bcd( 0x08 ));		 // (1000 0000) 0x80 = MFP asserts HIGH // (0000 8000) 0x08 = VBAT enabled
	WriteRTCRegister(4, bin2bcd( dt.day ));
	WriteRTCRegister(5, bin2bcd( dt.month ));
	WriteRTCRegister(6, bin2bcd( dt.year - 2000 ));
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Pull the saved time for the RTC IC, return it in a Struct
//
// RETURN/UPDATES:	dateTimeStruct
//---------------------------------------------------------------------------------------------
dateTimeStruct RTC_now()
{
    ReadRTCRegisters(0, 10);
    int ss = bcd2bin(RtcRxBuffer[0] & 0x7f);
    int mm = bcd2bin(RtcRxBuffer[1]);
    int hh = bcd2bin(RtcRxBuffer[2] & 0x3f);

    int d = bcd2bin(RtcRxBuffer[4]);

    int m = bcd2bin(RtcRxBuffer[5] ^ 0x20);
    int y = bcd2bin(RtcRxBuffer[6]) + 2000;

    return datetime(ss, mm, hh, d, m, y);
}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:
//
// RETURN/UPDATES:
//
//---------------------------------------------------------------------------------------------
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  switch(__even_in_range(UCB0IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;                           // Vector  4: NACKIFG
  case  6: break;   		                // Vector  6: STTIFG
  case  8: break;                           // Vector  8: STPIFG
  case 10:                                  // Vector 10: RXIFG
    RXByteCtr--;                            // Decrement RX byte counter
    if (RXByteCtr)
    {
      RtcRxBuffer[RxIdx++] = UCB0RXBUF;               // Move RX data to address PRxData

    }
    else
    {
      RtcRxBuffer[RxIdx++] = UCB0RXBUF;     // Get final RX data

      LastByte = 1;
      UCB0CTL1 |= UCTXSTP;                	// Generate I2C stop condition
      	  	  	  	  	  	  	  	  	  	// this will take place after next byte read so if placed here
      	  	  	  	  	  	  	  	  	  	// must read 2 bytes minimum
      	  	  	  	  	  	  	  	  	  	// to read a single byte poll UCTXSTT and when cleared set above stop
    }
    break;
  case 12:                                  // Vector 12: TXIFG
    if (TXByteCtr)                          // Check TX byte counter
    {
      UCB0TXBUF = TxData[TxIdx++];          // Load TX buffer
      TXByteCtr--;                          // Decrement TX byte counter
    }
    else
    {
    	if(Write == 1) {
    		UCB0CTL1 |= UCTXSTP;                  // I2C stop condition
    		UCB0IFG &= ~UCTXIFG;                  // Clear USCI_B0 TX int flag
    		LastByte = 1;
    	}
    	else {
			UCB0CTL1 &= ~UCTR;         			// I2C RX
			UCB0CTL1 |= UCTXSTT;         		// I2C start condition
    	}

    }
  default: break;
  }
}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:
//
// RETURN/UPDATES:
//
//---------------------------------------------------------------------------------------------
void ConfigureI2CForRTC(void)
{
	P2DIR &= ~IN_nRTC_INT;					  // RTC interrupt as input
	P3SEL |= RTC_I2C_SDA +	RTC_I2C_SCL;      // Assign I2C pins to USCI_B0

	UCB0CTL1 |= UCSWRST;                      // Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
	UCB0BR0 = 15;                             // fSCL = SMCLK/15 = ~100kHz
	UCB0BR1 = 0;
	//UCB0I2CSA = 0x6f;                         // Slave Address is B1101111
	UCB0I2CSA = RTC_ADDRESS;
}



//---------------------------------------------------------------------------------------------
// DESCRIPTION:		-Writes one byte(val) to a specific register(regAddr) of the RTC chip
//
// RETURN/UPDATES:	1: write was ack'd by RTC
//					0: Timeout happened
//---------------------------------------------------------------------------------------------
int WriteRTCRegister(unsigned char regAddr, unsigned char val)
{
	Write = 1;
	TxData[0] = regAddr;
	TxData[1] = val;
	TXByteCtr = 2;
	TxIdx = 0;
	LastByte = 0;

	UCB0CTL1 &= ~UCSWRST;                   // Clear SW reset, resume operation
	UCB0IE |= UCTXIE;              			// Enable TX and RX interrupts

	RtcTimeout = 10;
	WdtKeepAlive();
	while (UCB0CTL1 & UCTXSTP)
	{             // Ensure stop condition got sent
		if (RtcTimeout == 0)
		{
			KillI2C();
			return(0);
		}
	}

	UCB0CTL1 |= UCTR + UCTXSTT;             // I2C start condition

	while( (LastByte == 0) || ((UCB0CTL1 & UCTXSTP) != 0) )
	{
		if (RtcTimeout == 0)
		{
			KillI2C();
			return(0);
		}
	}

	UCB0IE &= ~(UCTXIE);
	return(1);
}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:		-Reads 1-8 bytes starting from register(regAddr) of the RTC chip
//					 and places them in global array RtcRxBuffer[]
// RETURN/UPDATES:	1: Read was ack'd by RTC
//					0: Timeout happened
//---------------------------------------------------------------------------------------------
int ReadRTCRegisters(unsigned char regAddr, unsigned char cnt)
{
	Write = 0;
	if(cnt > (sizeof RtcRxBuffer) )	cnt = sizeof RtcRxBuffer;
	if(cnt > 1) cnt = cnt-1;	// always read 2 bytes

	TxData[0] = regAddr;
	TXByteCtr = 1;
	TxIdx = 0;
	RxIdx = 0;
	RXByteCtr = cnt;
	LastByte = 0;

	UCB0CTL1 &= ~UCSWRST;                   // Clear SW reset, resume operation
	UCB0IE |= UCTXIE + UCRXIE;              // Enable TX and RX interrupts

	RtcTimeout = 10;
	WdtKeepAlive();
	while (UCB0CTL1 & UCTXSTP)
	{             							// Ensure stop condition got sent
		if(RtcTimeout == 0)
		{
			KillI2C();
			return(0);
		}
	}

	UCB0CTL1 |= UCTR + UCTXSTT;             // I2C start condition

	RtcTimeout = 20;
	while( (LastByte == 0) || ((UCB0CTL1 & UCTXSTP) != 0) )
	{
		if(RtcTimeout == 0)
		{
			KillI2C();
			return(0);
		}
	}

	if(cnt == 1)	RtcRxBuffer[1] = 0;		// clear 2nd reads data

	UCB0IE &= ~(UCTXIE + UCRXIE);                // Disable TX and RX interrupts
	return(1);
}

//---------------------------------------------------------------------------------------------
static void KillI2C(void)
{

	UCB0CTL1 |= UCTXSTP;		//generate stop
	UCB0IFG = 0;
	RtcTimeout = 10;
	WdtKeepAlive();
	while (UCB0CTL1 & UCTXSTP)
	{             // Ensure stop condition got sent
		if (RtcTimeout == 0)
		{
			UCB0CTL1 |= UCSWRST;
			UCB0IE &= ~(UCTXIE + UCRXIE);                // Disable TX and RX interrupts
		}
	}

}


#endif
