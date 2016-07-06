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


#define MODE_AUTO  	  	  0x0
#define MODE_STANDBY  	  0x1
#define MODE_LIGHT1H  	  0x2
#define MODE_RESET	  	  0x3
#define MODE_UP       	  0x4
#define MODE_DOWN     	  0x5


//--------------------------------------------------------------------

void buildButtonStateMachine();
void button_stateMachine();

void mastUpDown();
void mast_stateMachine(mast_States_t deltastate);

void handle_reset();
void handle_pressDiagButton();

//--------------------------------------------------------------------

#endif /* BUTTON_H_ */




