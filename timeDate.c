/*
 * timeDate.c
 *
 *  Created on: May 10, 2016
 *      Author: RCromack
 */

//--------------------------------------------------------------------

#include "timeDate.h"
#include "math.h"

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		C imitation of a Constructor for the time typedef.
//
// RETURN/UPDATES:	timeStruct time structure built on the arguments
//
//---------------------------------------------------------------------------------------------
timeStruct time(int s, int m, int h)
{
    if (h == 24)
	  h = 0;

    timeStruct ts = {s, m, h};

    return ts;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		C imitation of a Constructor for the date typedef.
//
// RETURN/UPDATES:	dateStruct date structure built on the arguments
//
//---------------------------------------------------------------------------------------------
dateStruct date(int d, int m, int y)
{
	dateStruct ds = {d, m, y};

    return ds;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		C imitation of a Constructor for the time and date typedef.
//
// RETURN/UPDATES:	dateTimeStruct date time structure built on the arguments
//
//---------------------------------------------------------------------------------------------
dateTimeStruct datetime(int s, int mm, int h, int d, int m, int y)
{
	dateTimeStruct dt = {s, mm, h, d, m, y};

    return dt;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		checks the dateStruct to see if it's valid
//
// RETURN/UPDATES:	1 - pass
//					0 - fail
//---------------------------------------------------------------------------------------------
int validateDate(dateStruct ds)
{
	return (ds.year < 2016 || ds.month == 0 || ds.day == 0) ? 1 : 0;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		checks the timeStruct to see if it's valid
//
// RETURN/UPDATES:	1 - pass
//					0 - fail
//---------------------------------------------------------------------------------------------
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



