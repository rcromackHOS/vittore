/*
 * button.h
 *
 *  Created on: May 25, 2016
 *      Author: rcromack
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <msp430.h>
#include "config.h"

//--------------------------------------------------------------------
//_UNIT_MODE
//--------------------------------------------------------------------
// state codes
#define STATE_NOMINAL     0x0
#define STATE_PRESSING    0x1
#define STATE_PRESSED     0x2
#define STATE_RELEASING	  0x3

#define MODE_AUTO  	  	  0x0
#define MODE_LIGHT1H  	  0x1
#define MODE_STANDBY  	  0x2
#define MODE_RESET	  	  0x3
#define MODE_UP       	  0x4
#define MODE_DOWN     	  0x5

#define MAST_MAXDOWN  	  0x0
#define MAST_RISING  	  0x1
#define MAST_NOMINAL  	  0x2
#define MAST_LOWERING  	  0x3
#define MAST_MAXUP  	  0x4

// Button/Mast outputs
#define RESET_LED_PIN     BIT0
#define LIGHT1H_LED_PIN   BIT1
#define AUTO_LED_PIN      BIT2
#define STANDBY_LED_PIN   BIT3
#define DOWN_LED_PIN      BIT4
#define UP_LED_PIN        BIT5

#define UP_MAST_PIN       BIT3
#define DOWN_MAST_PIN     BIT2

// Button/Mast inputs
#define RESET_PB_PIN      BIT0
#define LIGHT1H_PB_PIN    BIT3
#define AUTO_PB_PIN       BIT4
#define STANDBY_PB_PIN    BIT5
#define DOWN_PB_PIN       BIT6
#define UP_PB_PIN         BIT7

#define UP_MAST_PIN       BIT3
#define DOWN_MAST_PIN     BIT2
#define CUTOUT_MAST_PIN   BIT2
#define DOWN_EXTENT_PIN   BIT6
#define UP_EXTENT_PIN     BIT7


//--------------------------------------------------------------------
typedef struct
{
	int pin;
	int LEDpin;

	int mode;
	int state;
	int counts;

} button;


struct button buttonList[6]
						 {
{AUTO_PB_PIN, AUTO_LED_PIN, MODE_AUTO, STATE_NOMINAL, 0};
{STANDBY_PB_PIN, STANDBY_LED_PIN, MODE_LIGHT1H, STATE_NOMINAL, 0};
{LIGHT1H_PB_PIN, LIGHT1H_LED_PIN, MODE_STANDBY, STATE_NOMINAL, 0};
{RESET_PB_PIN, RESET_LED_PIN, MODE_RESET, STATE_NOMINAL, 0};
{DOWN_PB_PIN, DOWN_LED_PIN, MODE_UP, STATE_NOMINAL, 0};
{UP_PB_PIN, UP_LED_PIN, MODE_DOWN, STATE_NOMINAL, 0};
/*
buttonList[0] = {AUTO_PB_PIN, AUTO_LED_PIN, MODE_AUTO, STATE_NOMINAL, 0};
buttonList[1] = {STANDBY_PB_PIN, STANDBY_LED_PIN, MODE_LIGHT1H, STATE_NOMINAL, 0};
buttonList[2] = {LIGHT1H_PB_PIN, LIGHT1H_LED_PIN, MODE_STANDBY, STATE_NOMINAL, 0};
buttonList[3] = {RESET_PB_PIN, RESET_LED_PIN, MODE_RESET, STATE_NOMINAL, 0};
buttonList[4] = {DOWN_PB_PIN, DOWN_LED_PIN, MODE_UP, STATE_NOMINAL, 0};
buttonList[5] = {UP_PB_PIN, UP_LED_PIN, MODE_DOWN, STATE_NOMINAL, 0};
*/						 };

//--------------------------------------------------------------------

#endif /* BUTTON_H_ */




