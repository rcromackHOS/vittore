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

//--------------------------------------------------------------------

void updateDisplay();

void Norm_showCompanyName();
void Norm_showMode();
void Norm_showStateCode();

void DIAG_showDate();
void DIAG_showVoltage();
void DIAG_showRPM();
void DIAG_showBatteryTemp();
void DIAG_showInternalTemp();
void DIAG_showEngineHours();
void DIAG_showRPM();
void DIAG_showRuntime();
void DIAG_showSunRiseSet();
void DIAG_showLatLong();

void clearLine(int line);
void OLED_writeLine(char* chars, int line);

void InitializeDisplay();
void OLED_clockCycle();
void OLED_command(int c);
void OLED_sendByte(int tx_b);
void OLED_data(int d);
void OLED_clearDisplay();

//--------------------------------------------------------------------

#endif /* LCD_H_ */

