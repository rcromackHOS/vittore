/*
 * engineController.h
 *
 *  Created on: May 18, 2016
 *      Author: Rhys
 */

#ifndef ENGINECONTROLLER_H_
#define ENGINECONTROLLER_H_

#include <msp430.h>

//--------------------------------------------------------------------

typedef struct
{
  int mode;

  int runTime;

  int engineHours;

} engineStruct;

#define ENGINE_STOP  	0x00;
#define ENGINE_PRE      0x01;
#define ENGINE_CRANK    0x02;
#define ENGINE_POST     0x03;
#define ENGINE_RUNNING  0x04;
#define ENGINE_STOPPING 0x05;

//--------------------------------------------------------------------

#endif /* ENGINECONTROLLER_H_ */
