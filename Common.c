/*****< Flash.c >****************************************************************/
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   01/03/14  T. Nixon       Common Generic Functions & Defines			  */
/******************************************************************************/

#include <msp430.h>
//#include "MSP430Utils.h"   /* MSP430 defines and Processor Specific Utilities */

#include <stdbool.h>
#include "Common.h"

// accepts a single character and determines if it is an ASCII number 0-9
unsigned char IsAsciiDecNumber(unsigned char a) {
	if( (a >= '0') && (a <= '9') )  return(TRUE);
	else 							return(FALSE);
}


// accepts a single character and determines if it is an ASCII number 0-9, a-f,A-F
unsigned char IsAsciiNumber(unsigned char a) {
	if( ((a >= '0') && (a <= '9')) || ((a >= 'A') && (a <= 'F')) || ((a >= 'a') && (a <= 'f')) ) return(TRUE);
	else 	return(FALSE);
}

// Accepts a single ASCII digit (0-9 & A-F or a-f) returns hex value
unsigned char asctohex(unsigned char a) {
unsigned char c;
	if(a <= '9') c = a - '0';
	else {
		if(a <= 'F') 	c = (a - 'A') + 10;
		else 			c = (a - 'a') + 10;	//assume a = 'a'-'f'
	}
	return(c);
}

// Accepts a single decimal ASCII Digit (0-9) returns Decimal
unsigned char asctodec(unsigned char a) {
unsigned char c;
	if(a <= '9') c = a - '0';
	else c = 0;
	return(c);
}

// Accepts a single hex number 0-F converts to ascii and returns
unsigned char hextoascii(unsigned char h) {

	if(h <= 9){
		h = h + '0';
	}
	else {
		h = (h-10) + 'A';
	}

	if(h > 'F')	h = 'F';
	return(h);
}


void HextoAscii(unsigned char* str, unsigned char len, unsigned char val) // could use for larger vals like int or long
{
  unsigned char i,byte;

  for(i=1; i<=len; i++)
  {
    byte = (unsigned char)(val & 0xF);

    if(byte < 0xA)
    {
      str[len-i] = (unsigned char)(byte + '0');
    }
    else
    {
      byte -= 0xA;
      str[len-i] = (unsigned char)(byte + 'A');
    }
    val >>= 4;
  }

  str[i-1] = '\0';
}

// Accepts pointer to an array of Hex values which it converts to ascii and returns
unsigned char ConvertHexArrayToAscii(unsigned char *hexArray, unsigned int arrayLength, unsigned char *asciiString) {
unsigned int idx,idx1;

	if(arrayLength > 121) {
		return(FALSE);
	}
	else {

		for(idx=0,idx1=0;idx<arrayLength;idx++) {

			asciiString[idx1++] = hextoascii((hexArray[idx] >> 4) & 0x0f);
			asciiString[idx1++] = hextoascii(hexArray[idx] & 0x0f);
		}
		asciiString[idx1] =  '\0';
	}
	return(TRUE);

}



// Accepts a NULL terminated String pointer and copies it to passed string pointer
void copy_string(char *target, char *source) {

   while(*source)	{
      *target = *source;
      source++;
      target++;
   }
   *target = '\0';
}


/*****************************************************************************
INPUTS:
   buf   ->  Array containing message to be sent to controller.
   start ->  Start of loop in crc counter, usually 0.
   cnt   ->  Amount of bytes in message being sent to controller/
*****************************************************************************/
unsigned int crc(unsigned char *buf,unsigned int start, unsigned int cnt)
{
   int      i,j;
   unsigned int temp,temp2,flag;
   temp=0xFFFF;
   for (i=start; i<cnt; i++){
      temp=temp ^ buf[i];

      for (j=1; j<=8; j++){
	 	flag=temp & 0x0001;
	 	temp=temp >> 1;
		 if (flag) temp=temp ^ 0xA001;
      }
   }
   // Reverse byte order.
   temp2=temp >> 8;
   temp=(temp << 8) | temp2;
   return(temp);
}

//**************************************************************************

int bcd2bin (int val)
{
	return val - 6 * (val >> 4);
}

int bin2bcd (int val)
{
	return val + 6 * (val / 10);
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
