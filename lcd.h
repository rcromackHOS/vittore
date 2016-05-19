/*
 * lcd.h
 *
 *  Created on: May 17, 2016
 *      Author: rcromack
 */

#ifndef LCD_H_
#define LCD_H_

#define ROW_N  	 	4;                 // Number of display rows
#define COLUMN_N  	20;             // Number of display columns

#define OLED_RST	BIT4;
#define OLED_SCK	BIT5;
#define OLED_MOSI	BIT7;

//--------------------------------------------------------------------

void OLED_clockCycle();
//void clearDisplay();
//void setup_display();

#endif /* LCD_H_ */
