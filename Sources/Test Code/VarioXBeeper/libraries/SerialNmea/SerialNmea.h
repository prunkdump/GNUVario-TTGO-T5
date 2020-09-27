/* SerialNmea -- Read serial while keeping only valid NMEA sentences
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

#ifndef SERIAL_NMEA_H
#define SERIAL_NMEA_H

#include <Arduino.h>
#include <VarioSettings.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#ifdef HAVE_BLUETOOTH
#ifdef HAVE_BLE
#include "VarioBle.h"
#else
#include "BluetoothSerial.h"
#endif
#endif

/********************/
/* output parameter */
/********************/
#define SERIAL_NMEA_CIRCULAR_SERIAL_OUTPUT

#ifndef _DEBUGCONFIG_H_
//#define SERIAL_NMEA_SERIAL_OUTPUT
//#define SERIAL_NMEA_BLUETOOTH_OUTPUT
#endif //DEBUGCONFIG_H_

/***********************/
/* gps uart parameters */
/***********************/
#ifndef SERIAL_NMEA_UART_NUM
#define SERIAL_NMEA_UART_NUM 1
#endif

#ifndef SERIAL_NMEA_UART_BUFF_SIZE
#define SERIAL_NMEA_UART_BUFF_SIZE 256
#endif

#ifndef SERIAL_NMEA_RX_PIN
//#define SERIAL_NMEA_RX_PIN 15
#define SERIAL_NMEA_RX_PIN 19
#endif

#ifndef SERIAL_NMEA_TX_PIN
#define SERIAL_NMEA_TX_PIN 13
#endif

/* scheduler task parameters */
#define SERIAL_NMEA_STACK_SIZE 2000
#define SERIAL_NMEA_CORE 1
#define SERIAL_NMEA_PRIORITY 2

/*********************/
/* the GPS sentences */
/*********************/

/* the nmea tags */
#ifndef NMEA_TAG_SIZE
#define NMEA_TAG_SIZE 5
#endif

#ifndef NMEA_RMC_TAG
#define NMEA_RMC_TAG "GPRMC"
#endif

#ifndef NMEA_GGA_TAG
#define NMEA_GGA_TAG "GPGGA"
#endif

/* the maximum silent time before the timestamp is updated */
#define SERIAL_NMEA_MAX_SILENT_TIME 5

/*********************/
/* serial nmea class */
/*********************/

#define SERIAL_NMEA_BUFFER_SIZE 128

class SerialNmea
{

public:
  void begin(unsigned long baud, bool rxEnable);
  bool lockRMC(void);
  bool lockGGA(void);
  void addTagToRead(void); //by default the tag is not included, call this between lock and read
  uint8_t read(void);
  void lock(void); //lock rx completely to write manually
  void write(uint8_t c);
  void release(void);
  unsigned long getReceiveTimestamp(void);
  unsigned long getLastReceiveTimestamp(void);

  void rxCompleteVect(uint8_t c);
  void udrEmptyVect(void);
  uint8_t buffer[SERIAL_NMEA_BUFFER_SIZE];

private:
  uart_t *uart;
  TaskHandle_t schedulerTaskHandler;
  volatile bool txEnabled;
  volatile uint8_t state;
  volatile uint8_t txTail;
  volatile uint8_t txHead;
  volatile uint8_t writePos;
  volatile uint8_t rmcPos;
  volatile uint8_t ggaPos;
  volatile uint8_t nmeaPos;
  uint8_t readPos;
  volatile int8_t nmeaParseStep;
  volatile uint8_t nmeaParity;
  volatile uint8_t parityTag;
  volatile unsigned long receiveTimestamp;
  volatile unsigned long lastReceiveTimestamp;
  static void uartScheduler(void *param);
  static void serialWrite(uint8_t c);
};

extern SerialNmea serialNmea;

#if defined(SERIAL_NMEA_BLUETOOTH_OUTPUT)
#if defined(HAVE_BLE)
extern VarioBle SerialBT;
#elif defined(HAVE_BLUETOOTH)
extern BluetoothSerial SerialBT;
#endif
#endif

#endif
