/*
 * solar.h
 *
 *  Created on: Jun 1, 2016
 *      Author: rcromack
 */

#ifndef SOLAR_H_
#define SOLAR_H_

//--------------------------------------------------------------------

#include "timeDate.h"
#include "math.h"

//--------------------------------------------------------------------

static double _lng;
static double _lat;

static float _sunset_d;
static float _sunrise_d;

extern int validCalc;

dateStruct lastCalculated;

//--------------------------------------------------------------------

void solar_setCoords(double lat, double lng);

timeStruct solar_getSunset(dateTimeStruct ds);
timeStruct solar_getSunrise(dateTimeStruct ds);

int solar_calculate(float y, float m, float d);
float solar_toJulian(float y, float m, float d);
void solar_timeFromDayDec(float dec, timeStruct dt);

//--------------------------------------------------------------------

#endif /* SOLAR_H_ */
