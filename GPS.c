/*****< GPS.c >****************************************************************/
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   05/07/15  T. Nixon       u-Blox UC530M GPS module						  */
/*	 06/15/16  T. Nixon       change rx buf, add power save					  */
/******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <msp430.h>

#define _GPS_C_

#include "Hardware.h"
#include "Common.h"
#include "GPS.h"
#include "timeDate.h"
#include "solar.h"
#include "config.h"

   /* Internal local static variables.                                  */
                              /* The following is used to buffer        */
                              /* characters read from the Debug UART.   */
static unsigned char RecvBufferGps[GPS_UART_RX_BUFFER_SIZE];

                              /* The following are used to track the    */
                              /* Receive circular buffer.               */
static unsigned int  RxInIndexGps;
static unsigned int  RxOutIndexGps;

static unsigned char 	TransBufferGps[GPS_UART_TX_BUFFER_SIZE];
static unsigned int 	TxInIndexGps;
static unsigned int 	TxOutIndexGps;

StringInfo_t storedInstances[5];
long GpsStateCountdown = 100;
static int instanceIndex = 0;

//static const char VENUS_GGA_5SEC_REST_OFF[]		= {0xA0, 0xA1, 0x00, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x01}; // ADD CS,0x0D,0x0A
//#define  		  SIZE_VENUS_GGA_5SEC_REST_OFF 	sizeof(VENUS_GGA_5SEC_REST_OFF)
// CS=Checksum: starting with 0 exclusive or all bytes from message ID to end of payload

	/* Global variables                              					*/
GpsInformation_t GpsInformation;


	/* Internal static function prototypes.                              */
static unsigned int 		GetGpsIdx(unsigned int startIdx,signed int cnt);
static void 				ResetGPSRxBuffer(void);
static int					SendNMEACmd(void);

//char ggamsg[] = {"$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47"};


//---------------------------------------------------------------------------------------------
// DESCRIPTION: main.cs calls function to pull GPS sentences and verify they are good
//              if they pass error checking, they have their data pulled and loaded into "GPSinstance"
//				GPSinstance is used for both RMC and GGA. Once all the data is complete, the instance
//				is saved as a snapshot in "storeGPSInstance"
// RETURN/UPDATES:	void
//
//---------------------------------------------------------------------------------------------
void pollGPS()
{
	if (GpsStateCountdown == 0)
	{
		GpsStateCountdown = 100;

		if (GpsMessageRetrieve() == 1)
		{

			if (GpsDecode(GPSinstance) == 1)
			{
				if (GPSinstance.hasGGA == 1 && GPSinstance.hasRMC == 1)
					storeGPSInstance();

			}

			//else
			//	resetGpsInstance();
		}

		//resetGpsInstance();
	}
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:	saves "GPSinstance"s in an array of 5. Once 5 has been been saved, the results are
//				compared to each other. If they're all within a tolerence of each other the data
//				is saved into the system as the new standards. EEPROM and RTC values are updated, etc.
//
// RETURN/UPDATES: void
//
//---------------------------------------------------------------------------------------------
void storeGPSInstance()
{
	storedInstances[instanceIndex] = GPSinstance;

	instanceIndex++;

	if (instanceIndex == 5)
	{
		instanceIndex = 0;

		int APPLYING_LAT = (int)(GPSinstance.lat * 100);
		int APPLYING_LNG = (int)(GPSinstance.lng * 100);

		int lati;
		int longi;
		int fail = 0;

		int i = 0;

		for (i = 0; i < 5; i++)
		{
			lati = (int)storedInstances[instanceIndex].lat * 100;
			longi = (int)storedInstances[instanceIndex].lng * 100;

			if (APPLYING_LAT != lati || APPLYING_LNG != longi)
				fail = 1;
		}

		if (fail == 0)
		{
			now.hour = GPSinstance.hour;
			now.year = GPSinstance.year;
			now.day = GPSinstance.day;
			now.minute = GPSinstance.minute;
			now.month = GPSinstance.month;
			now.second = GPSinstance.second;

			// TODO: update RTC then update "now"

			lat = GPSinstance.lat;
			lng = GPSinstance.lng;

			solar_setCoords(lat, lng);

			sunSet = solar_getSunset(now);
			sunRise = solar_getSunrise(now);

			GpsStateCountdown = 360000;
		}

		// TODO: use saved data to determine new coords
	}

	resetGpsInstance();
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION: if a "GPSinstance" fails validation. We clear the partial information and try again
//
// RETURN/UPDATES: void
//
//---------------------------------------------------------------------------------------------
void resetGpsInstance()
{
	GPSinstance.lat = 0;
	GPSinstance.lng = 0;

	GPSinstance.hour = 0;
	GPSinstance.minute = 0;
	GPSinstance.second = 0;
	GPSinstance.month = 0;
	GPSinstance.year = 0;
	GPSinstance.day = 0;

	GPSinstance.fix = 0;
	GPSinstance.alt = 0;
	GPSinstance.sfov = 0;

	GPSinstance.hasGGA = 0;
	GPSinstance.hasRMC = 0;
}

//---------------------------------------------------------------------------------------------
/* GPS UART Receive Interrupt Handler.                             	*/
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
   unsigned char ch;

   switch(__even_in_range(UCA0IV,4))
   	{
   		case 0:break;									// Vector 0 - no interrupt
   		case 2:								   			// Vector 2 - RXIFG
					/* Read the received character.                                   */
					ch = UCA0RXBUF;

					  // Check for Receive errors flags
					while(UCA0STAT & UCRXERR)  ch = UCA0RXBUF;


					// Place received character into receive buffer at index = RxInIndexZb
					  // Note: RxInIndex starts at 1 while RxOutIndex starts at 0.
					  //	   RxInIndex can equal but will never pass RxOutIndexZb
					if(RxOutIndexGps != RxInIndexGps) {
						   /* Save the character in the Receive Buffer.                   */
					   RecvBufferGps[RxInIndexGps++] = ch;

						   /* Wrap the buffer if neccessary.                              */
					   if(RxInIndexGps >= GPS_UART_RX_BUFFER_SIZE)	RxInIndexGps = 0;

						   // check if we have now caught up to Out index
					   if(RxInIndexGps == RxOutIndexGps) {
							  // Throw out received byte
							  // Decrement In index as we never want in=out on a wrap
						   RxInIndexGps = GetGpsIdx(RxInIndexGps,-1);
					   }
					}
					else {
						// Buffer Overflow! We have not been able to keep up with decoding received GPS data

						   // Are we actively decoding GPS
					   if(GpsInformation.LookingForGpsMessages == 0) {
						   // Not decoding so throw out old data and keep new
						   RecvBufferGps[RxInIndexGps++] = ch;
						   /* Wrap the buffer if neccessary.                              */
						   if(RxInIndexGps >= GPS_UART_RX_BUFFER_SIZE)	RxInIndexGps = 0;
						   // Make sure OUT Index is always +1 above IN index
						   RxOutIndexGps = RxInIndexGps + 1;
						   if(RxOutIndexGps >= GPS_UART_RX_BUFFER_SIZE)	RxOutIndexGps = 0;
					   }
					   else {
						   // We are actively decoding Just throw-out what we receive
						   // All received data is now being thrown out, set flag to reset buffer in GpsDecode()
						   GpsInformation.BufferOverflow = 0;
					   }
					}
					break;

   		case 4:										// Vector 4 - TXIFG
					if(TxOutIndexGps < TxInIndexGps) {
						   // Send the next character out. increment out counter
						UCA0TXBUF = TransBufferGps[TxOutIndexGps++];
					}
					else {
						   // There is no more data, so disable the TX Interrupt.
						UCA0IE &= ~UCTXIE;
						TxOutIndexGps = TxInIndexGps = 0;

						//TEMP!!
						//UCA0IE |= UCRXIE;						 			// Enable USCI_A0 RX interrupt
					}
					break;


   		default: break;
   	}
}


void ConfigureGPSSerialPort()
{
	//queueInit(&inputQueue, MAX_SERIAL_INPUT_BUFFER, inputSerialBuffer);

	ResetGPSRxBuffer();

	P3SEL |= GPS_MODULE_RX + GPS_MODULE_TX;


	UCA0CTL1 |= UCSWRST;					  			// **Put state machine in reset**

	UCA0CTL1 |= UCSSEL_1;					 			// CLK = ACLK
	UCA0BR0 = 0x03;						   				// 32kHz/9600=3.41 (see User's Guide)
	UCA0BR1 = 0x00;						   				//
	UCA0MCTL = UCBRS_3+UCBRF_0;			   				// Modulation UCBRSx=3, UCBRFx=0

	UCA0CTL1 &= ~UCSWRST;					 			// **Initialize USCI state machine**
	UCA0IE |= UCRXIE;						 			// Enable USCI_A0 RX interrupt
}


typedef enum
{
	gpsFindStart = 0,
	gpsFindEnd,
	gpsGetChecksum1,
	gpsGetChecksum2
} GpsNmeaMessageFindState_t;

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		-Retrieves Checksum'd GPS NMEA messages from the GPS rx interrupts rotating buffer and places
//					 message in its entirety in Structure GpsInformation_t
//					-Looks for '$' then '*' followed by two byte checksum that must match calculated
//
// RETURN/UPDATES:	1: A complete NMEA message has been retrieved and placed in the Structure GpsInformation_t
//					0: No complete NMEA message was found.
//---------------------------------------------------------------------------------------------
int GpsMessageRetrieve(void)
{
	unsigned int outIndex,inIndex,endIdx,calcChecksum,checksum;
	unsigned char idx;
	GpsNmeaMessageFindState_t gpsDecodeState;

	__disable_interrupt();
		// Flag to tell interrupt not to overwite RxOutIndexGps
	GpsInformation.LookingForGpsMessages = 0;
	inIndex = RxInIndexGps;		// Copy Index to Receive buffer that have been received
	outIndex = RxOutIndexGps;	// Copy Index to Receive buffer that we have yet to decode
	__enable_interrupt();

	idx = 0;
	endIdx = 0;
	gpsDecodeState = gpsFindStart;
	while(outIndex != inIndex) {

		switch(gpsDecodeState) {
			case gpsFindStart:	if(RecvBufferGps[outIndex] == '$')
								{
									idx = 0;
									GpsInformation.Message[idx] = RecvBufferGps[outIndex];
									gpsDecodeState++;
								}
								break;
			case gpsFindEnd:	if(RecvBufferGps[outIndex] == '$') {
									idx = 0;
									GpsInformation.Message[idx] = RecvBufferGps[outIndex];
								}
								else {
									GpsInformation.Message[idx] = RecvBufferGps[outIndex];

									if(RecvBufferGps[outIndex] == '*') {	// could also verify size is >= to GGA_MIN_SIZE

										endIdx = idx - 1;
										gpsDecodeState++;
									}
								}
								break;

			case gpsGetChecksum1:
								if(RecvBufferGps[outIndex] == '$')
								{
									idx = 0;
									GpsInformation.Message[idx] = RecvBufferGps[outIndex];
									gpsDecodeState = gpsFindEnd;
								}
								else
								{
									GpsInformation.Message[idx] = RecvBufferGps[outIndex];
									gpsDecodeState++;
								}
								break;

			case gpsGetChecksum2:
								if(RecvBufferGps[outIndex] == '$')
								{
									idx = 0;
									GpsInformation.Message[idx] = RecvBufferGps[outIndex];
									gpsDecodeState = gpsFindEnd;
								}
								else
								{
									GpsInformation.Message[idx] = RecvBufferGps[outIndex];
									GpsInformation.Message[++idx] = NULL;

									//verify checksum
									checksum = asctohex(GpsInformation.Message[idx-1]);
									checksum += ( asctohex(GpsInformation.Message[idx-2]) ) << 4;

									calcChecksum = 0;
									for(idx=1; idx <= endIdx; idx++)
									{
										calcChecksum ^= GpsInformation.Message[idx];
									}

									__disable_interrupt();
									RxOutIndexGps = outIndex;	// regardless if crc matches update out buffer pointer
									__enable_interrupt();

									if(calcChecksum == checksum)
									{
										GpsInformation.LookingForGpsMessages = 0;
										return 1;
									}
									else
									{
										idx = 0;
										gpsDecodeState = gpsFindStart;
									}
								}
								break;

		} //end switch(gpsDecodeState

		idx++;
		if (idx > (GGA_MAX_EXAMPLE+6) )
		{
			// error start over
			idx = 0;
			gpsDecodeState = gpsFindStart;

			__disable_interrupt();
			RxOutIndexGps = outIndex;
			__enable_interrupt();
		}
			// Setup to check next byte in RecvBufferGps
		outIndex = GetGpsIdx(outIndex,1);


	}//end while(outIndex


	GpsInformation.LookingForGpsMessages = 0;
	return(0);

}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:			Passed: A Start index in the GPS Receive buffer, count to be + from Start index
//								Currently negative cnts smaller than -5 not allowed
// RETURN/UPDATES:		1 - GPS information has been decoded and loaded into "gpsData"
//						0 - The sentence or data was bad
//---------------------------------------------------------------------------------------------
static int GpsDecode()
{
	  char field[10];
	  char venusBuff[10];
	  NMEAgetField(field, 0);

	  if (strcmp(field, "$GPGGA") == 0)
	  {
			NMEAgetField(field, 6);
			GPSinstance.fix = atoi(field);

			if (GPSinstance.fix == 1)
			{

				NMEAgetField(field, 2);

				GPSinstance.lat = (float)atof(field) / 100;

				NMEAgetField(field, 3);
				if (strcmp(field, "S") == 0)
					GPSinstance.lat = GPSinstance.lat * -1;

				NMEAgetField(field, 4);

				GPSinstance.lng = (float)atof(field) / 100;

				NMEAgetField(field, 5);
				if (strcmp(field, "W") == 0)
					GPSinstance.lng = GPSinstance.lng * -1;

				NMEAgetField(field, 7);
				GPSinstance.sfov = atoi(field);

				NMEAgetField(field, 9);
				GPSinstance.alt = (int)atoi(field);

				if ((int)GPSinstance.lat == 0)
				  return 0;

				if ((int)GPSinstance.lng == 0)
				  return 0;

				GPSinstance.hasGGA = 1;

				return 1;
			}
		}

		if (strcmp(field, "$GPRMC") == 0)
		{
			NMEAgetField(field, 2);

			if (strcmp(field, "A") != 0)
			  return 0;

			NMEAgetField(field, 1);

			if (field != 0)
			{
			  venusBuff[0] = field[0];
			  venusBuff[1] = field[1];
			  venusBuff[2] = '\0';
			  GPSinstance.hour = atoi(venusBuff);

			  venusBuff[0] = field[2];
			  venusBuff[1] = field[3];
			  venusBuff[2] = '\0';
			  GPSinstance.minute = atoi(venusBuff);

			  venusBuff[0] = field[4];
			  venusBuff[1] = field[5];
			  venusBuff[2] = '\0';
			  GPSinstance.second = atoi(venusBuff);
			}

			NMEAgetField(field, 9);

			if (field != 0)
			{
			  venusBuff[0] = field[0];
			  venusBuff[1] = field[1];
			  venusBuff[2] = '\0';
			  GPSinstance.day = atoi(venusBuff);

			  venusBuff[0] = field[2];
			  venusBuff[1] = field[3];
			  venusBuff[2] = '\0';
			  GPSinstance.month = atoi(venusBuff);

			  venusBuff[0] = field[4];
			  venusBuff[1] = field[5];
			  venusBuff[2] = '\0';
			  GPSinstance.year = atoi(venusBuff) + 2000;
			}

			GPSinstance.hasRMC = 1;

			return 1;
		}

	    return 0;
}

//---------------------------------------------------------------------------------------------
//
int ConfigureGPSNmeaOutput(void)
{
	if( SendNMEACmd() == 0)
		return 0;
	// ADD!! could Wait for reply? no need

	return 0;
}


//---------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
// RETURN/UPDATES:
//---------------------------------------------------------------------------------------------
static int SendNMEACmd(void)
{
	unsigned int timeout;

		// Wait until the Transmit Interrrupt is disabled (done in Interrupts)
	timeout = 0xffff;			// ~4-6instructions x 25MHz = ~10ms
	while(UCA0IE &UCTXIE)
	{

		if(--timeout == 0) {
			__disable_interrupt();
			UCA0IE &= ~UCTXIE;
			TxOutIndexGps = TxInIndexGps = 0;
			__enable_interrupt();
			break;
		}
	}

	TransBufferGps[0] = 0xA0;
	TransBufferGps[1] = 0xA1;
	TransBufferGps[2] = 0x00;
	TransBufferGps[3] = 0x09;
	TransBufferGps[4] = 0x08;
	TransBufferGps[5] = 0x05;
	TransBufferGps[6] = 0x00;
	TransBufferGps[7] = 0x00;
	TransBufferGps[8] = 0x00;
	TransBufferGps[9] = 0x05;
	TransBufferGps[10] = 0x00;
	TransBufferGps[11] = 0x00;
	TransBufferGps[12] = 0x01;	// save to SRAM & FLASH

/*	idx = checksum = 0;
	for(n=4;n<13;n++) {
		checksum ^= TransBufferGps[idx];
	}
*/
	TransBufferGps[13] = 0x09;//checksum;
	TransBufferGps[14] = 0x0D;
	TransBufferGps[15] = 0x0A;

	TxInIndexGps = 16;	// size of buffer to be sent out

	TxOutIndexGps = 0;

		// Setup the first character
	UCA0TXBUF = TransBufferGps[TxOutIndexGps++];
			// Enable Transmit interrupt who will send out the rest

	UCA0IE |= UCTXIE;

	return 0;
}



//---------------------------------------------------------------------------------------------
// DESCRIPTION:			Passed: A Start index in the GPS Receive buffer, count to be + from Start index
//								Currently negative cnts smaller than -5 not allowed
// RETURN/UPDATES:		New index in the GPS Rx buffer = (startIdx +/- cnt)
//---------------------------------------------------------------------------------------------
static unsigned int GetGpsIdx(unsigned int startIdx,signed int cnt)
{

	if( (startIdx >= GPS_UART_RX_BUFFER_SIZE) || (cnt >= GPS_UART_RX_BUFFER_SIZE) || (cnt < (-5)) ) {
			// ADD!!
		GpsInformation.BufferOverflow = 0;
		return(startIdx);
	}

	if(cnt < 0) {	// Negative
		if(((signed int)startIdx + cnt) < 0) {
			return((unsigned int)( (GPS_UART_RX_BUFFER_SIZE) + (cnt + startIdx) ));
		}
		else return((unsigned int)(startIdx + cnt));
	}
	else {			// Positive
		if( (startIdx + cnt) >= GPS_UART_RX_BUFFER_SIZE) {
					// Wrap buffer
			 return(unsigned int)( (cnt - 1) - ((GPS_UART_RX_BUFFER_SIZE - 1)-startIdx) );
		}
		else return(unsigned int)(startIdx + cnt);
	}
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:			Passed: A Start index in the GPS Receive buffer, count to be + from Start index
//								Currently negative cnts smaller than -5 not allowed
// RETURN/UPDATES:		New index in the GPS Rx buffer = (startIdx +/- cnt)
//---------------------------------------------------------------------------------------------
static void ResetGPSRxBuffer(void)
{
	__disable_interrupt();
	RxOutIndexGps = 0;
	RxInIndexGps =  1;
	RecvBufferGps[0] = 0;
	RecvBufferGps[1] = 0;
	GpsInformation.LookingForGpsMessages = 0;
	GpsInformation.BufferOverflow = 0;
	__enable_interrupt();
}

//------------------------------------------------------------------
// Parse fields from sentence in question
//
int NMEAgetField(char* bufff, int index)
{
	  int sentencePos = 0;
	  int fieldPos = 0;
	  int commaCount = 0;

	  while (sentencePos < GPS_UART_RX_BUFFER_SIZE)
	  {
			if (GpsInformation.Message[sentencePos] == ',')
			{
				  commaCount ++;
				  sentencePos ++;
			}

			if (commaCount == index)
			{
				  bufff[fieldPos] = GpsInformation.Message[sentencePos];
				  fieldPos++;
			}

			sentencePos++;
	  }

	  if (fieldPos >= 12)
	  {
			fieldPos = 0;
			bufff[fieldPos] = '\0';
			return 0;
	  }

	  bufff[fieldPos] = '\0';
	  return 1;
}



