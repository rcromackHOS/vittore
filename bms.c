/*
 * bms.c
 *
 *  Created on: May 24, 2016
 *      Author: rcromack
 */

#include "bms.h"
#include "config.h"

//--------------------------------------------------------------------
extern struct engineStruct engine;
/*
extern volatile int _HIGH_SP_24V;
extern volatile int _LOW_SP_24V;
extern volatile int VALUE_24V;

extern volatile int _HIGH_SP_BMS;
extern volatile int _LOW_SP_BMS;

extern volatile int _LOW_SP_12V;
extern volatile int VALUE_12V;

extern volatile int _HIGH_SP_TEMP;
extern volatile int _NOMINAL_SP_TEMP;
extern volatile int _LOW_SP_TEMP;
extern volatile int VALUE_BAT_TEMP;
*/
//--------------------------------------------------------------------
//
void batteryStatus()
{
	//---------------------------------------------- VOLTAGE

	int _low24v;
	int _high24v;

	static int _contactorOn;
	static int _engineOn;
	static int _heaterOn;

	static int _BMSprefailure;

	_low24v = VALUE_24V < _LOW_SP_24V ? 1 : 0;
	_high24v = VALUE_24V > _HIGH_SP_24V ? 1 : 0;

	if (_low24v == 1 && _engineOn == 0)
	{
		_engineOn = 1;
		set_Engine_State(ENGINE_PRE);
	}

	if (_high24v == 1 && _engineOn == 1)
	{
		_engineOn = 0;
		set_Engine_State(ENGINE_STOP);
	}

	//---------------------------------------------- TEMPERATURE

	// if battery box is colder than 1C
	if (VALUE_BAT_TEMP <= 1)
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
	// BMS FAILURE DETECTED
	if (P2IN & BMS_PIN == 0 || VALUE_24V >= _HIGH_SP_BMS || VALUE_24V <= _LOW_SP_BMS)
	{
		_BMSprefailure++;
		set_Engine_State(ENGINE_STOPPING);
	}
	else
	{
		if (_BMSprefailure > 0)
			_BMSprefailure--;
	}

	// BMS FAILURE APPLIED
	if (_BMSprefailure >= 5)
	{
		failure = 0;

		// BMS failure
		P10OUT &= ~BATTERYHTR_PIN;
		P10OUT &= ~CONTACTOR_PIN;

		P3OUT &= ~BIT6;
	}


	//----------------------------------------------

}

//--------------------------------------------------------------------


