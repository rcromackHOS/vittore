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

extern int _ADCs_UPDATED_;

//--------------------------------------------------------------------

extern float _HIGH_SP_24V;
extern float _LOW_SP_24V;
extern float VALUE_24V;

extern float VALUE_PCB_24V;

//--------------------------------------------------------------------

extern int BMS_EVENT;

extern float _HIGH_SP_BMS;
extern float _LOW_SP_BMS;

extern int _CELLMONITOR_TMR_SP;
extern int _BANK_BMS_TMR_SP;

extern int _CELLMONITOR_TMR_D;
extern int _BANK_BMS_TMR_D;

//--------------------------------------------------------------------

extern float _LOW_SP_12V;
extern float VALUE_12V;

//--------------------------------------------------------------------

extern float _HIGH_SP_INTERNAL_TEMP;
extern float _LOW_SP_INTERNAL_TEMP;
extern float VALUE_INTERNAL_TEMP;
extern float VALUE_INTERNAL2_TEMP;

//--------------------------------------------------------------------

extern float _HIGH_SP_TEMP;
extern float _NOMINAL_SP_TEMP;
extern float _LOW_SP_TEMP;
extern float VALUE_BAT_TEMP;

extern int _HIGH_SP_RPM;
extern int _LOW_SP_RPM;
extern int VALUE_RPM;

extern int _UNIT_MODE;
extern int LIGHTS1HOUR_TMR;

extern int _STATE_CODE;

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

extern  int _PREHEAT_SP;
extern  int PREHEAT_D;

extern  int _CRANK_SP;
extern  int CRANK_D;

extern  int _POST_SP;
extern  int POST_D;

extern  int _ATTEMPT_SP;
extern  int ATTEMPT_D;

extern  int _REATTEMPTS_SP;
extern  int REATTEMPTS_D;

extern int count_run;
extern int count_RPM_fail;
extern int count_oil_fail;
extern int count_temp_fail;

//--------------------------------------------------------------------

extern int _SCREEN_UPDATE_D;
extern int _UPDATE_SCREEN_;

//--------------------------------------------------------------------

extern int _SYS_FAILURE_;

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

typedef enum
{
	STATE_NOMINAL = 0,
	STATE_PRESSING,
	STATE_PRESSED,
	STATE_RELEASED

} button_States_t;

//--------------------------------------------------------------------

typedef struct
{
	int pin;
	int LEDpin;

	int mode;
	button_States_t state;
	int Oncounts;
	int Offcounts;

} button;

button buttonList[6];

//--------------------------------------------------------------------

typedef enum
{
	MAST_MAXDOWN = 0,
	MAST_LOWERING,
	MAST_NOMINAL,
	MAST_RISING,
	MAST_MAXUP

} mast_States_t;

extern mast_States_t _MAST_STATUS;

//--------------------------------------------------------------------

extern int diagBackButton;

extern double lat;
extern double lng;

extern dateTimeStruct now;

extern timeStruct sunSet;
extern timeStruct sunRise;

//--------------------------------------------------------------------

#endif /* CONFIG_H_ */
