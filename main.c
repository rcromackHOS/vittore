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
/*   05/20/16  R Cromack       Initial creation.							  */
/*   06/14/16  R Cromack	   Integration of firmware with program main	  */
/******************************************************************************/

//#include <msp430.h>
#include "config.h"

#include "Hardware.h"
#include "msp430f5419A.h"
#include "Common.h"
#include "WatchdogTimerControl.h"
#include "AtoD.h"
#include "GPS.h"
#include "timeDate.h"
#include "engineController.h"
#include "lcd.h"
#include "button.h"
#include "bms.h"
#include "solar.h"
#include "mast.h"
#include "MAX31855.h"
#include "Flash.h"
#include "RTC.h"

//--------------------------------------------------------------------

void enterLowPowerMode();
void exitLowPowerMode();

void setLightsState(int s);
int isQuietTime(dateTimeStruct now);

void handleIndicators();

void handleSystemFailEvent();
void handleLowFuelEvent();
void handle_lighting();
void handleButtonLight();

void handleCabinetHeating();
void checkTimeToSaveMemory();
void incrementSecondCounts();

//--------------------------------------------------------------------

timeStruct defaultSunset;
timeStruct defaultSunrise;

//--------------------------------------------------------------------

static unsigned int tmrCnt = 0;
static int secondCount = 0;
static int preLoadADCs = 5;

//---------------------------------------------------------------------------------------------
// DESCRIPTION:	Main
//
// RETURN/UPDATES:	int
//---------------------------------------------------------------------------------------------
int main()
{
    WdtDisable();
    __disable_interrupt();

    // -------------------------- initization

    InitializeHardware();

    buildButtonStateMachine();

    InitializeDisplay();

    InitializeEngine();

	InitializeRTC();

    // --------------------------

	mast_stateMachine( MAST_NOMINAL );

    // -------------------------- default values

	defaultSunset = time(0, 0, 19);
	defaultSunrise = time(0, 0, 7);
	sunSet = time(0, 0, 19);
	sunRise = time(0, 0, 7);

    // --------------------------

    switch (GetConfiguration())
    {
   	  	  case SEG_VIRGIN:
   	  	  case 0:
    	  	  	  	  break;
   	  	  default: 	  // Flash is bad!
    	  	  	  	  // Run on defaults

    	  	  	  	  _nop();
   	  		  	  	  break;
   	 }

	solar_setCoords(lat, lng);

	sunSet = solar_getSunset(now);
	sunRise = solar_getSunrise(now);

	// --------------------------

	UpdateFlashMemory();
    static int newMode;

	// --------------------------

	if (_MAST_STATUS == MAST_MAXDOWN)
		enterLowPowerMode();

	_BIS_SR(GIE); // interrupts enabled
	WdtEnable();  // enable Watch dog

	// --------------------------

	while(1)
    {
		if (_ADCs_UPDATED_ == 1)
		{
			loadAnalogData();

			_ADCs_UPDATED_ = 0;
		}

		if (_LPMODE == 0)
		{
		    if (preLoadADCs == 0)
			{
				check_BatteryBox_Status();

				newMode = check_Engine_Status();
				set_Engine_State(newMode);
			}

			pollGPS();

			pollTime();

			handleIndicators();

			handleCabinetHeating();

			handle_pressDiagButton();

			handle_reset();

			handle_lighting();

			updateDisplay();

			checkTimeToSaveMemory();

    	}
		else
		{

			if (_MAST_STATUS != MAST_MAXDOWN)
			{
				InitializeDisplay();

				WdtEnable();  // enable Watch dog

				_LPMODE = 0;

				_SYS_FAILURE_ = 0;
				BMS_EVENT = 0;

				P5OUT |= BIT4;

				_UPDATE_SCREEN_ = 1;
			}
		}

		WdtKeepAlive();  // reset Watch dog
    }

}

//--------------------------------------------------------------------
// Timer A1 interrupt service routine
//#pragma vector = TIMER0_A0_VECTOR
//__interrupt void TIMER0_A0_ISR (void)
//---------------------------------------------------------------------------------------------
// DESCRIPTION:		-TimerA1 Interrupt
//
// RETURN/UPDATES:	n/a
//---------------------------------------------------------------------------------------------
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
   tmrCnt++;
	   // ten millis events
   if (tmrCnt % 2 == 0)
   {
		button_stateMachine();
		mastUpDown();
   }

   if (_LPMODE == 0)
   {
	   // second based events
	   if (tmrCnt % 100 == 0)
	   {
		   P8OUT ^= OUT_HEARTBEAT_LED;

		   incrementSecondCounts();
	   }

	   // count GPS state
	   if (GpsStateCountdown != 0)
		   GpsStateCountdown--;

	   // temp countdown
	   if (TempCountdown != 0)
		   TempCountdown--;

	   // count presses of the diagnostic button
	   if ((P9IN & BUTTON_nDIAGNOSTIC) == 0)
		   diagBackButton++;
	   else
		   diagBackButton = 0;

		// countdown before starting
		if (RtcCountdown != 0)
			RtcCountdown--;

		// the timeout for i2C comms.
		if (RtcTimeout != 0)
			RtcTimeout--;
   }
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		-PORT2 Interrupt
//
// RETURN/UPDATES:	-Checks for solar wing latch pulled.
//					-See if the mast is no longer down, leave low power mode
//---------------------------------------------------------------------------------------------
#pragma vector = PORT2_VECTOR
__interrupt void Port_2(void)
{
	if (_LPMODE == 1)
	{
		sleepModeProcess();

		if (_MAST_STATUS != MAST_MAXDOWN)
		{
			//__disable_interrupt();

			//_BIS_SR(GIE);
			//_BIC_SR(LPM3_EXIT); // wake up from low power mode

			//WdtEnable();  // enable Watch dog

			_LPMODE = 0;

			_SYS_FAILURE_ = 0;
			BMS_EVENT = 0;

			P5OUT |= BIT4;

			P2IFG &= ~IN_MAST_CUT_OUT;
		}
	}
}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:		handles the redumentary indicators
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void handleIndicators()
{
	handleSystemFailEvent();

	handleLowFuelEvent();

	handle_oilchangeClear();

	handleButtonLight();
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		light up the buttons, to indicate the mode we're in
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void handleButtonLight()
{
	int i;
	for (i = 2; i >= 0; i--)
	{
		if (_UNIT_MODE == buttonList[i].mode)
			P4OUT |= buttonList[i].LEDpin;

		else if (_UNIT_MODE != buttonList[i].mode || _LPMODE == 1)
			P4OUT &= ~buttonList[i].LEDpin;
	}
	P4OUT &= ~BIT0;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void setLightsState(int s)
{
	if (s == 1 && BMS_EVENT == 0)
		P3OUT |= OUT_LIGHTS_ON;

    else
    	P3OUT &= ~OUT_LIGHTS_ON;

	if (_LPMODE == 1)
		P3OUT &= ~OUT_LIGHTS_ON;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Calculates if we're inside or outside, sunrise and sunset
//					if no coordinates are found, use a default schedule
//
// RETURN/UPDATES:	0 - we're outside quiet time, turn on lights
//					1 - we're inside quiet time, lights off
//---------------------------------------------------------------------------------------------
int isQuietTime(dateTimeStruct now)
{
  if (_FORCE_LIGHTS_ON == 1)
    return 0;

  timeStruct SS = defaultSunset;
  timeStruct SR = defaultSunrise;

  if (SOLAR_validCalc == 1)
  {
    SS = sunSet;
    SR = sunRise;
  }

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
    if (now.hour < SS.hour || (now.hour == SS.hour && now.minute >= SS.minute))
      return 1;
    else if (now.hour > SR.hour || (now.hour == SR.hour && now.minute < SR.minute))
      return 1;
  }

  return 1;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		handles the indicator LEDs for a failed condition
//					runs the GPS output
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void handleSystemFailEvent()
{
	if (_SYS_FAILURE_ == 1)
	{
		P9OUT |= OUT_ENGINE_FAIL;
		P10OUT |= OUT_ASSET_I4;
	}
	else
	{
		P9OUT &= ~OUT_ENGINE_FAIL;
		P10OUT &= ~OUT_ASSET_I4;
	}

	// if theres a BMS problem, turn off the inverter
	if (BMS_EVENT == 1)
		P11OUT &= ~OUT_nSPARE2_ON;
	else
		P11OUT |= OUT_nSPARE2_ON;

	if (_LPMODE == 1)
	{
		P9OUT &= ~OUT_ENGINE_FAIL;
		P10OUT &= ~OUT_ASSET_I4;
	}
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		handles the low fuel event. Turn on the indicator LED
//					run the GPS input
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void handleLowFuelEvent()
{
	if ((P2IN & IN_nLOW_FUEL) == 0)
		P10OUT |= OUT_ASSET_I3;
	else
		P10OUT &= ~OUT_ASSET_I3;

	if (_LPMODE == 1)
		P10OUT &= ~OUT_ASSET_I3;

}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		runs through the conditions to run the lights
//						-	Auto
//								Checks for lighting schedule
//						-	Lights on 1 hr
//								runs the lights for a full hour
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void handle_lighting()
{
	// Lights 1 hour mode.
	// Count to 1 hour, then default to auto mode.
	if (_UNIT_MODE == MODE_LIGHT1H)
	{
		// set the lights to begin counting
		if (LIGHTS1HOUR_TMR == 0)
			LIGHTS1HOUR_TMR = 1;
		// count until 3600 seconds (1 hour) elapses
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

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		checks for conditions to run the PCB cabinet heater
//
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void handleCabinetHeating()
{
	if (VALUE_INTERNAL_TEMP <= _LOW_SP_INTERNAL_TEMP)
		P10OUT |= OUT_nCABINET_HEATER_ON;

	if (VALUE_INTERNAL_TEMP >= _HIGH_SP_INTERNAL_TEMP)
		P10OUT &= ~OUT_nCABINET_HEATER_ON;

	if (_LPMODE == 1)
		P10OUT &= ~OUT_nCABINET_HEATER_ON;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		checks the time to save our accumulated data
//						-	saves onces a day, at sunRise
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void checkTimeToSaveMemory()
{
	static int memSaved;

	if ((now.hour == sunRise.hour && now.minute == sunRise.minute && memSaved == 0) || _FORCE_SAVEMEMORY == 1)
	{
    	UpdateFlashMemory();
    	memSaved = 1;
    	_FORCE_SAVEMEMORY = 0;
	}
	else if (now.hour != sunRise.hour && now.minute != sunRise.minute)
		memSaved = 0;
}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Second based counts
//						increment the counters that require proper second increments
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void incrementSecondCounts()
{
	secondCount++;

	//---------------------------------------------
	// BMS centric timers
	if (_CELLMONITOR_TMR_D != 0)
		_CELLMONITOR_TMR_D--;

	if (_BANK_BMS_TMR_D != 0)
		_BANK_BMS_TMR_D++;

	//---------------------------------------------
	// Engine centric timers

	if (engine.runTime > 0)
	{
		engine.runTime++;
		runTime();
	}

	if (count_run != 0)
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

	if (OILCHANGE_PRESS_TMR != 0)
		OILCHANGE_PRESS_TMR++;

	//---------------------------------------------
	// screen centric timers

	if (_DIAGNOSTIC_MODE_TMR != 0)
		_DIAGNOSTIC_MODE_TMR--;

	if (_SCREEN_UPDATE_D != 0)
		_SCREEN_UPDATE_D--;

	if (preLoadADCs != 0)
		preLoadADCs--;
}








