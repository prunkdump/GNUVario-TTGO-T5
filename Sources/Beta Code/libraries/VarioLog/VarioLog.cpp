/* variolog -- 
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

#include <VarioLog.h>
#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#ifdef SDCARD_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include "mpp-console.h"
#include "mpp-logger.h"
#include "mpp-sdhal_log_handler.h"
#include "debug.h"
#include <sdcardHAL.h>

Logger LOGGER("GnuVarioE");
LogTimeDateFormatter formatter(&LOGGER);

#if defined(SDFAT_LIB)
SdFatFileLogHandler handler(&formatter, "log/GnuVarioE.log", 10000, 5);
#else
SdFatFileLogHandler handler(&formatter, &SDHAL_SD, "log/GnuVarioE.log", 10000, 5);
#endif

//******************************
void VarioLog::init(void) {
//******************************
	for(int i=0;i<20;i++) EnableDebug[i] = false;
	
	//lecture fichier debug.cfg
	
  console_init(&SerialPort);
  LOGGER.init(&SerialPort);
  LOGGER.setLevel(DEBUG);
  LOGGER.setFormatter(&formatter);	
}

//****************************************************
void VarioLog::setDebug(uint8_t module, bool status) {
//****************************************************
	EnableDebug[module] = status;
}

//********************************************
bool VarioLog::getDebug(uint8_t module) {
//********************************************
	return EnableDebug[module];
	
}

//********************************************
void VarioLog::logtrace(uint8_t type, uint8_t module, String file, int line, String function) {
//********************************************
	int i;
	for(i=file.length(); i>0; i--) {
/*		if ((file[i] == '/') || (file[i] == '\')) {
			break;
		}*/
	}
	
	String tmpfile;
	
	if (i > 0) tmpfile = file.substring(i+1);
	SerialPort.println("WARNING : " + tmpfile + ":" + line + " - " + function);
}


/*
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
*/

VarioLog varioLog;