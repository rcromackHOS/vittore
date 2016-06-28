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
int 	IsAsciiDecNumber(unsigned char a);
int 	IsAsciiNumber(unsigned char a);
int 	asctohex(unsigned char a);
int 	asctodec(unsigned char a);
int 	hextoascii(unsigned char h);
void 			HextoAscii(unsigned char* str, unsigned char len, unsigned char val);
extern char *itoa(int num, char *str, int radix);
extern void ftoa(float Value, char* Buffer);
extern char *dtostrf (double val, signed char width, unsigned char prec, char *sout);

int	ConvertHexArrayToAscii(unsigned char *hexArray, unsigned int arrayLength, unsigned char *asciiString);
void 			copy_string(char *target, char *source);

unsigned int 	crc(unsigned char *buf,unsigned int start, unsigned int cnt);

int bcd2bin (int val);
int bin2bcd (int val);

extern int map_adc(unsigned int x, unsigned int in_min, unsigned int in_max, unsigned int out_min, unsigned int out_max);
extern void setStateCode(int code);
extern void clearStateCode(int code);

#endif

