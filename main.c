//***************************************************************************************
//
//  Built with Code Composer Studio v5
//***************************************************************************************


#include <msp430.h>
//#include <msp430f47197.h>
//#include <msp430_math.h>
#include "timeDate.h"
#include "lcd.h"

//--------------------------------------------------------------------

volatile unsigned int tmrCnt = 0;
volatile unsigned int secondCount = 0;
volatile unsigned int checkMask = 0x0;

//--------------------------------------------------------------------

void setupPorts()
{
	P3DIR |= 0x80;
	P5DIR |= 0x3f;// P5REN |= BIT5;

	P8DIR |= 0x21;

	P5OUT |= OLED_RST;
}

//--------------------------------------------------------------------
//
int handle_secondEvents()
{
    // pulse heatbeat
	P5OUT ^= BIT1;

	OLED_clockCycle();

	//if (secondCount % 60 == 0)
	//	  handle_minuteEvents();
    return 1;
}

//--------------------------------------------------------------------
//
//void handle_minuteEvents()
//{}

//--------------------------------------------------------------------
//
int main()
{
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer

    // CCR0 interrupt enabled
	CCTL0 = CCIE;
	// SMCLK/8/countup
	TACTL = TASSEL_2 + MC_1 + ID_3;
	// 1MHz / 8(ID_3) / 100Hz
	CCR0 = 374;

	setupPorts();

	_BIS_SR(GIE); // interrupts enabled

	while(1)
    {

	   // ten millis events
	   //if (checkMask && 0x1 == 0x1)
       //

	   // half-second based events
	   if (checkMask && 0x2 == 0x2)
	   {
		  checkMask ^= 0x2;
		  P8OUT ^= BIT0;
	   }

	   // second based events
       if (checkMask && 0x4 == 0x4)
	   //if ((BIT2 >> checkMask) & 1 == 1)
	   {
    	 checkMask ^= 0x4;
    	 secondCount++;
    	 handle_secondEvents();
       }
    }
}

//--------------------------------------------------------------------
// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
   tmrCnt++;

   // ten millis events
   //checkMask |= 0x1;

   // half-second based events
   if (tmrCnt % 50 == 0)
	 checkMask |= 0x2;

   // second based events
   if (tmrCnt % 100 == 0)
	 checkMask |= 0x4;

}







