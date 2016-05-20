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
void set_Engine_State(engineStruct engine, int mode)
{
       if (mode == ENGINE_STOP)
	   {
		   if (engine.mode == ENGINE_RUNNING)
		     set_Engine_State(engine, ENGINE_STOPPING);

		   P8OUT ^= BIT1;

	   }
	   if (mode == ENGINE_PRE)
	   {
		   P8OUT ^= BIT1;


	   }
	   if (mode == ENGINE_CRANK)
	   {
		   P8OUT ^= BIT1;

	   }
	   if (mode == ENGINE_POST)
	   {
		   P8OUT ^= BIT1;


	   }
	   if (mode == ENGINE_RUNNING)
	   {

		   P8OUT ^= BIT1;
	   }
	   if (mode == ENGINE_STOPPING)
	   {

		   if (engine.mode != ENGINE_STOP && engine.mode != ENGINE_STOPPING)
		   {
			   P2OUT &= ~ACCESSORY_PIN;
			   P9OUT &= ~CRANK_PIN;
			   P9OUT &= ~GLOWPLUG_PIN;
		   }
		   else
			   engine.mode = ENGINE_STOP;

	   }

}






