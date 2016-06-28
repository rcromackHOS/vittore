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
// states
/*
typedef enum {
	ENGINE_STOP = 0,
	ENGINE_PRE,
	ENGINE_CRANK,
	ENGINE_REATTEMPT,
	ENGINE_POST,
	ENGINE_RUNNING,
	ENGINE_STOPPING
} engine_statuses_t;
extern engine_statuses_t engine_statuses;
*/

#define ENGINE_STOP  	 0x0
#define ENGINE_PRE       0x1
#define ENGINE_CRANK     0x2
#define ENGINE_REATTEMPT 0x3
#define ENGINE_POST      0x4
#define ENGINE_RUNNING   0x5
#define ENGINE_STOPPING  0x6

//--------------------------------------------------------------------

void InitializeEngine();
void buildIdleArray();

int check_Engine_Status();
void set_Engine_State(int mode);

void handle_oilchangeClear();
int checkOilPressure();
int checkEngineRPMs();
int checkEngineTemp();
void runTime();
int store_idleTime();

//--------------------------------------------------------------------

#endif /* ENGINECONTROLLER_H_ */
