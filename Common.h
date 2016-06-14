/*****< Flash.h >**************************************************************/
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   01/03/14  T. Nixon       Common Generic Functions & Defines  		      */
/******************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__


#include <stdbool.h>

#ifndef TRUE
   #define TRUE (1 == 1)
#endif

#ifndef FALSE
   #define FALSE (0 == 1)
#endif

#ifndef NULL
   #define NULL ((void *)0)
#endif


	// generic ASCII Hex conversions
unsigned char 	IsAsciiDecNumber(unsigned char a);
unsigned char 	IsAsciiNumber(unsigned char a);
unsigned char 	asctohex(unsigned char a);
unsigned char 	asctodec(unsigned char a);
unsigned char 	hextoascii(unsigned char h);
void 			HextoAscii(unsigned char* str, unsigned char len, unsigned char val);



unsigned char	ConvertHexArrayToAscii(unsigned char *hexArray, unsigned int arrayLength, unsigned char *asciiString);
void 			copy_string(char *target, char *source);

unsigned int 	crc(unsigned char *buf,unsigned int start, unsigned int cnt);

int bcd2bin (int val);
int bin2bcd (int val);
long map(long x, long in_min, long in_max, long out_min, long out_max)

#endif

