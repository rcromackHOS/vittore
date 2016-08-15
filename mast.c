/*
 * mast.c
 *
 *  Created on: Jun 24, 2016
 *      Author: rcromack
 */


//--------------------------------------------------------------------

#include "Hardware.h"
#include "config.h"
#include "mast.h"
#include "button.h"
#include "Common.h"
#include "WatchdogTimerControl.h"
#include "lcd.h"

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Checks the buttons and mast status for sleep mode
//
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void sleepModeProcess()
{
	button_stateMachine();
	mastUpDown();
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Logic for the mast up and down buttons
//						-	In diagnostic mode, up and down scroll pages
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void mastUpDown()
{
	if (_DIAGNOSTIC_MODE == 1)
	{
		if (buttonList[5].state == STATE_PRESSED && buttonList[4].state != STATE_PRESSED)
		{
			_DIAGNOSTIC_PAGE++;
			buttonList[5].state = STATE_NOMINAL;
		    if (_DIAGNOSTIC_PAGE > _DIAGNOSTIC_PAGES)
		    	_DIAGNOSTIC_PAGE = 0;
		}

		else if (buttonList[5].state != STATE_PRESSED && buttonList[4].state == STATE_PRESSED)
		{
			_DIAGNOSTIC_PAGE--;
			buttonList[4].state = STATE_NOMINAL;
			if (_DIAGNOSTIC_PAGE < 0)
		    	_DIAGNOSTIC_PAGE = _DIAGNOSTIC_PAGES;
		}
    }

	else
	{
		if (buttonList[5].state == STATE_PRESSED && buttonList[4].state != STATE_PRESSED)
			mast_stateMachine( MAST_RISING );

		else if (buttonList[5].state != STATE_PRESSED && buttonList[4].state == STATE_PRESSED)
			mast_stateMachine( MAST_LOWERING );

		else
			mast_stateMachine( MAST_NOMINAL );
	}
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		state machine for the logic to control the mast
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void mast_stateMachine(mast_States_t deltastate)
{
	int upExtentTriggered = (P8IN & IN_MAST_UP_EXTENT) != 0 ? 1 : 0; // 0 = untriggered
	int downExtentTriggered = (P8IN & IN_MAST_DOWN_EXTENT) != 0 ? 1 : 0; // 1 = untriggered
	int latchCutout = (P2IN & IN_MAST_CUT_OUT) != 0 ? 0 : 1; // 1 = untriggered

	switch (deltastate)
	{
		case MAST_NOMINAL:

			P7OUT &= ~OUT_DOWN_MAST;
			P7OUT &= ~OUT_UP_MAST;

			if (downExtentTriggered == 1)
				_MAST_STATUS = MAST_MAXDOWN;

			else if (upExtentTriggered == 1)
				_MAST_STATUS = MAST_MAXUP;

			if (downExtentTriggered == 0 && upExtentTriggered == 0)
				_MAST_STATUS = MAST_NOMINAL;

			break;

		case MAST_RISING:

			switch (_MAST_STATUS)
			{
				case MAST_MAXDOWN: 		// raise mast passed the lower extent

	/*				if (upExtentTriggered != 0)
						P7OUT |= OUT_UP_MAST;
					else
						P7OUT &= ~OUT_UP_MAST;
	*/
					if (latchCutout == 1)
						P7OUT |= OUT_UP_MAST;

					if (downExtentTriggered == 0)
						_MAST_STATUS = MAST_NOMINAL;

					break;

				case MAST_NOMINAL: 	// raise mast as normal, watch for upper threshold

					if (upExtentTriggered == 1)
					{
						_MAST_STATUS = MAST_MAXUP;
						P7OUT &= ~OUT_UP_MAST;
					}

					else if (upExtentTriggered == 0)
						P7OUT |= OUT_UP_MAST;

					break;

				case MAST_MAXUP: 		// don't let the mast keep rising if we're maxed

					P7OUT &= ~OUT_UP_MAST;
					break;
			}

			break;

		case MAST_LOWERING:

			switch (_MAST_STATUS)
			{
				case MAST_MAXDOWN: 		// lower mast into the lower extent

					P7OUT &= ~OUT_DOWN_MAST;

					break;

				case MAST_NOMINAL: 	// lower mast as normal, watch for lower threshold

					if (downExtentTriggered == 1)
					{
						_MAST_STATUS = MAST_MAXDOWN;
						P7OUT &= ~OUT_DOWN_MAST;

						enterLowPowerMode();
					}

					else if (downExtentTriggered == 0)
						P7OUT |= OUT_DOWN_MAST;

					break;

				case MAST_MAXUP: 		// No restrictions if the mast is coming down from max up

					P7OUT |= OUT_DOWN_MAST;

					if (upExtentTriggered == 0)
						_MAST_STATUS = MAST_NOMINAL;

					break;

			}

			break;
	}
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Handling function that preps the unit for low power mode
//						- turn off peripherals
//						- raise the flag
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void enterLowPowerMode()
{
	//OLED_clearDisplay();

	//WdtDisable();
	//_BIS_SR(LPM3_bits + GIE); // LPM3_bits (SCG1+SCG0+CPUOFF) disabled & interrupts enabled

	WdtDisable();
	_LPMODE = 1;

	P2OUT = ~OUT_ENGINE_ACC;
	P3OUT = ~OUT_LIGHTS_ON;
	P4OUT = ~(OUT_RESET_LED + OUT_LIGHT_1H_LED + OUT_AUTO_LED + OUT_STANDBY_LED + OUT_DOWN_LED + OUT_UP_LED);
	P5OUT = ~BIT4;
	P7OUT = ~(OUT_DOWN_MAST + OUT_UP_MAST);
	P8OUT = ~(OUT_HEARTBEAT_LED + OUT_ASSET_IGNITION);
	P9OUT = ~(OUT_ENGINE_FAIL + OUT_ENGINE_GLOW + OUT_ENGINE_CRANK);
	P10OUT = 0x0;
	P11OUT = ~(OUT_nSPARE3_ON + OUT_nSPARE2_ON + OUT_nSPARE4_ON);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Handling function that returns from low power mode
//						- enablePeripherals
//						- clear flag
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void HandleLowPowerMode()
{
	P4OUT = ~(OUT_RESET_LED + OUT_LIGHT_1H_LED + OUT_AUTO_LED + OUT_STANDBY_LED + OUT_DOWN_LED + OUT_UP_LED);

	if (_MAST_STATUS != MAST_MAXDOWN)
	{
		InitializeDisplay();

		WdtEnable();  // enable Watch dog

		_LPMODE = 0;

		_SYS_FAILURE_ = 0;
		BMS_EVENT = 0;

		P5OUT |= BIT4;

		_UPDATE_SCREEN_ = 1;
	}
}
