/*
 * config.h
 *
 *  Created on: May 18, 2016
 *      Author: Rhys
 */

#ifndef CONFIG_H_
#define CONFIG_H_
//--------------------------------------------------------------------

int _HIGH_SP_24V = 	 	2810;
int _LOW_SP_24V = 	 	2560;

int _HIGH_SP_BMS = 	 	2850;
int _LOW_SP_BMS = 	 	2300;

int _LOW_SP_12V = 	 	1100;

int _HIGH_SP_TEMP = 	4000;
int _NOMINAL_SP_TEMP = 	2000;
int _LOW_SP_TEMP =	 	500;

int _UNIT_MODE =		0;

int _STATE_CODE =		0;

//--------------------------------------------------------------------

int _PREHEAT_SP =		15;
int PREHEAT_D =			0;

int _CRANK_SP =			10;
int CRANK_D =			0;

int _POST_SP =			10;
int POST_D =			0;

int _ATTEMPT_SP =		10;
int ATTEMPT_D =			0;

//--------------------------------------------------------------------

#endif /* CONFIG_H_ */
