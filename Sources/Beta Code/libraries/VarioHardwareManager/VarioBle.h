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
 *                           VarioBle                                            *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0                                                                        *
 *    1.0.1    16/06/20 Trame BT dans json                                       *
 *                                                                               *
 *********************************************************************************
 */
 
#ifndef VARIO_BLE_H
#define VARIO_BLE_H

#include <Arduino.h>

#include <HardwareConfig.h>

#ifdef HAVE_BLE

//#include "SimpleBLE.h"

#ifdef HAVE_GPS
// #include <SerialNmea.h>
#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <GPSSentence.h>
#endif //HAVE_GPS

class VarioBle
{
public:
    //   VarioAlim();
    bool isSending;
    bool init();
    bool update(double velocity, double position, double calibratedPosition);
    bool sendTrame(char *trame);

    //		SimpleBLE ble;
    boolean lastSentence = false;

/*#if defined(VARIOMETER_SENT_LXNAV_SENTENCE)
		LxnavSentence bluetoothNMEA;
#elif defined(VARIOMETER_SENT_LK8000_SENTENCE)
		LK8Sentence bluetoothNMEA;
#else
#error No bluetooth sentence type specified !
#endif*/
		
		LxnavSentence bluetoothNMEA_Lx;
		LK8Sentence   bluetoothNMEA_Lk;		
};

#endif //HAVE_BLE

#endif //VARIO_BLE_H