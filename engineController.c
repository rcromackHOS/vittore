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
void set_Engine_State(engineStruct Engine, int mode)
{
	switch(mode)
	{
	   case ENGINE_STOP:
		   if (Engine.mode == ENGINE_RUNNING)
			   set_Engine_State(Engine, ENGINE_STOPPING);


	       break;

	   case ENGINE_PRE:


		   break;

	   case ENGINE_CRANK:


		   break;

	   case ENGINE_POST:


		   break;

	   case ENGINE_RUNNING:


		   break;

	   case ENGINE_STOPPING:


		   break;
	}
}






