/*****< Main.c>    ************************************************************/
/*                                                                            */
/*  Main Loop							  					  				  */
/*                                                                            */
/*  Author: Tom Nixon / Rhys Cromack                                          */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/13/16  Tom Nixon       Initial creation.
/*   06/14/16  R Cromack	   Integration with program main
/******************************************************************************/

#include <msp430.h>
#include "config.h"

// toms includes
#include "Hardware.h"
#include "msp430f5419A.h"
#include "Common.h"
#include "WatchdogTimerControl.h"

#include "timeDate.h"
#include "engineController.h"
#include "lcd.h"
#include "button.h"
#include "MCP7940N.h"
#include "bms.h"

//--------------------------------------------------------------------

void init_clocks();

int isQuietTime(dateTimeStruct now);
void setStateCode(int code);
void clearStateCode(int code);

int handle_secondEvents();
void handle_minuteEvents();

int setLightsState(int s);

void handle_failEvent();
void handleLowFuelEvent();
void handle_lighting();
void resetControl();
void buildIdleArray();
void handle_oilchangeClear();
void handle_reset();
int RTC_init();
void handle_unitModeIndicator();

//--------------------------------------------------------------------

timeStruct defaultSunset;
timeStruct defaultSunrise;

//--------------------------------------------------------------------

static unsigned int tmrCnt = 0;
static volatile int checkMask = 0x0;
static int secondCount = 0;

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

void init_clocks()
{
    // CCR0 interrupt enabled
	CCTL0 = CCIE;
	// SMCLK/8/countup
	TACTL = TASSEL_2 + MC_1 + ID_3;
	// 1MHz / 8(ID_3) / 100Hz
	CCR0 = 374;
	/*

	P5SEL |= 0x0C;                        // Port select XT2
	P7SEL |= 0x03;                        // Select XT1

	// -------------------------- clock

	TA0CCTL0 |= CCIE;
	TA0CCR0 = 1000;

	TA0CTL |= TASSEL__SMCLK + MC__UP + ID__1 + TACLR;
    */
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
		_BANK_BMS_TMR_D++;

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
    WdtDisable();
    __disable_interrupt();

    // -------------------------- initization

    init_clocks();

    ConfigurePins();

    ConfigureA2D();

	buildButtonStateMachine();

	OLED_setup();

	// -------------------------- Engine

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

	_UNIT_MODE = MODE_AUTO;

	buildIdleArray();

	volatile int countseconds = 0;

	static int newMode;

	failure = 0;

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

	   P2OUT &= ~ACCESSORY_PIN;
	   P9OUT &= ~CRANK_PIN;
	   P9OUT &= ~GLOWPLUG_PIN;
	   P8OUT &= ~ASSET_IGN_PIN;

	// --------------------------

	WdtEnable();  // enable Watch dog

	__enable_interrupt();

	//_BIS_SR(GIE); // interrupts enabled

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
		    P5OUT ^= OUT_HEARTBEAT_LED;
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

				if (_ADCs_UPDATED_ == 1)
				{
					loadAnalogData();

					_ADCs_UPDATED_ = 0;
				}

				state_system++;
				break;

			case BATTERY_STATUS:		// Check the battery box sensor data, also impliments BMS protection

				check_BatteryBox_Status();

				state_system++;
				break;

			case ENGINE_ANALYSIS:
				/*
				if (P1IN & BIT0 != 0)
					_FORCE_ENGINE_RUN = 1;

				else
					_FORCE_ENGINE_RUN = 0;
				*/

				newMode = check_Engine_Status();

				state_system++;
				break;

			case ENGINE_STATUS:

				set_Engine_State(newMode);

				state_system++;
				break;

			case ALERT_INDICATORS:

				handle_failEvent();
				//handle_IN_nLOW_FUELEvent();
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


	   WdtKeepAlive();  // reset Watch dog
    }

	// if theres a BMS problem, turn off the inverter
	//if (BMS_EVENT == 1)
	//	P11OUT &= ~OUT_nSPARE2_ON;
	//else
	//	P11OUT |= OUT_nSPARE2_ON;

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
//	button sample;
	for (i = 2; i >= 0; i--)
	{
//		sample = buttonList[i];
		if (_UNIT_MODE == buttonList[i].mode)
			P4OUT |= buttonList[i].LEDpin;

		else
			P4OUT &= ~buttonList[i].LEDpin;
	}
	P4OUT &= ~BIT0;
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
		P3OUT |= OUT_LIGHTS_ON;

    else
    	P3OUT &= ~OUT_LIGHTS_ON;

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
		P9OUT |= OUT_ENGINE_FAIL;
		P10OUT |= OUT_ASSET_I4;
	}
	else
	{
		P9OUT &= ~OUT_ENGINE_FAIL;
		P10OUT &= ~OUT_ASSET_I4;
	}
}

//--------------------------------------------------------------------
// Low fuel
// alert the asset tracker
void handleLowFuelEvent()
{

	if (P2IN & IN_nLOW_FUEL == 1)
		P10OUT |= OUT_ASSET_I3;
	else
		P10OUT &= ~OUT_ASSET_I3;
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
		P10OUT |= OUT_ASSET_I1;
		setStateCode( 4 );

		// pull up and still input?
		if (P2IN & BUTTON_nOIL_RST == 0)
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


