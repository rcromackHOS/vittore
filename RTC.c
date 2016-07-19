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
// DESCRIPTION:
//
// RETURN/UPDATES:
//
//---------------------------------------------------------------------------------------------
void InitializeRTC()
{

	/*
	now = datetime(0, 0, 12, 27, 6, 2015);

	if (RTC_isRunning() == 99)	// 99 = no I2C response
	{
		setStateCode(11);
	}
	else
    {
		// If we've lost the time
		if (RTC_lostTime() == 1)
	    {
			setStateCode(10);     // 10 = RTC lost time (LIR32 _SYS_FAILURE_?)
			RTC_adjust(now);
	    }
		else
			now = RTC_now();
    }
	*/
	now = datetime(0, 0, 12, 27, 6, 2015); //
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:
//
// RETURN/UPDATES:
//
//---------------------------------------------------------------------------------------------
int RTC_isRunning(void)
{
//  Wire.beginTransmission(RTC_ADDRESS);
//  Wire.write(i);

//  if (Wire.endTransmission() != 0)
//	return 99;

//  Wire.requestFrom(RTC_ADDRESS, 1);
//  int ss = Wire.read();
//  return (ss>>7);
	return 0;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:
//
// RETURN/UPDATES:
//
//---------------------------------------------------------------------------------------------
int RTC_lostTime(void)
{
//  Wire.beginTransmission(RTC_ADDRESS);
//  Wire.write(i);
//  Wire.endTransmission();

//  Wire.requestFrom(RTC_ADDRESS, 7);
//  Wire.read();//Serial.println(bcd2bin(Wire.read()));
//  Wire.read();//Serial.println(bcd2bin(Wire.read()));
//  Wire.read();//Serial.println(bcd2bin(Wire.read()));
//  Wire.read();
//  Wire.read();//Serial.println(bcd2bin(Wire.read()));
//  Wire.read();//Serial.println(bcd2bin(Wire.read()));
  //Wire.read();//Serial.println(bcd2bin(Wire.read()));
//  if (bcd2bin(Wire.read()) <= 15)
//	  return 1;
  return 0;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:
//
// RETURN/UPDATES:
//
//---------------------------------------------------------------------------------------------
int RTC_adjust(dateTimeStruct dt)
{ /*
	Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(i);
    Wire.write(bin2bcd( dt.second ) | 0x80);
    Wire.write(bin2bcd( dt.minute ));
    Wire.write(bin2bcd( dt.hour ));  // (0100 0000) 0x40 = 12 Hr clock
    Wire.write(bin2bcd( 0x08 ));		 // (1000 0000) 0x80 = MFP asserts HIGH // (0000 8000) 0x08 = VBAT enabled
    Wire.write(bin2bcd( dt.day ));
    //Serial.print("setMonth ");Serial.println(dt.month());
	//Serial.print("bin2bcd ");Serial.println( bin2bcd(dt.month()) );
	Wire.write(bin2bcd( dt.month ));
    Wire.write(bin2bcd( dt.year - 2000 ));
    Wire.write(i);
    Wire.endTransmission(); */
	WriteRTCRegister(RTC_ADDRESS, 0);
	WriteRTCRegister(RTC_ADDRESS, bin2bcd( dt.second ) | 0x80);
	WriteRTCRegister(RTC_ADDRESS, bin2bcd( dt.minute ));
	WriteRTCRegister(RTC_ADDRESS, bin2bcd( dt.hour )); // (0100 0000) 0x40 = 12 Hr clock
	WriteRTCRegister(RTC_ADDRESS, bin2bcd( 0x08 ));		 // (1000 0000) 0x80 = MFP asserts HIGH // (0000 8000) 0x08 = VBAT enabled
	WriteRTCRegister(RTC_ADDRESS, bin2bcd( dt.day ));
	WriteRTCRegister(RTC_ADDRESS, bin2bcd( dt.month ));
	WriteRTCRegister(RTC_ADDRESS, bin2bcd( dt.year - 2000 ));
	WriteRTCRegister(RTC_ADDRESS, 0);
	return 0;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:
//
// RETURN/UPDATES:
//
//---------------------------------------------------------------------------------------------
dateTimeStruct RTC_now()
{ /*
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(i);
  Wire.endTransmission();

  Wire.requestFrom(RTC_ADDRESS, 7);

  int ss = bcd2bin(Wire.read() & 0x7f);
  int mm = bcd2bin(Wire.read());
  int hh = bcd2bin(Wire.read() & 0x3f);
  Wire.read();
  int d = bcd2bin(Wire.read());

  int comp = B00100000;
  int m = bcd2bin(Wire.read() ^ comp);

  int y = bcd2bin(Wire.read()) + 2000;

  return datetime(ss, mm, hh, d, m, y);*/
  return datetime(0, 0, 12, 1, 1, 2015);
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
	UCB0I2CSA = 0x6f;                         // Slave Address is B1101111
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
		if(RtcTimeout == 0)
		{
			KillI2C();
			return(0);
		}
	}

	UCB0CTL1 |= UCTR + UCTXSTT;             // I2C start condition

	while( (LastByte == 0) || ((UCB0CTL1 & UCTXSTP) != 0) )
	{
		if(RtcTimeout ==0)
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
		if(RtcTimeout ==0)
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
		if(RtcTimeout == 0)
		{
			UCB0CTL1 |= UCSWRST;
			UCB0IE &= ~(UCTXIE + UCRXIE);                // Disable TX and RX interrupts
		}
	}

}


#endif
