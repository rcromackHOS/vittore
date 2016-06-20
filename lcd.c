/*
 * lcd.c
 *
 *  Created on: May 17, 2016
 *      Author: rcromack
 */

#include "lcd.h"
#include "config.h"
#include "Hardware.h"

//#include "strcpy.h"
#include "string.h"

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
const char msg25[] = {"                    "};

const char* const messages[] = {
msg1,
msg2,
msg3,
msg4,
msg5,
msg6,
msg7,
msg8,
msg9,
msg10,
msg11,
msg12,
msg13,
msg14,
msg15,
msg16,
msg17,
msg18,
msg19,
msg20,
msg21,
msg22,
msg23,
msg24
};

const char li0[] = "  HORIZON OILFIELD  ";
const char li1[] = "     SOLUTIONS      ";
//const char* const cmpyName[] = {li0, li1};

//--------------------------------------------------------------------

int OLED_new_line[4] = {0x80, 0xA0, 0xC0, 0xE0};
int OLED_rows = 0x08;

char __screenbuffer[20];
char __screenbuffer2[20];

//--------------------------------------------------------------------
//
void updateDisplay()
{
	static int toggle_screen_msg = 0;

	if (_DIAGNOSTIC_MODE_TMR == 0)
		_DIAGNOSTIC_MODE = 0;

	if (_DIAGNOSTIC_MODE == 1)
	{
		if (_DIAGNOSTIC_PAGE == 0)
		{
			DIAG_showDate();
			DIAG_showVoltage();
			DIAG_showRPM();
		}
		else if (_DIAGNOSTIC_PAGE == 1)
		{

		}
		else if (_DIAGNOSTIC_PAGE == 2)
		{

		}
		else if (_DIAGNOSTIC_PAGE == 3)
		{

		}
	}
	else
	{
		Norm_showCompanyName();

		toggle_screen_msg = ~toggle_screen_msg;

		if (toggle_screen_msg == 0)
			Norm_showMode();

		if (toggle_screen_msg != 0 && _STATE_CODE > 0)
			Norm_showStateCode();


	}

}

//--------------------------------------------------------------------
//
void Norm_showCompanyName()
{
	strcpy(__screenbuffer, li0);
	OLED_writeLine(__screenbuffer, 0);

	strcpy(__screenbuffer, li1);
	OLED_writeLine(__screenbuffer, 1);
}

//--------------------------------------------------------------------
//
void Norm_showMode()
{
	strcpy(__screenbuffer, modeArray[_UNIT_MODE]);
	OLED_writeLine(__screenbuffer, 3);
}

//--------------------------------------------------------------------
//
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

			/*
		case 0:
			strcpy(__screenbuffer, msg7);

		case 1:
			strcpy(__screenbuffer, msg8);

		case 2:
			strcpy(__screenbuffer, msg9);

		case 3:
			strcpy(__screenbuffer, msg10);

		case 10:
			strcpy(__screenbuffer, msg11);
		*/
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

		case 40:
			strcpy(__screenbuffer, msg19);
			break;

		case 41:
			strcpy(__screenbuffer, msg20);
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

//====================================================================
//
//====================================================================
//
void DIAG_showDate()
{


}

//--------------------------------------------------------------------
//
void DIAG_showVoltage()
{



}

//--------------------------------------------------------------------
//
void DIAG_showRPM()
{



}

//--------------------------------------------------------------------
//
//void DIAG_show12v()
//{



//}

//====================================================================
//
void OLED_writeLine(char* chars, int line)
{
	OLED_command(OLED_new_line[line]);

	unsigned int i;
	for (i = 0; i < 20; i++)
	{
		OLED_data(chars[i]);
	}
}

//====================================================================
//
void clearLine(int line)
{
	strcpy(__screenbuffer, msg25);
	OLED_writeLine(__screenbuffer, line);
}

//====================================================================
// Simulates a clock pulse
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

//--------------------------------------------------------------------
//  PREPAOLED_Reset THE TRANSMISSION OF A COMMAND
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

//--------------------------------------------------------------------
//  PREPAOLED_Reset THE TRANSMISSION OF A BYTE OF DATA
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

//--------------------------------------------------------------------
//  SEND TO THE DISPLAY THE BYTE IN tx_b
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


//--------------------------------------------------------------------
// Clears display (and cursor home)
void OLED_clearDisplay()
{
	OLED_command(0x01);
	int i;
	for(i=200; i>0; i--)
	   {}
}

//--------------------------------------------------------------------
// INITIAL SETUP
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




