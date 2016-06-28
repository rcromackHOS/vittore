/*
 * bms.h
 *
 *  Created on: May 24, 2016
 *      Author: rcromack
 */

#ifndef BMS_H_
#define BMS_H_

#include <msp430.h>
#include "config.h"

//--------------------------------------------------------------------

extern void check_BatteryBox_Status(void);

int setContactor(int s);
int setBatteryHeater(int s);
int setEngineRun(int s);

//--------------------------------------------------------------------

#endif /* BMS_H_ */
