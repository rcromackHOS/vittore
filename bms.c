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
void batteryStatus()
{
	static int _contactorOn;
	//static int _engineOn;
	static int _heaterOn;

	static int _softCount;
	static int _BMSprefailure;

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
	if (VALUE_BAT_TEMP < 1)
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

	// BMS cell monitors. Give them a 30 second grace
	if (P2IN & BMS_PIN == 0)
	    _softCount++;

	else if (_softCount > 0)
		_softCount--;

	// BMS FAILURE DETECTED
	if (_softCount > _CELLMONITOR_SP || VALUE_24V >= _HIGH_SP_BMS || VALUE_24V <= _LOW_SP_BMS)
	{
		_BMSprefailure++;
		_FORCE_ENGINE_RUN = 0;
	}
	else if (_BMSprefailure > 0)
		_BMSprefailure--;

	// BMS FAILURE APPLIED
	if (_BMSprefailure >= 5)
	{
		failure = 1;
		setStateCode( 99 );

		BMS_EVENT = 1;

		// BMS failure
		_heaterOn = 0;
		_contactorOn = 0;
		_FORCE_ENGINE_RUN = 0;

		P3OUT &= ~BIT6;
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




















