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

extern int SOLAR_validCalc;

dateStruct lastCalculated;

//--------------------------------------------------------------------

extern void solar_setCoords(double lat, double lng);

extern timeStruct solar_getSunset(dateTimeStruct ds);
extern timeStruct solar_getSunrise(dateTimeStruct ds);

static int solar_calculate(float y, float m, float d);
static float solar_toJulian(float y, float m, float d);
static timeStruct solar_timeFromDayDec(float dec);//, timeStruct dt);

double degrees(double r);
double radians(double d);

//--------------------------------------------------------------------

#endif /* SOLAR_H_ */
