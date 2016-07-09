/*****< Main.c>    ************************************************************/
/*                                                                            */
/*  Main Loop							  					  				  */
/*                                                                            */
/*  Author:  Tom Nixon                                                        */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/13/16  Tom Nixon       Initial creation.                              */
/******************************************************************************/


#include <msp430.h> 

/*
 * main.c
 */

#include "Hardware.h"
#include "msp430f5419A.h"
#include "Common.h"
#include "WatchdogTimerControl.h"
#include "AtoD.h"
#include "RTC.h"
#include "GPS.h"
#include "Flash.h"


unsigned int GpsStateCountdown,TempCountdown;
unsigned int GpsMsgOk;
float	temp;

int main(void) {

    WdtDisable();
    __disable_interrupt();

    InitializeHardware();

    switch(GetConfiguration()) {
   	  	  case SEG_VIRGIN:
   	  	  case 0:	  //Display( ("#2: FLASH is Good\r\n") );
   	  		  	  	  _nop();
    	  	  	  	  break;
   	  	  default: 	  // Flash is bad!
    	  	  	  	  // Run on defaults
   	  		  	  	  //Display( ("#2: FLASH ERROR\r\n") );
    	  	  	  	  _nop();
   	  		  	  	  break;
   	  }



    __enable_interrupt();

    WdtEnable();

/*Test clocks
    P11DIR |= 0x07;                           // ACLK, MCLK, SMCLK set out to pins
    P11SEL |= 0x07;                           // P11.0,1,2 for debugging purposes.
end test clocks */


     GpsStateCountdown = 50;
     TempCountdown = 100;
     GpsMsgOk = 0;
     P10DIR |= 0x01;

     while(GpsStateCountdown != 0);


/*
     	 // setup GPS module to only output the messages we want and at a reduced rate
     if(ConfigureGPSNmeaOutput() == FALSE) {

         		while(TRUE);
     }
     GpsStateCountdown = 50;

     //ADD!! Must handle when GPS module is not populated or bad!!
*/

     while(TRUE) {

/* if A/D's have updatd values
    	  	__disable_interrupt();
			copy int ADC[] to local variables
			__enable_interrupt();
*/
/*
    if(ReadRTCRegister(0x00)) {
    	_nop();
    }
    else {
    	_nop();
    }
*/
/* GPS Decode  */
/*    	 if(GpsStateCountdown == 0) {
    		 GpsStateCountdown = 50;
    		 if(GpsMessageRetrieve() == TRUE) {
    			 GpsMsgOk++;
    			 _nop();

    		 }
    	 }

    	 if(TempCountdown == 0) {
    		 TempCountdown = 100;

    		 temp = ConvertInternalTempToCelcius(ADC[AD_INTERNAL_TEMP]);
    	 }

*/
/* end GPS test */



/* Test outputs
    	P8OUT ^= OUT_HEARTBEAT_LED;
    	P2OUT ^= OUT_ENGINE_ACC;
    	P3OUT ^= OUT_LIGHTS_ON;
    	P7OUT ^= OUT_DOWN_MAST;
    	P7OUT ^= OUT_UP_MAST;
    	P8OUT ^= OUT_ASSET_IGNITION;
    	P9OUT ^= OUT_ENGINE_FAIL;
    	P9OUT ^= OUT_ENGINE_GLOW;
		P9OUT ^= OUT_ENGINE_CRANK;
		P10OUT ^= OUT_nBATTERY_HEATER_ON;
		P10OUT ^= OUT_nCONTACTOR_ON;
		P10OUT ^= OUT_nCABINET_HEATER_ON;
		P10OUT ^= OUT_ASSET_I1;
		P10OUT ^=  OUT_ASSET_I2;
		P10OUT ^=  OUT_ASSET_I3;
		P10OUT ^=  OUT_ASSET_I4;
		P11OUT ^= OUT_nSPARE3_ON;
		P11OUT ^= OUT_nSPARE2_ON;
		P11OUT ^= OUT_nSPARE4_ON;
end test outputs */


    	WdtKeepAlive();
    }



}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:		-TimerA1 Interrupt
//
// RETURN/UPDATES:	n/a
//---------------------------------------------------------------------------------------------
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void) {

	if(GpsStateCountdown != 0)	GpsStateCountdown--;

	if(TempCountdown != 0)	TempCountdown--;


	P10OUT ^= 0x01;
}


