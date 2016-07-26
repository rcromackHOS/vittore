/*
 * lcd.c
 *
 *  Created on: May 17, 2016
 *      Author: rcromack
 */

#include "lcd.h"
#include "config.h"
#include "Hardware.h"
#include "Common.h"
#include "string.h"
#include "stdio.h"

//--------------------------------------------------------------------

const char modename0[] = "  Automatic Lights  ";
const char modename1[] = "      No Lights     ";
const char modename2[] = "Temporary Lights 1hr";
const char* const modeArray[] = {modename0, modename1, modename2};

const char msg1[] = {"   Contactor Open   "};
const char msg2[] = {"    Low Voltage     "};
const char msg3[] = {"   Cold Batteries   "};
const char msg4[] = {"Oil Change due soon "};
const char msg5[] = {"    Aquiring Time   "};
const char msg6[] = {"   RTC Malfunction  "};
const char msg7[] = {"Unexpected Exception"};
const char msg8[] = {"Voltage Reg. failure"};
const char msg9[] = {" Thermocouple Open  "};
const char msg10[] = {"Reaquiring Satellite"};
const char msg11[] = {"Aquiring Satellites "};
const char msg12[] = {"Engine Failed Start "};
const char msg13[] = {"  Engine Overspeed  "};
const char msg14[] = {"  Engine Underspeed "};
const char msg15[] = {"Engine Overheat Fail"};
const char msg16[] = {"Engine Low oil pres."};
const char msg17[] = {" Hardware Exception "};
const char msg18[] = {"BMS FAIL, CALL TECH."};
const char msg19[] = {"   Engine Starting  "};
const char msg20[] = {"    Bank Charging   "};
const char msg21[] = {"Sunrise:    "};
const char msg22[] = {"Sunset:     "};
const char msg23[] = {"Lat:         "};
const char msg24[] = {"Lng:         "};
const char emptyLine_char[] = {"                    "};
const char empty_char[] = {""};
const char slash_char[] = {"/"};
const char space_char[] = {" "};
const char zero_char[] = {"0"};
const char colon_char[] = {":"};
const char VOLT_char[] = {" V"};
const char tcOPEN_string[] =  {" OPEN"};
const char bt_tmp_string[] =  {"Battery Temp:  "};
const char int_tmp_string[] = {"Internal Temp: "};

const char enghrs_string[] = {"Engine Hours: "};
const char oilchange_string[] = {"Oil Change Due:  "};
const char RPM_string[] =     {"Engine RPMs:    "};
const char runtime_string[] = {"Engine Runtime: "};

const char li0[] = "CLEANETEK INDUSTRIES";
const char li1[] = "        INC         ";
//const char* const cmpyName[] = {li0, li1};

//--------------------------------------------------------------------

int OLED_new_line[4] = {0x80, 0xA0, 0xC0, 0xE0};
int OLED_rows = 0x08;

char __screenbuffer[20];
char __screenbuffer2[20];

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Statemachine for the screen messages. flip screen every 3 seconds in normal mode
//					update screen every second in diagnostic mode
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void updateDisplay()
{
	if (_SCREEN_UPDATE_D == 0 || _UPDATE_SCREEN_ == 1)
	{
		static int toggle_screen_msg = 0;

		if (_SCREEN_UPDATE_D == 0)
		{
			if (_DIAGNOSTIC_MODE != 1)
			{
				_SCREEN_UPDATE_D = 3;
				toggle_screen_msg = ~toggle_screen_msg;
			}
			else
				_SCREEN_UPDATE_D = 1;
		}


		if (_DIAGNOSTIC_MODE_TMR == 0)
			_DIAGNOSTIC_MODE = 0;

		if (_DIAGNOSTIC_MODE == 1)
		{
			if (_DIAGNOSTIC_PAGE == 0)
			{
				DIAG_showDate();
				DIAG_showVoltage();
				DIAG_showBatteryTemp();
				DIAG_showInternalTemp();
			}
			else if (_DIAGNOSTIC_PAGE == 1)
			{
				DIAG_showEngineHours();
				DIAG_showRPM();
				DIAG_showRuntime();
			}
			else if (_DIAGNOSTIC_PAGE == 2)
			{
				DIAG_showSunRiseSet();
				DIAG_showLatLong();
			}
			else if (_DIAGNOSTIC_PAGE == 3)
			{
				OLED_clearDisplay();
			}
		}
		else
		{
			Norm_showCompanyName();
			clearLine(2);

			if (toggle_screen_msg == 0 || _STATE_CODE == 0)
				Norm_showMode();

			if (toggle_screen_msg != 0 && _STATE_CODE > 0)
				Norm_showStateCode();

		}

		_UPDATE_SCREEN_ = 0;
	}

}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		show the company name
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void Norm_showCompanyName()
{
	strcpy(__screenbuffer, li0);
	OLED_writeLine(__screenbuffer, 0);

	strcpy(__screenbuffer, li1);
	OLED_writeLine(__screenbuffer, 1);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Show the unit mode
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void Norm_showMode()
{
	strcpy(__screenbuffer, modeArray[_UNIT_MODE]);
	OLED_writeLine(__screenbuffer, 3);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Show the message relative to every state code
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void Norm_showStateCode()
{
	switch (_STATE_CODE)
	{
		case 1:
			strcpy(__screenbuffer, msg1);
			break;

		case 2:
			strcpy(__screenbuffer, msg2);
			break;

		case 3:
			strcpy(__screenbuffer, msg3);
			break;

		case 4:
			strcpy(__screenbuffer, msg4);
			break;

		case 10:
			strcpy(__screenbuffer, msg5);
			break;

		case 11:
			strcpy(__screenbuffer, msg6);
			break;

		case 12:
			strcpy(__screenbuffer, msg11);
			break;

		case 14:
			strcpy(__screenbuffer, msg9);
			break;

			/*
		case 0:
			strcpy(__screenbuffer, msg7);
			break;

		case 1:
			strcpy(__screenbuffer, msg8);
			break;


		case 3:
			strcpy(__screenbuffer, msg10);
			break;

		*/
		case 28:
			strcpy(__screenbuffer, msg19);
			break;

		case 29:
			strcpy(__screenbuffer, msg20);
			break;

		case 30:
			strcpy(__screenbuffer, msg12);
			break;

		case 31:
			strcpy(__screenbuffer, msg13);
			break;

		case 32:
			strcpy(__screenbuffer, msg14);
			break;

		case 33:
			strcpy(__screenbuffer, msg15);
			break;

		case 34:
			strcpy(__screenbuffer, msg16);
			break;

		case 98:
			strcpy(__screenbuffer, msg17);
			break;

		case 99:
			strcpy(__screenbuffer, msg18);
			break;

	}

	OLED_writeLine(__screenbuffer, 3);
}

/*
const char msg7[] = {" Driver Malfunction "};
const char msg8[] = {"Voltage Reg. failure"};
const char msg9[] = {" Battery TC failure "};
const char msg10[] = {"Reaquiring Satellite"};
const char msg11[] = {"Aquiring Satellites "};

*/

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Convert the date and time to the buffer for printing
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void DIAG_showDate()
{
	strcpy(__screenbuffer, "");

	// date
	itoa(now.year, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, slash_char); 	//-

	if (now.month < 10)
		strcat(__screenbuffer, zero_char);
	itoa(now.month, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, slash_char); 	//-

	if (now.day < 10)
		strcat(__screenbuffer, zero_char);
	itoa(now.day, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	// space
	int i;
	for (i = 0; i < 2; i++)
		strcat(__screenbuffer, space_char);

	// time
	if (now.second < 10)
		strcat(__screenbuffer, zero_char);
	itoa(now.second, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, colon_char); 	//-

	if (now.minute < 10)
		strcat(__screenbuffer, zero_char);
	itoa(now.minute, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, colon_char); 	//-

	if (now.hour < 10)
		strcat(__screenbuffer, zero_char);
	itoa(now.hour, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	OLED_writeLine(__screenbuffer, 0);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Convert the 24v and 12v to the buffer for printing
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void DIAG_showVoltage()
{
	strcpy(__screenbuffer, empty_char);
	strcpy(__screenbuffer2, empty_char);
	int i = 0;

	// date
	if (VALUE_12V < 10)
		strcat(__screenbuffer, zero_char);

	//float d12 = (double)VALUE_12V / 100;
	sprintf(__screenbuffer2, "%f", VALUE_12V);
	__screenbuffer2[5] = '\0';
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, VOLT_char); 	// V

	for (i = 0; i < 3; i++)
		strcat(__screenbuffer, space_char);
	strcat(__screenbuffer, slash_char); 	// -

	for (i = 0; i < 2; i++)
		strcat(__screenbuffer, space_char);

	//d12 = (double)VALUE_24V / 100;
	sprintf(__screenbuffer2, "%f", VALUE_24V);

	if (VALUE_24V < 10)
	{
		strcat(__screenbuffer, zero_char);
		__screenbuffer2[4] = '\0';
	}
	else
		__screenbuffer2[5] = '\0';

	//strcat(__screenbuffer2, VOLT_char);

	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, VOLT_char); 	// V

	OLED_writeLine(__screenbuffer, 1);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Convert the battery temp to the buffer for printing
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void DIAG_showBatteryTemp()
{
	strcpy(__screenbuffer, bt_tmp_string);
	strcpy(__screenbuffer2, "");

	if (VALUE_BAT_TEMP == -300)
		strcpy(__screenbuffer2, tcOPEN_string);

	else
	{
		//float dtemp = (double)VALUE_BAT_TEMP / 100;
		sprintf(__screenbuffer2, "%f", VALUE_BAT_TEMP);
	}

	strcat(__screenbuffer, __screenbuffer2);

	OLED_writeLine(__screenbuffer, 2);
}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Convert the internal temp to the buffer for printing
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void DIAG_showInternalTemp()
{
	strcpy(__screenbuffer, int_tmp_string);

	//float dtemp = (double)VALUE_INTERNAL_TEMP / 100;
	sprintf(__screenbuffer2, "%f", VALUE_INTERNAL_TEMP);

	strcat(__screenbuffer, __screenbuffer2);

	OLED_writeLine(__screenbuffer, 3);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Convert the RPMs to the buffer for printing
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void DIAG_showRPM()
{
	strcpy(__screenbuffer, RPM_string);

	if (VALUE_RPM < 1000)
		strcat(__screenbuffer, space_char);
	if (VALUE_RPM < 100)
		strcat(__screenbuffer, space_char);
	if (VALUE_RPM < 10)
		strcat(__screenbuffer, space_char);

	itoa(VALUE_RPM, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	OLED_writeLine(__screenbuffer, 2);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Convert the engine hours to the buffer for printing
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void DIAG_showEngineHours()
{
	strcpy(__screenbuffer, enghrs_string);

	if (engine.engineHours < 1000)
		strcat(__screenbuffer, space_char);
	if (engine.engineHours < 100)
		strcat(__screenbuffer, space_char);
//	if (engine.engineHours < 10)
//		strcat(__screenbuffer, space_char);

	itoa(engine.engineHours, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, colon_char);

	if (engine.engineMins < 10)
		strcat(__screenbuffer, zero_char);

	strcpy(__screenbuffer2, "");
	itoa(engine.engineMins, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	OLED_writeLine(__screenbuffer, 0);

	strcpy(__screenbuffer, oilchange_string);

	itoa(_OILCHANGE_DUE, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	OLED_writeLine(__screenbuffer, 1);

}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Convert the runtime to the buffer for printing
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void DIAG_showRuntime()
{
	strcpy(__screenbuffer, runtime_string);

	int fuck;
	fuck = (int)engine.runTime / 60;

	if (fuck < 1000)
		strcat(__screenbuffer, space_char);
	if (fuck < 100)
		strcat(__screenbuffer, space_char);
	if (fuck < 10)
		strcat(__screenbuffer, space_char);

	itoa(fuck, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	OLED_writeLine(__screenbuffer, 3);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Convert the sunrise and sunset to the buffer for printing
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void DIAG_showSunRiseSet()
{
	timeStruct ss = sunSet;
	timeStruct sr = sunRise;

	strcpy(__screenbuffer, msg22);

	if (ss.hour < 10)
		strcat(__screenbuffer, zero_char);
	itoa(ss.hour, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, colon_char); 	//-

	if (ss.minute < 10)
		strcat(__screenbuffer, zero_char);
	itoa(ss.minute, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, colon_char); 	//-

	strcat(__screenbuffer, zero_char);
	strcat(__screenbuffer, zero_char);

	OLED_writeLine(__screenbuffer, 1);

	//

	strcpy(__screenbuffer, msg21);

	if (sr.hour < 10)
		strcat(__screenbuffer, zero_char);
	itoa(sr.hour, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, colon_char); 	//-

	if (sr.minute < 10)
		strcat(__screenbuffer, zero_char);
	itoa(sr.minute, __screenbuffer2, 10);
	strcat(__screenbuffer, __screenbuffer2);

	strcat(__screenbuffer, colon_char); 	//-

	strcat(__screenbuffer, zero_char);
	strcat(__screenbuffer, zero_char);

	OLED_writeLine(__screenbuffer, 0);

}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Convert the globals lat and long to the buffer for printing
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void DIAG_showLatLong()
{
	strcpy(__screenbuffer, msg23);

	float l = lat;
	sprintf(__screenbuffer2, "%f", l);

	strcat(__screenbuffer, __screenbuffer2);

	OLED_writeLine(__screenbuffer, 2);

	//

	strcpy(__screenbuffer, msg24);

	l = lng;
	sprintf(__screenbuffer2, "%f", l);

	strcat(__screenbuffer, __screenbuffer2);

	OLED_writeLine(__screenbuffer, 3);
}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Print the buffer 'char' on line, 'line'
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void OLED_writeLine(char* chars, int line)
{
	OLED_command(OLED_new_line[line]);

	unsigned int i;
	for (i = 0; i < 20; i++)
	{
		OLED_data(chars[i]);
	}
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Clear out the line in the argument
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void clearLine(int line)
{
	strcpy(__screenbuffer, emptyLine_char);
	OLED_writeLine(__screenbuffer, line);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Simulates a clock pulse
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void OLED_clockCycle()
{
   int i = 0;
   for(i = 0; i < 5; i++)
   {
	 if (i == 1)
	   P5OUT &= ~LCD_SPI_SCK;
	 if (i == 3)
	   P5OUT |= LCD_SPI_SCK;
   }
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		PREPAOLED_Reset THE TRANSMISSION OF A COMMAND
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void OLED_command(int c)
{
   int i = 0;

   P3OUT |= LCD_SPI_SIMO;

   for(i = 0; i<5; i++)
      OLED_clockCycle();

   P3OUT &= ~LCD_SPI_SIMO;

   for(i = 3; i > 0; i--)
      OLED_clockCycle();

   // Transmits the byte
   OLED_sendByte(c);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		PREPAOLED_Reset THE TRANSMISSION OF A BYTE OF DATA
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void OLED_data(int d)
{
   int i = 0;

   P3OUT |= LCD_SPI_SIMO;
   for(i = 0; i<5; i++)
      OLED_clockCycle();

   P3OUT &= ~LCD_SPI_SIMO;
   OLED_clockCycle();

   P3OUT |= LCD_SPI_SIMO;
   OLED_clockCycle();

   P3OUT &= ~LCD_SPI_SIMO;
   OLED_clockCycle();

   OLED_sendByte(d);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		SEND TO THE DISPLAY THE BYTE IN tx_b
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void OLED_sendByte(int tx_b)
{
   int i = 0;

   for(i=4; i>0; i--)
   {
      if((tx_b & 0x1) == 1)
      {
    	  P3OUT |= LCD_SPI_SIMO;
      }
      else
      {
    	  P3OUT &= ~LCD_SPI_SIMO;
      }
      OLED_clockCycle();
      tx_b = tx_b >> 1;
   }

   for(i=4; i>0; i--)
   {
	   P3OUT &= ~LCD_SPI_SIMO;
	   OLED_clockCycle();
   }

   for(i=4; i>0; i--)
   {
      if((tx_b & 0x1) == 0x1)             // <------- Change
      {
    	  P3OUT |= LCD_SPI_SIMO;
      }
      else
      {
    	  P3OUT &= ~LCD_SPI_SIMO;
      }
      OLED_clockCycle();
      tx_b = tx_b >> 1;
   }

   for(i=4; i>0; i--)
   {
	   P3OUT &= ~LCD_SPI_SIMO;
	   OLED_clockCycle();
   }
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Clears display (and cursor home)
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void OLED_clearDisplay()
{
	OLED_command(0x01);
	int i;
	for(i=200; i>0; i--)
	   {}
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		INITIAL SETUP, set up the ports, and configure the display
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void InitializeDisplay()
{
   // Sets HIGH the LCD_SPI_SCK line of the display
   P5OUT |= LCD_SPI_SCK;

   // Sets LOW the LCD_SPI_SIMO line of the display
   P3OUT &= ~LCD_SPI_SIMO;

   //Sets HIGH the OLED_Reset line of the display (optional, can be always high)
   P5OUT |= LCD_SPI_RESET;
/*
    P3SEL = LCD_SPI_SIMO;
	P5SEL = LCD_SPI_SCK;

	UCB1CTL1 = UCSWRST;
	UCB1CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
	UCB1CTL1 |= UCSSEL__SMCLK; // SMCLK
	UCB1BR0 |= 0x02; // /2
    UCB1BR1 = 0; //
    UCB1ICTL = 0; // No modulation
	UCB1CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

	while (!(UCTXIFG & UCB1IFG)); // USCI_A0 TX buffer ready?
    UCB1TXBUF = 0xAA; // Send 0xAA over SPI to Slave
	//while (!(IFG2 & UCB1RXIFG)); // USCI_A0 RX Received?
	//received_ch = UCB1RXBUF; // Store received data
*/

   int i;
   for(i=200; i>0; i--)
   {}

   OLED_command(0x2A); // Function set: extended command set (RE=1), lines #
   OLED_command(0x71);        // Function selection A:
   //data(0x5C);           //  enable internal Vdd regulator at 5V I/O mode (def. value) (0x00 for disable, 2.8V I/O)
   OLED_data(0x00);
   OLED_command(0x28); // Function set: fundamental command set (RE=0) (exit from extended command set), lines #
   OLED_command(0x08);        // Display ON/OFF control: display off, cursor off, blink off (default values)
   OLED_command(0x2A); // Function set: extended command set (RE=1), lines #
   OLED_command(0x79);        // OLED characterization: OLED command set enabled (SD=1)
   OLED_command(0xD5);        // Set display clock divide ratio/oscillator frequency:
   OLED_command(0x70);        //  divide ratio=1, frequency=7 (default values)
   OLED_command(0x78);        // OLED characterization: OLED command set disabled (SD=0) (exit from OLED command set)

   OLED_command(0x09);  // Extended function set (RE=1): 5-dot font, B/W inverting disabled (def. val.), 3/4 lines

   OLED_command(0x06);        // Entry Mode set - COM/SEG direction: COM0->COM31, SEG99->SEG0 (BDC=1, BDS=0)
   OLED_command(0x72);        // Function selection B:
   OLED_data(0x0A);           //  ROM/CGRAM selection: ROM C, CGROM=250, CGRAM=6 (ROM=10, OPR=10)
   OLED_command(0x79);        // OLED characterization: OLED command set enabled (SD=1)
   OLED_command(0xDA);        // Set SEG pins hardware configuration:
   OLED_command(0x10);        //  alternative odd/even SEG pin, disable SEG left/right remap (default values)
   OLED_command(0xDC);        // Function selection C:
   OLED_command(0x00);        //  internal VSL, GPIO input disable
   OLED_command(0x81);        // Set contrast control:
   OLED_command(0x7F);        //  contrast=127 (default value)
   OLED_command(0xD9);        // Set phase length:
   OLED_command(0xF1);        //  phase2=15, phase1=1 (default: 0x78)
   OLED_command(0xDB);        // Set VCOMH deselect level:
   OLED_command(0x40);        //  VCOMH deselect level=1 x Vcc (default: 0x20=0,77 x Vcc)
   OLED_command(0x78);        // OLED characterization: OLED command set disabled (SD=0) (exit from OLED command set)
   OLED_command(0x28); // Function set: fundamental command set (RE=0) (exit from extended command set), lines #

   OLED_clearDisplay();

   OLED_command(0x80);        // Set DDRAM addOLED_Resets 0x00 in addOLED_Resets counter (cursor home) (default value)
   OLED_command(0x0C);        // Display ON/OFF control: display ON, cursor off, blink off

   // Waits 250 ms for stabilization purpose after display on
   for(i=2000; i>0; i--)
   {}

   OLED_clearDisplay();

}




