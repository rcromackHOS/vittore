/*
 * MAX31855.c
 *
 *  Created on: Jun 27, 2016
 *      Author: LightTower
 */

#include "MAX31855.h"
#include "Hardware.h"
#include "config.h"

//--------------------------------------------------------------------



//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Query the Thermocouple for it's data, parse it into a read-able format
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void getThermocoupleData()
{
	// poll the SPI
	pollMAX31855();

	//----------------------------------

	// Internal temp is sent with the probe temp, just need to parse it out
    internal = (TCRXVALUE >> 4) & 0x7FF;
	internal *= 0.0625;

	if ((TCRXVALUE >> 4) & 0x800)
	    internal *= -1;

	//----------------------------------

	// error checking bit. typically open probe. set an error value
	if (TCRXVALUE & 0x7)
	{
		TCRXVALUE = 0;
		centigrade = -300.0;
	}

	if (TCRXVALUE != 0)
	{
		// check for negative value
		if (TCRXVALUE & 0x80000000)
		    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
			TCRXVALUE = 0xFFFFC000 | ((TCRXVALUE >> 18) & 0x00003FFFF);

		// Positive value, just drop the lower 18 bits.
		else
			TCRXVALUE >>= 18;

		centigrade = TCRXVALUE;

		// LSB = 0.25 degrees C
		centigrade *= 0.25;
	}

	VALUE_BAT_TEMP = centigrade;
	VALUE_INTERNAL2_TEMP = internal;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		poll the TC IC, read the incoming data on the port, load it into a buffer
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void pollMAX31855()
{
	P9OUT &= ~THERM_CS;
	P9OUT &= ~THERM_SPI_CLK;

	_delay_ms(1);

	int i;
	for (i = 31; i >= 0; i--)
	{
		P9OUT &= ~THERM_SPI_CLK;
	    _delay_ms(1);

	    TCRXVALUE <<= 1;

	    if ((P9IN & THERM_SPI_MISO) > 0)
	    	TCRXVALUE |= 1;

	    P9OUT |= THERM_SPI_CLK;
	    _delay_ms(1);
	}

	P9OUT |= THERM_CS;
}

//---------------------------------------------------------------------------------------------
// DESCRIPTION:		Simulates a clock pulse
//
// RETURN/UPDATES:	void
//---------------------------------------------------------------------------------------------
void _delay_ms(int t)
{
   int i = 0;
   for(i = 0; i < (50 * t); i++){ }
}

//--------------------------------------------------------------------
