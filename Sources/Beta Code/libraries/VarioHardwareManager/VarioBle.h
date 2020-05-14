#ifndef VARIO_BLE_H
#define VARIO_BLE_H

#include <HardwareConfig.h>

#ifdef HAVE_BLUETOOTH

#include "SimpleBLE.h"

#ifdef HAVE_GPS
#include <SerialNmea.h>
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
    bool init();
		bool update(double velocity, double position, double calibratedPosition);
		
		SimpleBLE ble;
		boolean lastSentence  = false;

#if defined(VARIOMETER_SENT_LXNAV_SENTENCE)
		LxnavSentence bluetoothNMEA;
#elif defined(VARIOMETER_SENT_LK8000_SENTENCE)
		LK8Sentence bluetoothNMEA;
#else
#error No bluetooth sentence type specified !
#endif
		
};

#endif //HAVE_BLUETOOTH

#endif //VARIO_BLE_H