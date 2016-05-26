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
int engineSetup(int engHrs)
{
	engine.engineHours = engHrs;
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
void engineStatus()
{
	static int count_run;
	static int count_RPM_fail;
	static int count_oil_fail;
	static int count_temp_fail;

	int mode = engine.mode;

	// See if the engine started
	if (engine.mode != ENGINE_RUNNING &&
		engine.mode != ENGINE_POST &&
		engine.mode != ENGINE_STOP &&
		checkOilPressure() == 1)
	{
		count_run++;
		if (count_run > 2)
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
			count_RPM_fail++;
			if (count_RPM_fail > 3)
			{
				if (checkEngineRPMs() == 0)
				{
					mode = ENGINE_STOPPING;
					failure = 1;
					setStateCode( 32 );
				}

				if (checkEngineRPMs() == 2)
				{
					mode = ENGINE_STOPPING;
					failure = 1;
					setStateCode( 31 );
				}
			}
		}
		else if (count_RPM_fail > 0)
			count_RPM_fail--;

		// Check Engine Oil Pressure
		if (checkOilPressure() == 0)
		{
			count_oil_fail++;
		    if (count_oil_fail > 3)
		    {
		    	mode = ENGINE_STOPPING;

				failure = 1;
				setStateCode( 34 );
		    }
		}
		else if (count_oil_fail > 0)
			count_oil_fail--;

		// Check Engine Temperature
		if (checkEngineTemp() == 0)
		{
			count_temp_fail++;
		    if (count_temp_fail > 3)
		    {
		    	mode = ENGINE_STOPPING;

				failure = 1;
				setStateCode( 33 );
		    }
		}
		else if (count_temp_fail > 0)
			count_temp_fail--;
	}

	// After all else, increment the engine state.
	set_Engine_State(mode);

}

//--------------------------------------------------------------------
//
void set_Engine_State(int mode)
{
	if (failure == 1)
	  mode = ENGINE_STOPPING;

	   if (mode == ENGINE_STOP)
	   {
		   if (engine.mode == ENGINE_RUNNING)
			 engine.mode = ENGINE_STOPPING;
	   }

	   if (mode == ENGINE_PRE)
	   {

		 if (engine.mode != ENGINE_PRE)
		 {
		   P2OUT |= ACCESSORY_PIN;
		   P9OUT |= GLOWPLUG_PIN;
		   P8OUT |= ASSET_IGN_PIN;

		   engine.mode = ENGINE_PRE;

		   PREHEAT_D = _PREHEAT_SP;
		   REATTEMPTS_D = _REATTEMPTS_SP;
		 }

		 if (PREHEAT_D > 0)
		 {
		   PREHEAT_D--;

		   if (PREHEAT_D == 0)
		     set_Engine_State(ENGINE_CRANK);
		 }

	   }

	   else if (mode == ENGINE_CRANK)
	   {
		 if (engine.mode != ENGINE_CRANK)
		 {
		   P9OUT |= CRANK_PIN;

		   engine.mode = ENGINE_CRANK;

		   CRANK_D = _CRANK_SP;
		 }

		 if (CRANK_D > 0)
		 {
		    CRANK_D--;

		    if (CRANK_D == 0)
		        set_Engine_State(ENGINE_REATTEMPT);

		 }

	   }

	   else if (mode == ENGINE_REATTEMPT)
	   {
		 if (engine.mode != ENGINE_REATTEMPT)
		 {
		    P9OUT &= ~CRANK_PIN;

		    engine.mode = ENGINE_REATTEMPT;

		    ATTEMPT_D = _ATTEMPT_SP;
		    REATTEMPTS_D--;
		 }

		 if (REATTEMPTS_D > 0)
		 {
		 	if (ATTEMPT_D > 0)
		 	   ATTEMPT_D--;

		 	else if (ATTEMPT_D == 0)
			   set_Engine_State(ENGINE_CRANK);
		 }

 	 	 if (REATTEMPTS_D <= 0)
		 {
		    failure = 1;
		    setStateCode( 30 );

		    set_Engine_State(ENGINE_STOPPING);
		 }

	   }

	   else if (mode == ENGINE_POST)
	   {
		 if (engine.mode != ENGINE_POST)
		 {
		   engine.mode = ENGINE_POST;

		   P9OUT &= ~CRANK_PIN;

		   POST_D = _POST_SP;
		 }

		 if (POST_D > 0)
		 {
		   POST_D--;

		   if (POST_D == 0)
		   {
			 if (checkEngineRPMs())
			 {
			   P9OUT &= ~GLOWPLUG_PIN;
		       set_Engine_State(ENGINE_RUNNING);
			 }
			 else
			   set_Engine_State(ENGINE_REATTEMPT);
		   }
		 }

	   }

	   else if (mode == ENGINE_RUNNING)
	   {
			if (engine.mode != ENGINE_RUNNING)
		   	    engine.mode = ENGINE_RUNNING;
			else
			{
				engine.runTime++;					// increment seconds of running
				if (engine.runTime % 60)			// every 60 seconds, increment minutes
				{
					engine.engineMins++;
					if (engine.engineMins % 60)
						engine.engineHours++;
 				}

			}

			P2OUT |= ACCESSORY_PIN;
			P9OUT &= ~CRANK_PIN;
		    P9OUT &= ~GLOWPLUG_PIN;

	   }

	   else if (mode == ENGINE_STOPPING)
	   {

		   if (engine.mode != ENGINE_STOP && engine.mode != ENGINE_STOPPING)
		   {
			   if (mode == ENGINE_RUNNING)
				 engine.lastRun = now;

			   P2OUT &= ~ACCESSORY_PIN;
			   P9OUT &= ~CRANK_PIN;
			   P9OUT &= ~GLOWPLUG_PIN;
			   P8OUT &= ~ASSET_IGN_PIN;

		   	   engine.mode = ENGINE_STOP;
		   }
		   else
			   engine.mode = ENGINE_STOP;

	   }

}





