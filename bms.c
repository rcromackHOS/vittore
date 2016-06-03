/*
 * bms.c
 *
 *  Created on: May 24, 2016
 *      Author: rcromack
 */

#include "bms.h"
#include "config.h"
#include "engineController.h"

//--------------------------------------------------------------------

//extern struct engineStruct engine;

//--------------------------------------------------------------------
//
void check_BatteryBox_Status()
{
	static int _contactorOn;
	static int _heaterOn;

	//---------------------------------------------- VOLTAGE

	if (VALUE_24V < _LOW_SP_24V && _FORCE_ENGINE_RUN == 0)
	{
		_FORCE_ENGINE_RUN = 1;
		set_Engine_State(ENGINE_PRE);
	}

	// doesn't look for _engineOn because we want it to respect _FORCE_ENGINE_RUN calls
	if (VALUE_24V > _HIGH_SP_24V && _FORCE_ENGINE_RUN == 1)
	{
		_FORCE_ENGINE_RUN = 0;
		set_Engine_State(ENGINE_STOP);
	}

	//---------------------------------------------- TEMPERATURE

	// if battery box is colder than 1C
	if (VALUE_BAT_TEMP < 100)
	    _contactorOn = 0;

	// if battery box is colder than 5C
	if (VALUE_BAT_TEMP <= _LOW_SP_TEMP)
		_heaterOn = 1;

	// if battery box is warmer than 5C and the contactor has previously been open
	if (_contactorOn == 0 && VALUE_BAT_TEMP > _LOW_SP_TEMP)
		_contactorOn = 1;

	// if battery box is warmer than 20C and the batteryBox heater was on
	if (_heaterOn == 1 && VALUE_24V >= _NOMINAL_SP_TEMP)
		_heaterOn = 0;

	//---------------------------------------------- BMS

	// BMS cell monitors. Give them a grace period
	if (P2IN & BMS_PIN == 0)
	{
		// if this is a newly detected problem
		if (_CELLMONITOR_TMR_D == 0 && BMS_EVENT == 0)
			_CELLMONITOR_TMR_D = _CELLMONITOR_TMR_SP;
	}

	// BMS FAILURE DETECTED
	// If our cell monitors detect a problem for 30 seconds, and
	if ( (_CELLMONITOR_TMR_D == 0 && P2IN & BMS_PIN == 0) || VALUE_24V >= _HIGH_SP_BMS || VALUE_24V <= _LOW_SP_BMS)
	{
		_BANK_BMS_TMR_D = _BANK_BMS_TMR_SP;		// start counting down the cool-down period
		_FORCE_ENGINE_RUN = 0;					// turn off the engine
	}

	// BMS FAILURE APPLIED
	// apply the failure, alert the system
	if (_BANK_BMS_TMR_D == 0)
	{
		failure = 1;
		setStateCode( 99 );

		BMS_EVENT = 1;

		// Shut down all the loads
		_heaterOn = 0;
		_contactorOn = 0;
		_FORCE_ENGINE_RUN = 0;

		P3OUT &= ~BIT6;			// lights off
	}

	//----------------------------------------------

	setContactor(_contactorOn);
	setBatteryHeater(_heaterOn);
	setEngineRun(_FORCE_ENGINE_RUN);
}

//--------------------------------------------------------------------
//
int setContactor(int s)
{
	  if (s && BMS_EVENT == 0)
	  {
	    P10OUT |= CONTACTOR_PIN;
	    P10OUT |= ASSET_IN2_PIN;
	  }
	  else
	  {
	    P10OUT &= ~CONTACTOR_PIN;
	    P10OUT &= ~ASSET_IN2_PIN;
	  }
	  return 1;
}

//--------------------------------------------------------------------
//
int setBatteryHeater(int s)
{
	if (s && BMS_EVENT == 0)
	{
	    setStateCode(2);
	    P10OUT |= BATTERYHTR_PIN;
	}
    else
	{
	    clearStateCode(2);
	    P10OUT &= ~BATTERYHTR_PIN;
	}
	return 1;
}

//--------------------------------------------------------------------
//
int setEngineRun(int s)
{
	if (s == 0 || BMS_EVENT == 1)
		engine.mode = ENGINE_STOPPING;
	else
		engine.mode = ENGINE_PRE;

	return 1;
}




















