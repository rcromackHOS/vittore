/*
 * engineController.c
 *
 *  Created on: May 18, 2016
 *      Author: Rhys
 */

#include "engineController.h"
#include "config.h"
#include "Hardware.h"
#include "timeDate.h"
#include "Common.h"

//--------------------------------------------------------------------

void InitializeEngine()
{
	// pull stored oil change due hours
	_OILCHANGE_DUE = 500;

    // pull stored engine hours
    engine.engineHours = 0;
	// pull store engine minutes
	engine.engineMins = 0;

	engine.lastRun = datetime( 0, 0, 0, 0, 0, 0 );
	engine.lastRunEnd = datetime( 0, 0, 0, 0, 0, 0 );

	// assemble the array of stored idle times
	buildIdleArray();
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

//--------------------------------------------------------------------
//
int checkEngineTemp()
{
	if (P7IN & IN_ENGINE_TEMP_FAIL == 0)
	  return 1;
	return 0;
}
int skippp = 0;

//--------------------------------------------------------------------
//
int checkOilPressure()
{
	return skippp;

	if ((P7IN & IN_OIL_PRESSURE_OK) == 0)
	  return 1;
	return 0;
}

//--------------------------------------------------------------------
//
int checkEngineRPMs()
{
	if (P1IN & BIT7 != 0)
		return 1;
	return 0;
}

//--------------------------------------------------------------------
//
int check_Engine_Status()
{
	int mode = engine.mode;

	if (_FORCE_ENGINE_RUN == 1)
	{
		if (mode == ENGINE_STOP ||
			mode == ENGINE_STOP)
		{
			mode = ENGINE_PRE;
		}

		// ---------------------------------------- See if the engine started
		if (mode != ENGINE_RUNNING &&
			mode != ENGINE_POST &&
			mode != ENGINE_STOP)
		{
			if (checkOilPressure() == 1)
			{
				if (count_run == 0 && mode != ENGINE_POST)
					count_run = 1;

				if (count_run == 3)
				{
					mode = ENGINE_POST;
					count_run = 0;
				}
			}
			else if (count_run > 0)
				count_run = 0;
		}

		// ---------------------------------------- Engine running checks
		else if (mode == ENGINE_RUNNING)
		{
			// Check engine RPMS
			if (checkEngineRPMs() != 1)
			{
				if (count_RPM_fail == 0)
					count_RPM_fail = 1;

				if (count_RPM_fail == 4)
				{
					if (checkEngineRPMs() == 0)
						setStateCode( 32 );

					if (checkEngineRPMs() == 2)
						setStateCode( 31 );

					mode = ENGINE_STOP;
					_SYS_FAILURE_ = 1;

					count_RPM_fail = 0;
				}
			}

			// ---------------------------------------- Check Engine Oil Pressure
			if (checkOilPressure() == 0)
			{
				if (count_oil_fail == 0)
					count_oil_fail = 1;

				if (count_oil_fail == 4)
				{
					mode = ENGINE_STOP;

					_SYS_FAILURE_ = 1;
					setStateCode( 34 );

					count_oil_fail = 0;
				}
			}
			/*
			// ----------------------------------------  Check Engine Temperature
			if (checkEngineTemp() == 0)
			{
				if (count_temp_fail == 0)
					count_temp_fail = 1;

				if (count_temp_fail == 4)
				{
					mode = ENGINE_STOP;

					_SYS_FAILURE_ = 1;
					setStateCode( 33 );

					count_temp_fail = 0;
				}
			}
			 */
		}
	}
	else
	{
		if (mode != ENGINE_STOP &&
			mode != ENGINE_STOP)
		{
			mode = ENGINE_STOP;
		}
	}

	return mode;
}

//--------------------------------------------------------------------
//
void set_Engine_State(int mode)
{
	if (_SYS_FAILURE_ == 1)
	  mode = ENGINE_STOP;

	switch (mode)
	{

		case ENGINE_PRE:

			 if (engine.mode != ENGINE_PRE)
			 {
			   P2OUT |= OUT_ENGINE_ACC;
			   P9OUT |= OUT_ENGINE_GLOW;
			   P8OUT |= OUT_ASSET_IGNITION;

			   engine.mode = ENGINE_PRE;

			   PREHEAT_D = _PREHEAT_SP;
			   REATTEMPTS_D = _REATTEMPTS_SP;

			   setStateCode(40);
			 }

			 if (PREHEAT_D == 0)
				 set_Engine_State(ENGINE_CRANK);

			 break;


		case ENGINE_CRANK:

			 if (engine.mode != ENGINE_CRANK)
			 {
			   P9OUT |= OUT_ENGINE_CRANK;

			   engine.mode = ENGINE_CRANK;

			   CRANK_D = _CRANK_SP;
			 }

			 if (CRANK_D == 0)
				 set_Engine_State(ENGINE_REATTEMPT);

			 break;


		case ENGINE_REATTEMPT:

			 if (engine.mode != ENGINE_REATTEMPT)
			 {
				P9OUT &= ~OUT_ENGINE_CRANK;

				engine.mode = ENGINE_REATTEMPT;

				ATTEMPT_D = _ATTEMPT_SP;
				REATTEMPTS_D--;
			 }

			 if (ATTEMPT_D == 0)
				 set_Engine_State(ENGINE_CRANK);

			 if (REATTEMPTS_D <= 0)
			 {
				_SYS_FAILURE_ = 1;
				setStateCode( 30 );

				set_Engine_State(ENGINE_STOP);
			 }

			 break;


		case ENGINE_POST:

			 if (engine.mode != ENGINE_POST)
			 {
			   engine.mode = ENGINE_POST;

			   P9OUT &= ~OUT_ENGINE_CRANK;

			   POST_D = _POST_SP;
			 }

			 if (POST_D == 0)
			 {
				 if (checkEngineRPMs() == 1)
				 {
					 P9OUT &= ~OUT_ENGINE_GLOW;
					 set_Engine_State(ENGINE_RUNNING);
				 }
				 else
					 set_Engine_State(ENGINE_REATTEMPT);
			 }

			 break;


		case ENGINE_RUNNING:

			if (engine.mode != ENGINE_RUNNING)
			{
		   	    engine.mode = ENGINE_RUNNING;

		   	    engine.lastRun = now;
				store_idleTime( );

				engine.lastRunEnd = datetime( 0, 0, 0, 0, 0, 0 );

				setStateCode(41);

				engine.runTime=1;
			}

			P2OUT |= OUT_ENGINE_ACC;
			P9OUT &= ~OUT_ENGINE_CRANK;
		    P9OUT &= ~OUT_ENGINE_GLOW;
		    break;


		case ENGINE_STOP:

		   //if (engine.mode != ENGINE_STOP && engine.mode != ENGINE_STOP)
		   //{
			clearStateCode(40);
			clearStateCode(41);

			if (engine.mode == ENGINE_RUNNING)
				   engine.lastRunEnd = now;

			   P2OUT &= ~OUT_ENGINE_ACC;
			   P9OUT &= ~OUT_ENGINE_CRANK;
			   P9OUT &= ~OUT_ENGINE_GLOW;
			   P8OUT &= ~OUT_ASSET_IGNITION;
		   //}

		   engine.mode = ENGINE_STOP;

		   break;


	}
}

//--------------------------------------------------------------------
//
void runTime()
{
	// increment seconds of running
	if (engine.runTime % 60 == 0)			// every 60 seconds, increment minutes
	{
		engine.engineMins++;

		if (engine.engineMins == 60)		// every 60 minutes increment hours
		{
			engine.engineHours++;
			engine.engineMins = 0;
			// write hours to EEPROM
		}

		// write minutes to EEPROM
	}
}

//--------------------------------------------------------------------
//
int store_idleTime()
{
	if (engine.lastRunEnd.year == 0)
	  return 0;

	int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	int hrs;
	int mins;
	int dayDif;
	int month;

	if (engine.lastRunEnd.minute > engine.lastRun.minute)
	{
		hrs = -1;
		mins = (60 - engine.lastRunEnd.minute) + engine.lastRun.minute;
	}
	else
		mins = engine.lastRun.minute - engine.lastRunEnd.minute;

	if (engine.lastRunEnd.hour > engine.lastRun.hour)
	{
		hrs += (24 - engine.lastRunEnd.hour) + engine.lastRun.hour;
		dayDif = -1;
	}
	else if (engine.lastRunEnd.hour < engine.lastRun.hour)
		hrs += engine.lastRun.hour - engine.lastRunEnd.hour;

	if (engine.lastRun.day < engine.lastRunEnd.day)
	{
		month = engine.lastRun.month - 1;
		if (month == 0)
			month = 11;

		dayDif += (daysInMonth[month] - engine.lastRunEnd.day) + engine.lastRun.day;
	}
	else
		dayDif += engine.lastRun.day - engine.lastRunEnd.day;

	hrs += (dayDif * 24);

	int i;
	for (i = 3; i > 0; i--)
	{
		idles[i+1].hours = idles[i].hours;
		idles[i+1].minutes = idles[i].minutes;
	}

	idles[0].hours = hrs;
	idles[0].minutes = mins;

	return 1;
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
		if ((P2IN & BUTTON_nOIL_RST) == 0)
		{
			if (OILCHANGE_PRESS_TMR == 0)
				OILCHANGE_PRESS_TMR = 1;

			if (OILCHANGE_PRESS_TMR == 6)
			{
				OILCHANGE_PRESS_TMR = 0;
				_OILCHANGE_DUE += engine.engineHours;

				clearStateCode( 4 );

				// write oil change hours to EEPROM
			}
		}
	}
}












