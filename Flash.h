/*****< Flash.h >**************************************************************/
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   07/08/16  T. Nixon       non-volatile Flash functions					  */
/******************************************************************************/
#ifndef __FLASH_H__
#define __FLASH_H__



// note: all 4 sections of Info Memory are FLASH and 128bytes long
// 	 	 Erase/write cycles 10000min, 100000typical
#define INFOD 	0x1800
#define INFOC 	0x1880
#define INFOB 	0x1900
#define INFOA 	0x1980

#define MAX_INFO_SIZE 		128


#define SEGS_GOOD		0
#define SEGB_BAD		1
#define SEGC_BAD		2
#define SEG_VIRGIN		4

#define	CRC_OK			1
#define CRC_BAD			0

typedef enum
{
   segA,
   segB,
   segC,
   segD
} flashseg_t;



//ADD!! custom size

#define MAX_INFO_SIZE 		128
#define MEM_SEG_SIZE	 	11

#define MEM_POINTER_EXTENT 	10
#define MEM_SEG_EXTENT 		100

int mem_pointer =			0;

extern unsigned int DEFAULT_CONFIG[MAX_INFO_SIZE];
extern unsigned char FlashRegisters[MAX_INFO_SIZE];

extern unsigned char GetConfiguration(void);
extern void UpdateFlashMemory(void);

#endif


