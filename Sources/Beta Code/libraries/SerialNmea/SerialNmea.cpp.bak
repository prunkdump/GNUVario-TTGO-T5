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

#include <SerialNmea.h>

#include <Arduino.h>

#include <esp32-hal-uart.h>
#include "freertos/queue.h"
#include "soc/uart_struct.h"
#include "BluetoothSerial.h"

#include <HardwareConfig.h>
#include <DebugConfig.h>
#include <VarioSettings.h>

#if defined(SERIAL_NMEA_BLUETOOTH_OUTPUT) && defined(HAVE_BLUETOOTH)
BluetoothSerial SerialBT;
#endif

struct UartInternal {
  uart_dev_t* dev;
#if !CONFIG_DISABLE_HAL_LOCKS
  xSemaphoreHandle lock;
#endif
  uint8_t num;
  xQueueHandle queue;
  intr_handle_t intr_handle;
};

const char rmcTag[] PROGMEM = { NMEA_RMC_TAG };
const char ggaTag[] PROGMEM = { NMEA_GGA_TAG };

#define serialState_set(bit) state |= (1 << bit)
#define serialState_unset(bit) state &= ~(1 << bit)
#define serialState_isset(bit) (state & (1 << bit))

#define sbi(reg, bit) reg |= (1 << bit)
#define cbi(reg, bit) reg &= ~(1 << bit)

#define LOCKED 0
#define SENTENCE_LOCKED 1
#define LOCKED_RMC 2
#define LOCKED_GGA 3
#define RMC_TAG_FOUND 4
#define GGA_TAG_FOUND 5
#define RMC_FOUND 6
#define GGA_FOUND 7

/* serial class declaration */
SerialNmea serialNmea;

void SerialNmea::begin(unsigned long baud, bool rxEnable) {
   
  /*********************/
  /* init rx/tx buffer */
  /*********************/
  state = 0;
  txTail = 0;
  txHead = 0;
  writePos = 0;
  nmeaParseStep = -1;
  txEnabled = false;

#if defined(SERIAL_NMEA_BLUETOOTH_OUTPUT) && defined(HAVE_BLUETOOTH)
  /******************/
  /* init bluetooth */
  /******************/
	
  if (GnuSettings.VARIOMETER_ENABLE_BT)
  {
#ifdef BT_DEBUG
//    SerialPort.setDebugOutput(true);
    SerialPort.print("ESP32 SDK: ");
    SerialPort.println(ESP.getSdkVersion());
#endif //BT_DEBUG
	
		SerialBT.begin(VARIOMETER_MODEL);
  }	
#endif

  /*************/
  /* init uart */
  /*************/
  uart = uartBegin(SERIAL_NMEA_UART_NUM, baud, SERIAL_8N1, SERIAL_NMEA_RX_PIN, SERIAL_NMEA_TX_PIN, SERIAL_NMEA_UART_BUFF_SIZE, false);

  /********************/
  /* create uart task */
  /********************/
  xTaskCreatePinnedToCore(uartScheduler, "US", SERIAL_NMEA_STACK_SIZE, this, SERIAL_NMEA_PRIORITY, &schedulerTaskHandler, SERIAL_NMEA_CORE);
}




void SerialNmea::rxCompleteVect(uint8_t c) {

  /* update timestamps */
  unsigned long currentTime = millis();
  if( currentTime - lastReceiveTimestamp > SERIAL_NMEA_MAX_SILENT_TIME ) {
    receiveTimestamp = currentTime;
  }
  lastReceiveTimestamp = currentTime;

  /* check '$' that reset write pos */
  if( c == '$' ) {
    /* reset write pos */
    writePos = txHead;
    nmeaParseStep = 0;
  }   

  /* check if write is needed */
  if( serialState_isset(LOCKED) || nmeaParseStep == -1 ) {
    return;
  }

  /*******************/
  /* write to buffer */
  /*******************/

  /* check if we can write to the buffer, we never use the last byte of the buffer */
  uint8_t nextPos = (writePos + 1) % SERIAL_NMEA_BUFFER_SIZE;
  
  if( nextPos == txTail ||
      ( serialState_isset(SENTENCE_LOCKED) && writePos == readPos ) || //never overwrite reading sentence
      ( serialState_isset(RMC_FOUND) && writePos == rmcPos ) ||        //never overwrite same sentence
      ( serialState_isset(GGA_FOUND) && writePos == ggaPos ) ) {
    /* one character is not saved so stop parser */
    nmeaParseStep = -1;
    return;
  }
  
  /* write */
  buffer[writePos] = c;
  writePos = nextPos;

  /***************/
  /* nmea parser */
  /***************/

  /* start nmea parser */
  if( c == '$' ) {
    /* prepare nmea parser */
    serialState_set(RMC_TAG_FOUND);
    serialState_set(GGA_TAG_FOUND);
    nmeaParity = '*';  //we will add the '*' on parity
    return;
  }

  /* check if we need to compute parity */
  if( nmeaParseStep <= NMEA_TAG_SIZE + 1 ) { //before or on '*'
    nmeaParity ^= c;
  } else {
    if( nmeaParseStep <= NMEA_TAG_SIZE + 3 ) { //on the parity tag
      parityTag <<= 4;
      if( c <= '9' ) {
	parityTag += c - '0';
      } else {
	parityTag += c - 'A' + 10;
      }
    }
    nmeaParseStep++;
  }
  
  /* parse nmea tag */
  if( nmeaParseStep < NMEA_TAG_SIZE ) { // on the nmea tag
    if( c != pgm_read_byte_near(rmcTag + nmeaParseStep) ) {
      serialState_unset(RMC_TAG_FOUND);
    }
    if( c != pgm_read_byte_near(ggaTag + nmeaParseStep) ) {
      serialState_unset(GGA_TAG_FOUND);
    }
    nmeaParseStep++;
    return;
  }

  /* check nmea tag */
  else if( nmeaParseStep == NMEA_TAG_SIZE ) { //on the ',' just after the tag
   
    if( c != ',' || (! serialState_isset(RMC_TAG_FOUND) && ! serialState_isset(GGA_TAG_FOUND) ) ) {
      nmeaParseStep = -1; //bad sensence
    }

    else {
      /* save pointer */
      nmeaPos = nextPos;
      nmeaParseStep++;
    }
    return;
  }

  /* wait for parity */
  else if( nmeaParseStep == NMEA_TAG_SIZE + 1 ) { //before or on dot
   
    if( c == '*' ) {
      parityTag = 0;
      nmeaParseStep++;
      return;
    }
  }

  /* check parity */
  else {

    /* wait for end with '\n' */
    if( c == '\n' ) {
      
      if( nmeaParity != parityTag ) {
	nmeaParseStep = -1; //bad sensence
      } else {
	/*********************************/
	/* we have a new nmea sentence ! */
	/*********************************/

	/* save pointer */
	if( serialState_isset(RMC_TAG_FOUND) ) {
	  rmcPos = nmeaPos;
	  serialState_set(RMC_FOUND);
	} else { //this is gga
	  ggaPos = nmeaPos;
	  serialState_set(GGA_FOUND);
	}
	
	/* send with serial */
	txHead = nextPos;
	txEnabled = true;
      }
    }
  }
}


void SerialNmea::udrEmptyVect(void) {

  /* send */
  uint8_t c = buffer[txTail];
  txTail = (txTail + 1) % SERIAL_NMEA_BUFFER_SIZE;

  serialWrite(c);

  /* if no remaining bytes, stop interrupt */
  if (txHead == txTail) {
    txEnabled = false;
  }
  
}

void SerialNmea::uartScheduler(void* param) {

  /* get queue */
  UartInternal* uartInternal = reinterpret_cast<UartInternal*>(serialNmea.uart);
  xQueueHandle uartRxQueue = uartInternal->queue;

  while( true ) {

    /* wait for rx */
    uint8_t c;
    xQueueReceive(uartRxQueue, &c, portMAX_DELAY);

    /* check rx */
    serialNmea.rxCompleteVect(c);

    /* check tx */
    while( serialNmea.txEnabled ) {
      serialNmea.udrEmptyVect();
    }
  }
}

void SerialNmea::serialWrite(uint8_t c) {

#ifdef SERIAL_NMEA_CIRCULAR_SERIAL_OUTPUT
  /* write to uart TX */
  uartWrite(serialNmea.uart, c);
#endif

#if defined(SERIAL_NMEA_BLUETOOTH_OUTPUT) && defined(HAVE_BLUETOOTH)
  /* write to bluetooth */
  SerialBT.write(c);
#endif

#ifdef SERIAL_NMEA_SERIAL_OUTPUT
  /* serial output */
  SerialPort.write(c);
#endif
}


bool SerialNmea::lockRMC(void) {

  /* check if we have RMC sentence */
  if( ! serialState_isset(RMC_FOUND) ) {
    return false;
  }

  /* get the RMC pointer and lock */
  readPos = rmcPos;
  serialState_set(SENTENCE_LOCKED);
  serialState_set(LOCKED_RMC);

  return true;
}

bool SerialNmea::lockGGA(void) {
  /* check if we have RMC sentence */
  if( ! serialState_isset(GGA_FOUND) ) {
    return false;
  }

  /* get the RMC pointer and lock */
  readPos = ggaPos;
  serialState_set(SENTENCE_LOCKED);
  serialState_set(LOCKED_GGA);
  
  return true;
}

void SerialNmea::addTagToRead(void) {

  /* go backward */
  if( readPos >= NMEA_TAG_SIZE + 1 ) {
    readPos -= NMEA_TAG_SIZE + 1;
  } else {
    readPos += SERIAL_NMEA_BUFFER_SIZE - (NMEA_TAG_SIZE + 1);
  }
}

void SerialNmea::release() {
  
  if( serialState_isset(LOCKED_RMC) ) {
    serialState_unset(RMC_FOUND);
    serialState_unset(LOCKED_RMC);
  }

  else if( serialState_isset(LOCKED_GGA) ) {
    serialState_unset(GGA_FOUND);
    serialState_unset(LOCKED_GGA);
  }

  serialState_unset(SENTENCE_LOCKED);
  serialState_unset(LOCKED);
}

unsigned long SerialNmea::getReceiveTimestamp(void) {

  return receiveTimestamp;
}

unsigned long SerialNmea::getLastReceiveTimestamp(void) {

  return lastReceiveTimestamp;
}

uint8_t SerialNmea::read(void) {

  uint8_t c = buffer[readPos];
  readPos = (readPos + 1) % SERIAL_NMEA_BUFFER_SIZE;

  return c;
}

void SerialNmea::lock(void) {

  serialState_set(LOCKED);

  /* clear if partial sentences are parsed */
  nmeaParseStep = -1;
  writePos = txHead;
  
}

void SerialNmea::write(uint8_t c) {


  /* wait for task writes done */
  while( txEnabled ) { }

  /* write now */
  serialWrite(c);
  
}
