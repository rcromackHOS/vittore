/*****< WatchdogTimerControl.c >************************************************************/
/*                                                                            */
/*  Watchdog Timer functions							  					  */
/*                                                                            */
/*  Author:  Tom Nixon                                                        */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/13/16  Tom Nixon       Initial creation.                              */
/******************************************************************************/


#include "msp430f5419.h"
#include "WatchdogTimerControl.h"

void WdtInit()
{
    // TBD
}

void WdtDisable()
{
    WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer
}

void WdtEnable()
{
    WDTCTL = WDTPW + WDTSSEL__ACLK + WDTIS_4 ;  // enable WatchDog Timer every 1s
}

void WdtKeepAlive()
{
    WDTCTL = WDTPW + WDTSSEL__ACLK + WDTIS_4  + WDTCNTCL;  // enable WatchDog Timer every 1s
}
