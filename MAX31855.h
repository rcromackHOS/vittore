/*
 * MAX31855.h
 *
 *  Created on: Jun 27, 2016
 *      Author: LightTower
 */

#ifndef MAX31855_H_
#define MAX31855_H_

//--------------------------------------------------------------------

void getThermocoupleData();
static void _delay_ms(int t);
void pollMAX31855();

//--------------------------------------------------------------------

static unsigned long TCRXVALUE = 0;
static float centigrade = 0.0;
static float internal = 0.0;

//--------------------------------------------------------------------

#endif /* MAX31855_H_ */
