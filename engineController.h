/*
 * engineController.h
 *
 *  Created on: May 18, 2016
 *      Author: Rhys
 */

#ifndef ENGINECONTROLLER_H_
#define ENGINECONTROLLER_H_

#include <msp430.h>

//--------------------------------------------------------------------

typedef struct
{
  int mode;

  int runTime;

  int engineHours;

} engineStruct;

// state codes
#define ENGINE_STOP  	 0x0;
#define ENGINE_PRE       0x1;
#define ENGINE_CRANK     0x2;
#define ENGINE_REATTEMPT 0x3;
#define ENGINE_POST      0x4;
#define ENGINE_RUNNING   0x5;
#define ENGINE_STOPPING  0x6;

// engine controller outputs
#define ACCESSORY_PIN    BIT0;
#define CRANK_PIN        BIT6;
#define GLOWPLUG_PIN     BIT5;

//--------------------------------------------------------------------

engineStruct engineSetup(int engHrs);
void incrementEngineState(engineStruct engine);
void set_Engine_State(engineStruct engine, int mode);

//--------------------------------------------------------------------

#endif /* ENGINECONTROLLER_H_ */
