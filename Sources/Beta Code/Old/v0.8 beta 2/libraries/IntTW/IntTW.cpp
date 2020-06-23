/* InTW -- Interrupt driven Two Wire library 
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

/***************************************************************************************************/
/*                                INTTW                                                            */
/*                                                                                                 */
/*  Ver     Date                                                                                   */
/*  1.0                                                                                            */
/*                                                                                                 */
/*  1.0.1   28/07/2019   Ajout reinit i2c                                                          */
/*                                                                                                 */
/***************************************************************************************************/

#include <IntTW.h>

#include <Arduino.h>
#include <esp32-hal-i2c.h>

#include <VarioSettings.h>
#include <DebugConfig.h>
#include <HardwareConfig.h>

#define INTTW_I2C_BUS_NUM 0
#define INTTW_TIMEOUT_MS 50

#ifndef VARIO_TW_SDA_PIN
#define VARIO_TW_SDA_PIN 22
#endif

#ifndef VARIO_TW_SCL_PIN
#define VARIO_TW_SCL_PIN 23
#endif
 

IntTW intTW;

  
void IntTW::begin(bool internalPullUp) {

  /* init TW */
  i2c = i2cInit(INTTW_I2C_BUS_NUM, VARIO_TW_SDA_PIN, VARIO_TW_SCL_PIN, VARIO_TW_FREQ);
}

void IntTW::setTxBuffer(uint8_t* buff) {

  txBuffer = buff;
}

void IntTW::setRxBuffer(uint8_t* buff) {

  rxBuffer = buff;
}


// note : commandFlags are useless on ESP32
void IntTW::start(uint8_t* commands, uint8_t commandLength, uint8_t commandFlags, void (*successCallback)(void)) {

  /******************/
  /* check commands */
  /******************/
  int commandPos = 0;
  uint32_t readCount;
  
  while( commandPos < commandLength ) {

    /***************************/
    /* address and action byte */
    /***************************/
    uint8_t action = commands[commandPos];
    uint8_t address = action >> 1;
    action &= 0x01;

    /* next */
    commandPos++;

    /***********************/
    /* count and dest byte */
    /***********************/
    uint8_t dest = commands[commandPos];
    uint8_t count = dest & INTTW_COUNT_MASK;
    dest &= INTTW_COUNT_FLAG_MASK;

    /* next */
    commandPos++;

    /*****************/
    /* check pointer */
    /*****************/
    
    /* default to command buffer */
    uint8_t* commandDestP = &commands[commandPos];

#ifdef INTTW_LOAD_POINTER_FUNC
    /* if needed read pointer */
    if( dest == INTTW_SET_POINTER ) {
      
      commandDestP = *((uint8_t**)(&commands[commandPos]));
      commandPos += sizeof(uint8_t*);
    }
#endif

    /* pointer set with setTxBuffer/setRxBuffer ? */
    if( dest == INTTW_AT_POINTER ) {

      if( action == INTTW_WRITE ) {
	commandDestP = txBuffer;
	txBuffer += count;
      } else {
	commandDestP = (uint8_t*)rxBuffer;
	rxBuffer += count;
      }
    }

    /* next */
    if( dest == INTTW_IN_CMD )
      commandPos += count;

    
    /**************/
    /* check stop */
    /**************/
    bool sendStop = false;

    /* send if last command */
    if( commandPos >= commandLength )
      sendStop = true;


    /********************/
    /* queue TW command */
    /********************/
    if( action == INTTW_WRITE ) {
      twError = i2cWrite(i2c, address, commandDestP, count, sendStop, INTTW_TIMEOUT_MS);
    } else {
      twError = i2cRead(i2c, address, commandDestP, count, sendStop, INTTW_TIMEOUT_MS, &readCount);
    }
  }
  

  /************/
  /* callback */
  /************/
#ifdef INTTW_CALLBACK_ON_ERROR_FUNC
  if( successCallback ) {
    successCallback();
  }
#else
  if( (twError == I2C_ERROR_OK) && successCallback ) {
    successCallback();
  } else {
		i2cRelease(i2c);
		i2c = i2cInit(INTTW_I2C_BUS_NUM, VARIO_TW_SDA_PIN, VARIO_TW_SCL_PIN, VARIO_TW_FREQ);
	}
#endif
}
  
  
/* start is a blocking function */
bool IntTW::transmitting(void) {

  return false;
}


bool IntTW::succeeded(void) {
 
  return (twError == I2C_ERROR_OK);
}


/* never keeping bus */	
void IntTW::stop(void) {
  
}

/**********************/
/* standard functions */
/**********************/
bool IntTW::writeBytes(uint8_t address, uint8_t cmd, uint8_t count, uint8_t* buff) {
  
  uint8_t buildCommand[] = { INTTW_ACTION(address, INTTW_WRITE),
			     INTTW_DEST(1, INTTW_IN_CMD),
			     cmd,
			     INTTW_ACTION(address, INTTW_WRITE),
			     INTTW_DEST(count, INTTW_AT_POINTER) };
  
  /* The current I2C library does not */
  /* support burst writes. So we need */
  /* a workaround.                    */
  uint8_t* newCommand;
  uint8_t cmdLength;
  if( ! count ) {
    newCommand = buildCommand;
    cmdLength = 3;
  } else {
    newCommand = (uint8_t*)malloc(3 + count);
    newCommand[0] = INTTW_ACTION(address, INTTW_WRITE);
    newCommand[1] = INTTW_DEST(count + 1, INTTW_IN_CMD);
    newCommand[2] = cmd;
    for( int i = 0; i<count; i++) {
      newCommand[i+3] = buff[i];
    }
    cmdLength = 3 + count;
  }
  
  
  /* check count */
  /*
    uint8_t cmdLength = sizeof(buildCommand);
    if( ! count )
    cmdLength -= 2; //remove second write
  */
  
  /* launch command */
  setTxBuffer(buff);
  //start(buildCommand, cmdLength, INTTW_NONE);
  start(newCommand, cmdLength, INTTW_NONE);
  
  if( count ) {
    free(newCommand);
  }
  
   return succeeded();
}



bool IntTW::readBytes(uint8_t address, uint8_t cmd, uint8_t count, uint8_t* buff) {

  uint8_t buildCommand[] = { INTTW_ACTION(address, INTTW_WRITE),
			     INTTW_DEST(1, INTTW_IN_CMD),
			     cmd,
			     INTTW_ACTION(address, INTTW_READ),
			     INTTW_DEST(count, INTTW_AT_POINTER) };

  /* check count */
  uint8_t cmdLength = sizeof(buildCommand);
  if( ! count )
    cmdLength -= 2; //remove read
  
  
  /* launch command */
  setRxBuffer(buff);
  start(buildCommand, cmdLength, INTTW_NONE);
    
  return succeeded();
}
