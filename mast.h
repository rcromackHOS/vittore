/*
 * mast.h
 *
 *  Created on: Jun 24, 2016
 *      Author: rcromack
 */

#ifndef MAST_H_
#define MAST_H_

//--------------------------------------------------------------------

#include "Hardware.h"
#include "config.h"

//--------------------------------------------------------------------

extern void mastUpDown();
extern void mast_stateMachine(mast_States_t deltastate);
extern void enterLowPowerMode();
extern void sleepModeProcess();

//--------------------------------------------------------------------

#endif /* MAST_H_ */
