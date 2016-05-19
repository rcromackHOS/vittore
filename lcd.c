/*
 * lcd.c
 *
 *  Created on: May 17, 2016
 *      Author: rcromack
 */

#include "lcd.h"

//--------------------------------------------------------------------

int OLED_new_line[4] = {0x80, 0xA0, 0xC0, 0xE0};               // DDRAM addOLED_Resets for each line of the display
int OLED_rows = 0x08;

//--------------------------------------------------------------------
// Simulates a clock pulse

void OLED_clockCycle(void)
{
   P5OUT ^= OLED_RST;
   int i = 0;
   while(i >= 5)
   {
	 if (i == 0)
	   P5OUT ^= OLED_RST;
	 if (i == 3)
	   P5OUT |= OLED_RST;
     i++;
   }
   P5OUT |= OLED_RST;
}
/*
//--------------------------------------------------------------------
// Clears display (and cursor home)
void clearDisplay()
{
  command(0x01);
}

//--------------------------------------------------------------------
// C imitation of a Constructor for the time typedef.
void setup_display()                      // INITIAL SETUP
{
   pinMode(OLED_SCK, OUTPUT);             // Initializes Arduino pin for the OLED_SCK line
   digitalWrite(OLED_SCK, HIGH);          // Sets HIGH the OLED_SCK line of the display
   pinMode(OLED_MOSI, OUTPUT);             // Initializes Arduino pin for the OLED_MOSI line
   digitalWrite(OLED_MOSI, LOW);           // Sets LOW the OLED_MOSI line of the display
   pinMode(OLED_Reset, OUTPUT);            // Initializes Arduino pin for the OLED_Resetet line (optional)
   digitalWrite(OLED_Reset, HIGH);         // Sets HIGH the OLED_Resetet line of the display (optional, can be always high)
   delayMicroseconds(200);            // Waits 200 us for stabilization purpose

   if (ROW_N == 2 || ROW_N == 4)
      rows = 0x08;                    // Display mode: 2/4 lines
   else
      rows = 0x00;                    // Display mode: 1/3 lines

   command(0x22 | rows); // Function set: extended command set (RE=1), lines #
   command(0x71);        // Function selection A:
   //data(0x5C);           //  enable internal Vdd regulator at 5V I/O mode (def. value) (0x00 for disable, 2.8V I/O)
   data(0x00);
   command(0x20 | rows); // Function set: fundamental command set (RE=0) (exit from extended command set), lines #
   command(0x08);        // Display ON/OFF control: display off, cursor off, blink off (default values)
   command(0x22 | rows); // Function set: extended command set (RE=1), lines #
   command(0x79);        // OLED characterization: OLED command set enabled (SD=1)
   command(0xD5);        // Set display clock divide ratio/oscillator frequency:
   command(0x70);        //  divide ratio=1, frequency=7 (default values)
   command(0x78);        // OLED characterization: OLED command set disabled (SD=0) (exit from OLED command set)

   if (ROW_N > 2)
      command(0x09);  // Extended function set (RE=1): 5-dot font, B/W inverting disabled (def. val.), 3/4 lines
   else
      command(0x08);  // Extended function set (RE=1): 5-dot font, B/W inverting disabled (def. val.), 1/2 lines

   command(0x06);        // Entry Mode set - COM/SEG direction: COM0->COM31, SEG99->SEG0 (BDC=1, BDS=0)
   command(0x72);        // Function selection B:
   data(0x0A);           //  ROM/CGRAM selection: ROM C, CGROM=250, CGRAM=6 (ROM=10, OPR=10)
   command(0x79);        // OLED characterization: OLED command set enabled (SD=1)
   command(0xDA);        // Set SEG pins hardware configuration:
   command(0x10);        //  alternative odd/even SEG pin, disable SEG left/right remap (default values)
   command(0xDC);        // Function selection C:
   command(0x00);        //  internal VSL, GPIO input disable
   command(0x81);        // Set contrast control:
   command(0x7F);        //  contrast=127 (default value)
   command(0xD9);        // Set phase length:
   command(0xF1);        //  phase2=15, phase1=1 (default: 0x78)
   command(0xDB);        // Set VCOMH deselect level:
   command(0x40);        //  VCOMH deselect level=1 x Vcc (default: 0x20=0,77 x Vcc)
   command(0x78);        // OLED characterization: OLED command set disabled (SD=0) (exit from OLED command set)
   command(0x20 | rows); // Function set: fundamental command set (RE=0) (exit from extended command set), lines #
   command(0x01);        // Clear display
   delay(2);             // After a clear display, a minimum pause of 1-2 ms is required
   command(0x80);        // Set DDRAM addOLED_Resets 0x00 in addOLED_Resets counter (cursor home) (default value)
   command(0x0C);        // Display ON/OFF control: display ON, cursor off, blink off
   delay(250);           // Waits 250 ms for stabilization purpose after display on

   if (ROW_N == 2)
      new_line[1] = 0xC0;             // DDRAM addOLED_Resets for each line of the display (only for 2-line mode)

   clearDisplay();
}

*/



