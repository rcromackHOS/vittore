/*
 * bms.c
 *
 *  Created on: May 24, 2016
 *      Author: rcromack
 */

#include "bms.h"
#include "config.h"
#include "Hardware.h"
#include "engineController.h"
#include "Common.h"

//--------------------------------------------------------------------

//extern struct engineStruct engine;

//---------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//
// RETURN/UPDATES:
//
//---------------------------------------------------------------------------------------------
void check_BatteryBox_Status()
{
	static int _contactorOn;
	static int _heaterOn;
	static int _engineOn;

	//---------------------------------------------- VOLTAGE

	if (VALUE_24V < _LOW_SP_24V && _FORCE_ENGINE_RUN == 0)
	{
		setStateCode(2);
		_engineOn = 1;
	}
	else
		clearStateCode(2);

	if (VALUE_24V > _HIGH_SP_24V)
		_engineOn = 0;

	//---------------------------------------------- TEMPERATURE

	// if battery box is colder than 1C
	if (VALUE_BAT_TEMP < 100)
	    _contactorOn = 0;

	// if battery box is colder than 5C
	if (VALUE_BAT_TEMP <= _LOW_SP_TEMP)
		_heaterOn = 1;

	// if battery box is warmer than 5C and the contactor has previously been open
	if (VALUE_BAT_TEMP > _LOW_SP_TEMP)
		_contactorOn = 1;

	// if battery box is warmer than 20C and the batteryBox heater was on
	if (_heaterOn == 1 && VALUE_BAT_TEMP >= _NOMINAL_SP_TEMP)
		_heaterOn = 0;

	//---------------------------------------------- BMS

	// BMS cell monitors. Give them a grace period
	if ((P2IN & IN_BAT_CELL_FAIL) != 0 && _BANK_BMS_TMR_D == 0 && BMS_EVENT == 0)
	{
		// if this is a newly detected problem
		if (_CELLMONITOR_TMR_D == 0 && BMS_EVENT == 0)
			_CELLMONITOR_TMR_D = _CELLMONITOR_TMR_SP;
	}
	else if ((P2IN & IN_BAT_CELL_FAIL) == 0 && _CELLMONITOR_TMR_D != 0)
		_CELLMONITOR_TMR_D = 0;

	// BMS _SYS_FAILURE_ DETECTED
	// If our cell monitors detect a problem for 30 seconds, and
	if ( (_CELLMONITOR_TMR_D == 1 && (P2IN & IN_BAT_CELL_FAIL) == 0) ||
		 VALUE_24V >= _HIGH_SP_BMS || VALUE_24V <= _LOW_SP_BMS  &&
		 _BANK_BMS_TMR_D == 0 && BMS_EVENT == 0)
	{
		_BANK_BMS_TMR_D = 1;		// start counting down the cool-down period
		_CELLMONITOR_TMR_D = 0;
		_engineOn = 0;					// turn off the engine
	}

	// BMS _SYS_FAILURE_ APPLIED
	// apply the _SYS_FAILURE_, alert the system
	if (_BANK_BMS_TMR_D >= _BANK_BMS_TMR_SP)
	{
		_SYS_FAILURE_ = 1;
		setStateCode( 99 );

		BMS_EVENT = 1;

		_BANK_BMS_TMR_D = 0;

		// Shut down all the loads
		_heaterOn = 0;
		_contactorOn = 0;
		_engineOn = 0;

		P3OUT &= ~BIT6;			// lights off
	}

	//----------------------------------------------

	setContactor(_contactorOn);
	setBatteryHeater(_heaterOn);
	setEngineRun(_engineOn);
}

//--------------------------------------------------------------------
//
int setContactor(int s)
{
	  if (s == 1 && BMS_EVENT == 0)
	  {
		clearStateCode(1);
		P10OUT |= OUT_nCONTACTOR_ON;
	    P10OUT &= ~OUT_ASSET_I2;
	  }
	  else
	  {
		setStateCode(1);
		P10OUT &= ~OUT_nCONTACTOR_ON;
	    P10OUT |= OUT_ASSET_I2;
	  }
	  return 1;
}

//--------------------------------------------------------------------
//
int setBatteryHeater(int s)
{
	if (VALUE_BAT_TEMP == -300)
	{
		setStateCode(14);
		P10OUT &= ~OUT_nBATTERY_HEATER_ON;
	}
	else
	{
		clearStateCode(14);

		if (s == 1 && BMS_EVENT == 0)
		{
			setStateCode(3);
			P10OUT |= OUT_nBATTERY_HEATER_ON;
		}
		else
		{
			clearStateCode(3);
			P10OUT &= ~OUT_nBATTERY_HEATER_ON;
		}
	}

	return 1;
}

//--------------------------------------------------------------------
//
int setEngineRun(int s)
{
	if (s == 1 &&
		BMS_EVENT == 0 &&
		_SYS_FAILURE_ == 0)
	{
		_FORCE_ENGINE_RUN = 1;
	}
	else
	{
		_FORCE_ENGINE_RUN = 0;
		engine.mode = ENGINE_STOP;
	}
	return 1;
}




















