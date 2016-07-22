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

float _HIGH_SP_24V = 			28.1;//27.5 for 15 min
float _LOW_SP_24V = 	    	25.50;//26.5 for 15 min
float VALUE_24V = 	    		0.0;

float VALUE_PCB_24V = 	    	0.0;

float _LOW_SP_12V = 	    	11.00;
float VALUE_12V = 	    		0.0;

float _HIGH_SP_TEMP = 			40.00;
float _NOMINAL_SP_TEMP = 		20.00;
float _LOW_SP_TEMP =	   	  	5.00;
float VALUE_BAT_TEMP = 			6.00;

float _HIGH_SP_INTERNAL_TEMP = 	10.00;
float _LOW_SP_INTERNAL_TEMP = 	1.00;
float VALUE_INTERNAL_TEMP = 	6.00;
float VALUE_INTERNAL2_TEMP =	6.00;

//--------------------------------------------------------------------

int _HIGH_SP_RPM = 				3900;
int _LOW_SP_RPM = 	 			1000;
int VALUE_RPM = 	   		    0;

int _UNIT_MODE =				0;
int LIGHTS1HOUR_TMR =			0;

int _STATE_CODE =				0;

//--------------------------------------------------------------------

int BMS_EVENT =					0;

float _HIGH_SP_BMS = 			28.60;
float _LOW_SP_BMS = 			22.50;

int _BANK_BMS_TMR_SP = 			5;
int _BANK_BMS_TMR_D = 			0;

int _CELLMONITOR_TMR_SP = 		30;
int _CELLMONITOR_TMR_D = 		0;

//--------------------------------------------------------------------

mast_States_t _MAST_STATUS =    MAST_NOMINAL;

int _RESETTING_ =      			0;
int RESET_TMR =      			0;
int OILCHANGE_PRESS_TMR =      	0;

int _FORCE_LIGHTS_ON =   		0;
int _FORCE_ENGINE_RUN =   		0;

int _ADCs_UPDATED =				0;

int _DIAGNOSTIC_MODE =   		0;
int _DIAGNOSTIC_MODE_TMR =   	0;
int _DIAGNOSTIC_PAGE = 			0;
int _DIAGNOSTIC_PAGES = 		2;

int _OILCHANGE_DUE =     		0;

int _SYS_FAILURE_ = 			0;

int _POINTER_TO_ENGINE_MINS = 	0;

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

 float VALUE_CURRENT_OUT =		0.0;

 //--------------------------------------------------------------------

int _SCREEN_UPDATE_D =  		0;
int _UPDATE_SCREEN_ =  			1;

 //--------------------------------------------------------------------

int idleCount = 				0;
int diagBackButton =			0;

//--------------------------------------------------------------------

double lat =					0.0;
double lng = 					0.0;

dateTimeStruct now;
timeStruct sunSet;
timeStruct sunRise;

