/*
 * MCP7940N.h
 *
 *  Created on: May 26, 2016
 *      Author: rcromack
 */

#ifndef MCP7940N_H_
#define MCP7940N_H_

#include <msp430.h>
#include "config.h"
#include "timeDate.h"

//--------------------------------------------------------------------

// state codes
#define STATE_NOMINAL     0x0
#define STATE_PRESSING    0x1
#define STATE_PRESSED     0x2
#define STATE_RELEASING	  0x3

#define RTC_ADDRESS 	  0x6F

//--------------------------------------------------------------------

unsigned int I2C_RXBUFFER;
unsigned int I2C_TXBUFFER;

//--------------------------------------------------------------------

int RTC_begin(void);
int RTC_adjust(dateTimeStruct dt);
int RTC_isRunning(void);
int RTC_lostTime(void);
dateTimeStruct RTC_now();

//--------------------------------------------------------------------

#endif /* MCP7940N_H_ */
