/*
 * config.h
 *
 *  Created on: May 18, 2016
 *      Author: Rhys
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "timeDate.h"

//--------------------------------------------------------------------

extern volatile int _HIGH_SP_24V;
extern volatile int _LOW_SP_24V;
extern volatile int VALUE_24V;

//--------------------------------------------------------------------

extern int BMS_EVENT;

extern int _HIGH_SP_BMS;
extern int _LOW_SP_BMS;
extern int _CELLMONITOR_TMR_SP;
extern int _BANK_BMS_TMR_SP;

extern int _CELLMONITOR_TMR_D;
extern int _BANK_BMS_TMR_D;

//--------------------------------------------------------------------
extern volatile int _LOW_SP_12V;
extern volatile int VALUE_12V;

extern volatile int _HIGH_SP_TEMP;
extern volatile int _NOMINAL_SP_TEMP;
extern volatile int _LOW_SP_TEMP;
extern volatile int VALUE_BAT_TEMP;

extern volatile int _HIGH_SP_RPM;
extern volatile int _LOW_SP_RPM;
extern volatile int VALUE_RPM;

extern int _UNIT_MODE;
extern int LIGHTS1HOUR_TMR;

extern volatile int _STATE_CODE;

extern volatile int _MAST_STATUS;

extern int _RESETTING_;
extern int RESET_TMR;
extern int OILCHANGE_PRESS_TMR;

extern int _FORCE_LIGHTS_ON;
extern int _FORCE_ENGINE_RUN;

extern int _DIAGNOSTIC_MODE;
extern int _DIAGNOSTIC_MODE_TMR;
extern int _DIAGNOSTIC_PAGE;
extern int _DIAGNOSTIC_PAGES;

extern int _OILCHANGE_DUE;

extern int _ADCs_UPDATED;

//--------------------------------------------------------------------

extern volatile int _PREHEAT_SP;
extern volatile int PREHEAT_D;

extern volatile int _CRANK_SP;
extern volatile int CRANK_D;

extern volatile int _POST_SP;
extern volatile int POST_D;

extern volatile int _ATTEMPT_SP;
extern volatile int ATTEMPT_D;

extern volatile int _REATTEMPTS_SP;
extern volatile int REATTEMPTS_D;

extern int count_run;
extern int count_RPM_fail;
extern int count_oil_fail;
extern int count_temp_fail;

//--------------------------------------------------------------------

extern int failure;

//--------------------------------------------------------------------

typedef struct
{
  int mode;

  int runTime;

  int engineMins;
  int engineHours;

  dateTimeStruct lastRunEnd;
  dateTimeStruct lastRun;

} engineStruct;

engineStruct engine;

//--------------------------------------------------------------------

typedef struct
{
  int hours;
  int minutes;

} idleTime;

idleTime idles[4];

extern int idleCount;


//--------------------------------------------------------------------

typedef struct
{
	int pin;
	int LEDpin;

	int mode;
	int state;
	int Oncounts;
	int Offcounts;

} button;

button buttonList[6];

//--------------------------------------------------------------------

extern double lat;
extern double lng;

dateTimeStruct now;

timeStruct sunSet;
timeStruct sunRise;

//--------------------------------------------------------------------

#endif /* CONFIG_H_ */
