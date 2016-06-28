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
#include "config.h"
#include <math.h>
#include <stdint.h>

//--------------------------------------------------------------------
//
void ftoa(float Value, char* Buffer)
 {
     union
     {
         float f;

         struct
         {
             unsigned int    mantissa_lo : 16;
             unsigned int    mantissa_hi : 7;
             unsigned int     exponent : 8;
             unsigned int     sign : 1;
         };
     } helper;

     unsigned long mantissa;
     signed char exponent;
     unsigned int int_part;
     char frac_part[3];
     int i, count = 0;

     helper.f = Value;
     //mantissa is LS 23 bits
     mantissa = helper.mantissa_lo;
     mantissa += ((unsigned long) helper.mantissa_hi << 16);
     //add the 24th bit to get 1.mmmm^eeee format
     mantissa += 0x00800000;
     //exponent is biased by 127
     exponent = (signed char) helper.exponent - 127;

     //too big to shove into 8 chars
     if (exponent > 18)
     {
         Buffer[0] = 'I';
         Buffer[1] = 'n';
         Buffer[2] = 'f';
         Buffer[3] = '\0';
         return;
     }

     //too small to resolve (resolution of 1/8)
     if (exponent < -3)
     {
         Buffer[0] = '0';
         Buffer[1] = '\0';
         return;
     }

     count = 0;

     //add negative sign (if applicable)
     if (helper.sign)
     {
         Buffer[0] = '-';
         count++;
     }

     //get the integer part
     int_part = mantissa >> (23 - exponent);
     //convert to string
     itoa(int_part, &Buffer[count], 10);

     //find the end of the integer
     for (i = 0; i < 8; i++)
         if (Buffer[i] == '\0')
         {
             count = i;
             break;
         }

     //not enough room in the buffer for the frac part
     if (count > 5)
         return;

     //add the decimal point
     Buffer[count++] = '.';

     //use switch to resolve the fractional part
     switch (0x7 & (mantissa  >> (20 - exponent)))
     {
         case 0:
             frac_part[0] = '0';
             frac_part[1] = '0';
             frac_part[2] = '0';
             break;
         case 1:
             frac_part[0] = '1';
             frac_part[1] = '2';
             frac_part[2] = '5';
             break;
         case 2:
             frac_part[0] = '2';
             frac_part[1] = '5';
             frac_part[2] = '0';
             break;
         case 3:
             frac_part[0] = '3';
             frac_part[1] = '7';
             frac_part[2] = '5';
             break;
         case 4:
             frac_part[0] = '5';
             frac_part[1] = '0';
             frac_part[2] = '0';
             break;
         case 5:
             frac_part[0] = '6';
             frac_part[1] = '2';
             frac_part[2] = '5';
             break;
         case 6:
             frac_part[0] = '7';
             frac_part[1] = '5';
             frac_part[2] = '0';
             break;
         case 7:
             frac_part[0] = '8';
             frac_part[1] = '7';
             frac_part[2] = '5';
             break;
     }

     //add the fractional part to the output string
     for (i = 0; i < 3; i++)
         if (count < 7)
             Buffer[count++] = frac_part[i];

     //make sure the output is terminated
     Buffer[count] = '\0';
 }



char *dtostrf (double val, signed char width, unsigned char prec, char *sout)
{
  char fmt[20];

  int whole = val;
  float mantissa = val - whole;

  unsigned int frac = mantissa * powf(10, prec);
  if  (frac < 0)
	  frac = -frac;

  sprintf(fmt, "%%0%dd.%%0%dd", width, prec);
  sprintf(sout, fmt, whole, frac);
  return sout;
}


//--------------------------------------------------------------------
//
char *itoa(int num, char *str, int radix)
{
    char sign = 0;
    char temp[17];  //an int can only be 16 bits long
                    //at radix 2 (binary) the string
                    //is at most 16 + 1 null long.
    int temp_loc = 0;
    int digit;
    int str_loc = 0;

    //save sign for radix 10 conversion
    if (radix == 10 && num < 0) {
        sign = 1;
        num = -num;
    }

    //construct a backward string of the number.
    do {
        digit = (unsigned int)num % radix;
        if (digit < 10)
            temp[temp_loc++] = digit + '0';
        else
            temp[temp_loc++] = digit - 10 + 'A';
        num = (((unsigned int)num) / radix);
    } while ((unsigned int)num > 0);

    //now add the sign for radix 10
    if (radix == 10 && sign) {
        temp[temp_loc] = '-';
    } else {
        temp_loc--;
    }


    //now reverse the string.
    while ( temp_loc >=0 ) {// while there are still chars
        str[str_loc++] = temp[temp_loc--];
    }
    str[str_loc] = 0; // add null termination.

    return str;
}

//--------------------------------------------------------------------
// accepts a single character and determines if it is an ASCII number 0-9
int IsAsciiDecNumber(unsigned char a )
{
	if ( (a >= '0') && (a <= '9') )
		return(1);
	else
		return(0);
}


//--------------------------------------------------------------------
// accepts a single character and determines if it is an ASCII number 0-9, a-f,A-F
int IsAsciiNumber(unsigned char a)
{
	if( ((a >= '0') && (a <= '9')) || ((a >= 'A') && (a <= 'F')) || ((a >= 'a') && (a <= 'f')) ) return(1);
	else 	return(0);
}

//--------------------------------------------------------------------
// Accepts a single ASCII digit (0-9 & A-F or a-f) returns hex value
int asctohex(unsigned char a)
{
	unsigned char c;
	if(a <= '9') c = a - '0';
	else {
		if(a <= 'F') 	c = (a - 'A') + 10;
		else 			c = (a - 'a') + 10;	//assume a = 'a'-'f'
	}
	return(c);
}

//--------------------------------------------------------------------
// Accepts a single decimal ASCII Digit (0-9) returns Decimal
int asctodec(unsigned char a)
{
	unsigned char c;
	if(a <= '9') c = a - '0';
	else c = 0;
	return(c);
}

//--------------------------------------------------------------------
// Accepts a single hex number 0-F converts to ascii and returns
int hextoascii(unsigned char h)
{

	if(h <= 9){
		h = h + '0';
	}
	else {
		h = (h-10) + 'A';
	}

	if(h > 'F')	h = 'F';
	return(h);
}


//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
// Accepts pointer to an array of Hex values which it converts to ascii and returns
int ConvertHexArrayToAscii(unsigned char *hexArray, unsigned int arrayLength, unsigned char *asciiString)
{
	unsigned int idx,idx1;

	if(arrayLength > 121)
	{
		return(0);
	}
	else
	{

		for(idx=0,idx1=0;idx<arrayLength;idx++)
		{

			asciiString[idx1++] = hextoascii((hexArray[idx] >> 4) & 0x0f);
			asciiString[idx1++] = hextoascii(hexArray[idx] & 0x0f);
		}
		asciiString[idx1] =  '\0';
	}
	return(1);

}



//--------------------------------------------------------------------
// Accepts a NULL terminated String pointer and copies it to passed string pointer
void copy_string(char *target, char *source)
{

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

int map_adc(unsigned int x, unsigned int in_min, unsigned int in_max, unsigned int out_min, unsigned int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//--------------------------------------------------------------------
//
void clearStateCode(int code)
{
	if (code == _STATE_CODE)
		_STATE_CODE = 0;
}

void setStateCode(int code)
{
	if (code > _STATE_CODE)
		_STATE_CODE = code;
}
