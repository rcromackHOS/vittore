/*
 * button.h
 *
 *  Created on: May 25, 2016
 *      Author: rcromack
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <msp430.h>
#include "config.h"

//--------------------------------------------------------------------
//_UNIT_MODE
//--------------------------------------------------------------------
// state codes
#define STATE_NOMINAL     0x0
#define STATE_PRESSING    0x1
#define STATE_PRESSED     0x2
#define STATE_RELEASED	  0x3

#define MODE_AUTO  	  	  0x0
#define MODE_STANDBY  	  0x1
#define MODE_LIGHT1H  	  0x2
#define MODE_RESET	  	  0x3
#define MODE_UP       	  0x4
#define MODE_DOWN     	  0x5

#define MAST_MAXDOWN  	  0x0
#define MAST_RISING  	  0x1
#define MAST_NOMINAL  	  0x2
#define MAST_LOWERING  	  0x3
#define MAST_MAXUP  	  0x4

//--------------------------------------------------------------------

void buildButtonStateMachine();
void button_stateMachine();

void mastUpDown();
void mast_stateMachine(int deltastate);

//--------------------------------------------------------------------

#endif /* BUTTON_H_ */




