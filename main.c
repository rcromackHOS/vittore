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
/*   06/13/16  Tom Nixon       Initial creation.							  */
/*   06/14/16  R Cromack	   Integration with program main				  */
/******************************************************************************/

#include <msp430.h>
#include "config.h"

// toms includes
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
#include "MCP7940N.h"
#include "bms.h"
#include "solar.h"

//--------------------------------------------------------------------

void InitializeClocks();

int isQuietTime(dateTimeStruct now);

int handle_secondEvents();
void handle_minuteEvents();

int setLightsState(int s);

void handleSystemFailEvent();
void handleLowFuelEvent();
void handle_lighting();
void resetControl();

void handle_unitModeIndicator();
void handleCabinetHeating();

//--------------------------------------------------------------------

timeStruct defaultSunset;
timeStruct defaultSunrise;

//--------------------------------------------------------------------

static unsigned int tmrCnt = 0;
static int checkMask = 0;
static int secondCount = 0;

//--------------------------------------------------------------------
//
void inc_secondCounts()
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

	if (OILCHANGE_PRESS_TMR != 0)
		OILCHANGE_PRESS_TMR++;

	//---------------------------------------------
	// screen centric timers

	if (_DIAGNOSTIC_MODE_TMR != 0)
		_DIAGNOSTIC_MODE_TMR--;

	if (_SCREEN_UPDATE_D != 0)
		_SCREEN_UPDATE_D--;

}

//--------------------------------------------------------------------
//
void handle_minuteEvents()
{
	secondCount = 0;
}

//--------------------------------------------------------------------
//
void InitializeClocks()
{
	// ACLK/countup/divby 1
	TA0CTL = TASSEL__ACLK + MC_1 + ID_2;

	//TA0CCR0 = ACLK / ID_2 / 100 - 1;
	TA0CCR0 = ((32768 / 4) / 100) - 1;

	// CCR0 interrupt enabled
	TA0CCTL0 = CCIE;
}

//--------------------------------------------------------------------
//
int main()
{
    WdtDisable();
    __disable_interrupt();

    // -------------------------- initization

    InitializeClocks();

    InitializeHardware();

	InitializeRTC();

    buildButtonStateMachine();

    InitializeDisplay();

    InitializeEngine();

    InitializeGPS();

    //InitializeSolar();

    // -------------------------- default values

	defaultSunset = time(0, 0, 19);
	defaultSunrise = time(0, 0, 7);

	// pull coorindates from memory
	//lat = 51.2;
	//lng = -113.9;
	/*
	lat = 40.00000;
	lng = -105.00000;

	solar_setCoords(lat, lng);

	sunSet = solar_getSunset(now);
	sunRise = solar_getSunrise(now);
	*/

	// --------------------------

	_UNIT_MODE = MODE_AUTO;
    _SYS_FAILURE_ = 0;

    updateDisplay();

    // --------------------------

    static int newMode;

    typedef enum
	{
		INIT,
		ADCs_UPDATED,
		BATTERY_STATUS,
		ENGINE_ANALYSIS,
		ENGINE_STATUS,
		CHECK_GPS,
		GENERAL_EVENTS,
		HANDLE_UNITMODES,
		UPDATE_SCREEN,
		_SYS_FAILURE__RESET

	} state_types;

	state_types state_system = ADCs_UPDATED;

	int done = 0;

	// --------------------------

	WdtEnable();  // enable Watch dog
	_BIS_SR(GIE); // interrupts enabled

	// --------------------------

	while(1)
    {
	   // ---------------------------------------
	   // half-second based events
	   if ((checkMask & 0x1) == 1)
	   {
			checkMask &= ~0x1;
	   }

	   // ---------------------------------------
	   // Increment second counts
	   if ((checkMask & 0x2) == 2)
	   {
		    checkMask &= ~0x2;

			inc_secondCounts();

			// pulse heatbeat
		    P8OUT ^= OUT_HEARTBEAT_LED;
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

					//get thermocouple data over SPI

					_ADCs_UPDATED_ = 0;
				}

				state_system++;
				break;

			case BATTERY_STATUS:		// Check the battery box sensor data, also impliments BMS protection

				check_BatteryBox_Status();

				state_system++;
				break;

			case ENGINE_ANALYSIS:

				if (_UNIT_MODE == MODE_LIGHT1H)
					_FORCE_ENGINE_RUN = 1;

				newMode = check_Engine_Status();

				state_system++;
				break;

			case ENGINE_STATUS:

				set_Engine_State(newMode);

				state_system++;
				break;

			case CHECK_GPS:

				handleGPSevent();

				state_system++;
				break;

			case GENERAL_EVENTS:

				handleSystemFailEvent();
				handleLowFuelEvent();

				handle_oilchangeClear();
				handleCabinetHeating();

				state_system++;
				break;

			case HANDLE_UNITMODES:

				handle_unitModeIndicator();

				if (_RESETTING_ == 1)
					handle_reset();

				handle_lighting();

				state_system++;
				break;

			case UPDATE_SCREEN:

				if (_SCREEN_UPDATE_D == 0)
				{
					if (_DIAGNOSTIC_MODE != 1)
						_SCREEN_UPDATE_D = 3;
					else
						_SCREEN_UPDATE_D = 1;

					updateDisplay();
				}

				state_system = ADCs_UPDATED;
				break;
	   }


	   WdtKeepAlive();  // reset Watch dog
    }
}

//--------------------------------------------------------------------
// Timer A0 interrupt service routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
   tmrCnt++;
   // ten millis events
   if (tmrCnt % 2 == 0)
   	   button_stateMachine();

   // half-second based events
   if (tmrCnt % 50 == 0)
	   checkMask |= 0x1;

   // second based events
   if (tmrCnt % 100 == 0)
	   checkMask |= 0x2;

   // count GPS state
   if (GpsStateCountdown != 0)
	   GpsStateCountdown--;

   // temp countdown
   if (TempCountdown != 0)
	   TempCountdown--;

}

//------------------------------------------------------------------------------------------------------------------------------------
//
void handle_unitModeIndicator()
{
	int i;

	for (i = 2; i >= 0; i--)
	{
		if (_UNIT_MODE == buttonList[i].mode)
			P4OUT |= buttonList[i].LEDpin;

		else
			P4OUT &= ~buttonList[i].LEDpin;
	}
	P4OUT &= ~BIT0;
}

//--------------------------------------------------------------------
//
int setLightsState(int s)
{
	if (s == 1 && BMS_EVENT == 0)
		P3OUT |= OUT_LIGHTS_ON;

    else
    	P3OUT &= ~OUT_LIGHTS_ON;

	return 1;
}

//--------------------------------------------------------------------
//
int isQuietTime(dateTimeStruct now)
{
  if (_FORCE_LIGHTS_ON == 1)
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
    if (now.hour < SS.hour || (now.hour == SS.hour && now.minute >= SS.minute))
      return 1;
    else if (now.hour > SR.hour || (now.hour == SR.hour && now.minute < SR.minute))
      return 0;
  }

  return 1;
}

//--------------------------------------------------------------------
// System Failure
// Turn on the LED and alert the asset tracker
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
}

//--------------------------------------------------------------------
// Low fuel
// alert the asset tracker
void handleLowFuelEvent()
{
	if ((P2IN & IN_nLOW_FUEL) == 0)
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
//
void handleCabinetHeating()
{
	if (VALUE_INTERNAL_TEMP <= _LOW_SP_INTERNAL_TEMP)
		P10OUT |= OUT_nCABINET_HEATER_ON;

	if (VALUE_INTERNAL_TEMP >= _HIGH_SP_INTERNAL_TEMP)
		P10OUT &= ~OUT_nCABINET_HEATER_ON;
}









