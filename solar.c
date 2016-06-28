/*
 * solar.c
 *
 *  Created on: Jun 1, 2016
 *      Author: rcromack
 */

//--------------------------------------------------------------------

#include "solar.h"
#include "timeDate.h"
#include "math.h"

//--------------------------------------------------------------------

#define	M_PI		3.14159265358979323846	/* pi */
//#define	M_PI_2		1.57079632679489661923	/* pi/2 */

//--------------------------------------------------------------------

double radians(double d)
{
	return (double)d * (M_PI / 180);
}

double degrees(double r)
{
	return (double)r * (180 / M_PI);
}

//------------------------------------------------------------------
// initialize scope, Latitude and Longitude

void solar_setCoords(double lat, double lng)
{
   _lat = lat;
   _lng = lng;

   lastCalculated = date(0, 1, 2015);
   validCalc = 0;
}

//------------------------------------------------------------------
// Convert date to a Julian ordinal integer
//
float solar_toJulian(float y, float m, float d)
{
  if (m < 3)
  {
	  m = m + 12;
	  y = y - 1;
  }

  return d + (153 * m - 457) / 5 + 365 * y + (y / 4) - (y / 100) + (y / 400) + 1721119;
}

//------------------------------------------------------------------
// Returns Sunrise in a dateTimeStruct class
timeStruct solar_getSunrise(dateTimeStruct ds)
{
	  timeStruct ts;

	  if (lastCalculated.day == ds.day && lastCalculated.month == ds.month)
	  {
		  ts = solar_timeFromDayDec((float)_sunrise_d);//, ts);
		  return ts;
	  }

	  validCalc = 0;

	  if (solar_calculate(ds.year, ds.month, ds.day) == 1)
	  {
		  lastCalculated = date(ds.day, ds.month, ds.year);
		  validCalc = 1;

		  ts = solar_timeFromDayDec((float)_sunrise_d);//, ts);
		  return ts;
	  }

	  return time(0, 0, 0);
}


//------------------------------------------------------------------
// Returns Sunset in a timeStruct
timeStruct solar_getSunset(dateTimeStruct ds)
{
	  timeStruct ts;

	  if (lastCalculated.day == ds.day && lastCalculated.month == ds.month)
	  {
		  ts = solar_timeFromDayDec((float)_sunset_d);//, ts);
		  return ts;
	  }

	  validCalc = 0;

	  if (solar_calculate(ds.year, ds.month, ds.day) == 1)
	  {
		  lastCalculated = date(ds.day, ds.month, ds.year);
		  validCalc = 1;

		  ts = solar_timeFromDayDec((float)_sunset_d);//, ts);
		  return ts;
	  }

	  return time(0, 0, 0);
}

//------------------------------------------------------------------
// Calculates the Sunrise/Sunset by the Date, Latitude and Longitude - http://www.esrl.noaa.gov/gmd/grad/solcalc/
//	populates _sunset_d & _sunrise_d doubles
//	added a 15m offset to these day decimals, for the Light Tower

int solar_calculate(float y, float m, float d)
{
	  double dwdw = degrees(0.5);

	  //double offset = ceil(_lng * 24 / 360);
	  //TZ_offset = (int)offset;
	  double offset = 0;
	  double Jday = solar_toJulian(y, m, d);
	  //Serial.print("Julian Day ");Serial.println(Jday, 2);

	  double Jcent = (Jday - 2451545) / 36525;

	  // Geom Mean Anom Sun (degrees)
	  double Manom = 357.52911+Jcent*(35999.05029-0.0001537*Jcent);

	  // Geom Mean Long Sun (degrees)
	  double Mlong = fmod((280.46646+Jcent*(36000.76983+Jcent*0.0003032)), 360);

	  // Eccent Earth Orbit
	  double Eccent = 0.016708634-Jcent*(0.000042037+0.0001537*Jcent);

	   // Sun Eq of Ctr
	  double Seqcent  = sin(radians(Manom))*(1.914602-Jcent*(0.004817+0.000014*Jcent))+sin(radians(2*Manom))*(0.019993-0.000101*Jcent)+sin(radians(3*Manom))*0.000289;

	  // Sun True Anom (degrees)
	  double Struelong = Mlong + Seqcent;

	  // Sun App Long (degrees)
	  double Sapplong = Struelong-0.00569-0.00478*sin(radians(125.04-1934.136*Jcent));

	  // Mean Obliq Ecliptic (degrees)
	  double Mobliq   = 23+(26+((21.448-Jcent*(46.815+Jcent*(0.00059-Jcent*0.001813))))/60)/60;

	  // Obliq Correction (degrees)
	  double obliq    = Mobliq+0.00256*cos(radians(125.04-1934.136*Jcent));

	  // vary
	  double vary = tan(radians(obliq/2))*tan(radians(obliq/2));

	  double declination = degrees(asin(sin(radians(obliq))*sin(radians(Sapplong))));

	  //-----------------------------------

	  //float sunrtasc = degrees()

	  //float declination = -20.42; // Novemeber
	  //float declination = 19.98; // July

	  // Eq of time
	  double eqtime = 4*degrees(vary*sin(2*radians(Mlong))-2*Eccent*sin(radians(Manom))+4*Eccent*vary*sin(radians(Manom))*cos(2*radians(Mlong))-0.5*vary*vary*sin(4*radians(Mlong))-1.25*Eccent*Eccent*sin(2*radians(Manom)));

	  double hourangle = degrees(acos(cos(radians(90.833))/(cos(radians(_lat))*cos(radians(declination)))-tan(radians(_lat))*tan(radians(declination))));

	  double solarnoon_t = (720-4*_lng-eqtime+offset*60)/1440;

	 // _sunrise_d = (solarnoon_t-hourangle*4/1440)+0.011;  // added 0.011 for the 15m offset.
	  //_sunset_d = (solarnoon_t+hourangle*4/1440)-0.011;  // subtracted 0.011 for the 15m offset.
	  double l0 = offset * 60;
	  double l1 = 4 * _lng;
	  double l2 = eqtime + l0;
	  double l3 = 720 - l1;
	  double l4 = l3 - l2;
	  double l5 = l4/1400;

	  _sunrise_d = (solarnoon_t-hourangle*4/1440);
	  _sunset_d = (solarnoon_t+hourangle*4/1440);

	  //Serial.print("_sunrise_d ");Serial.println(_sunrise_d);
	  //Serial.print("_sunset_d ");Serial.println(_sunset_d);

	  return 1;
}

//------------------------------------------------------------------
// Calculate HH/MM by daytime decimal, returns dateTimeStruct class with hour and minute loaded
//
timeStruct solar_timeFromDayDec(float dec)//, timeStruct dt)
{
	float hours = 24.0*dec;
    //Serial.println(hours);
	float minutes = (hours - (int)hours)*60;

	//Serial.println(minutes); //float seconds= (minutes-int(minutes))*60;
	if (hours > 24)
		hours = hours - 24;

	//dt.hour = (int)hours;
	//dt.minute = (int)minutes;
	//dt.second = 0;
	return time(0, (int)minutes, (int)hours);
}



