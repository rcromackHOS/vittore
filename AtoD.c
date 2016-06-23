/*****< AtoD.c >************************************************************/
/*                                                                            */
/*  Hardware - Analogto Digital Sourcer 								  */
/*                                                                            */
/*  Author:  Tom Nixon                                                        */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/13/16  Tom Nixon       Initial creation.                              */
/******************************************************************************/


#ifndef _AtoD_C_
#define _AtoD_C_

#include <msp430.h>        /* Compiler specific Chip header.                  */
#include "Hardware.h"
#include "AtoD.h"
#include "config.h"

#define CALADC12_15V_30C  *((unsigned int *)0x1A22)   // Temperature Sensor Calibration-30 C
                                                      //See device datasheet for TLV table memory mapping
#define CALADC12_15V_85C  *((unsigned int *)0x1A24)   // Temperature Sensor Calibration-85 C

AtoDInputs_t AtoDInput;
unsigned int ADC[9];
int TempCountdown = 0;

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
	switch(__even_in_range(ADC12IV,34))
	{
		case  0: break;									// Vector  0:  No interrupt
		case  2: break;									// Vector  2:  ADC overflow
		case  4: break;									// Vector  4:  ADC timing overflow
		case  6: break;									// Vector  6:  ADC12IFG0
		case  8: break;									// Vector  8:  ADC12IFG1
		case 10: break;									// Vector 10:  ADC12IFG2
		case 12: break;									// Vector 12:  ADC12IFG3
		case 14: break;									// Vector 14:  ADC12IFG4
		case 16: break;									// Vector 16:  ADC12IFG5			
		case 18: break;						  			// Vector 18:  ADC12IFG6
		case 20: break;									// Vector 20:  ADC12IFG7
		case 22: 			
					ADC[AD_P_BATTERY_VLT] = 	(ADC[AD_P_BATTERY_VLT] + ADC12MEM0)/2;    // Move A0 results, IFG is cleared
					ADC[AD_N_BATTERY_VLT] = 	(ADC[AD_N_BATTERY_VLT] + ADC12MEM1)/2;    // Move A1 results, IFG is cleared
					ADC[AD_P_ENGINE_VER] = 		(ADC[AD_P_ENGINE_VER] + ADC12MEM2)/2;
					ADC[AD_N_ENGINE_VER] = 		(ADC[AD_N_ENGINE_VER] + ADC12MEM3)/2;          
					ADC[AD_P_ENGINE_CUR] = 		(ADC[AD_P_ENGINE_CUR] + ADC12MEM4)/2;          
					ADC[AD_N_ENGINE_CUR] = 		(ADC[AD_N_ENGINE_CUR] + ADC12MEM5)/2;
					ADC[AD_12V_BATTERY_VLT] = 	(ADC[AD_12V_BATTERY_VLT] + ADC12MEM6)/2;          
					ADC[AD_24V_POWER_VLT] = 	(ADC[AD_24V_POWER_VLT] + ADC12MEM7)/2;          
					ADC[AD_INTERNAL_TEMP] = 	(ADC[AD_INTERNAL_TEMP] + ADC12MEM8)/2;		

					_ADCs_UPDATED_ = 1;
					break;								// Vector 22:  ADC12IFG8
					
		case 24: break;									// Vector 24:  ADC12IFG9
		case 26: break;									// Vector 26:  ADC12IFG10
		case 28: break;									// Vector 28:  ADC12IFG11
		case 30: break;									// Vector 30:  ADC12IFG12
		case 32: break;									// Vector 32:  ADC12IFG13
		case 34: break;									// Vector 34:  ADC12IFG14
		default: break; 
	}
}


void ConfigureA2D(void)
{

	TempCountdown = 100;

	// --------------------------

	P6SEL |= (AD_P_BATTERY_PIN + AD_N_BATTERY_PIN + AD_P_ENGINE_VER_PIN + AD_N_ENGINE_VER_PIN + AD_P_ENGINE_CUR_PIN + AD_N_ENGINE_CUR_PIN + AD_12V_MONITOR_PIN);
	P7SEL |= AD_24V_MONITOR_PIN;

	/* Initialize the shared reference module */
	REFCTL0 |= REFMSTR + REFVSEL_3 + REFON;    // Enable internal 2.5V reference & internal temp sensor

	//ADC12CTL0 = ADC12SHT0_10 + ADC12MSC + ADC12REF2_5V + ADC12REFON + ADC12ON;		// sample&hold 512 cycles,Multiple sample/conversions,2.5V reference,Turn on ADC12_A
	ADC12CTL0 = ADC12SHT0_10 + ADC12MSC + ADC12ON;		// sample&hold 512 cycles,Multiple sample/conversions,2.5V reference,Turn on ADC12_A
	ADC12CTL1 = ADC12SHP + ADC12SSEL_1 + ADC12CONSEQ_3;		 							// start at MEM0,Use sampling timer, ACLK as source, repeated sequence


	ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_0;				// VREF+ to AVss, Input channel A0
	ADC12MCTL1 = ADC12SREF_1 + ADC12INCH_1;
	ADC12MCTL2 = ADC12SREF_1 + ADC12INCH_2;
	ADC12MCTL3 = ADC12SREF_1 + ADC12INCH_3;
	ADC12MCTL4 = ADC12SREF_1 + ADC12INCH_4;
	ADC12MCTL5 = ADC12SREF_1 + ADC12INCH_5;
	ADC12MCTL6 = ADC12SREF_1 + ADC12INCH_6;
	ADC12MCTL7 = ADC12SREF_1 + ADC12INCH_15;
	ADC12MCTL8 = ADC12SREF_1 + ADC12INCH_10 + ADC12EOS;	// Internal Temp Sensor, end of sequence of conversions

	ADC12IFG = 0;
	ADC12IE |= ADC12IE8;								// Enable interrupt on last A/D reading of sequence

	ADC12CTL0 |= ADC12ENC;								// Enable conversions
	ADC12CTL0 |= ADC12SC;				   				// Start sampling/conversion
}

//--------------------------------------------------------------------
float ConvertInternalTempToCelcius(unsigned int rawCount)
{
	float temperatureDegC;
	//float temperatureDegF;

	// Temperature in Celsius. See the Device Descriptor Table section in the
    // System Resets, Interrupts, and Operating Modes, System Control Module
    // chapter in the device user's guide for background information on the
    // used formula.
    temperatureDegC = (float)(((long)rawCount - CALADC12_15V_30C) * (85 - 30)) /
            (CALADC12_15V_85C - CALADC12_15V_30C) + 30.0f;

    // Temperature in Fahrenheit Tf = (9/5)*Tc + 32
    //temperatureDegF = temperatureDegC * 9.0f / 5.0f + 32.0f;

    return(temperatureDegC);
}

//--------------------------------------------------------------------
//
void loadAnalogData()
{
	 if	(TempCountdown == 0)
	 {
		 TempCountdown = 100;

		 VALUE_INTERNAL_TEMP = ConvertInternalTempToCelcius(ADC[AD_INTERNAL_TEMP]) * 100;
	 }
}

#endif
