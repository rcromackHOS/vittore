/*
 * config.h
 *
 *  Created on: May 18, 2016
 *      Author: Rhys
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//--------------------------------------------------------------------

extern volatile int _HIGH_SP_24V;
extern volatile int _LOW_SP_24V;
extern volatile int VALUE_24V;

extern volatile int _HIGH_SP_BMS;
extern volatile int _LOW_SP_BMS;
extern volatile int _CELLMONITOR_SP;
extern volatile int _LOW_SP_12V;
extern volatile int VALUE_12V;

extern volatile int _HIGH_SP_TEMP;
extern volatile int _NOMINAL_SP_TEMP;
extern volatile int _LOW_SP_TEMP;
extern volatile int VALUE_BAT_TEMP;

extern volatile int _HIGH_SP_RPM;
extern volatile int _LOW_SP_RPM;
extern volatile int VALUE_RPM;

extern volatile int _UNIT_MODE;

extern volatile int _STATE_CODE;

extern volatile int _MAST_STATUS;

extern volatile int _RESETTING_;

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

//--------------------------------------------------------------------

extern int failure;

//--------------------------------------------------------------------

#endif /* CONFIG_H_ */
