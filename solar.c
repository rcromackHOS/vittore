/*
 * solar.c
 *
 *  Created on: Jun 1, 2016
 *      Author: rcromack
 */

//--------------------------------------------------------------------

#include "timeDate.h"
#include "solar.h"
#include "math.h"

//------------------------------------------------------------------
// initialize scope, Latitude and Longitude

void solar_setCoords(double lat, double lng)
{
   _lat = lat;
   _lng = lng;
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
	  timeStruct ts = time(0, 0, 0);

	  if (lastCalculated.day == ds.day && lastCalculated.month == ds.month)
	  {
		  solar_timeFromDayDec(_sunrise_d, ts);
		  return ts;
	  }

	  validCalc = 0;

	  if (solar_calculate(ds.year, ds.month, ds.day) == 1)
	  {
		  lastCalculated = date(ds.day, ds.month, ds.year);
		  validCalc = 1;

		  solar_timeFromDayDec(_sunrise_d, ts);
		  return ts;
	  }

	  return ts;
}

//------------------------------------------------------------------
// Returns Sunset in a timeStruct
timeStruct solar_getSunset(dateTimeStruct ds)
{
	  timeStruct ts = time(0, 0, 0);

	  if (lastCalculated.day == ds.day && lastCalculated.month == ds.month)
	  {
		  solar_timeFromDayDec(_sunset_d, ts);
		  return ts;
	  }

	  validCalc = 0;

	  if (solar_calculate(ds.year, ds.month, ds.day) == 1)
	  {
		  lastCalculated = date(ds.day, ds.month, ds.year);
		  validCalc = 1;

		  solar_timeFromDayDec(_sunset_d, ts);
		  return ts;
	  }

	  return ts;
}

//------------------------------------------------------------------
// Calculates the Sunrise/Sunset by the Date, Latitude and Longitude - http://www.esrl.noaa.gov/gmd/grad/solcalc/
//	populates _sunset_d & _sunrise_d doubles
//	added a 15m offset to these day decimals, for the Light Tower

int solar_calculate(float y, float m, float d)
{
	  //double offset = ceil(_lng * 24 / 360);
	  //TZ_offset = (int)offset;
	  double offset = 0;
	  double Jday = solar_toJulian(y, m, d);
	  //Serial.print("Julian Day ");Serial.println(Jday, 2);

	  double Jcent = (Jday - 2451545) / 36525;
	  //Serial.print("Julian Century ");Serial.println(Jcent, 4);
	  // Geom Mean Anom Sun (deg)
	  float Manom = 357.52911+Jcent*(35999.05029-0.0001537*Jcent);
	  //Serial.print("Geom Mean Anom Sun ");Serial.println(Manom, 4);
	  // Geom Mean Long Sun (deg)
	  float Mlong = fmod((280.46646+Jcent*(36000.76983+Jcent*0.0003032)), 360);
	  //Serial.print("Geom Mean Long Sun ");Serial.println(Mlong, 4);
	  // Eccent Earth Orbit
	  float Eccent = 0.016708634-Jcent*(0.000042037+0.0001537*Jcent);
	  //Serial.print("Eccent Earth Orbit ");Serial.println(Eccent, 4);
	   // Sun Eq of Ctr
	  float Seqcent  = sin(radians(Manom))*(1.914602-Jcent*(0.004817+0.000014*Jcent))+sin(radians(2*Manom))*(0.019993-0.000101*Jcent)+sin(radians(3*Manom))*0.000289;
	  //Serial.print("Sun Eq of Ctr ");Serial.println(Seqcent, 4);
	  // Sun True Anom (deg)
	  float Struelong = Mlong + Seqcent;
	  //Serial.print("Sun True Anom ");Serial.println(Struelong, 4);
	  // Sun App Long (deg)
	  float Sapplong = Struelong-0.00569-0.00478*sin(radians(125.04-1934.136*Jcent));
	  //Serial.print("Sun App Long ");Serial.println(Sapplong, 4);

	  // Mean Obliq Ecliptic (deg)
	  float Mobliq   = 23+(26+((21.448-Jcent*(46.815+Jcent*(0.00059-Jcent*0.001813))))/60)/60;
	  //Serial.print("Mean Obliq Ecliptic ");Serial.println(Mobliq, 4);
	  // Obliq Correction (deg)
	  float obliq    = Mobliq+0.00256*cos(radians(125.04-1934.136*Jcent));
	  //Serial.print("Obliq Correction ");Serial.println(obliq, 4);
	  // vary
	  float vary = tan(radians(obliq/2))*tan(radians(obliq/2));
	  //Serial.print("vary ");Serial.println(vary, 4);

	  float declination = degrees(asin(sin(radians(obliq))*sin(radians(Sapplong))));
	  //Serial.print("Declination ");Serial.println(declination, 4);
	  //float declination = -20.42; // Novemeber
	  //float declination = 19.98; // July

	  float eqtime = 4*degrees(vary*sin(2*radians(Mlong))-2*Eccent*sin(radians(Manom))+4*Eccent*vary*sin(radians(Manom))*cos(2*radians(Mlong))-0.5*vary*vary*sin(4*radians(Mlong))-1.25*Eccent*Eccent*sin(2*radians(Manom)));

	  float hourangle = degrees(acos(cos(radians(90.833))/(cos(radians(_lat))*cos(radians(declination)))-tan(radians(_lat))*tan(radians(declination))));
	  //Serial.print("hourangle ");Serial.println(hourangle, 4);

	  float solarnoon_t = (720-4*_lng-eqtime+offset*60)/1440;
	  //Serial.println(solarnoon_t);

	 // _sunrise_d = (solarnoon_t-hourangle*4/1440)+0.011;  // added 0.011 for the 15m offset.
	  //_sunset_d = (solarnoon_t+hourangle*4/1440)-0.011;  // subtracted 0.011 for the 15m offset.

	  _sunrise_d = (solarnoon_t-hourangle*4/1440);
	  _sunset_d = (solarnoon_t+hourangle*4/1440);

	  //Serial.print("_sunrise_d ");Serial.println(_sunrise_d);
	  //Serial.print("_sunset_d ");Serial.println(_sunset_d);

	  return 1;
}

//------------------------------------------------------------------
// Calculate HH/MM by daytime decimal, returns dateTimeStruct class with hour and minute loaded
//
void solar_timeFromDayDec(float dec, timeStruct dt)
{
	  float hours = 24.0*dec;
	  //Serial.println(hours);
	  float minutes = ((int)hours-(int)hours)*60;
	  //Serial.println(minutes); //float seconds= (minutes-int(minutes))*60;
	  if (hours > 24)
		hours = hours - 24;

	  dt.hour = (int)hours;
	  dt.minute = (int)minutes;
	  dt.second = 0;

}



