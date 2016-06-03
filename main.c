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
#include "button.h"
#include "MCP7940N.h"
#include "bms.h"

//--------------------------------------------------------------------

int isQuietTime(dateTimeStruct now);
void setStateCode(int code);
void clearStateCode(int code);
void setupPorts();
int handle_secondEvents();
void handle_minuteEvents();
int setLightsState(int s);

void handle_failEvent();
void handle_lowfuelEvent();
void handle_lighting();
void resetControl();
void buildIdleArray();
void handle_oilchangeClear();
void handle_reset();
int RTC_init();
void handle_unitModeIndicator();

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
	P1DIR |= 0x02;
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
void inc_secondCounts()
{
	//---------------------------------------------
	// BMS centric timers
	if (_CELLMONITOR_TMR_D != 0)
		_CELLMONITOR_TMR_D--;

	if (_BANK_BMS_TMR_D != 0)
		_BANK_BMS_TMR_D--;

	if (_BANK_BMS_TMR_D != 0)
		_BANK_BMS_TMR_D--;

	//---------------------------------------------
	// Engine centric timers

	if (count_run > 0)
		count_run++;

	if (count_RPM_fail > 0)
		count_RPM_fail++;

	if (count_oil_fail > 0)
		count_oil_fail++;

	if (count_temp_fail > 0)
		count_temp_fail++;

	//

	if (PREHEAT_D != 0)
		PREHEAT_D--;

	if (CRANK_D != 0)
		CRANK_D--;

	if (POST_D != 0)
		POST_D--;

	if (ATTEMPT_D != 0)
		ATTEMPT_D--;


	//---------------------------------------------
	// Lights centric timers

	if (LIGHTS1HOUR_TMR != 0)
		LIGHTS1HOUR_TMR--;

	if (_RESETTING_ == 1)
		RESET_TMR++;

	if (OILCHANGE_PRESS_TMR != 0)
		OILCHANGE_PRESS_TMR++;

	//---------------------------------------------
	// screen centric timers

	if (_DIAGNOSTIC_MODE_TMR != 0)
		_DIAGNOSTIC_MODE_TMR--;

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

	//RTC_begin();

	//now = RTC_now();

	// -------------------------- default values

	defaultSunset = time(0, 0, 19);
	defaultSunrise = time(0, 0, 7);
	now = datetime(0, 0, 0, 1, 1, 2015);

	// pull coorindates from memory
	lat = 0.0;
	lng = 0.0;

	solar_setCoords(lat, lng);

	//sunSet = solar_getSunset(now);
	//sunRise = solar_getSunrise(now);

	_UNIT_MODE = 1;

	buildIdleArray();

	volatile int countseconds = 0;

	static int newMode;

	typedef enum
	{
		INIT,
		ADCs_UPDATED,
		BATTERY_STATUS,
		ENGINE_ANALYSIS,
		ENGINE_STATUS,
		ALERT_INDICATORS,
		HANDLE_UNITMODES,
		UPDATE_SCREEN,
		FAILURE_RESET

	} state_types;

	state_types state_system = ADCs_UPDATED;

	int done = 0;

	// --------------------------

	_BIS_SR(GIE); // interrupts enabled

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
	   // half-second based events
	   if (checkMask && 0x2 == 0x2)
	   {
		   //checkMask ^= 0x2;
			checkMask &= ~0x2;
			P8OUT ^= BIT0;

			countseconds++;
		}

	   // ---------------------------------------
	   // Increment second counts
	   if (countseconds == 2)
	   {
			countseconds = 0;
			secondCount++;

			inc_secondCounts();

			// pulse heatbeat
		    P5OUT ^= HEARTBEAT_PIN;
	   }

	   // ---------------------------------------
	   // State machine
	   switch (state_system)
	   {

			case INIT:
					if (done == 1)
						state_system++;
					else
					{
						//RTC_begin();

						done = 1;
					}

					break;

			case ADCs_UPDATED:

				//if (_ADCs_UPDATED == 1)
				//{
				//	_ADCs_UPDATED = 0;
				//}

				state_system++;
				break;

			case BATTERY_STATUS:		// Check the battery box sensor data, also impliments BMS protection

				//check_BatteryBox_Status();

				state_system++;
				break;

			case ENGINE_ANALYSIS:

				//newMode = check_Engine_Status();

				state_system++;
				break;

			case ENGINE_STATUS:

				//set_Engine_State(newMode);

				state_system++;
				break;

			case ALERT_INDICATORS:

				//handle_failEvent();
				//handle_lowfuelEvent();
				//handle_oilchangeClear();

				state_system++;
				break;

			case HANDLE_UNITMODES:

				handle_unitModeIndicator();

				//if (_RESETTING_ == 1)
				//	handle_reset();

				//handle_lighting();

				state_system++;
				break;

			case UPDATE_SCREEN:

				//updateDisplay();
				P8OUT ^= BIT0;
				//for (j = 100; j > 0; j--)
				//{}
				state_system = ADCs_UPDATED;
				break;
	   }

    }

	// if theres a BMS problem, turn off the inverter
	//if (BMS_EVENT == 1)
	//	P11OUT &= ~INVERTER_PIN;
	//else
	//	P11OUT |= INVERTER_PIN;

}

//--------------------------------------------------------------------
// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
   tmrCnt++;
   // ten millis events
   if (tmrCnt % 2 == 0)
   	   button_stateMachine();

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

//------------------------------------------------------------------------------------------------------------------------------------

void handle_unitModeIndicator()
{
	int i;
	for (i = 2; i >= 0; i--)
	{
		if (_UNIT_MODE == buttonList[i].mode)
		{
			P4OUT |= buttonList[i].LEDpin;
			break;
		}
		else
			P4OUT &= ~buttonList[i].LEDpin;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
// Initialize the RTC
// Check for lost tme or errors
int RTC_init()
{
	if (RTC_isrunning() == 99)	// 99 = no I2C response
	{
		setStateCode(11);
	}
	else
    {
		// If we've lost the time
		if (RTC_lostTime() == 1)
	    {
	      setStateCode(10);     // 10 = RTC lost time (LIR32 failure?)
	      RTC_adjust(now);
	    }
	}

	return 1;
}

//------------------------------------------------------------------------------------------------------------------------------------
// Initialize the GPS
// If
int GPS_init(void)
{
	/*
	if (pokeGPS())
	{
		if (stateCode == 10) // lost time, update by GPS
	    {
	        RTC.adjust(GPS._utc);
	        now = RTC.now();
	    }
	}
*/
	return 1;
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



//--------------------------------------------------------------------
// System failure
// Turn on the LED and alert the asset tracker
void handle_failEvent()
{
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
}

//--------------------------------------------------------------------
// Low fuel
// alert the asset tracker
void handle_lowfuelEvent()
{

	if (P2IN & LOWFUEL == 1)
		P10OUT |= ASSET_IN3_PIN;
	else
		P10OUT &= ~ASSET_IN3_PIN;
}

//--------------------------------------------------------------------
// Handles the way the unit using
void handle_lighting()
{
	// Lights 1 hour mode.
	// Count to 1 hour, then default to auto mode.
	if (_UNIT_MODE == MODE_LIGHT1H)
	{
		if (LIGHTS1HOUR_TMR == 0)
			LIGHTS1HOUR_TMR = 1;

		if (LIGHTS1HOUR_TMR == 3600)
		{
			_UNIT_MODE = MODE_AUTO;
			LIGHTS1HOUR_TMR = 0;
		}
	}

	// Control the light logic.
	// If it's in auto and it's nighttime, turn lights. Or if lights on mode.
	if ( (_UNIT_MODE == MODE_AUTO && isQuietTime( now ) == 0) || _UNIT_MODE == MODE_LIGHT1H)
		setLightsState(1);
	else
		setLightsState(0);
}

//--------------------------------------------------------------------
// Handle the reset button functionality
void handle_reset()
{
	// Implimentation of reset functionality
	// hold reset button 5 seconds, apply the reset
	// then clear, reset flag a second after
		if (buttonList[3].state == MAST_NOMINAL)
		{
			if (RESET_TMR == 5)
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

		if (RESET_TMR == 6)
		{
			RESET_TMR = 0;
			_RESETTING_ = 0;
		}
}



//--------------------------------------------------------------------
// Handle oil change indication and reset
// Oil Change alert functionality
// 25 hours before an oil change is due, report to the asset tracker
// hold OCC button for 5 seconds to reset the oil change to 500 hours in the future
void handle_oilchangeClear()
{
	if (_OILCHANGE_DUE - engine.engineHours <= 25)
	{
		P10OUT |= ASSET_IN1_PIN;
		setStateCode( 4 );

		// pull up and still input?
		if (P2IN & OIL_RST_PIN == 0)
		{
			if (OILCHANGE_PRESS_TMR == 0)
				OILCHANGE_PRESS_TMR = 1;

			if (OILCHANGE_PRESS_TMR == 6)
			{
				OILCHANGE_PRESS_TMR = 0;
				_OILCHANGE_DUE += engine.engineHours;

				// write oil change hours to EEPROM
			}
		}
	}
}

//--------------------------------------------------------------------
// Build the Idle time array with 0s
void buildIdleArray()
{
	idleTime idles[4] =
			{
					{0, 0},
					{0, 0},
					{0, 0},
					{0, 0}
			};
}


