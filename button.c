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
	// PIN, LEDPIN, MODE, STATE, COUNT

}

//--------------------------------------------------------------------
//
void button_stateMachine(int state)
{
	int high;
	int i;

	for (i = 0; i < 6; i++)
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

				if (buttonList[i].mode < MODE_RESET)
					_UNIT_MODE = buttonList[i].mode;

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

	if (buttonList[5].state == STATE_PRESSED && buttonList[4].state != STATE_PRESSED)
		mast_stateMachine( MAST_RISING );

	else if (buttonList[5].state != STATE_PRESSED && buttonList[4].state == STATE_PRESSED)
		mast_stateMachine( MAST_LOWERING );

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







