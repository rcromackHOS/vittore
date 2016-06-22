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
/******************************************************************************/


#ifndef _Hardware_H_
#define _Hardware_H_

#include <msp430.h>        /* Compiler specific Chip header.                  */


/******************************************************************************/
/** CONFIGURE HARDWARE DEFINE: Which assembly of PCB are we targettinge      **/
/******************************************************************************/

#define _HRDWCFG_PROC_07P_			
#define _HRDWCFG_RELAY_06P_		

/******************************************************************************/
/** CONFIGURE HARDWARE TESTING DEFINE: Are we doing a particular Hardware test**/
/******************************************************************************/

// TESTING OPTIONS
// END TESTING OPTIONS

/**************************************************************************/
/*
	SYSTEM TIMING Parameters: Jan 233, 2014

	MCLK = BT_CPU_FREQ = 25MHZ

	SMCLK = Disabled if DEBUG BAUD is >= 9600
	DEBUG BAUD RATE = BT_DEBUG_UART_BAUDRATE = 9600BAUD

*/
/***************************************************************************/

/******************************************************************************/
//	PORT 1 Defines
/******************************************************************************/
#define BUTTON_nRESET			(BIT0)
#define BSL_TX					(BIT1)
#define BSL_RX					(BIT2)
#define BUTTON_nLIGHT_1H		(BIT3)
#define BUTTON_nAUTO		 	(BIT4)
#define BUTTON_nSTANDBY			(BIT5)
#define BUTTON_nDOWN			(BIT6)
#define BUTTON_nUP				(BIT7)


/******************************************************************************/
//	PORT 2 Defines
/******************************************************************************/
#define OUT_ENGINE_ACC			(BIT0)
#define BUTTON_nOIL_RST			(BIT1)
#define IN_MAST_CUT_OUT			(BIT2)		// High when Mast cut out is enabled (latched)
#define IN_BAT_CELL_FAIL		(BIT3)		// High when Cells fail
//							 	(BIT4)
#define IN_nLOW_FUEL			(BIT5)
#define PGM_CTRL				(BIT6)
#define IN_nRTC_INT				(BIT7)


/******************************************************************************/
//	PORT 3 Defines
/******************************************************************************/
//								(BIT0)
#define RTC_I2C_SDA				(BIT1)
#define RTC_I2C_SCL				(BIT2)
//								(BIT3)
#define GPS_MODULE_RX			(BIT4)
#define GPS_MODULE_TX			(BIT5)
#define OUT_LIGHTS_ON			(BIT6)
#define LCD_SPI_SIMO			(BIT7)


/******************************************************************************/
//	PORT 4 Defines
/******************************************************************************/
#define OUT_RESET_LED			(BIT0)
#define OUT_LIGHT_1H_LED		(BIT1)
#define OUT_AUTO_LED			(BIT2)
#define OUT_STANDBY_LED			(BIT3)
#define OUT_DOWN_LED			(BIT4)
#define OUT_UP_LED				(BIT5)
#define IN_VERSION_BIT3			(BIT6)
#define IN_VERSION_BIT2			(BIT7)


/******************************************************************************/
//	PORT 5 Defines
/******************************************************************************/
//								(BIT0)
//								(BIT1)
#define XTAL_12MHZ_IN			(BIT2)
#define XTAL_12MHZ_OUT			(BIT3)
#define LCD_SPI_RESET			(BIT4)		// active low or high???
#define LCD_SPI_SCK				(BIT5)
#define PGM_TX					(BIT6)
#define PGM_RX					(BIT7)


/******************************************************************************/
//	PORT6 Defines
/******************************************************************************/
#define AD_P_BATTERY_PIN		(BIT0)
#define AD_N_BATTERY_PIN		(BIT1)
#define AD_P_ENGINE_VER_PIN		(BIT2)
#define AD_N_ENGINE_VER_PIN		(BIT3)
#define AD_P_ENGINE_CUR_PIN		(BIT4)
#define AD_N_ENGINE_CUR_PIN		(BIT5)
#define AD_12V_MONITOR_PIN		(BIT6)
#define IN_OIL_PRESSURE_OK		(BIT7)	// High when Oil pressure is OK


/******************************************************************************/
//	PORT7 Defines
/******************************************************************************/
#define XTAL_32kHZ_IN			(BIT0)
#define XTAL_32kHZ_OUT			(BIT1)
#define OUT_DOWN_MAST			(BIT2)
#define OUT_UP_MAST				(BIT3)
//								(BIT4)
//								(BIT5)
#define IN_ENGINE_TEMP_FAIL		(BIT6)	// High When Engine Temp is Failing
#define AD_24V_MONITOR_PIN		(BIT7)


/******************************************************************************/
//	PORT8 Defines
/******************************************************************************/
#define OUT_HEARTBEAT_LED		(BIT0)
#define IN_ENGINE_RPM			(BIT1)
#define IN_VERSION_BIT1			(BIT2)
#define OUT_USB_nRESET			(BIT3)
#define IN_VERSION_BIT0			(BIT4)
#define OUT_ASSET_IGNITION		(BIT5)
#define IN_MAST_DOWN_EXTENT		(BIT6)
#define IN_MAST_UP_EXTENT		(BIT7)


/******************************************************************************/
//	PORT9 Defines
/******************************************************************************/
#define THERM_CS				(BIT0)
#define THERM_SPI_MOSI			(BIT1)
#define THERM_SPI_MISO			(BIT2)
#define THERM_SPI_CLK			(BIT3)
#define OUT_ENGINE_FAIL			(BIT4)
#define OUT_ENGINE_GLOW			(BIT5)
#define OUT_ENGINE_CRANK		(BIT6)
#define BUTTON_nDIAGNOSTIC		(BIT7)


/******************************************************************************/
//	PORT10 Defines
/******************************************************************************/
#define OUT_ASSET_I1			(BIT0)
#define OUT_ASSET_I2			(BIT1)
#define OUT_ASSET_I3			(BIT2)
#define OUT_ASSET_I4			(BIT3)
#define OUT_nBATTERY_HEATER_ON	(BIT4)
#define OUT_nCONTACTOR_ON		(BIT5)
//								(BIT6)
#define OUT_nCABINET_HEATER_ON	(BIT7)


/******************************************************************************/
//	PORT11 Defines
/******************************************************************************/
#define OUT_nSPARE3_ON			(BIT0)
#define OUT_nSPARE2_ON			(BIT1)
#define OUT_nSPARE4_ON			(BIT2)

extern void InitializeHardware(void);


#endif
