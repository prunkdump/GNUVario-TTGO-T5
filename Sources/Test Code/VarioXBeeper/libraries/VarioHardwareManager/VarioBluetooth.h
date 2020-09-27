/* VarioBluetooth -- 
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
 *                          VarioBluetooth                                             *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0                                                                        *
 *    1.0.1  09/06/20   Ajout GnuSettings.VARIOMETER_SENT_LXNAV_SENTENCE         *
 *											Ajout GnuSettings.BLUETOOTH_SEND_CALIBRATED_ALTITUDE     *                                          
 *                                                                               *
 *********************************************************************************
 */

#ifndef VARIO_BLUETOOTH_H
#define VARIO_BLUETOOTH_H

#include <HardwareConfig.h>

#ifdef HAVE_BLUETOOTH

//#include "SimpleBLE.h"

#ifdef HAVE_GPS
#include <SerialNmea.h>
#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <GPSSentence.h>
#endif //HAVE_GPS

class VarioBluetooth
{
public:
	bool init();
	bool update(double velocity, double position, double calibratedPosition);

	boolean lastSentence = false;

	LxnavSentence bluetoothNMEA_Lx;
	LK8Sentence bluetoothNMEA_Lk;
};

#endif //HAVE_BLUETOOTH

#endif //VARIO_BLUETOOTH_H