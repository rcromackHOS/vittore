/*
 * engineController.c
 *
 *  Created on: May 18, 2016
 *      Author: Rhys
 */

#include "engineController.h"

//--------------------------------------------------------------------




//--------------------------------------------------------------------
// C imitation of a Constructor for the engine
engineStruct engineSetup(int engHrs)
{
	engineStruct ts = {0, 0, engHrs};

    return ts;
}

//--------------------------------------------------------------------
//
void incrementEngineState(engineStruct engine)
{
	set_Engine_State(engine, engine.mode);
}

//--------------------------------------------------------------------
//
void set_Engine_State(engineStruct engine, int mode)
{
   if (mode == ENGINE_STOP )
	 P8OUT ^= BIT1;
	/*
	   if (mode == ENGINE_STOP)
	   {
		   if (engine.mode == ENGINE_RUNNING)
		     set_Engine_State(engine, ENGINE_STOPPING);

	   }

	   else if (mode == ENGINE_PRE)
	   {
		 if (engine.mode != ENGINE_PRE)
		 {
		   P2OUT |= ACCESSORY_PIN;
		   P9OUT |= GLOWPLUG_PIN;

		   engine.mode = ENGINE_PRE;

		   PREHEAT_D = _PREHEAT_SP;
		   REATTEMPTS_D = _REATTEMPTS_SP;
		 }

		 if (PREHEAT_D > 0)
		 {
		   PREHEAT_D--;

		   if (PREHEAT_D == 0)
		     set_Engine_State(engine, ENGINE_CRANK);
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
		   {
			 P9OUT &= ~CRANK_PIN;
		     set_Engine_State(engine, ENGINE_REATTEMPT);
		   }
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

		   	if (ATTEMPT_D == 0)
			   set_Engine_State(engine, ENGINE_CRANK);
		 }

 	 	 if (REATTEMPTS_D == 0)
		 {
		    // fail mode

		    set_Engine_State(engine, ENGINE_STOPPING);
		 }

	   }

	   else if (mode == ENGINE_POST)
	   {
		 if (engine.mode != ENGINE_POST)
		 {
		   engine.mode = ENGINE_POST;

		   POST_D = _POST_SP;
		 }

		 if (POST_D > 0)
		 {
		   POST_D--;

		   if (POST_D == 0)
		   {
			 P9OUT &= ~GLOWPLUG_PIN;
		     set_Engine_State(engine, ENGINE_RUNNING);
		   }
		 }

	   }

	   else if (mode == ENGINE_RUNNING)
	   {
			if (engine.mode != ENGINE_RUNNING)
		   	   engine.mode = ENGINE_RUNNING;
	   }

	   else if (mode == ENGINE_STOPPING)
	   {

		   if (engine.mode != ENGINE_STOP && engine.mode != ENGINE_STOPPING)
		   {
			   P2OUT &= ~ACCESSORY_PIN;
			   P9OUT &= ~CRANK_PIN;
			   P9OUT &= ~GLOWPLUG_PIN;

		   	   engine.mode = ENGINE_STOP;
		   }
		   else
			   engine.mode = ENGINE_STOP;

	   }

	   */
}





