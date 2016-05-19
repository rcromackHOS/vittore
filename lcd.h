/*
 * lcd.h
 *
 *  Created on: May 17, 2016
 *      Author: rcromack
 */

#ifndef LCD_H_
#define LCD_H_

#include <msp430.h>

//--------------------------------------------------------------------

#define ROW_N  	 	4;                 // Number of display rows
#define COLUMN_N  	20;             // Number of display columns

#define OLED_RST	BIT4;
#define OLED_SCK	BIT5;
#define OLED_MOSI	BIT7;

//--------------------------------------------------------------------

void OLED_setup();
void OLED_clockCycle();
void OLED_command(int c);
void OLED_sendByte(int tx_b);
void OLED_data(int d);
void OLED_clearDisplay();

//--------------------------------------------------------------------

#endif /* LCD_H_ */

