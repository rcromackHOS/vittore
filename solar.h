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

static double _sunset_d;
static double _sunrise_d;

int validCalc;

dateStruct lastCalculated;

//--------------------------------------------------------------------

void solar_setCoords(double lat, double lng);

timeStruct solar_getSunset(dateTimeStruct ds);
timeStruct solar_getSunrise(dateTimeStruct ds);

int solar_calculate(float y, float m, float d);
float solar_toJulian(float y, float m, float d);
timeStruct solar_timeFromDayDec(float dec);//, timeStruct dt);

double degrees(double r);
double radians(double d);

//--------------------------------------------------------------------

#endif /* SOLAR_H_ */
