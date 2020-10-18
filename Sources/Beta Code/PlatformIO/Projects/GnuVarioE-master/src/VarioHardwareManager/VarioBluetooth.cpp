/* VarioBle -- 
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
 *                          VarioBle                                             *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0                                                                        *
 *    1.0.1  09/06/20   Ajout GnuSettings.VARIOMETER_SENT_LXNAV_SENTENCE         *
 *											Ajout GnuSettings.BLUETOOTH_SEND_CALIBRATED_ALTITUDE     *                                          
 *                                                                               *
 *********************************************************************************
 */
#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#include <VarioLog.h>

#ifndef ARDUINOTRACE_ENABLE
#ifdef HARDWARE_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif
#endif

#ifndef ARDUINOTRACE_SERIAL
#define ARDUINOTRACE_SERIAL SerialPort
#endif

#include <ArduinoTrace.h>

#ifdef HAVE_BLUETOOTH

#include "VarioBluetooth.h"

//#include "SimpleBLE.h"
#include <VarioSettings.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// ******************************
bool VarioBluetooth::init()
// ******************************
{
	if (GnuSettings.VARIOMETER_ENABLE_BT)
	{
		TRACE();
		return true;
	}
	else
	{
		TRACE();
		return false;
	}
}

// ******************************
bool VarioBluetooth::update(double velocity, double position, double calibratedPosition)
// ******************************
{
	// ********************
	// * update bluetooth *
	// ********************
#ifdef HAVE_GPS
	/* in priority send vario nmea sentence */
	if (GnuSettings.VARIOMETER_SENT_LXNAV_SENTENCE == LK8000_SENTENCE)
	{
		if (bluetoothNMEA_Lk.available())
		{
			while (bluetoothNMEA_Lk.available())
			{
				serialNmea.write(bluetoothNMEA_Lk.get());
			}
			serialNmea.release();
			return true;
		}
	}
	else
	{
		if (bluetoothNMEA_Lx.available())
		{
			while (bluetoothNMEA_Lx.available())
			{
				serialNmea.write(bluetoothNMEA_Lx.get());
			}
			serialNmea.release();
			return true;
		}
	}
#else  //!HAVE_GPS
	/* check the last vario nmea sentence */
	if (millis() - lastVarioSentenceTimestamp > VARIOMETER_SENTENCE_DELAY)
	{
		lastVarioSentenceTimestamp = millis();

		if (GnuSettings.VARIOMETER_SENT_LXNAV_SENTENCE == LK8000_SENTENCE)
		{
			//#ifdef VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE
			if (GnuSettings.BLUETOOTH_SEND_CALIBRATED_ALTITUDE)
				bluetoothNMEA_Lk.begin(calibratedPosition, velocity);
			//#else
			else
				bluetoothNMEA_Lk.begin(position, velocity);
			//#endif
			while (bluetoothNMEA_Lk.available())
			{
				serialNmea.write(bluetoothNMEA_Lk.get());
			}
		}
		else
		{
			//#ifdef VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE
			if (GnuSettings.BLUETOOTH_SEND_CALIBRATED_ALTITUDE)
				bluetoothNMEA_Lx.begin(calibratedPosition, velocity);
			//#else
			else
				bluetoothNMEA_Lx.begin(position, velocity);
			//#endif
			while (bluetoothNMEA_Lx.available())
			{
				serialNmea.write(bluetoothNMEA_Lx.get());
			}
		}
		return true;
	}
}
#endif //!HAVE_GPS
	return false;
}

#endif //HAVE_BLUETOOTH