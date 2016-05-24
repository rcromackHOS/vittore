/*
 * engineController.c
 *
 *  Created on: May 18, 2016
 *      Author: Rhys
 */

#include "engineController.h"
#include "config.h"

//--------------------------------------------------------------------

engineStruct engine = {0, 0, 0};

//--------------------------------------------------------------------
// C imitation of a Constructor for the engine
int engineSetup(int engHrs)
{
	engine.engineHours = engHrs;
	return 1;
}

//--------------------------------------------------------------------
//
void engineStatus()
{
	static int count_run;
	static int count_RPM_fail;

	if (engine.mode != ENGINE_RUNNING &&
		engine.mode != ENGINE_POST &&
		engine.mode != ENGINE_STOP &&
		checkOilPressure())
	{
		count_run++;
		if (count_run > 2)
		{
			set_Engine_State(ENGINE_POST);
			count_run = 0;
		}
	}

	else if (engine.mode == ENGINE_RUNNING)
	{
		if (checkEngineRPMs() == 1)
		{
			if (count_RPM_fail > 0)
				count_RPM_fail--;
		}
		else
		{
			count_RPM_fail++;
			if (count_RPM_fail > 3)
			{
				if (checkEngineRPMs() == 0)
				{
					set_Engine_State(ENGINE_STOPPING);
				//  fail slow condition
				}

				if (checkEngineRPMs() == 2)
				{
					set_Engine_State(ENGINE_STOPPING);
				//  fail slow condition
				}
			}
		}
	}

	else
		set_Engine_State(engine.mode);
}

//--------------------------------------------------------------------
//
void set_Engine_State(int mode)
{
	   if (mode == ENGINE_STOP)
	   {
		   if (engine.mode == ENGINE_RUNNING)
		     set_Engine_State(ENGINE_STOPPING);

	   }

	   else if (mode == ENGINE_PRE)
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
		    // fail mode

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

			P2OUT |= ACCESSORY_PIN;
			P9OUT &= ~CRANK_PIN;
		    P9OUT &= ~GLOWPLUG_PIN;

	   }

	   else if (mode == ENGINE_STOPPING)
	   {

		   if (engine.mode != ENGINE_STOP && engine.mode != ENGINE_STOPPING)
		   {
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





