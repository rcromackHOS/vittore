/*
 * button.c
 *
 *  Created on: May 25, 2016
 *      Author: rcromack
 */


#include "button.h"
#include "config.h"

//--------------------------------------------------------------------

void buildButtonStateMachine()
{
    button x = {AUTO_PB_PIN, AUTO_LED_PIN, MODE_AUTO, STATE_NOMINAL, 0, 0};
    buttonList[0] = x;

    button y = {STANDBY_PB_PIN, STANDBY_LED_PIN, MODE_STANDBY, STATE_NOMINAL, 0, 0};
    buttonList[1] = y;

    button z = {LIGHT1H_PB_PIN, LIGHT1H_LED_PIN, MODE_LIGHT1H, STATE_NOMINAL, 0, 0};
    buttonList[2] = z;

    button a = {RESET_PB_PIN, RESET_LED_PIN, MODE_RESET, STATE_NOMINAL, 0, 0};
    buttonList[3] = a;

    button b = {DOWN_PB_PIN, DOWN_LED_PIN, MODE_UP, STATE_NOMINAL, 0, 0};
    buttonList[4] = b;

    button c = {UP_PB_PIN, UP_LED_PIN, MODE_DOWN, STATE_NOMINAL, 0, 0};
    buttonList[5] = c;


    /*
	 buttonList[5] =
	{
			{AUTO_PB_PIN, AUTO_LED_PIN, MODE_AUTO, STATE_NOMINAL, 0, 0},
			{STANDBY_PB_PIN, STANDBY_LED_PIN, MODE_STANDBY, STATE_NOMINAL, 0, 0},
			{LIGHT1H_PB_PIN, LIGHT1H_LED_PIN, MODE_LIGHT1H, STATE_NOMINAL, 0, 0},
			{RESET_PB_PIN, RESET_LED_PIN, MODE_RESET, STATE_NOMINAL, 0, 0},
			{DOWN_PB_PIN, DOWN_LED_PIN, MODE_UP, STATE_NOMINAL, 0, 0},
			{UP_PB_PIN, UP_LED_PIN, MODE_DOWN, STATE_NOMINAL, 0, 0}
	};
	*/
}

//--------------------------------------------------------------------
//
void button_stateMachine()
{
	int high;
	int i;

	P4OUT &= ~BIT2;

	// if it seems we're not getting a 3 finger salute, dont enter diagnostic mode
	for (i = 5; i >= 0; i--)
	{
		high = P1IN & buttonList[i].pin;

		switch (buttonList[i].state)
		{
			case STATE_NOMINAL:

				if (high != 0)
				{
					buttonList[i].Oncounts++;

					if (buttonList[i].Oncounts > 2)
						buttonList[i].state = STATE_PRESSED;
				}

				else
					buttonList[i].Oncounts = 0;

				break;

			case STATE_PRESSED:

				if (high == 0)
				{
					buttonList[i].Offcounts++;

					if (buttonList[i].Offcounts >= 2)
						buttonList[i].state = STATE_RELEASED;
				}
				else
					buttonList[i].Oncounts++;

				break;

			case STATE_RELEASED:

				if (buttonList[i].mode == MODE_RESET && buttonList[i].Oncounts >= 120)
					_RESETTING_ = 1;

				if (high == 0)
				{
					buttonList[i].Offcounts++;

					if (buttonList[i].Offcounts >= 4)
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

    if (deltastate == 0)
    {
		P7OUT &= ~DOWN_MAST_PIN;
		P7OUT &= ~UP_MAST_PIN;
    }
}






