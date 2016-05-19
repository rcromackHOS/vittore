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
/*
void set_Engine_State(engineStruct Engine, int mode)
{
	switch (mode)
	{
	   case 0:
	   {
		   if (Engine.mode == ENGINE_RUNNING)
		   {
			  set_Engine_State(Engine, ENGINE_STOPPING);
		   }
		   P8OUT ^= BIT1;
	       break;
	   }

	   case 1:
       {

		   break;
	   }
	   case 2:
	   {

		   break;
	   }
	   case 3:
	   {

		   break;
	   }
	   case 4:
	   {

		   break;
       }
	   case 5:
	   {
		   if (Engine.mode != ENGINE_STOP && Engine.mode != ENGINE_STOPPING)
		   {
			   P2OUT &= ~ACCESSORY_PIN;
			   P9OUT &= ~CRANK_PIN;
			   P9OUT &= ~GLOWPLUG_PIN;
		   }
		   else
			   Engine.mode = ENGINE_STOP;

		   break;
	   }

	}
}
*/





