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


unsigned char FlashRegisters[MAX_INFO_SIZE];

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

	if( !(verify_info_checksum((unsigned char *)INFOB)) )	{			// If Segment B of Info memory is not programmed or != CRC

		if(verify_info_checksum((unsigned char *)INFOC)) {				// If Segment C is ok (ie segment B was corrupted...)
			copy_segCtoB();
			if(!(verify_info_checksum((unsigned char *)INFOB))) {
				LoadRamRegistersFromFlash(segC);
				return(SEGB_BAD);										// Do not proceed to Application
			}
			else LoadRamRegistersFromFlash(segB);
		}
		else {
			seg_state = SEG_VIRGIN;										// virgin unit! Not programmed


			//ADD!! Your default settings HERE to be written to FLASH
			UpdateFlashRegistersFromRam();

			if( !(verify_info_checksum((unsigned char *)INFOB)) ) {		// Segment B Bad?
				if( !(verify_info_checksum((unsigned char *)INFOC)) ) {	// Segment C Bad?
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
static void LoadRamRegistersFromFlash(flashseg_t seg) {
const unsigned char size = MAX_INFO_SIZE;
unsigned char *Flash_ptr;
unsigned char reg;

	switch(seg) {
		case segA:	Flash_ptr = (unsigned char *)INFOA;
					break;

		case segB:	Flash_ptr = (unsigned char *)INFOB;
					break;

		case segC:	Flash_ptr = (unsigned char *)INFOC;
					break;

		case segD:	Flash_ptr = (unsigned char *)INFOD;
					break;
	}

	for(reg=0;reg<size;reg++) {

		FlashRegisters[reg] = Flash_ptr[reg];

	}
}



//---------------------------------------------------------------------------------------------
// DESCRIPTION:				Erases then overwrites Info FLASH with Registers from Default array
// FUNCTION CALLED BY:		get_configuration()
// VARIABLES:
//---------------------------------------------------------------------------------------------
static void UpdateFlashRegistersFromRam(void) {

unsigned char idx;
unsigned char data[128];
unsigned int checksum;


		// default all values to 0xff
	for(idx=0;idx<MAX_INFO_SIZE;idx++ )	 data[idx] = 0xff;



	idx = 0;
	for(idx=0;idx<(MAX_INFO_SIZE-2);idx++) {

//TEMP!! just write dummy data to FLASH
		data[idx] = idx;

	}

	checksum = crc(&data[0],0,MAX_INFO_SIZE-2);
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

	switch(seg) {
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
	for(i=0;i<size;i++) {
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




