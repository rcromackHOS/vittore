/*
 * timeDate.h
 *
 *  Created on: May 10, 2016
 *      Author: RCromack
 */

#ifndef TIMEDATE_H_
#define TIMEDATE_H_

#define SECONDS_FROM_1970_TO_2000 946684800

//--------------------------------------------------------------------


//--------------------------------------------------------------------

typedef struct
{
  int second;
  int minute;
  int hour;

} timeStruct;

typedef struct
{
  int day;
  int month;
  int year;

} dateStruct;

typedef struct
{
	  int second;
	  int minute;
	  int hour;
	  int day;
	  int month;
	  int year;

} dateTimeStruct;


//--------------------------------------------------------------------

timeStruct time(int s, int m, int h);
dateStruct date(int d, int m, int y);
dateTimeStruct datetime(int s, int mm, int h, int d, int m, int y);

int validateDate(dateStruct ds);
int validateTime(timeStruct ts);

//--------------------------------------------------------------------

#endif /* TIMEDATE_H_ */


