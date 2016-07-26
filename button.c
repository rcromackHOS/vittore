/*
 * button.c
 *
 *  Created on: May 25, 2016
 *      Author: rcromack
 */


#include "button.h"
#include "config.h"
#include "Hardware.h"
#include "Common.h"
#include "mast.h"

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		build and populate the button arrays for the statemachine
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void buildButtonStateMachine()
{
    button x = {BUTTON_nAUTO, OUT_AUTO_LED, MODE_AUTO, STATE_NOMINAL, 0, 0};
    buttonList[0] = x;

    button y = {BUTTON_nSTANDBY, OUT_STANDBY_LED, MODE_STANDBY, STATE_NOMINAL, 0, 0};
    buttonList[1] = y;

    button z = {BUTTON_nLIGHT_1H, OUT_LIGHT_1H_LED, MODE_LIGHT1H, STATE_NOMINAL, 0, 0};
    buttonList[2] = z;

    button a = {BUTTON_nRESET, OUT_RESET_LED, MODE_RESET, STATE_NOMINAL, 0, 0};
    buttonList[3] = a;

    button b = {BUTTON_nDOWN, OUT_DOWN_LED, MODE_UP, STATE_NOMINAL, 0, 0};
    buttonList[4] = b;

    button c = {BUTTON_nUP, OUT_UP_LED, MODE_DOWN, STATE_NOMINAL, 0, 0};
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

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		button state machine
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
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
				{
					buttonList[i].Oncounts++;

					if (buttonList[i].mode == MODE_RESET)
						_RESETTING_ = 1;
				}

				break;

			case STATE_RELEASED:

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

	if ((buttonList[0].state == STATE_PRESSED && buttonList[0].Oncounts >= 50) &&
		(buttonList[1].state == STATE_PRESSED && buttonList[1].Oncounts >= 50) &&
		(buttonList[2].state == STATE_PRESSED && buttonList[2].Oncounts >= 50))
	{
		_DIAGNOSTIC_MODE = 1;
		_DIAGNOSTIC_MODE_TMR = 60;
	}

	if (_DIAGNOSTIC_MODE == 0)
	{
		for (i = 2; i >= 0; i--)
		{
			if (buttonList[i].state == STATE_RELEASED)
			{
			    if (_UNIT_MODE != buttonList[i].mode)
			    	_UNIT_MODE = buttonList[i].mode;

			    else
			    {
			    	_UNIT_MODE = buttonList[i].mode;
			    	_UPDATE_SCREEN_ = 1;
			    }
			}
		}
	}

}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Implimentation of reset functionality
// 					hold reset button 5 seconds, apply the reset
// 					then clear, reset flag a second after
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void handle_reset()
{
	if (_RESETTING_ == 1)
	{
		if (buttonList[3].Oncounts >= 200)
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

			   _UPDATE_SCREEN_ = 1;
			}
			_RESETTING_ = 0;
		}
	}
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		an alternative way to enter diagnostic mode. Button on the back of the PCB
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void handle_pressDiagButton()
{
	if (diagBackButton >= 200)
	{
		diagBackButton = 0;

		_DIAGNOSTIC_MODE = 1;
		_DIAGNOSTIC_MODE_TMR = 60;
	}
}





