/*
 * button.c
 *
 *  Created on: May 25, 2016
 *      Author: rcromack
 */


#include "button.h"

//--------------------------------------------------------------------

void buildButtonStateMachine()
{
	button buttonList[6] =
	{
			{AUTO_PB_PIN, AUTO_LED_PIN, MODE_AUTO, STATE_NOMINAL, 0},
			{STANDBY_PB_PIN, STANDBY_LED_PIN, MODE_STANDBY, STATE_NOMINAL, 0},
			{LIGHT1H_PB_PIN, LIGHT1H_LED_PIN, MODE_LIGHT1H, STATE_NOMINAL, 0},
			{RESET_PB_PIN, RESET_LED_PIN, MODE_RESET, STATE_NOMINAL, 0},
			{DOWN_PB_PIN, DOWN_LED_PIN, MODE_UP, STATE_NOMINAL, 0},
			{UP_PB_PIN, UP_LED_PIN, MODE_DOWN, STATE_NOMINAL, 0}
	};
}



//--------------------------------------------------------------------
//
void button_stateMachine()
{
	int high;
	int i;

	int nom_buts;
	for (i = 3; i >= 0; i--)
	{
		if (buttonList[i].state == STATE_PRESSED)
			nom_buts++;
	}

	if (nom_buts == 3)
	{
		_DIAGNOSTIC_MODE = 1;
		_DIAGNOSTIC_MODE_TMR = 60;

		nom_buts = 0;
	}

	// if it seems we're not getting a 3 finger salute, dont enter diagnostic mode
	for (i = 5; i >= 0; i--)
	{
		high = P1IN & buttonList[i].pin;

		switch (buttonList[i].state)
		{
			case STATE_NOMINAL:

				if (high == 1)
				{
					buttonList[i].counts++;
					if (buttonList[i].counts > 4)
						buttonList[i].state = STATE_PRESSED;
				}
				else
					buttonList[i].counts = 0;

				break;

			case STATE_PRESSED:

				if (buttonList[i].mode < MODE_RESET && _DIAGNOSTIC_MODE == 0)
				{
					_UNIT_MODE = buttonList[i].mode;
					P4OUT |= buttonList[i].LEDpin;
				}

				else if (buttonList[i].mode == MODE_RESET)
					_RESETTING_ = 1;

				else if (buttonList[i].mode > MODE_RESET)
					mastUpDown();


				if (high == 0)
				{
					buttonList[i].counts--;

					if (buttonList[i].counts == 0)
						buttonList[i].state = STATE_NOMINAL;
				}

				break;

		}
	}
}

//--------------------------------------------------------------------
//
void mastUpDown()
{
	if (_DIAGNOSTIC_MODE == 1)
	{
		if (buttonList[5].state == STATE_PRESSED && buttonList[4].state != STATE_PRESSED)
			diagnostic_PageUp();

		else if (buttonList[5].state != STATE_PRESSED && buttonList[4].state == STATE_PRESSED)
			diagnostic_PageDown();
	}
	else
	{
		if (buttonList[5].state == STATE_PRESSED && buttonList[4].state != STATE_PRESSED)
			mast_stateMachine( MAST_RISING );

		else if (buttonList[5].state != STATE_PRESSED && buttonList[4].state == STATE_PRESSED)
			mast_stateMachine( MAST_LOWERING );
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

				if (P2IN & CUTOUT_MAST_PIN == 1)
					P7OUT |= UP_MAST_PIN;
				else
					P7OUT &= ~UP_MAST_PIN;

				if (P8IN & DOWN_EXTENT_PIN == 1)
					_MAST_STATUS = MAST_NOMINAL;
				break;

			case MAST_NOMINAL: 	// raise mast as normal, watch for upper threshold

				if (P8IN & UP_EXTENT_PIN == 0)
				{
					_MAST_STATUS = MAST_MAXUP;
					P7OUT &= ~UP_MAST_PIN;
				}
				else if (P8IN & UP_EXTENT_PIN == 1)
					P7OUT |= UP_MAST_PIN;
				break;

			case MAST_MAXUP: 		// don't let the mast keep rising if we're maxed

				P7OUT &= ~UP_MAST_PIN;
				break;
    	}
    }

    if (deltastate == MAST_LOWERING)
    {
    	switch (_MAST_STATUS)
    	{
			case MAST_MAXDOWN: 		// raise mast passed the lower extent

				P7OUT &= ~DOWN_MAST_PIN;
				break;

			case MAST_NOMINAL: 	// lower mast as normal, watch for lower threshold

				if (P8IN & DOWN_EXTENT_PIN == 0)
				{
					_MAST_STATUS = MAST_MAXDOWN;
					P7OUT &= ~DOWN_MAST_PIN;
				}
				else if (P8IN & DOWN_EXTENT_PIN == 1)
					P7OUT |= DOWN_MAST_PIN;
				break;

			case MAST_MAXUP: 		// No restrictions if the mast is coming down from max up

				P7OUT |= DOWN_MAST_PIN;

				if (P8IN & UP_EXTENT_PIN == 1)
					_MAST_STATUS = MAST_NOMINAL;
				break;

    	}
    }
}







