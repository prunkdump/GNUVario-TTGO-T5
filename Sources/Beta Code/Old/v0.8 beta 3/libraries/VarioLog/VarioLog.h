/* varioLog -- 
 *
 * Copyright 2019 Jean-philippe GOI
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

/*********************************************************************************/
/*                                                                               */
/*                           VarioStat                                           */
/*                                                                               */
/*  version    Date        Description                                           */
/*    1.0      18/12/19                                                          */
/*    1.1      03/01/20    Ajout ouverture du fichier dans init et writesd       */
/*                         Ajout gestion du fichier log.cfg                      */
/*    1.1.1    06/01/20		 Ajout logvariable																		 */
/*    1.1.2    19/01/20    Ajout DEEPSLEEP_DEBUG                                 */
/*    1.1.3    16/02/20    Correction rotation des fichiers log                  */
/*                                                                               */
/*********************************************************************************/
/*                                                                               */
/*   TRACELOG(type, module)                                                      */
/*                                    Type                                       */
/*																						LOG_TYPE_DEBUG				DEBUG				 */
/*																						LOG_TYPE_INFO					INFO         */
/*																						LOG_TYPE_WARNING			WARNING      */
/*																						LOG_TYPE_ERROR				ERROR        */
/*																						LOG_TYPE_CRITICAL			CRITICAL		 */
/*																																							 */
/*                                    Module  de 1 à 20                          */
/*																																							 */
/*	DUMPLOG(type, module, variable)																							 */
/* 																																							 */
/*	MESSLOG(type, module, Text)																									 */
/*																																							 */
/*	INFOLOG(Text)																																 */
/*																																							 */
/*********************************************************************************/


#ifndef VARIOLOG_H
#define VARIOLOG_H

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>
#include <ArduinoTrace.h>

#include <sdcardHAL.h>
#include <VarioSettings.h>

#define ALL_DEBUG_LOG         0
#define MAIN_DEBUG_LOG 				1			  
#define SCREEN_DEBUG_LOG 			2
#define GPS_DEBUG_LOG					3
#define BUTTON_DEBUG_LOG			4
#define MS5611_DEBUG_LOG			5
#define MPU_DEBUG_LOG					6
#define KALMAN_DEBUG_LOG			7
#define EEPROM_DEBUG_LOG			8
#define SDCARD_DEBUG_LOG			9
#define IGC_DEBUG_LOG					10
#define BT_DEBUG_LOG					11
#define WIFI_DEBUG_LOG				12
#define SOUND_DEBUG_LOG				13
#define DEEPSLEEP_DEBUG_LOG   14
#define VOLTAGE_DEBUG_LOG     15
#define DATA_DEBUG_LOG				16

#define LOG_TYPE_DEBUG				0
#define LOG_TYPE_INFO					1
#define LOG_TYPE_WARNING			2
#define LOG_TYPE_ERROR				3
#define LOG_TYPE_CRITICAL			4

/********************/
/* The Log class   */
/********************/

class VarioLog {
 public:
   void init(void);
	 void setDebug(uint8_t module, bool status);
	 bool getDebug(uint8_t module);
	 void setEnableSerial(bool status);
	 void setEnableSdCard(bool status);
	 void setEnableDebug(bool status);
	 void setEnableDebugESP32(bool status);
	 void logtrace(uint8_t type, uint8_t module, String file, int line, String function);
	 void logmessage(uint8_t type, uint8_t module, String file, int line, String function, String message);
	 void logvariable(uint8_t type, uint8_t module, String file, int line, String function, String namevariable, String variable);
	 void loginfo(String message);
	 String formatlog(uint8_t type, uint8_t module, String file, int line, String function);
	 void traitement(uint8_t type, uint8_t module, String msg);
	 
#ifdef SDFAT_LIB
		SdFile *log_sd;
#else
    SdCardHAL *log_sd;
#endif //SDFAT_LIB
		
#ifdef SDFAT_LIB
		SdFile logFile;
#else
    File logFile;
#endif //SDFAT_LIB
		
#ifdef SDFAT_LIB
   SdFile openLog(void);
#else
   File openLog(void);
#endif //SDFAT_LIB
   int logRotate(void);

   VarioLog(char *directoty, char *filename, size_t maxSize, int backupCount);
   void send(const char *msg);
	 void send(String msg);
	 void writeSd(const char *msg);
	 void writeSd(String msg);
	 void openSd(void);
	 
protected:
	 char *log_file;
	 char *log_dir;
	 size_t log_size;
	 int log_backup;

 private:
   bool EnableDebugModule[20];
	 bool EnableSerial;
	 bool EnableSdCard;
	 bool EnableDebugESP32;
	 bool EnableDebug;
};

extern VarioLog varioLog;

#define TRACELOG(type, module)                      \
  varioLog.logtrace(type, module, __FILE__,         \
										__LINE__, __PRETTY_FUNCTION__)

//, __COUNTER__
#define DUMPLOG(type, module, variable)             \
  varioLog.logvariable(type, module, __FILE__,       \
										__LINE__, __PRETTY_FUNCTION__, String(#variable), String(variable))

#define MESSLOG(type, module, Text)                 \
  varioLog.logmessage(type, module, __FILE__,       \
										__LINE__, __PRETTY_FUNCTION__, Text)

#define INFOLOG(Text) \
	varioLog.loginfo(Text)
#endif
