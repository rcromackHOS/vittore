/*
 * MCP7940N.c
 *
 *  Created on: May 26, 2016
 *      Author: rcromack
 */


#include "MCP7940N.h"
#include "timeDate.h"
#include "Common.h"

//--------------------------------------------------------------------

//const int daysInMonth [] = { 31,28,31,30,31,30,31,31,30,31,30,31 }; //has to be const or compiler compaints

static int i = 0;

//--------------------------------------------------------------------
//
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

//------------------------------------------------------------------
//
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

//------------------------------------------------------------------
//
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
	return 0;
}

//------------------------------------------------------------------
//
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
  return datetime(0, 0, 0, 1, 1, 2015);
}




//-------------------------------------------------------------------------------
// The USCI_B0 data ISR is used to move received data from the I2C slave
// to the MSP430 memory. It is structured such that it can be used to receive
//-------------------------------------------------------------------------------
#pragma vector = USCIAB0TX_VECTOR
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















