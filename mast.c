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

//--------------------------------------------------------------------
//
void sleepModeProcess()
{
	button_stateMachine();
	mastUpDown();
}

//--------------------------------------------------------------------
//
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

//--------------------------------------------------------------------
//
void mast_stateMachine(mast_States_t deltastate)
{
	int upExtentTriggered = (P8IN & IN_MAST_UP_EXTENT) != 0 ? 1 : 0; // 0 = untriggered
	int downExtentTriggered = (P8IN & IN_MAST_DOWN_EXTENT) != 0 ? 1 : 0; // 1 = untriggered
	int latchCutout = (P2IN & IN_MAST_CUT_OUT) != 0 ? 1 : 0; // 1 = untriggered

    if (deltastate == MAST_NOMINAL)
    {
		P7OUT &= ~OUT_DOWN_MAST;
		P7OUT &= ~OUT_UP_MAST;

		if (downExtentTriggered == 1)
			_MAST_STATUS = MAST_MAXDOWN;

		else if (upExtentTriggered == 1)
			_MAST_STATUS = MAST_MAXUP;

		if (downExtentTriggered == 0 && upExtentTriggered == 0)
			_MAST_STATUS = MAST_NOMINAL;
    }

    if (deltastate == MAST_RISING)
    {
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
    }

    if (deltastate == MAST_LOWERING)
    {
    	switch (_MAST_STATUS)
    	{
			case MAST_MAXDOWN: 		// raise mast passed the lower extent

				P7OUT &= ~OUT_DOWN_MAST;

				// enable input ISR
				break;

			case MAST_NOMINAL: 	// lower mast as normal, watch for lower threshold

				if (downExtentTriggered == 1)
				{
					_MAST_STATUS = MAST_MAXDOWN;
					P7OUT &= ~OUT_DOWN_MAST;
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
    }
}

//--------------------------------------------------------------------
// Handle the reset button functionality
void handle_reset()
{
	// Implimentation of reset functionality
	// hold reset button 5 seconds, apply the reset
	// then clear, reset flag a second after
	if (buttonList[3].state == STATE_NOMINAL)
	{
	   if (_DIAGNOSTIC_MODE == 1)
	   {
		   _DIAGNOSTIC_MODE = 0;
		   _DIAGNOSTIC_MODE_TMR = 0;
	   }
	   else if (_DIAGNOSTIC_MODE == 0)
	   {
		   _SYS_FAILURE_ = 0;
		   BMS_EVENT = 0;

		   clearStateCode(_STATE_CODE);

		   _RESETTING_ = 0;
	   }
	}
}




