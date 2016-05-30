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
#include "button.h"
#include "MCP7940N.h"

//--------------------------------------------------------------------

int isQuietTime(dateTimeStruct now);
void setStateCode(int code);
void clearStateCode(int code);
void setupPorts();
int handle_secondEvents();
void handle_minuteEvents();
int setLightsState(int s);

//--------------------------------------------------------------------

// outputs
#define HEARTBEAT_PIN 	BIT1
#define FAIL_LED_PIN   	BIT4
#define LIGHTS_PIN   	BIT4

// port 2
#define OIL_RST_PIN   	BIT1

// port 10
#define ASSET_IN1_PIN   BIT0
#define ASSET_IN3_PIN   BIT2
#define ASSET_IN4_PIN  	BIT3

// Port 11
#define CABINET_HTR_PIN	BIT7
#define INVERTER_PIN	BIT1
#define SPARE3_PIN   	BIT0
#define SPARE4_PIN  	BIT2

// Inputs
#define LOWFUEL   		BIT5

//--------------------------------------------------------------------

timeStruct defaultSunset;
timeStruct defaultSunrise;

//--------------------------------------------------------------------

static unsigned int tmrCnt = 0;
static volatile int checkMask = 0x0;
static int secondCount = 0;

//--------------------------------------------------------------------

void setupPorts()
{
	P1DIR |= 0xdf;
	P2DIR |= 0x21;
	P3DIR |= 0x80;
	P4DIR |= 0x3f;
	P5DIR |= 0x3f;

	P7DIR |= 0x00;
	P8DIR |= 0x21;
	P9DIR |= 0x7f;
	P10DIR |= 0xff;

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
    static int resetCount;
    static int lights1hCount;
    static int clearOilChangeCount;
    static int oldMinute;

	// pulse heatbeat
	P5OUT ^= HEARTBEAT_PIN;

	//OLED_command(0xA0); //on second line
	//OLED_data(0x1F); //write solid blocks

	// Check the battery box sensor data, also impliments BMS protection
	batteryStatus();

	// System failure
	// Turn on the LED and alert the asset tracker
	if (failure == 1)
	{
		P9OUT |= FAIL_LED_PIN;
		P10OUT |= ASSET_IN4_PIN;
	}
	else
	{
		P9OUT &= ~FAIL_LED_PIN;
		P10OUT &= ~ASSET_IN4_PIN;
	}

	// if theres a BMS problem, turn off the inverter
	//if (BMS_EVENT == 1)
	//	P11OUT &= ~INVERTER_PIN;
	//else
	//	P11OUT |= INVERTER_PIN;

	// Low fuel
	// aler the asset tracker
	if (P2IN & LOWFUEL == 1)
		P10OUT |= ASSET_IN3_PIN;
	else
		P10OUT &= ~ASSET_IN3_PIN;

	// Lights 1 hour mode.
	// Count to 1 hour, then default to auto mode.
	if (_UNIT_MODE == MODE_LIGHT1H)
	{
		lights1hCount++;
		if (lights1hCount == 3600)
		{
			lights1hCount = 0;
			_UNIT_MODE = MODE_AUTO;
		}
	}

	// Control the light logic.
	// If it's in auto and it's nighttime, turn lights. Or if lights on mode.
	if ( (_UNIT_MODE == MODE_AUTO && isQuietTime( now ) == 0) || _UNIT_MODE == MODE_LIGHT1H)
		setLightsState(1);
	else
		setLightsState(0);

	// Increment through the engine state machine
	engineStatus();

	// Diagnostic Mode
	// high-jacks button functions
	// reset becomes close
	// mast up/down become page up/down
	// stays active for 60 seconds
	if (_DIAGNOSTIC_MODE == 1)
	{
		_DIAGNOSTIC_MODE_TMR--;
		if (_DIAGNOSTIC_MODE_TMR == 0)
			_DIAGNOSTIC_MODE = 0;

	    // update screen diagnostic page
	}
	//else
	//{

		// update screen
	//}


	// Implimentation of reset functionality
	// hold reset button 5 seconds, apply the reset
	// then clear, reset flag a second after
	if (_RESETTING_ == 1)
	{
		resetCount++;

		if (buttonList[3].state == MAST_NOMINAL)
		{
			if (resetCount == 5)
			{
				if (_DIAGNOSTIC_MODE == 1)
				{
					_DIAGNOSTIC_MODE = 0;
					_DIAGNOSTIC_MODE_TMR = 0;
				}
				else
				{
					failure = 0;
					BMS_EVENT = 0;

					clearStateCode(_STATE_CODE);
				}
			}

		}

		if (resetCount == 6)
		{
		  resetCount = 0;
		  _RESETTING_ = 0;
		}
	}

	// Oil Change alert functionality
	// 25 hours before an oil change is due, report to the asset tracker
	// hold OCC button for 5 seconds to reset the oil change to 500 hours in the future
	if (_OILCHANGE_DUE - engine.engineHours <= 25)
	{
		P10OUT |= ASSET_IN1_PIN;
		setStateCode( 4 );

		// pull up and still input?
		if (P2IN & OIL_RST_PIN == 0)
		{
			clearOilChangeCount++;
			if (clearOilChangeCount == 5)
			{
				clearOilChangeCount = 0;
				_OILCHANGE_DUE += engine.engineHours;

				// write oil change hours to EEPROM
			}
		}
		else if (clearOilChangeCount > 0)
			clearOilChangeCount = 0;
	}

	// Jump to minute events
	if (oldMinute != now.minute())
	{
		handle_minuteEvents();
		oldMinute = now.minute();
	}

	return 1;
}

//--------------------------------------------------------------------
//
void handle_minuteEvents()
{
	secondCount = 0;
}

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

	// -------------------------- initization

	setupPorts();
	buildButtonStateMachine();
	OLED_setup();


	// pull stored oil change due hours
	_OILCHANGE_DUE = 500;

	// pull stored engine hours
    int EngineHours = 0;
	// pull store engine minutes
    int EngineMinutes = 0;

	engineSetup(EngineMinutes, EngineHours);

	RTC_begin();

	now = RTC_now();

	// -------------------------- default values

	defaultSunset = time(0, 0, 19);
	defaultSunrise = time(0, 0, 7);
	now = datetime(0, 0, 0, 1, 1, 2015);

	_UNIT_MODE = 0;

	// --------------------------

	_BIS_SR(GIE); // interrupts enabled

	volatile int countseconds = 0;

	while(1)
    {
	    // I2C stuff
		//Transmit process
	    //Rx = 0;
	    //TXByteCtr = 1;
	    //Transmit();
	    //Receive process
	    //Rx = 1;
	    //Receive();

	   // ---------------------------------------

		enum state_stystem;
		{

		}

		Switch(state_system) {

			case Intit: //are we done
					if(done)
						state_system++;
					else
						break;

			case A_Ds_updated:

			case engine_analysis:
			case engine_control:
			case

		}

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

   //tom
   if(engine_countdown != 0)
	   engine_countdown--;

   // ten millis events
   //if (tmrCnt % 2 == 0)
	//   button_stateMachine();

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

//--------------------------------------------------------------------
//
int setLightsState(int s)
{
	if (s && BMS_EVENT == 0)
		P3OUT |= LIGHTS_PIN;

    else
    	P3OUT &= ~LIGHTS_PIN;

	return 1;
}

//--------------------------------------------------------------------
//
int isQuietTime(dateTimeStruct now)
{
  if (_FORCE_LIGHTS_ON)
    return 0;

  timeStruct SS = defaultSunset;
  timeStruct SR = defaultSunrise;

  /*
  if (Rhys.validCalc)
  {
    SS = sunSet;
    SR = sunRise;
  }
  */

  if (SR.hour > SS.hour)
  {
    if (now.hour > SS.hour || (now.hour == SS.hour && now.minute >= SS.minute))
    {
      if (now.hour < SR.hour || (now.hour == SR.hour && now.minute < SR.minute))
        return 0;
    }
  }
  else
  {
    if (now.hour > SS.hour || (now.hour == SS.hour && now.minute >= SS.minute))
      return 1;
    else if (now.hour < SR.hour || (now.hour == SR.hour && now.minute < SR.minute))
      return 0;
  }

  return 1;
}

//--------------------------------------------------------------------
//
void clearStateCode(int code)
{
	if (code == _STATE_CODE)
		_STATE_CODE = 0;
}

void setStateCode(int code)
{
	if (code > _STATE_CODE)
		_STATE_CODE = code;
}




