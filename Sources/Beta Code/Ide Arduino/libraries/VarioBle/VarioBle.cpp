/*  VarioBle -- BLE UART library
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

#include "VarioBle.h"

#include <Arduino.h>
/*
#include <VarioSettings.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>
*/

#include <NimBLEDevice.h>
#include "freertos/FreeRTOS.h"

/* init static variables */
unsigned long VarioBle::lastNotifyTimestamp = 0;
BLEServer *VarioBle::server = NULL;
BLEService *VarioBle::service = NULL;
BLECharacteristic *VarioBle::characteristic = NULL;

int VarioBle::deviceCount = 0;
uint8_t VarioBle::charBuffer[VARIO_BLE_BUFFER_LIMIT];
int VarioBle::pos = 0;
bool VarioBle::needAdvertising = false;

VarioBleServerCallbacks varioBleServerCallbacks;

/* init the BLE service */
void VarioBle::init(const std::string name)
{

  /* init device */
  BLEDevice::init(name);

  /* init server */
  server = BLEDevice::createServer();
  server->setCallbacks(&varioBleServerCallbacks);

  /* create the BLE Service */
  service = server->createService(VARIO_BLE_SERVICE_UUID);

  /* create the BLE Characteristic */
  characteristic = service->createCharacteristic(VARIO_BLE_CHARACTERISTIC_UUID_TX, NIMBLE_PROPERTY::NOTIFY);
  //characteristic->createDescriptor("2902");

  /* start the service */
  service->start();

  /* start advertising */
  server->getAdvertising()->addServiceUUID(service->getUUID());
  server->getAdvertising()->start();
  resetBuffer();
}

void VarioBle::begin(const std::string name)
{
  init(name);
}

void VarioBle::writeCharacteristic(int length)
{

  /* check last notify */
  unsigned long ellapsedTime = millis() - lastNotifyTimestamp;
  if (ellapsedTime <= VARIO_BLE_NOTIFY_MIN_PERIOD)
  {

    /* wait */
    const TickType_t delay = (VARIO_BLE_NOTIFY_MIN_PERIOD + 1 - ellapsedTime) / portTICK_PERIOD_MS;
    vTaskDelay(delay);
  }

  /* write and notify */
  characteristic->setValue(charBuffer, length);
  characteristic->notify();

  /* reset */
  resetBuffer();
}

/* write a char to the TX buffer */
void VarioBle::write(char c)
{

  /* check if we are advertising */
  if (needAdvertising)
  {

    /* need to wait 500 ms */
    unsigned long ellapsedTime = millis() - lastNotifyTimestamp;
    if (ellapsedTime < VARIO_BLE_ADVERTISE_MIN_PERIOD)
    {
      return;
    }

    /* start advertising */
    server->startAdvertising();
    needAdvertising = false;

    /* reset */
    resetBuffer();
  }

  /* check if we have clients */
  if (deviceCount == 0)
  {
    return;
  }

  /* write a char to buffer */
  charBuffer[pos] = c;
  pos++;

  /* check if we reach buffer end */
  if (pos == VARIO_BLE_BUFFER_LIMIT)
  {

    /* write string */
    writeCharacteristic(VARIO_BLE_BUFFER_LIMIT);
  }
}

/* force data transfert */
void VarioBle::flush()
{

  /* check we have data */
  if (deviceCount == 0 || pos == 0)
  {
    return;
  }

  /*!!! need to be checked */
  /* end string */
  charBuffer[pos] = '\0';

  /* send data : without the '\0' */
  writeCharacteristic(pos);
}

void VarioBle::resetBuffer()
{

  lastNotifyTimestamp = millis();
  pos = 0;
}

/* new device is connected */
void VarioBle::deviceConnect()
{

  deviceCount++;
}

/* a device disconnect */
void VarioBle::deviceDisconnect()
{

  deviceCount--;
  if (deviceCount == 0)
  {
    needAdvertising = true;
    lastNotifyTimestamp = millis();
  }
}
