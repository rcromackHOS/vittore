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
// state codes
#define ENGINE_STOP  	 0x0
#define ENGINE_PRE       0x1
#define ENGINE_CRANK     0x2
#define ENGINE_REATTEMPT 0x3
#define ENGINE_POST      0x4
#define ENGINE_RUNNING   0x5
#define ENGINE_STOPPING  0x6

// engine controller outputs
#define ACCESSORY_PIN    BIT0
#define CRANK_PIN        BIT6
#define GLOWPLUG_PIN     BIT5

// asset tracker outputs
#define ASSET_IGN_PIN    BIT5
#define OIL_PIN		     BIT7
#define ENGINE_TEMP_PIN  BIT6

//--------------------------------------------------------------------

int engineSetup(int engMins, int engHrs);

void engineStatus();
void set_Engine_State(int mode);

int checkOilPressure();
int checkEngineRPMs();
int checkEngineTemp();

int store_idleTime();

//--------------------------------------------------------------------

#endif /* ENGINECONTROLLER_H_ */
