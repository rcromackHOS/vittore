/*
 * timeDate.c
 *
 *  Created on: May 10, 2016
 *      Author: RCromack
 */

//--------------------------------------------------------------------

#include "timeDate.h"
#include "math.h"

//--------------------------------------------------------------------
// C imitation of a Constructor for the time typedef.
timeStruct time(int s, int m, int h)
{
    if (h == 24)
	  h = 0;

    timeStruct ts = {s, m, h};

    return ts;
}

//--------------------------------------------------------------------
// C imitation of a Constructor for the date typedef.
dateStruct date(int d, int m, int y)
{
	dateStruct ds = {d, m, y};

    return ds;
}

//--------------------------------------------------------------------
//
int validateDate(dateStruct ds)
{
	return (ds.year < 2016 || ds.month == 0 || ds.day == 0) ? 1 : 0;
}

//--------------------------------------------------------------------
//
int validateTime(timeStruct ts)
{
	if (ts.second > 60)// || ts.second == NaN)
	  return 0;
	if (ts.minute > 60)// || ts.minute == NaN)
	  return 0;
	if (ts.hour > 60)// || ts.hour == NaN)
	  return 0;
    return 1;
}

//--------------------------------------------------------------------
//



