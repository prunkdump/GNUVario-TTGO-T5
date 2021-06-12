/* VarioGps -- 
 *
 * Copyright 2020 MichelPa / Jpg63
 * 
 * This file is part of GnuVario-E.
 *
 * ToneHAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ToneHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 *********************************************************************************
 *                                                                               *
 *                           VarioHardwareManager                                *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0                                                                        *
 *    1.0.1  25/12/20   Modif getCap                                             *
 *                                                                               *
 *********************************************************************************
 */
#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#ifdef HAVE_GPS

#include "VarioGps.h"

#include <SerialNmea.h>
#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <GPSSentence.h>

// *********************
void VarioGps::init()
// *********************
{
	serialNmea.begin(9600, true);
#ifdef GPS_DEBUG
	SerialPort.println("SerialNmea init");
#endif //GPS_DEBUG
}

// *********************
bool VarioGps::update(Kalmanvert kalmanvert, bool *lastSentence)
// *********************
{
	/* try to lock sentences */
	if (serialNmea.lockRMC())
	{

#ifdef GPS_DEBUG
		SerialPort.println("mneaParser : beginRMC");
#endif //GPS_DEBUG

		nmeaParser.beginRMC();
	}
	else if (serialNmea.lockGGA())
	{

#ifdef GPS_DEBUG
		SerialPort.println("mneaParser : beginGGA");
#endif //GPS_DEBUG

		nmeaParser.beginGGA();
#ifdef HAVE_BLUETOOTH
		*lastSentence = true;
#endif //HAVE_BLUETOOTH
#ifdef HAVE_SDCARD
		/* start to write IGC B frames */
		if (!GnuSettings.NO_RECORD)
			igcSD.writePosition(kalmanvert);
#endif //HAVE_SDCARD
	}

	/* parse if needed */
	if (nmeaParser.isParsing())
	{

#ifdef GPS_DEBUG
		SerialPort.println("mneaParser : isParsing");
#endif //GPS_DEBUG

#ifdef SDCARD_DEBUG
		SerialPort.print("writeGGA : ");
#endif //SDCARD_DEBUG

		while (nmeaParser.isParsing())
		{
			uint8_t c = serialNmea.read();

			/* parse sentence */
			nmeaParser.feed(c);

#ifdef NMEAPARSER_DEBUG
			char tmpchar = c;
			SerialPort.print(tmpchar);
#endif //NMEAPARSER_DEBUG

#ifdef HAVE_SDCARD
			/* if GGA, convert to IGC and write to sdcard */
			if (sdcardState == SDCARD_STATE_READY && nmeaParser.isParsingGGA())
			{
				igc.feed(c);
				/*          while( igc.available() ) {
					fileIgc.write( igc.get() );
				}*/
				if (!GnuSettings.NO_RECORD)
					igcSD.writeGGA();
			}
#endif //HAVE_SDCARD
		}

#ifdef NMEAPARSER_DEBUG
		SerialPort.println("");
#endif //NMEAPARSER_DEBUG

		serialNmea.release();
#ifdef HAVE_SDCARD
		fileIgc.flush();
#endif //HAVE_SDCARD
#ifdef SDCARD_DEBUG
		SerialPort.println("");
#endif //SDCARD_DEBUG

		return true;
	}
	return false;
}

#endif //HAVE_GPS