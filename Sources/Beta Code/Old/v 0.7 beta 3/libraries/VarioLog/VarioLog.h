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
/*                                                                               */
/*********************************************************************************/


#ifndef VARIOLOG_H
#define VARIOLOG_H

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>
#include <ArduinoTrace.h>
#include "mpp-console.h"
#include "mpp-logger.h"
#include "mpp-sdhal_log_handler.h"

#define MAIN_DEBUG_LOG 				0			  
#define SCREEN_DEBUG_LOG 			1
#define GPS_DEBUG_LOG					2
#define BUTTON_DEBUG_LOG			3
#define MS5611_DEBUG_LOG			4
#define MPU_DEBUG_LOG					5
#define KALMAN_DEBUG_LOG			6
#define EEPROM_DEBUG_LOG			7
#define SDCARD_DEBUG_LOG			8
#define IGC_DEBUG_LOG					9
#define BT_DEBUG_LOG					10
#define WIFI_DEBUG_LOG				11
#define SOUND_DEBUG_LOG				12

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
	 void logtrace(uint8_t type, uint8_t module, String file, int line, String function);


 private:
   bool EnableDebug[20];

};

extern VarioLog varioLog;

#define TRACELOG(module)                             \
  varioLog.logtrace(module, __COUNTER__, __FILE__,   \
										__LINE__, __PRETTY_FUNCTION__)

#define DUMPLOG(variable)
#define MESSLOG(Text)

extern Logger LOGGER;

#define LOGS_ACTIVE

#ifndef LOGS_ACTIVE
#define log_setLevel
#define log_debug
#define log_info
#define log_warning
#define log_error
#define log_critical
#else
#define log_setLevel    LOGGER.setLevel
#define log_printf      LOGGER.log
#define log_debug       LOGGER.debug
#define log_info        LOGGER.info
#define log_warning     LOGGER.warning
#define log_error       LOGGER.error
#define log_critical    LOGGER.critical
#endif

#endif
