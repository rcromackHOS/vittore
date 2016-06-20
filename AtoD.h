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



typedef enum {
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

extern AtoDInputs_t AtoDInputs;
extern unsigned int ADC[];

extern void loadAnalogData();

extern void ConfigureA2D(void);
extern float ConvertInternalTempToCelcius(unsigned int);

//--------------------------------------------------------------------

int v12_readings[20];
int v12_index;
int v12_total;

int iT_readings[20];
int iT_index;
int iT_total;


//--------------------------------------------------------------------


#endif
