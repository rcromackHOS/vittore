//***************************************************************************************
//
//  Built with Code Composer Studio v5
//***************************************************************************************


//#include <msp430f47197.h>
//#include <msp430_math.h>
#include <msp430.h>
#include "config.h"

#include "timeDate.h"
#include "engineController.h"
#include "lcd.h"
#include "bms.h"

//--------------------------------------------------------------------

static volatile unsigned int tmrCnt = 0;
static volatile unsigned int secondCount = 0;
volatile unsigned int checkMask = 0x0;

//--------------------------------------------------------------------

void setupPorts()
{
	P2DIR |= 0x01;
	P3DIR |= 0x80;

	P5DIR |= 0x3f;// P5REN |= BIT5;

	P8DIR |= 0x21;
	P10DIR |= 0x30;

	P5OUT |= OLED_RST;
}

//--------------------------------------------------------------------
//
void delayms(int m)
{
  int i;
  //int j;
  for (i = m; i > 0; i--)
  {
	//  for (j = m; j > 0; j--) { }
  }

}

//--------------------------------------------------------------------
//
int handle_secondEvents()
{
    // pulse heatbeat
	P5OUT ^= BIT1;

	//OLED_command(0xA0); //on second line
	//OLED_data(0x1F); //write solid blocks



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

	engineSetup(0);

	OLED_setup();

	_BIS_SR(GIE); // interrupts enabled

	volatile int countseconds = 0;

	while(1)
    {
	   // ten millis events
	   //if (checkMask && 0x1 == 0x1)
       //

	   // half-second based events
	   if (checkMask && 0x2 == 0x2)
	   {
		  //checkMask ^= 0x2;
		   checkMask &= ~0x2;
		   P8OUT ^= BIT0;

		   countseconds++;
	       if (countseconds == 2)
	       {
	    	   countseconds = 0;
	    	   secondCount++;
	    	   handle_secondEvents();
           }

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
   //if (tmrCnt % 100 == 0)
   //{
	 //if (checkMask && 0x4 != 0x4)
   //	   checkMask |= 0x4;
   //}
}







