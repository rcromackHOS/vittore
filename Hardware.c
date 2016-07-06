/*****< Hardware.h >************************************************************/
/*                                                                            */
/*  Hardware - Hardware Configuration header 								  */
/*                                                                            */
/*  Author:  Tom Nixon                                                        */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   06/13/16  Tom Nixon       Initial creation.                              */
/*   06/17/16  R Cromack       Added port DIR for LCD                         */
/*   06/23/16  R Cromack       Added Timer A0 config                          */
/******************************************************************************/


#ifndef _Hardware_C_
#define _Hardware_C_

#include <msp430.h>        /* Compiler specific Chip header.                  */
#include "Hardware.h"
#include "AtoD.h"
#include "GPS.h"

static void ConfigurePins(void);
static void ConfigureCrystals(void);
static void ConfigureTimerA0(void);
static void StopTimerA0(void);
static void ConfigureTimerA1(void);
static void StopTimerA1(void);
static void SetVcoreUp (unsigned int level);


void InitializeHardware(void)

{
	ConfigurePins();
	ConfigureCrystals();

	//ConfigureTimerA0();
	ConfigureTimerA1();

	ConfigureA2D();
	ConfigureGPSSerialPort();

	//Add I2C, UARTS, SPI, General Timer,

}


//---------------------------------------------------------------------------------------------
// DESCRIPTION: Sets pin registers
//
// RETURN/UPDATES: void
//
//---------------------------------------------------------------------------------------------
static void ConfigurePins(void)
{
	// unconnected pins should be set to Output with REN enabled

	P1DIR = 0;						// 1= Output, 0= Input
	P1OUT = BUTTON_nRESET;			// If setup as Input: 0=Pulldown, 1=Pullup
	P1REN = BUTTON_nRESET;			// If setup as Input: 0=Disables pullup or down, 1=Enables pullup or down
	P1SEL = 0;						// 0=I/O, 1=Peripheral function enabled
	P1DS = 0;						// Default Low drive strength
	P1IES = BUTTON_nRESET + BUTTON_nLIGHT_1H + BUTTON_nAUTO + BUTTON_nSTANDBY + BUTTON_nDOWN + BUTTON_nUP;		// Int flag will be set on: 0=Rising edge, 1=Falling edge
	P1IE = 0;						// 1=Port Interrupt enabled

	P2DIR = OUT_ENGINE_ACC + BIT4;
	P2OUT = BUTTON_nOIL_RST + IN_MAST_CUT_OUT + IN_BAT_CELL_FAIL + BIT4 + IN_nLOW_FUEL;
	P2REN = BUTTON_nOIL_RST + IN_MAST_CUT_OUT + IN_BAT_CELL_FAIL + BIT4 + IN_nLOW_FUEL;
	P2SEL = 0;
	P2DS = 0;
	P2IES = IN_MAST_CUT_OUT;
	P2IE = IN_MAST_CUT_OUT;						// falling edge
	P2IFG &= ~IN_MAST_CUT_OUT;

	P3DIR = BIT0 + BIT3 + OUT_LIGHTS_ON + LCD_SPI_SIMO;
	P3OUT = BIT0 + BIT3;
	P3REN = BIT0 + BIT3;
	P3DS = 0;
	P3SEL = 0; 	//enable peripherals individually

	P4DIR = OUT_RESET_LED + OUT_LIGHT_1H_LED + OUT_AUTO_LED + OUT_STANDBY_LED + OUT_DOWN_LED + OUT_UP_LED;
	P4OUT = 0;
	P4REN = 0;
	P4DS = 0;
	P4SEL = 0;

	P5DIR = BIT0 + BIT1 + LCD_SPI_RESET + LCD_SPI_SCK;
	P5OUT = BIT0 + BIT1;
	P5REN = BIT0 + BIT1;
	P5DS = 0;
	P5SEL = 0; //enable peripherals individually

	P6DIR = 0;
	P6OUT = IN_OIL_PRESSURE_OK;
	P6REN = IN_OIL_PRESSURE_OK;
	P6DS = 0;
	P6SEL = 0; //enable peripherals individually

	P7DIR = OUT_DOWN_MAST + OUT_UP_MAST;
	P7OUT = IN_ENGINE_TEMP_FAIL;
	P7REN = IN_ENGINE_TEMP_FAIL;
	P7DS = 0;
	P7SEL = 0;  //enable peripherals individually

	P8DIR = OUT_HEARTBEAT_LED + OUT_USB_nRESET + OUT_ASSET_IGNITION;
	P8OUT = IN_MAST_DOWN_EXTENT + IN_MAST_UP_EXTENT;
	P8REN = IN_MAST_DOWN_EXTENT + IN_MAST_UP_EXTENT;
	P8DS = 0;
	P8SEL = 0;

	P9DIR = THERM_CS + OUT_ENGINE_FAIL + OUT_ENGINE_GLOW + OUT_ENGINE_CRANK + THERM_SPI_CLK;
	P9OUT = BUTTON_nDIAGNOSTIC;
	P9REN = BUTTON_nDIAGNOSTIC;
	P9DS = 0;
	//P9SEL |= THERM_SPI_MOSI + THERM_SPI_MISO + THERM_SPI_CLK;

	P10DIR = OUT_ASSET_I1 + OUT_ASSET_I2 + OUT_ASSET_I3 + OUT_ASSET_I4 + OUT_nBATTERY_HEATER_ON + OUT_nCONTACTOR_ON + BIT6 + OUT_nCABINET_HEATER_ON;
	P10OUT = BIT6;
	P10REN = BIT6;
	P10DS = 0;
	P10SEL = 0;

	P11DIR = OUT_nSPARE3_ON + OUT_nSPARE2_ON + OUT_nSPARE4_ON;
	P11OUT = 0;
	P11REN = 0;
	P11DS = 0;
	P11SEL = 0;

}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		-Set up Crystals & clock
//					-XT1=32KHz, XT2=12MHz
//					-ACLK = 32KHz, MCLK = 12MHz, SMCLK = 1.5MHz
//
// RETURN/UPDATES:	n/a
//---------------------------------------------------------------------------------------------
static void ConfigureCrystals(void)
{
	// Verify!!
	SetVcoreUp(PMMCOREV_1);
	SetVcoreUp(PMMCOREV_2);                     	// Set VCore to 1.8MHz for 20MHz

	P5SEL |= (XTAL_12MHZ_IN + XTAL_12MHZ_OUT);			// XT2 pins 12MHz
	P7SEL |= XTAL_32kHZ_IN + XTAL_32kHZ_OUT;			// XT1 pins 32kHz

	UCSCTL6 &= ~(XT1OFF + XT2OFF);            		// Set XT1 & XT2 ON

	UCSCTL6 |= XCAP_3;

   	// Loop until XT1,XT2 & DCO stabilizes
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
													// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG;                    		// Clear fault flags
	}while (SFRIFG1&OFIFG);                   		// Test oscillator fault flag

	UCSCTL6 &= ~XT2DRIVE1;							// XT2 drive 1 (01)

	UCSCTL4 = SELA_0 + SELS__XT2CLK + SELM__XT2CLK;	// ACLK<-XT1, SMCK<-XT2, MCLK<-XT2

	UCSCTL5 |= DIVS__8;								// SMCLK/4 = 1.5MHz
}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:		-Setup TimerA0 to continually count up to TA0CCR0, interrupt then start over
//
// RETURN/UPDATES:	n/a
//---------------------------------------------------------------------------------------------

void ConfigureTimerA0(void)
{
	StopTimerA0();
	// ACLK/countup/divby 1
	TA0CTL = TASSEL__ACLK + MC_1 + ID_2;
	//TA0CCR0 = ACLK / ID_2 / 100 - 1;
	TA0CCR0 = ((32768 / 4) / 100) - 1;
	// CCR0 interrupt enabled
	TA0CCTL0 = CCIE;
}

static void StopTimerA0(void)
{
	TA0CTL = 0;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		-Setup TimerA1 to continually count up to TA1CCR0, interrupt then start over
//
// RETURN/UPDATES:	n/a
//---------------------------------------------------------------------------------------------
void ConfigureTimerA1(void)
{
	StopTimerA1();
	TA1CCTL0 = CCIE;									// CCR0 interrupt enabled
	TA1CCR0 = 328;										// 328;
	TA1CTL = TASSEL_1 + MC_1 + TACLR;					// ACLK, upmode, clear TAR
}

static void StopTimerA1(void)
{
	TA1CTL = 0;
}

//--------------------------------------------------------------------
// INITIAL GPS
static void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}






#endif
