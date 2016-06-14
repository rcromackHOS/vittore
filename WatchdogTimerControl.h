/*****< WatchdogTimerControl.h >************************************************************/
/*                                                                            */
/*  Watchdog Timer header							  					  */
/*                                                                            */
/*  Author:  Tom Nixon                                                        */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/13/16  Tom Nixon       Initial creation.                              */
/******************************************************************************/


#ifndef WATCHDOGTIMERCONTROL_H_
#define WATCHDOGTIMERCONTROL_H_

extern void WdtInit();
extern void WdtDisable();
extern void WdtEnable();
extern void WdtKeepAlive();

#endif
