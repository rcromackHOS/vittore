/*
 * config.c
 *
 *  Created on: May 24, 2016
 *      Author: rcromack
 */

#include "config.h"

//--------------------------------------------------------------------

int _ADCs_UPDATED_	=			0;

//--------------------------------------------------------------------

 int _HIGH_SP_24V = 			2810;
 int _LOW_SP_24V = 	    		2560;
 int VALUE_24V = 	    		2600;

 int VALUE_PCB_24V = 	    	2600;

 int _LOW_SP_12V = 	    		1100;
 unsigned int VALUE_12V = 	    1250;

 int _HIGH_SP_TEMP = 			4000;
 int _NOMINAL_SP_TEMP = 		2000;
 int _LOW_SP_TEMP =	   	  		500;
 int VALUE_BAT_TEMP = 			600;

int _HIGH_SP_INTERNAL_TEMP = 	1000;
int _LOW_SP_INTERNAL_TEMP = 	100;
int VALUE_INTERNAL_TEMP = 		0;

 int _HIGH_SP_RPM = 			3900;
 int _LOW_SP_RPM = 	 			1000;
 int VALUE_RPM = 	   		    0;

int _UNIT_MODE =				0;
int LIGHTS1HOUR_TMR =			0;

 int _STATE_CODE =				0;

//--------------------------------------------------------------------

int BMS_EVENT =					0;

int _HIGH_SP_BMS = 				2850;
int _LOW_SP_BMS = 				2300;

int _BANK_BMS_TMR_SP = 			5;
int _BANK_BMS_TMR_D = 			0;

int _CELLMONITOR_TMR_SP = 		30;
int _CELLMONITOR_TMR_D = 		0;

//--------------------------------------------------------------------

 int _MAST_STATUS =     		0;

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

int _SYS_FAILURE_ = 			0;

//--------------------------------------------------------------------

int count_run =					0;
int count_RPM_fail =			0;
int count_oil_fail =			0;
int count_temp_fail =			0;

 int _PREHEAT_SP =				15;
 int PREHEAT_D =				0;

 int _CRANK_SP =				10;
 int CRANK_D =					0;

 int _POST_SP =					10;
 int POST_D =					0;

 int _ATTEMPT_SP =				10;
 int ATTEMPT_D =				0;

 int _REATTEMPTS_SP =  		 	3;
 int REATTEMPTS_D =				0;

 //--------------------------------------------------------------------

int _SCREEN_UPDATE_D =  		0;

 //--------------------------------------------------------------------

int idleCount = 				0;

//--------------------------------------------------------------------

double lat =					0.0;
double lng = 					0.0;



