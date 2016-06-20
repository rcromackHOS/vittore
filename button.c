/*
 * button.c
 *
 *  Created on: May 25, 2016
 *      Author: rcromack
 */


#include "button.h"
#include "config.h"
#include "Hardware.h"

//--------------------------------------------------------------------

void buildButtonStateMachine()
{
    button x = {BUTTON_nAUTO, AUTO_LED_PIN, MODE_AUTO, STATE_NOMINAL, 0, 0};
    buttonList[0] = x;

    button y = {BUTTON_nSTANDBY, STANDBY_LED_PIN, MODE_STANDBY, STATE_NOMINAL, 0, 0};
    buttonList[1] = y;

    button z = {BUTTON_nLIGHT_1H, LIGHT1H_LED_PIN, MODE_LIGHT1H, STATE_NOMINAL, 0, 0};
    buttonList[2] = z;

    button a = {BUTTON_nRESET, RESET_LED_PIN, MODE_RESET, STATE_NOMINAL, 0, 0};
    buttonList[3] = a;

    button b = {BUTTON_nDOWN, DOWN_LED_PIN, MODE_UP, STATE_NOMINAL, 0, 0};
    buttonList[4] = b;

    button c = {BUTTON_nUP, UP_LED_PIN, MODE_DOWN, STATE_NOMINAL, 0, 0};
    buttonList[5] = c;


    /*
	 buttonList[5] =
	{
			{BUTTON_nAUTO, AUTO_LED_PIN, MODE_AUTO, STATE_NOMINAL, 0, 0},
			{BUTTON_nSTANDBY, STANDBY_LED_PIN, MODE_STANDBY, STATE_NOMINAL, 0, 0},
			{BUTTON_nLIGHT_1H, LIGHT1H_LED_PIN, MODE_LIGHT1H, STATE_NOMINAL, 0, 0},
			{BUTTON_nRESET, RESET_LED_PIN, MODE_RESET, STATE_NOMINAL, 0, 0},
			{BUTTON_nDOWN, DOWN_LED_PIN, MODE_UP, STATE_NOMINAL, 0, 0},
			{BUTTON_nUP, UP_LED_PIN, MODE_DOWN, STATE_NOMINAL, 0, 0}
	};
	*/
}

//--------------------------------------------------------------------
//
void button_stateMachine()
{
	int high;
	int i;

	//P4OUT &= ~BIT2;

	// if it seems we're not getting a 3 finger salute, dont enter diagnostic mode
	for (i = 5; i >= 0; i--)
	{
		high = 0;

		if ((P1IN & buttonList[i].pin) == 0)
			high = 1;

		switch (buttonList[i].state)
		{
			case STATE_NOMINAL:

				if (high == 1)
				{
					buttonList[i].Oncounts++;

					if (buttonList[i].Oncounts == 4)
						buttonList[i].state = STATE_PRESSED;
				}

				else
					buttonList[i].Oncounts = 0;

				break;

			case STATE_PRESSED:

				if (high == 0)
				{
					buttonList[i].Offcounts++;

					if (buttonList[i].Offcounts > 4)
						buttonList[i].state = STATE_RELEASED;
				}
				else
					buttonList[i].Oncounts++;

				break;

			case STATE_RELEASED:

				if (buttonList[i].mode == MODE_RESET && buttonList[i].Oncounts >= 170)
					_RESETTING_ = 1;

				if (high == 0)
				{
					buttonList[i].Offcounts++;

					if (buttonList[i].Offcounts > 8)
					{
						buttonList[i].Oncounts = 0;
						buttonList[i].Offcounts = 0;

						buttonList[i].state = STATE_NOMINAL;
					}
				}
				break;
		}
	}

	if ((buttonList[0].state == STATE_RELEASED && buttonList[0].Oncounts >= 50) &&
		(buttonList[1].state == STATE_RELEASED && buttonList[1].Oncounts >= 50) &&
		(buttonList[2].state == STATE_RELEASED && buttonList[2].Oncounts >= 50))
	{
		_DIAGNOSTIC_MODE = 1;
		_DIAGNOSTIC_MODE_TMR = 60;
	}

	if (_DIAGNOSTIC_MODE == 0)
	{
		for (i = 2; i >= 0; i--)
		{
			if (buttonList[i].state == STATE_RELEASED)
			    _UNIT_MODE = buttonList[i].mode;

		}
	}

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
		    if (_DIAGNOSTIC_PAGE > _DIAGNOSTIC_PAGES)
		    	_DIAGNOSTIC_PAGE = 0;
		}

		else if (buttonList[5].state != STATE_PRESSED && buttonList[4].state == STATE_PRESSED)
		{
			_DIAGNOSTIC_PAGE--;
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
			mast_stateMachine( 0 );
	}
}

//--------------------------------------------------------------------
//
void mast_stateMachine(int deltastate)
{
    if (deltastate == MAST_RISING)
    {
    	switch (_MAST_STATUS)
    	{
			case MAST_MAXDOWN: 		// raise mast passed the lower extent

				if (P2IN & IN_MAST_CUT_OUT != 0)
					P7OUT |= OUT_UP_MAST;
				else
					P7OUT &= ~OUT_UP_MAST;

				if (P8IN & IN_MAST_DOWN_EXTENT != 0)
					_MAST_STATUS = MAST_NOMINAL;

				break;

			case MAST_NOMINAL: 	// raise mast as normal, watch for upper threshold

				if (P8IN & IN_MAST_UP_EXTENT == 0)
				{
					_MAST_STATUS = MAST_MAXUP;
					P7OUT &= ~OUT_UP_MAST;
				}

				else if (P8IN & IN_MAST_UP_EXTENT != 0)
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
				break;

			case MAST_NOMINAL: 	// lower mast as normal, watch for lower threshold

				if (P8IN & IN_MAST_DOWN_EXTENT == 0)
				{
					_MAST_STATUS = MAST_MAXDOWN;
					P7OUT &= ~OUT_DOWN_MAST;
				}

				else if (P8IN & IN_MAST_DOWN_EXTENT != 0)
					P7OUT |= OUT_DOWN_MAST;

				break;

			case MAST_MAXUP: 		// No restrictions if the mast is coming down from max up

				P7OUT |= OUT_DOWN_MAST;

				if (P8IN & IN_MAST_UP_EXTENT != 0)
					_MAST_STATUS = MAST_NOMINAL;
				break;

    	}
    }

    if (deltastate == 0)
    {
		P7OUT &= ~OUT_DOWN_MAST;
		P7OUT &= ~OUT_UP_MAST;
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




