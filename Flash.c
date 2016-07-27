/*****< Flash.c >****************************************************************/
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   07/06/16  T. Nixon       non-Volatile Flash functions					  */
/******************************************************************************/

#include <msp430.h>

#define  _FLASH_C_

#include "Common.h"
#include "WatchdogTimerControl.h"
#include "Flash.h"
#include "config.h"
#include "string.h"

unsigned char FlashRegisters[MAX_INFO_SIZE];

unsigned int DEFAULT_CONFIG[] = { 0, 0, 0, 0, 0, 20, 70, 0, 51, 65, 1 };
int mem_pointer;

/*
 * 0-1  	Engine Hours
 * 2		pointer to minutes
 * 3-4		Oil Change
 * 5-6		latitude
 * 7 		sign for lat
 * 8-9		longitude
 * 10 		sign for lng
 *
 * 11		Engine Mins
 */

   /* Internal local static variables.                                  */
   /* Internal static function prototypes.                              */
static unsigned char 	verify_info_checksum(unsigned char *);
static void 			LoadRamRegistersFromFlash(flashseg_t seg);
static void 			overwriteInfoSeg(flashseg_t seg, unsigned char *data);
static void 			copy_segCtoB(void);
static void 			UpdateFlashRegistersFromRam(void);

//---------------------------------------------------------------------------------------------
// DESCRIPTION:				Info Segment B is the Master copy of registers, Segment C is a copy of B
//							checks info memory integrity and loads values to system Ram
// 						    Interrupts & watchdog must be disabled.
// FUNCTION CALLED BY:
// VARIABLES:
// RETURNS:					0: everthing ok
//							bit1: Problem with flash segment B
//							bit2: Problem with flash segment C
//--------------------------------------------------------------------------------------------
unsigned char GetConfiguration(void)
{
	unsigned char seg_state = 0;

	if( !(verify_info_checksum((unsigned char *)INFOB)) )			// If Segment B of Info memory is not programmed or != CRC
	{
		if(verify_info_checksum((unsigned char *)INFOC))			// If Segment C is ok (ie segment B was corrupted...)
		{
			copy_segCtoB();
			if	(!(verify_info_checksum((unsigned char *)INFOB)))
			{
				LoadRamRegistersFromFlash(segC);
				return(SEGB_BAD);									// Do not proceed to Application
			}
			else
				LoadRamRegistersFromFlash(segB);
		}
		else
		{
			seg_state = SEG_VIRGIN;									// virgin unit! Not programmed

			//ADD!! Your default settings HERE to be written to FLASH
			UpdateFlashRegistersFromRam();

			if( !(verify_info_checksum((unsigned char *)INFOB)) )
			{		// Segment B Bad?
				if( !(verify_info_checksum((unsigned char *)INFOC)) )
				{	// Segment C Bad?
					return(seg_state + SEGC_BAD + SEGB_BAD);			// Unit is dead!
				}

				LoadRamRegistersFromFlash(segC);
				return(seg_state + SEGB_BAD);
			}
		}
	}

	LoadRamRegistersFromFlash(segB);

	return(seg_state + SEGS_GOOD);
}





//---------------------------------------------------------------------------------------------
// DESCRIPTION:
// FUNCTION CALLED BY:		get_configuration()
// RETURNS:					1: crc match(good)
//							0: crc doesn't equal
//---------------------------------------------------------------------------------------------
static unsigned char verify_info_checksum(unsigned char *data)
{
unsigned int j,k;
unsigned char size = MAX_INFO_SIZE;

	j = crc(data,0,size-2);
	k = (unsigned int)((data[size-2] << 8) & 0xff00);
	k += (unsigned int)data[size-1];
	if(j == k) return(CRC_OK);
	else return(CRC_BAD);
}



//---------------------------------------------------------------------------------------------
// DESCRIPTION:				-Copies registers from Info FLASH to RAM structure.
//
// FUNCTION CALLED BY:		get_configuration()
// VARIABLES:
//---------------------------------------------------------------------------------------------
static void LoadRamRegistersFromFlash(flashseg_t seg)
{
	const unsigned char size = MAX_INFO_SIZE;
	unsigned char *Flash_ptr;
	unsigned char reg;
	unsigned long temp;
	int i = 0;
	int pointer = 0;

	switch(seg)
	{
		case segA:	Flash_ptr = (unsigned char *)INFOA;
					break;

		case segB:	Flash_ptr = (unsigned char *)INFOB;
					break;

		case segC:	Flash_ptr = (unsigned char *)INFOC;
					break;

		case segD:	Flash_ptr = (unsigned char *)INFOD;
					break;
	}

	for (reg = 0; reg < size; reg++)
	{
		FlashRegisters[reg] = Flash_ptr[reg];
	}

	//----------------------------- load engine hours

	temp = FlashRegisters[0];
	temp = temp << 8;

	temp |= FlashRegisters[1];

	engine.engineHours = temp;

	//----------------------------- load engine minutes
	/*
	// flashRegister[2] is the pointer to where the engine minutes are stored.
	_POINTER_TO_ENGINE_MINS = FlashRegisters[2];

	engine.engineMins = FlashRegisters[_POINTER_TO_ENGINE_MINS];
	*/
	engine.engineMins = FlashRegisters[2];

	//----------------------------- Next oil change

	temp = FlashRegisters[3];
	temp = temp << 8;

	temp |= FlashRegisters[4];

	_OILCHANGE_DUE = temp;

	//----------------------------- Lat and Long

	temp = FlashRegisters[5];
	temp = temp << 8;
	temp |= FlashRegisters[6];

	lat = (float)(temp) / 100;
	if (FlashRegisters[7] > 0)
		lat *= -1;

	temp = FlashRegisters[8];
	temp = temp << 8;
	temp |= FlashRegisters[9];

	lng = (float)(temp) / 100;
	if (FlashRegisters[10] > 0)
		lng *= -1;

	//----------------------------- idle time save

	for (i = 0; i < 4; i++)
	{
		pointer = IDLES_SEG_START + (i * 4);

		temp = FlashRegisters[pointer];
		temp = temp << 8;
		temp |= FlashRegisters[pointer+1];

		idles[i].hours = temp;

		temp = FlashRegisters[pointer+2];
		temp = temp << 8;
		temp |= FlashRegisters[pointer+3];

		idles[i].minutes = temp;
	}

}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:				-	overwrite flash memory with new values
//							- 	called
// FUNCTION CALLED BY:		-

// VARIABLES: void
//---------------------------------------------------------------------------------------------
void UpdateFlashMemory(void)
{
	//unsigned char idx;
	unsigned char data[128];
	unsigned int checksum;

	// default all values to 0xff
	//for(idx = 0; idx < MAX_INFO_SIZE; idx++)
	//	data[idx] = 0xff;

	/*
	 * 0-1  	Engine Hours
	 * 2		Engine Mins
	 * 3-4		Oil Change
	 * 5-6		latitude
	 * 7 		sign for lat
	 * 8-9		longitude
	 * 10 		sign for lng
	 */
	/*
	// point = base offset (10) + size of saved data (11) * point of scrolling memory (0-9)
	POINTER = MEM_POINTER_EXTENT + (MEM_SEG_SIZE + mem_pointer);

	// if you go over the possible number of memory blocks
	if (POINTER > 8)
		POINTER = MEM_POINTER_EXTENT;	// reset to the first block spot
	*/

	data[0] = (engine.engineHours >> 8) & 0xFF;
  	data[1] = engine.engineHours & 0xFF;

	/*
  	_POINTER_TO_ENGINE_MINS++;

	_POINTER_TO_ENGINE_MINS = FlashRegisters[2];

	engine.engineMins = FlashRegisters[_POINTER_TO_ENGINE_MINS];
	*/
  	data[2] = engine.engineMins & 0xFF;

	data[3] = (_OILCHANGE_DUE >> 8) & 0xFF;
	data[4] = _OILCHANGE_DUE & 0xFF;

	//------------------------------------------
	int temp;
	temp = (int)(lat * 100);

	// this is a check to deal with negative floating point values IE, Lat and Long
	data[7] = 0;
	if (lat < 0)
	{
		data[7] = 1;
		temp *= -1;
	}

	data[5] = (temp >> 8) & 0xFF;
	data[6] = temp & 0xFF;

	//------------------------------------------

	temp = (int)(lng * 100);

	// this is a check to deal with negative floating point values IE, Lat and Long
	data[10] = 0;
	if (lng < 0)
	{
		data[10] = 1;
		temp *= -1;
	}

	data[8] = (temp >> 8) & 0xFF;
	data[9] = temp & 0xFF;

	//------------------------------------------ save idle times

	/*
	int i = 0;
	//int j = 0;
	int pointer = 0;

	for (i = 0; i < 4; i++)
	{
		pointer = IDLES_SEG_START + (i * 4);

		//for (j = 0; j < 2; j++)
		//{
			data[pointer] = (idles[i].hours >> 8) & 0xFF;
			data[pointer+1] = idles[i].hours & 0xFF;

			data[pointer+2] = (idles[i].minutes >> 8) & 0xFF;
			data[pointer+3] = idles[i].minutes & 0xFF;
		//}

		_nop();
	}
	*/
	//------------------------------------------

	// TODO: Save the time of last engine run

	//------------------------------------------

	checksum = crc(&data[0],0,MAX_INFO_SIZE-2);
	data[MAX_INFO_SIZE-2] = (checksum >> 8) & 0x00ff;
	data[MAX_INFO_SIZE-1] = checksum & 0x00ff;

	overwriteInfoSeg(segB,&data[0]);
	overwriteInfoSeg(segC,&data[0]);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:				Erases then overwrites Info FLASH with Registers from Default array
//							- saves defaults into first position of scrolling memory
// FUNCTION CALLED BY:		get_configuration()
// VARIABLES:
//---------------------------------------------------------------------------------------------
static void UpdateFlashRegistersFromRam(void)
{
	unsigned char idx;
	unsigned char data[128];
	unsigned int checksum;

	// default all values to 0xff
	for(idx = 0; idx < MAX_INFO_SIZE; idx++)
		data[idx] = 0xff;

	idx = 0;
  	for (idx = 0; idx < MAX_INFO_SIZE; idx++)
	{
		data[idx] = DEFAULT_CONFIG[idx] & 0xff;
	}

  	for (idx = 80; idx < 97; idx++)
	{
		data[idx] = 0x0;
	}

	checksum = crc(&data[0], 0, MAX_INFO_SIZE-2);
	data[MAX_INFO_SIZE-2] = (checksum >> 8) & 0x00ff;
	data[MAX_INFO_SIZE-1] = checksum & 0x00ff;

	overwriteInfoSeg(segB,&data[0]);
	overwriteInfoSeg(segC,&data[0]);
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:				-Overwrites the entire contents of a Passed INFO segment.
//							-Assumption is made that passed data buffer is the size of the Info Segmemnt
//							-Must disable Watchdog & interrutps before Call
// FUNCTION CALLED BY:		get_configuration()
// VARIABLES:
//---------------------------------------------------------------------------------------------
static void overwriteInfoSeg(flashseg_t seg, unsigned char *data)
{
	unsigned char i;
	unsigned char size = MAX_INFO_SIZE;
	unsigned char *Flash_ptr;

	switch(seg)
	{
		case segA:	Flash_ptr = (unsigned char *)INFOA;
					break;

		case segB:	Flash_ptr = (unsigned char *)INFOB;
					break;

		case segC:	Flash_ptr = (unsigned char *)INFOC;
					break;

		case segD:	Flash_ptr = (unsigned char *)INFOD;
					break;
	}
	__disable_interrupt();
	WdtDisable();

	FCTL1 = FWKEY + ERASE;
	FCTL3 = FWKEY;
	Flash_ptr[0] = 0;													// Dummy write to erase segment A

	FCTL1 = FWKEY + WRT;
	for(i=0;i<size;i++)
	{
		Flash_ptr[i] = data[i];											// Copy data to Segment A
	}

	_nop();
	FCTL1 = FWKEY;                            // Clear WRT bit
	while(FCTL3 & BUSY);
	FCTL3 = FWKEY+LOCK;                       // Set LOCK bit

//	__enable_interrupt();
	WdtEnable();
}



static void copy_segCtoB(void)
{
	unsigned char i;
	unsigned char size = MAX_INFO_SIZE;
	unsigned char *Flash_ptrC = (unsigned char *)INFOC;
	unsigned char *Flash_ptrB = (unsigned char *)INFOB;

	__disable_interrupt();
	WdtDisable();

	FCTL1 = FWKEY + ERASE;
	FCTL3 = FWKEY;
	Flash_ptrB[0] = 0;													// Dummy write to erase segment B

	FCTL1 = FWKEY + WRT;
	for(i=0;i<size;i++) {
		Flash_ptrB[i] = Flash_ptrC[i];									// Copy Segment A to B completely
	}

	_nop();
	FCTL1 = FWKEY;                            // Clear WRT bit
	while(FCTL3 & BUSY);
	FCTL3 = FWKEY+LOCK;                       // Set LOCK bit

//	__enable_interrupt();
	WdtEnable();
}




