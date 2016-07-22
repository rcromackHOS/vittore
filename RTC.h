/*****< RTC.h >************************************************************/
/*                                                                            */
/*  Hardware - Real Time Clock header 								  */
/*                                                                            */
/*  Author:  Tom Nixon                                                        */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/13/16  Tom Nixon       Initial creation.                              */
/******************************************************************************/


#ifndef _RTC_H_
#define _RTC_H_

#define RTC_ADDRESS		0x6f

#include <msp430.h>        /* Compiler specific Chip header.                  */
#include "timeDate.h"

extern void InitializeRTC();
extern void pollTime();
extern int RTC_adjust(dateTimeStruct dt);
int RTC_isRunning(void);
int RTC_timeIsSet(void);
extern dateTimeStruct RTC_now();

extern void ConfigureI2CForRTC(void);
int WriteRTCRegister(unsigned char regAddr, unsigned char val);
int ReadRTCRegisters(unsigned char regAddr, unsigned char cnt);

extern unsigned char RtcRxBuffer[];
extern unsigned int	RtcCountdown;
extern unsigned int	RtcTimeout;

#endif
