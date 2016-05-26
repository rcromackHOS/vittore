/*
 * bms.h
 *
 *  Created on: May 24, 2016
 *      Author: rcromack
 */

#ifndef BMS_H_
#define BMS_H_

#include <msp430.h>

//--------------------------------------------------------------------
// battery controller outputs
#define BMS_PIN      	  BIT3
#define BATTERYHTR_PIN    BIT4
#define CONTACTOR_PIN     BIT5
#define ASSET_IN2_PIN     BIT1

//--------------------------------------------------------------------

void batteryStatus();
int setContactor(int s);
int setBatteryHeater(int s);
int setEngineRun(int s);

//--------------------------------------------------------------------

#endif /* BMS_H_ */
