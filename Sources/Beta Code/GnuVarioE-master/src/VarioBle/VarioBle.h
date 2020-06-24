/* VarioBle -- BLE UART library
 *
 * Copyright 2016-2019 Baptiste PELLEGRIN
 * 
 * This file is part of GNUVario.
 *
 * GNUVario is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNUVario is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef VARIO_BLE_H
#define VARIO_BLE_H

#include <Arduino.h>
/*
#include <VarioSettings.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>
*/

#include <NimBLEDevice.h>

#define VARIO_BLE_SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define VARIO_BLE_CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define VARIO_BLE_BUFFER_LIMIT 20
#define VARIO_BLE_NOTIFY_MIN_PERIOD 10
#define VARIO_BLE_ADVERTISE_MIN_PERIOD 500

class VarioBle
{

public:
  static void init(const std::string name);
  static void begin(const std::string name);
  static void write(char c);
  static void flush();
  static void resetBuffer();
  static void deviceConnect();
  static void deviceDisconnect();

private:
  static void writeCharacteristic(int length);
  static unsigned long lastNotifyTimestamp;
  static BLEServer *server;
  static BLEService *service;
  static BLECharacteristic *characteristic;
  static int deviceCount;
  static uint8_t charBuffer[VARIO_BLE_BUFFER_LIMIT];
  static int pos;
  static bool needAdvertising;
};

class VarioBleServerCallbacks : public BLEServerCallbacks
{

  void onConnect(BLEServer *pServer)
  {
    VarioBle::deviceConnect();
  }

  void onDisconnect(BLEServer *pServer)
  {
    VarioBle::deviceDisconnect();
  }
};

extern VarioBleServerCallbacks varioBleServerCallbacks;

#endif //VARIO_BLE_H
