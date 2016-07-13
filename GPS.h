/*****< GPS.h >****************************************************************/
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   03/18/13  T. Nixon       u-Blox UC530M GPS module						  */
/*	 06/19/15  T. Nixon       add to gps struct, add power save				  */
/******************************************************************************/
#ifndef __GPS_H__
#define __GPS_H__


#define GPS_UART_RX_BUFFER_SIZE		512
#define GPS_UART_TX_BUFFER_SIZE		64


#define GGA_MIN_EXAMPLE				"$GPGGA,211205.310,,,,,0,0,,,M,,M,,*4F\r\0"
#define GGA_MIN_SIZE				strlen(GGA_MIN_EXAMPLE)

#define GGA_MAX_EXAMPLE				"$GPGGA,114353.000,6016.3245,N,02458.3270,E,v,ss,d.dd,hh.hh,M,-gg.g,M,a.aaa,xxxx*hh\r\0"
#define GGA_MAX_SIZE				strlen(GGA_MAX_EXAMPLE)	//

#define RMC_MAX_EXAMPLE				"$GPRMC,114353.000,6016.3245,V,02458.3270,N,11408.5743,W,000.0,343.9,210616,,,A*hh\r\0"
#define RMC_MAX_SIZE				strlen(RMC_MAX_EXAMPLE)	//

#define VTG_MIN_EXAMPLE				"$GNVTG,,,,,,,,,N*2E\r\0"
#define VTG_MIN_SIZE				strlen(VTG_MIN_EXAMPLE)

#define VTG_MAX_EXAMPLE				"$GPVTG,00.00,T,,M,0.034,N,0.064,K,D*23\r\0"
#define VTG_MAX_SIZE				strlen(VTG_MAX_EXAMPLE)

typedef struct //_gpsInformation_t
{
	int				decoding;
	int				LookingForGpsMessages;
	int				BufferOverflow;
	unsigned char	Message[127];
	//Put rest of GPS variables here

	unsigned char	buffer[20];

} GpsInformation_t;


//#ifndef _GPS_C_
extern GpsInformation_t GpsInformation;
//#endif

typedef struct //StringInfo_t
{
	float		lat;
	float		lng;

	int			hour;
	int			minute;
	int			second;
	int			month;
	int			year;
	int			day;

	int 		fix;
	int 		alt;
	int 		sfov;

	int hasGGA;
	int hasRMC;

} StringInfo_t;

StringInfo_t GPSinstance;

StringInfo_t storedInstances[5];
static int instanceIndex;

extern long GpsStateCountdown;

extern void pollGPS();
static int GpsDecode();
static int NMEAgetField(char* bufff, int index);

void storeGPSInstance();
void resetGpsInstance();

extern void ConfigureGPSSerialPort();
extern int ConfigureGPSNmeaOutput(void);
extern int GpsMessageRetrieve(void);


#endif

