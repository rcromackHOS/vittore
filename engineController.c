/*
 * engineController.c
 *
 *  Created on: May 18, 2016
 *      Author: Rhys
 */

#include "engineController.h"
#include "config.h"

//--------------------------------------------------------------------



//--------------------------------------------------------------------
// C imitation of a Constructor for the engine
int engineSetup(int engMins, int engHrs)
{
	engine.engineHours = engHrs;
	engine.engineMins = engMins;

	engine.lastRun = datetime( 0, 0, 0, 0, 0, 0 );
	engine.lastRunEnd = datetime( 0, 0, 0, 0, 0, 0 );

	return 1;
}

//--------------------------------------------------------------------
//
int checkEngineTemp()
{
	if (P7IN & ENGINE_TEMP_PIN == 0)
	  return 0;
	return 1;
}

//--------------------------------------------------------------------
//
int checkOilPressure()
{
	if (P7IN & OIL_PIN == 0)
	  return 1;
	return 0;
}

//--------------------------------------------------------------------
//
int checkEngineRPMs()
{
	return 0;
}

//--------------------------------------------------------------------
//
int check_Engine_Status()
{
	int mode = engine.mode;

	// See if the engine started
	if (engine.mode != ENGINE_RUNNING &&
		engine.mode != ENGINE_POST &&
		engine.mode != ENGINE_STOP &&
		checkOilPressure() == 1)
	{
		if (count_run == 0 && mode != ENGINE_POST)
			count_run = 1;

		if (count_run == 3)
		{
			mode = ENGINE_POST;
			count_run = 0;
		}
	}

	// Engine running checks
	else if (engine.mode == ENGINE_RUNNING)
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

				mode = ENGINE_STOPPING;
				failure = 1;

				count_RPM_fail = 0;
			}
		}

		// Check Engine Oil Pressure
		if (checkOilPressure() == 0)
		{
			if (count_oil_fail == 0)
				count_oil_fail = 1;

		    if (count_oil_fail == 4)
		    {
		    	mode = ENGINE_STOPPING;

				failure = 1;
				setStateCode( 34 );

				count_oil_fail = 0;
		    }
		}

		// Check Engine Temperature
		if (checkEngineTemp() == 0)
		{
			if (count_temp_fail == 0)
				count_temp_fail = 1;

		    if (count_temp_fail == 4)
		    {
		    	mode = ENGINE_STOPPING;

				failure = 1;
				setStateCode( 33 );

				count_temp_fail = 0;
		    }
		}
	}

	return mode;
}

//--------------------------------------------------------------------
//
void set_Engine_State(int mode)
{
	if (failure == 1)
	  mode = ENGINE_STOPPING;

	switch (mode)
	{
		case ENGINE_STOP:

			   engine.mode = ENGINE_STOPPING;

			   break;


		case ENGINE_PRE:

			 if (engine.mode != ENGINE_PRE)
			 {
			   P2OUT |= ACCESSORY_PIN;
			   P9OUT |= GLOWPLUG_PIN;
			   P8OUT |= ASSET_IGN_PIN;

			   engine.mode = ENGINE_PRE;

			   PREHEAT_D = _PREHEAT_SP;
			   REATTEMPTS_D = _REATTEMPTS_SP;
			 }

			 if (PREHEAT_D == 0)
				 set_Engine_State(ENGINE_CRANK);

			 break;


		case ENGINE_CRANK:

			 if (engine.mode != ENGINE_CRANK)
			 {
			   P9OUT |= CRANK_PIN;

			   engine.mode = ENGINE_CRANK;

			   CRANK_D = _CRANK_SP;
			 }

			 if (CRANK_D == 0)
				 set_Engine_State(ENGINE_REATTEMPT);

			 break;


		case ENGINE_REATTEMPT:

			 if (engine.mode != ENGINE_REATTEMPT)
			 {
				P9OUT &= ~CRANK_PIN;

				engine.mode = ENGINE_REATTEMPT;

				ATTEMPT_D = _ATTEMPT_SP;
				REATTEMPTS_D--;
			 }

			 if (ATTEMPT_D == 0)
				 set_Engine_State(ENGINE_CRANK);

			 if (REATTEMPTS_D <= 0)
			 {
				failure = 1;
				setStateCode( 30 );

				set_Engine_State(ENGINE_STOPPING);
			 }

			 break;


		case ENGINE_POST:

			 if (engine.mode != ENGINE_POST)
			 {
			   engine.mode = ENGINE_POST;

			   P9OUT &= ~CRANK_PIN;

			   POST_D = _POST_SP;
			 }

			 if (POST_D == 0)
			 {
				 if (checkEngineRPMs() == 1)
				 {
					 P9OUT &= ~GLOWPLUG_PIN;
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
			}
		   	else
			{
				engine.runTime++;					    // increment seconds of running
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

			P2OUT |= ACCESSORY_PIN;
			P9OUT &= ~CRANK_PIN;
		    P9OUT &= ~GLOWPLUG_PIN;
		    break;


		case ENGINE_STOPPING:

		   if (engine.mode != ENGINE_STOP && engine.mode != ENGINE_STOPPING)
		   {
			   if (engine.mode == ENGINE_RUNNING)
				   engine.lastRunEnd = now;

			   P2OUT &= ~ACCESSORY_PIN;
			   P9OUT &= ~CRANK_PIN;
			   P9OUT &= ~GLOWPLUG_PIN;
			   P8OUT &= ~ASSET_IGN_PIN;
		   }

		   engine.mode = ENGINE_STOP;

		   break;


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














