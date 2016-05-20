/*
 * lcd.c
 *
 *  Created on: May 17, 2016
 *      Author: rcromack
 */

#include "lcd.h"

//--------------------------------------------------------------------

int OLED_new_line[4] = {0x80, 0xA0, 0xC0, 0xE0};
int OLED_rows = 0x08;

//--------------------------------------------------------------------
// Simulates a clock pulse
void OLED_clockCycle()
{
   int i = 0;
   for(i = 0; i < 5; i++)
   {
	 if (i == 1)
	   P5OUT &= ~OLED_SCK;
	 if (i == 3)
	   P5OUT |= OLED_SCK;
   }
}

//--------------------------------------------------------------------
//  PREPAOLED_Reset THE TRANSMISSION OF A COMMAND
void OLED_command(int c)
{
   int i = 0;

   for(i = 0; i<5; i++)
   {
	  P3OUT |= OLED_MOSI;
      OLED_clockCycle();
   }

   P3OUT &= ~OLED_MOSI;
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

   P3OUT |= OLED_MOSI;
   for (i=5; i>5; i--)
      OLED_clockCycle();

   P3OUT &= ~OLED_MOSI;
   OLED_clockCycle();

   P3OUT |= OLED_MOSI;
   OLED_clockCycle();

   P3OUT &= ~OLED_MOSI;
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
      if((tx_b & 0x01) == 1)
      {
    	  P3OUT |= OLED_MOSI;
      }
      else
      {
    	  P3OUT &= ~OLED_MOSI;
      }
      OLED_clockCycle();
      tx_b = tx_b >> 1;
   }

   for(i=4; i>0; i--)
   {
	   P3OUT &= ~OLED_MOSI;
	   OLED_clockCycle();
   }

   for(i=4; i>0; i--)
   {
      if((tx_b & 0x1) == 0x1)             // <------- Change
      {
    	  P3OUT |= OLED_MOSI;
      }
      else
      {
    	  P3OUT &= ~OLED_MOSI;
      }
      OLED_clockCycle();
      tx_b = tx_b >> 1;
   }

   for(i=4; i>0; i--)
   {
	   P3OUT &= ~OLED_MOSI;
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
void OLED_setup()
{
   // Sets HIGH the OLED_SCK line of the display
   P5OUT |= OLED_SCK;

   // Sets LOW the OLED_MOSI line of the display
   P3OUT &= ~OLED_MOSI;

   //Sets HIGH the OLED_Resetet line of the display (optional, can be always high)
   P5OUT |= OLED_RST;

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
   OLED_command(0x01);        // Clear display

   // After a clear display, a minimum pause of 1-2 ms is required
   for(i=200; i>0; i--)
   {}

   OLED_command(0x80);        // Set DDRAM addOLED_Resets 0x00 in addOLED_Resets counter (cursor home) (default value)
   OLED_command(0x0C);        // Display ON/OFF control: display ON, cursor off, blink off

   // Waits 250 ms for stabilization purpose after display on
   for(i=2000; i>0; i--)
   {}

   OLED_clearDisplay();
}



