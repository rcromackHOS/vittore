/*
 * config.c
 *
 *  Created on: May 24, 2016
 *      Author: rcromack
 */

#include "config.h"

//--------------------------------------------------------------------

volatile int _HIGH_SP_24V = 	2810;
volatile int _LOW_SP_24V = 	    2560;
volatile int VALUE_24V = 	    2600;		//

volatile int _LOW_SP_12V = 	    1100;
volatile int VALUE_12V = 	    1250;		//

volatile int _HIGH_SP_TEMP = 	4000;
volatile int _NOMINAL_SP_TEMP = 2000;
volatile int _LOW_SP_TEMP =	    500;
volatile int VALUE_BAT_TEMP = 	600;		//

volatile int _HIGH_SP_RPM = 	3900;
volatile int _LOW_SP_RPM = 	 	1000;
volatile int VALUE_RPM = 	    0;

int _UNIT_MODE =				0;
int LIGHTS1HOUR_TMR =			0;

volatile int _STATE_CODE =		0;

//--------------------------------------------------------------------

int BMS_EVENT =					0;

int _HIGH_SP_BMS = 				2850;
int _LOW_SP_BMS = 				2300;
int _CELLMONITOR_TMR_SP = 		30;
int _BANK_BMS_TMR_SP = 			5;

int _CELLMONITOR_TMR_D = 		0;
int _BANK_BMS_TMR_D = 			0;

//--------------------------------------------------------------------

volatile int _MAST_STATUS =     0;

int _RESETTING_ =      			0;
int RESET_TMR =      			0;
int OILCHANGE_PRESS_TMR =      	0;

int _FORCE_LIGHTS_ON =   		0;
int _FORCE_ENGINE_RUN =   		0;

int _ADCs_UPDATED =				0;

int _DIAGNOSTIC_MODE =   		0;
int _DIAGNOSTIC_MODE_TMR =   	0;
int _DIAGNOSTIC_PAGE = 			0;
int _DIAGNOSTIC_PAGES = 		1;

int _OILCHANGE_DUE =     		0;

int failure = 					0;

//--------------------------------------------------------------------

int count_run =					0;
int count_RPM_fail =			0;
int count_oil_fail =			0;
int count_temp_fail =			0;

volatile int _PREHEAT_SP =		15;
volatile int PREHEAT_D =		0;

volatile int _CRANK_SP =		10;
volatile int CRANK_D =			0;

volatile int _POST_SP =			10;
volatile int POST_D =			0;

volatile int _ATTEMPT_SP =		10;
volatile int ATTEMPT_D =		0;

volatile int _REATTEMPTS_SP =   3;
volatile int REATTEMPTS_D =		0;

//--------------------------------------------------------------------

int idleCount = 				0;

//--------------------------------------------------------------------

double lat =					0;
double lng = 					0;



