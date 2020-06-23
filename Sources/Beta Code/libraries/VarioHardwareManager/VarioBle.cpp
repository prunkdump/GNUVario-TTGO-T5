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

#ifdef HAVE_BLE

#include <NimBLEDevice.h>

#include "VarioBle.h"

//#include "SimpleBLE.h"
#include <VarioSettings.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
    /***************** New - Security handled here ********************
  ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest()
    {
        Serial.println("Server PassKeyRequest");
        return 123456;
    }

    bool onConfirmPIN(uint32_t pass_key)
    {
        Serial.print("The passkey YES/NO number: ");
        Serial.println(pass_key);
        return true;
    }

    void onAuthenticationComplete(ble_gap_conn_desc desc)
    {
        Serial.println("Starting BLE work!");
    }
    /*******************************************************************/
};

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0)
        {
            Serial.println("*********");
            Serial.print("Received Value: ");
            for (int i = 0; i < rxValue.length(); i++)
                Serial.print(rxValue[i]);

            Serial.println();
            Serial.println("*********");
        }
    }
};

// ******************************
bool VarioBle::init()
// ******************************
{
    if (GnuSettings.VARIOMETER_ENABLE_BT)
    {
        TRACE();
        BLEDevice::init("NimESP32");
        //BLEDevice::getAddress(); // Retrieve our own local BD BLEAddress
        pServer = BLEDevice::createServer();
        pServer->setCallbacks(new MyServerCallbacks());

        // Create the BLE Service
        BLEService *pService = pServer->createService(SERVICE_UUID);

        // Create a BLE Characteristic
        pTxCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID_TX,
            /******* Enum Type NIMBLE_PROPERTY now *******      
                                        BLECharacteristic::PROPERTY_NOTIFY
                                        );
                                    **********************************************/
            NIMBLE_PROPERTY::NOTIFY);
        /** Add properties the same way as characteristics now **/
        pTxCharacteristic->createDescriptor("2902" /** , NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE **/);

        BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID_RX,
            /******* Enum Type NIMBLE_PROPERTY now *******       
                                            BLECharacteristic::PROPERTY_WRITE
                                            );
                                    *********************************************/
            NIMBLE_PROPERTY::WRITE);

        pRxCharacteristic->setCallbacks(new MyCallbacks());

        // Start the service
        pService->start();

        // Start advertising
        pServer->getAdvertising()->start();
        Serial.println("Waiting a client connection to notify...");
        return true;
    }
    else
    {
        TRACE();
        return false;
    }
}

// ******************************
bool VarioBle::update(double velocity, double position, double calibratedPosition)
// ******************************
{
    // ********************
    // * update bluetooth *
    // ********************

    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        // delay(10);                  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }

#ifdef HAVE_GPS
    if (deviceConnected)
    {
					//il faudrait envoyer la trame GPS
					//todo

			if (GnuSettings.VARIOMETER_SENT_LXNAV_SENTENCE == LK8000_SENTENCE) {
				if (bluetoothNMEA_Lk.available())
					//if (bluetoothNMEA.available())
				{
					//la trame LXNAV ou LX8000
					char trame[50] = "";
					uint8_t i = 0;
					SerialPort.println("LX8000 - available");
					SerialPort.print("LX8000 - trame");
	//            while (bluetoothNMEA.available())
					while (bluetoothNMEA_Lk.available())
					{
						if (i > 15)
						{
								trame[i] = '\0';
								sendTrame(trame);
								i = 0;
						}
						uint8_t charTrame = bluetoothNMEA_Lk.get();
						trame[i] = charTrame;

						SerialPort.print(charTrame);
						i++;
					}
					trame[i] = '\0';
					sendTrame(trame);

					//la trame GPS

					SerialPort.println("LX8000 - fin trame");
					return true;
				}
			} else {
				
				if (bluetoothNMEA_Lx.available())
					//if (bluetoothNMEA.available())
				{
					//la trame LXNAV ou LX8000
					char trame[50] = "";
					uint8_t i = 0;
					SerialPort.println("LXNAV - available");
					SerialPort.print("LXNAV - trame");
	//            while (bluetoothNMEA.available())
					while (bluetoothNMEA_Lx.available())
					{
						if (i > 15)
						{
								trame[i] = '\0';
								sendTrame(trame);
								i = 0;
						}
						uint8_t charTrame = bluetoothNMEA_Lk.get();
						trame[i] = charTrame;

						SerialPort.print(charTrame);
						i++;
					}
					trame[i] = '\0';
					sendTrame(trame);

					//la trame GPS

					SerialPort.println("LXNAV - fin trame");
					return true;
				}		
			}
		}
#else //!HAVE_GPS
    /* check the last vario nmea sentence */
    if (millis() - lastVarioSentenceTimestamp > VARIOMETER_SENTENCE_DELAY)
    {
        lastVarioSentenceTimestamp = millis();
#ifdef VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE
        bluetoothNMEA_Lx.begin(calibratedPosition, velocity);
#else
        bluetoothNMEA_Lx.begin(position, velocity);
#endif
        while (bluetoothNMEA_Lx.available())
        {
            serialNmea.write(bluetoothNMEA_Lx.get());
        }
        return true;
    }
#endif //!HAVE_GPS

    return false;
}

bool VarioBle::sendTrame(char *trame)
{
    pTxCharacteristic->setValue(trame);
    pTxCharacteristic->notify();
}
#endif //HAVE_BLE
