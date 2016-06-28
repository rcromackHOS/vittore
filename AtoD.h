/*****< AtoD.h >************************************************************/
/*                                                                            */
/*  Hardware - Analogto Digital header 								  */
/*                                                                            */
/*  Author:  Tom Nixon                                                        */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/13/16  Tom Nixon       Initial creation.                              */
/******************************************************************************/


#ifndef _AtoD_H_
#define _AtoD_H_

#include <msp430.h>        /* Compiler specific Chip header.                  */
#include <math.h>

/***************  A/D notes **********************/
/*
	AD_P_BATTERY_VLT:		-Positive of Battery bank (directly from battery)
							-Calc: 9.09k/(100k+9.09k) so 30.003V will give 2.5V or 4095 cnts

	AD_12V_BATTERY_VLT: 	-12V Battery read on Procor board after filter.
							-Calc: 20k/(100k+20k) so 15V will give 2.5V or 4095 cnts

	AD_24V_POWER_VLT:		-24V that supplies the Relay board (this can be from several diff sources)
							-Calc: 9.09k/(100.1k+9.09k) so 30V will give 2.5V or 4095 cnts

*/
/************ end A/D notes *********************/

typedef enum
{
   AD_P_BATTERY_VLT = 0,
   AD_N_BATTERY_VLT,
   AD_P_ENGINE_VER,
   AD_N_ENGINE_VER,
   AD_P_ENGINE_CUR,
   AD_N_ENGINE_CUR,
   AD_12V_BATTERY_VLT,
   AD_24V_POWER_VLT,
   AD_INTERNAL_TEMP
} AtoDInputs_t;

extern int TempCountdown;

extern AtoDInputs_t AtoDInputs;
extern unsigned int ADC[];
extern void loadAnalogData();

extern void ConfigureA2D(void);
extern float ConvertInternalTempToCelcius(unsigned int);

#endif
