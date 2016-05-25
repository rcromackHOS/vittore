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
extern struct engineStruct engine;

//--------------------------------------------------------------------
//
void batteryStatus()
{
	//---------------------------------------------- VOLTAGE

	static int _contactorOn;
	static int _engineOn;
	static int _heaterOn;

	static int _softCount;
	static int _BMSprefailure;

	if (VALUE_24V < _LOW_SP_24V && _engineOn == 0)
	{
		_engineOn = 1;
		set_Engine_State(ENGINE_PRE);
	}

	if (VALUE_24V > _HIGH_SP_24V && _engineOn == 1)
	{
		_engineOn = 0;
		set_Engine_State(ENGINE_STOP);
	}

	//---------------------------------------------- TEMPERATURE

	// if battery box is colder than 1C
	if (VALUE_BAT_TEMP < 1)
	{
		_contactorOn = 0;
		P10OUT &= ~CONTACTOR_PIN;
	}

	// if battery box is colder than 5C
	if (VALUE_BAT_TEMP <= _LOW_SP_TEMP)
	{
		_heaterOn = 1;
		P10OUT |= BATTERYHTR_PIN;
	}

	// if battery box is warmer than 5C and the contactor has previously been open
	if (_contactorOn == 0 && VALUE_BAT_TEMP > _LOW_SP_TEMP)
	{
		_contactorOn = 1;
		P10OUT |= CONTACTOR_PIN;
	}

	// if battery box is warmer than 20C and the batteryBox heater was on
	if (_heaterOn == 1 && VALUE_24V >= _NOMINAL_SP_TEMP)
	{
		_heaterOn = 0;
		P10OUT &= ~BATTERYHTR_PIN;
	}

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
		set_Engine_State(ENGINE_STOPPING);
	}
	else if (_BMSprefailure > 0)
		_BMSprefailure--;

	// BMS FAILURE APPLIED
	if (_BMSprefailure >= 5)
	{
		failure = 0;
		setStateCode( 99 );

		// BMS failure
		P10OUT &= ~BATTERYHTR_PIN;
		P10OUT &= ~CONTACTOR_PIN;

		P3OUT &= ~BIT6;
	}

	//----------------------------------------------



}

//--------------------------------------------------------------------


