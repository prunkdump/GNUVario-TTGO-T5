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
 * VarioLog is distributed in the hope that it will be useful,
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

#include <VarioLog.h>
#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#ifdef SDCARD_DEBUG
#define ARDUINOTRACE_ENABLE 1
#elif not defined(ARDUINOTRACE_ENABLE)
#define ARDUINOTRACE_ENABLE 0
#endif

#ifndef ARDUINOTRACE_SERIAL
#define ARDUINOTRACE_SERIAL SerialPort
#endif

#include <ArduinoTrace.h>

#include <sdcardHAL.h>
#include <VarioSettings.h>

#if defined(ESP32)
//static const char* TAG = "GnuvarioE";
#include "esp_log.h"
#endif //ESP32

#define FILE_MAX_NAME   50 //15

char logDirectory[20] = "logs/";
char logFile[20] = "vario.log";
VarioLog varioLog(logDirectory, logFile, 1000000, 5);

//******************************
VarioLog::VarioLog(char *directory, char *filename, size_t maxSize, int backupCount) {
//******************************

	log_dir     = directory;
  log_file 		= filename;
  log_size 		= maxSize;
  log_backup 	= backupCount;
}

//******************************
void VarioLog::init(void) {
//******************************
	for(int i=0;i<20;i++) EnableDebugModule[i] = false;
	EnableDebugModule[0] = true;
	
	//lecture fichier debug.cfg
	
	boolean ModifValue = false;
	char tmpFileName[15] = "log.cfg";
 
	if (SDHAL_SD.exists(tmpFileName)) {
		GnuSettings.readSDSettings(tmpFileName, &ModifValue);

		SerialPort.println("");
		SerialPort.println("Lecture du fichier log.cfg");

		if (EnableDebug) SerialPort.println("log : Activé");
		else						 SerialPort.println("log : Desactivé");

		if (EnableSerial) SerialPort.println("log Serial : Activé");
		else						  SerialPort.println("log Serial : Desactivé");

		if (EnableSdCard) SerialPort.println("log SdCard : Activé");
		else						  SerialPort.println("log SdCard : Desactivé");

		if (EnableDebugESP32) SerialPort.println("log Debug : Activé");
		else						  		SerialPort.println("log Debug : Desactivé");

	}   	
	
	// Ouverture du fichier de log
	if (EnableDebug && EnableSdCard) openSd();
}

//****************************************************
void VarioLog::setDebug(uint8_t module, bool status) {
//****************************************************
#ifdef SDCARD_DEBUG
    SerialPort.print("setDebug Module : ");
		SerialPort.print(module);
		SerialPort.print(" - Status : ");
		SerialPort.println(status);
#endif
	EnableDebugModule[module] = status;
}

//********************************************
bool VarioLog::getDebug(uint8_t module) {
//********************************************
	return EnableDebugModule[module];
}

//****************************************************
void VarioLog::setEnableSerial(bool status) {
//****************************************************
	EnableSerial = status;
}

//****************************************************
void VarioLog::setEnableSdCard(bool status) {
//****************************************************
	EnableSdCard = status;
}

//****************************************************
void VarioLog::setEnableDebug(bool status) {
//****************************************************
	EnableDebug = status;
}
	 
//****************************************************
void VarioLog::setEnableDebugESP32(bool status) {
//****************************************************
	EnableDebugESP32 = status;
}

//********************************************
void VarioLog::logtrace(uint8_t type, uint8_t module, String file, int line, String function) {
//********************************************

	if (!EnableDebug) return;

  String tmpMessage;
	tmpMessage = formatlog(type, module, file, line, function);
	traitement(type, module, tmpMessage);
}

//********************************************
void VarioLog::logmessage(uint8_t type, uint8_t module, String file, int line, String function, String message) {
//********************************************
	if (!EnableDebug) return;

  String tmpMessage;
	tmpMessage = formatlog(type, module, file, line, function);
	tmpMessage = tmpMessage + " - " + message;
	traitement(type, module, tmpMessage);
}

//********************************************
void VarioLog::logvariable(uint8_t type, uint8_t module, String file, int line, String function, String namevariable, String variable) {
//********************************************
	if (!EnableDebug) return;

  String tmpMessage;
	tmpMessage = formatlog(type, module, file, line, function);
	tmpMessage = tmpMessage + " - " + namevariable + " = " + variable;
	traitement(type, module, tmpMessage);
}

//********************************************
void VarioLog::traitement(uint8_t type, uint8_t module, String msg) {
//********************************************

#ifdef SDCARD_DEBUG
   SerialPort.print("traitement - module : ");
	 SerialPort.println(module);
   SerialPort.print("EnableDebugModule : ");
	 SerialPort.println(EnableDebugModule[module]);
#endif

	if (EnableSerial && EnableDebugModule[module]) 	DUMP(msg);
	if (EnableSdCard && EnableDebugModule[module])	writeSd(msg);
//		send(tmpMessage);
	if (EnableDebugESP32 && EnableDebugModule[module]) {
		char tmpstr[100];
		msg.toCharArray(tmpstr,100);

		switch (type) {
			case LOG_TYPE_DEBUG :
				ESP_LOGD(TAG, "%s",tmpstr);
				break;
			case LOG_TYPE_INFO :
				ESP_LOGI(TAG, "%s",tmpstr);
				break;
			case LOG_TYPE_WARNING :
				ESP_LOGW(TAG, "%s",tmpstr);
				break;
			case LOG_TYPE_ERROR :
				ESP_LOGE(TAG, "%s",tmpstr);
				break;
			case LOG_TYPE_CRITICAL :
				ESP_LOGE(TAG, "%s",tmpstr);
				break;
			default:
				break;
		}
	}
}

//********************************************
void VarioLog::loginfo(String message) {
//********************************************
	if (!EnableDebug) return;

  String tmpMessage;
	tmpMessage = "INFO - " + message;
	if (EnableSerial) DUMP(tmpMessage);
	if (EnableSdCard) writeSd(tmpMessage);
	if (EnableDebugESP32) {
		char tmpstr[100];
		tmpMessage.toCharArray(tmpstr,100);
		ESP_LOGI(TAG, "%s",tmpstr);
	}
}

//********************************************
String VarioLog::formatlog(uint8_t type, uint8_t module, String file, int line, String function) {
//********************************************

	int i;
	
	for(i=file.length(); i>0; i--) {
		if ((file[i] == '//') || (file[i] == '\\')) {
			break;
		}
	}
		
	String tmpfile;
	
	if (i > 0) tmpfile = file.substring(i+1);
	String message;
	message = tmpfile + ":" + line + " - "; // + function;
	
	switch (type) {
		case LOG_TYPE_DEBUG	:
		  message = "DEBUG - " + message + function;
//			log_debug("%s", message);
//				log_debug("This is a %s message", "debug");
			break;
		case LOG_TYPE_INFO  :
		  message = "INFO - " + message;
//			log_info("%s", message);
//			log_info("This is an %s message", "info");
			break;
		case LOG_TYPE_WARNING :
		  message += "WARNING - " + message;
//			log_warning("%s", message);
			break;
		case LOG_TYPE_ERROR	:
		  message += "ERROR - " + message + function;
//			log_error("%s", message);
			break;
		case LOG_TYPE_CRITICAL :
		  message += "CRITICAL - " + message + function;
//			log_critical("%s", message);
			break;
		default:
    // statements
    break;	
	}
	
//	SerialPort.println("WARNING : " + tmpfile + ":" + line + " - " + function);
  return message;
}


#if defined(ESP32) && not defined (SDFAT_LIB) 

//********************************************
File VarioLog::openLog(void)
//********************************************
{
  File f;
	char log_name[50];
  strcpy(log_name, log_dir);
	strcat(log_name, log_file);
	

  if (SDHAL_SD->exists(log_name)) {
#ifdef SDCARD_DEBUG
    SerialPort.print("openLog: %s exists\n", log_name);
#endif
    f = SDHAL_SD->open(log_name, FILE_WRITE);  //FILE_APPEND);
    if (!f) {
#ifdef SDCARD_DEBUG
      SerialPort.print("openLog: %s error\n", log_name);
#endif
      return f;
    }
  }
  else {
#ifdef SDCARD_DEBUG
    SerialPort.print("openLog: %s missing\n", log_name);
#endif
    f = SDHAL_SD->open(log_name, FILE_WRITE);
    if (!f) {
#ifdef SDCARD_DEBUG
      SerialPort.print("openLog: %s error\n", log_name);
#endif
      return f;
    }
    return f;
  }
  if (f.size() > log_size) {
#ifdef SDCARD_DEBUG
    SerialPort.print("openLog %s %ld > %ld\n", log_name, f.size(), log_size);
#endif
    f.close();
    logRotate();
#ifdef SDCARD_DEBUG
    SerialPort.print("openLog: create %s\n", log_name);
#endif
    File f = SDHAL_SD->open(log_name, FILE_WRITE);
    if (!f) {
#ifdef SDCARD_DEBUG
      SerialPort.print("openLog: %s error\n", log_name);
#endif
      return f;
    }
#ifdef SDCARD_DEBUG
    SerialPort.print("%s: created\n", log_name);
#endif
    return f;
  }
  return f;
}

#else

//********************************************
SdFile VarioLog::openLog(void)
//********************************************
{
  SdFile f;
	char log_name[50];
  strcpy(log_name, log_dir);
	strcat(log_name, log_file);

  if (SDHAL_SD.exists(log_name)) {
#ifdef SDCARD_DEBUG
		char tmpstr[100];
    sprintf(tmpstr, "openLog: %s exists", log_name);
    SerialPort.println(tmpstr);
#endif
    bool retour = f.open(log_name, O_WRONLY | O_APPEND);  //FILE_APPEND);
    if (!retour) {
#ifdef SDCARD_DEBUG
			char tmpstr[100];
			sprintf(tmpstr, "openLog: %s error", log_name);
      SerialPort.println(tmpstr);
#endif
      return f;
    }
  }
  else {
		bool retour = f.open(log_name, O_WRONLY | O_CREAT);
		if (!retour) {
#ifdef SDCARD_DEBUG
			char tmpstr[100];
			sprintf(tmpstr, "openLog: %s: error 0x%x\n", log_name, 0);
			SerialPort.println(tmpstr); //SDHAL_SD.card()->errorCode());
#endif
			return f;
		}
  }
  f.seekEnd();
  if (f.fileSize() > log_size) {
#ifdef SDCARD_DEBUG
		char tmpstr[100];
		sprintf(tmpstr, "openLog %s %ld > %ld\n", log_name, f.fileSize(), log_size);
    SerialPort.println(tmpstr);
#endif
    f.close();
    logRotate();
    SdFile f;
		bool retour = f.open(log_name, O_WRONLY | O_CREAT);
    if (!retour) {
#ifdef SDCARD_DEBUG
			char tmpstr[100];
			sprintf(tmpstr, "openLog: %s error 0x%x\n", log_name, 0);
      SerialPort.println(tmpstr); //log_sd->card()->errorCode());
#endif
      return f;
    }
    f.seekEnd();
    return f;
  }
  return f;
}

#endif

#if defined(ESP32) && not defined(SDFAT_LIB)

//********************************************
int VarioLog::logRotate(void)
//********************************************
{
#ifdef SDCARD_DEBUG
    SerialPort.println("LogRotate");
#endif
	
  int count = 0, fno, flast = 0;
  size_t size;
	char log_name[50];
  strcpy(log_name, log_dir);
	strcat(log_name, log_file);

  File dir = SDHAL_SD.open("/logs/");
  if (!dir) {
#ifdef SDCARD_DEBUG
    SerialPort.println("/logs : error");
#endif
    return 0;
  }
  dir.rewindDirectory();
  while (true) {
    File entry = dir.openNextFile();
		
    if (!entry) {
      break;
    }
		
    char fname[FILE_MAX_NAME];
    strcpy(fname, entry.name());
    if (!strncmp(fname, log_file, strlen(log_file))) {
#ifdef SDCARD_DEBUG
			char tmpstr[100];
			sprintf(tmpstr, "found %s %ld\n", fname, entry.size());
      SerialPort.print(tmpstr);
#endif
      char *p = strrchr(fname, '.');
      char *ext = p+1;
      fno = atoi(ext);
      if (fno != 0) {
        if (fno > flast) {
          flast = fno;
        }
      }
    }
    entry.close();
    yield();
  }
  dir.close();
#ifdef SDCARD_DEBUG
	char tmpstr[100];
	sprintf(tmpstr, "logRotate: %d files\n", flast);
  SerialPort.print(tmpstr);
#endif
  if (flast != 0) {
    for (fno = flast ; fno >= log_backup ; fno--) {
      char name[FILE_MAX_NAME];
      sprintf(name, "%s.%d", log_name, fno);
      if (SDHAL_SD.exists(name)) {
#ifdef SDCARD_DEBUG
				char tmpstr[100];
				sprintf(tmpstr, "%s: delete\n", name);
        SerialPort.print(tmpstr);
#endif
        if (SDHAL_SD.remove(name) != true) {
#ifdef SDCARD_DEBUG
					char tmpstr[100];
					sprintf(tmpstr, "%s: cannot delete\n", name);
          SerialPort.print(tmpstr);
#endif
        }
#ifdef SDCARD_DEBUG
				char tmpstr[100];
				sprintf(tmpstr, "%s: successfully deleted\n", name);
        SerialPort.print(tmpstr);
#endif
      }
    }
    for (fno = flast ; fno > 0 ; fno--) {
      char name[FILE_MAX_NAME];
      char newname[FILE_MAX_NAME];
      sprintf(name, "logs/%s.%d", log_name, fno);
      sprintf(newname, "logs/%s.%d", log_name, fno+1);
#ifdef SDCARD_DEBUG
			char tmpstr[100];
			sprintf(tmpstr, "%s: rename to %s\n", name, newname);
      SerialPort.print(tmpstr);
#endif
      if (SDHAL_SD.rename(name, newname) != true) {
#ifdef SDCARD_DEBUG
				char tmpstr[100];
				sprintf(tmpstr, "%s: cannot rename to %s\n", name, newname);
        SerialPort.print(tmpstr);
#endif
      }
#ifdef SDCARD_DEBUG
			char tmpstr[100];
			sprintf(tmpstr, "%s: successfully renamed to %s\n", name, newname);
      SerialPort.print(tmpstr);
#endif
    }
  }
  char newname[FILE_MAX_NAME];
  sprintf(newname, "%s.%d", log_name, fno+1);
#ifdef SDCARD_DEBUG
	char tmpstr[100];
	sprintf(tmpstr, "%s: rename to %s\n", log_name, newname);
  SerialPort.print(tmpstr);
#endif
  if (SDHAL_SD.rename(log_name, newname) != true) {
#ifdef SDCARD_DEBUG
		char tmpstr[100];
		sprintf(tmpstr, "%s: cannot rename to %s\n", log_name, newname);
    SerialPort.print(tmpstr);
#endif
  }
#ifdef SDCARD_DEBUG
	char tmpstr[100];
	sprintf(tmpstr, "%s: successfully renamed to %s\n", log_name, newname);
  SerialPort.print(tmpstr);
#endif
  return 0;
}

#else

//********************************************
int VarioLog::logRotate(void)
//********************************************
{
  //int count = 0, 
  int fno=0, flast = 0;
 // size_t size;
  SdFile file;
	SdFile dir;
	
	char log_name[50];
  strcpy(log_name, log_dir);
	strcat(log_name, log_file);
	
	bool retour = dir.open("/logs", O_RDONLY);
  if (!retour) {
#ifdef SDCARD_DEBUG
    SerialPort.println("/logs: error");
#endif
    return 0;
  }
	
  dir.rewind();
#ifdef SDCARD_DEBUG
  SerialPort.println("reniew");
#endif
	
  while (file.openNext(&dir, O_RDONLY)) {
    char fname[FILE_MAX_NAME];
    file.getName(fname, FILE_MAX_NAME);
		
#ifdef SDCARD_DEBUG
    SerialPort.print("log file : ");
		SerialPort.println(fname);
#endif

#ifdef SDCARD_DEBUG
    SerialPort.print("log name : ");
		SerialPort.print(log_name);
		SerialPort.print(" / ");
		SerialPort.println(strlen(log_name));
#endif

    if (!strncmp(fname, log_file, strlen(log_file))) {
#ifdef SDCARD_DEBUG
			char tmpstr[100];
//			sprintf(tmpstr, "found %s %ld\n", fname, size);
			sprintf(tmpstr, "found %s\n", fname);
      SerialPort.print(tmpstr);
#endif
      char *p = strrchr(fname, '.');
      char *ext = p+1;
      fno = atoi(ext);
      if (fno != 0) {
        if (fno > flast) {
          flast = fno;
        }
      }
    }

    file.close();
  }
	
#ifdef SDCARD_DEBUG
	char tmpstr[100];
	sprintf(tmpstr, "logRotate: %d files\n", flast);
  SerialPort.print(tmpstr);
#endif

  if (flast != 0) {
    for (fno = flast ; fno >= log_backup ; fno--) {
      char name[FILE_MAX_NAME];
      sprintf(name, "%s.%d", log_name, fno);
#ifdef SDCARD_DEBUG
			sprintf(tmpstr, "%s: delete\n", name);
      SerialPort.print(tmpstr);
#endif
      if (SDHAL_SD.remove(name) != true) {
#ifdef SDCARD_DEBUG
				char tmpstr[100];
				sprintf(tmpstr, "%s: cannot delete\n", name);
        SerialPort.print(tmpstr);
#endif
      }
#ifdef SDCARD_DEBUG
			sprintf(tmpstr, "%s: successfully deleted\n", name);
      SerialPort.print(tmpstr);
#endif
      flast--;
    }
		
    for (fno = flast ; fno > 0 ; fno--) {
      char name[FILE_MAX_NAME];
      char newname[FILE_MAX_NAME];
      sprintf(name, "%s.%d", log_name, fno);
      sprintf(newname, "%s.%d", log_name, fno+1);
#ifdef SDCARD_DEBUG
			char tmpstr[100];
			sprintf(tmpstr, "%s : rename to %s\n", name, newname);
      SerialPort.print(tmpstr);
#endif
      if (SDHAL_SD.rename(name, newname) != true) {
#ifdef SDCARD_DEBUG
				char tmpstr[100];
				sprintf(tmpstr, "%s: cannot rename to %s\n", name, newname);
        SerialPort.print(tmpstr);
#endif
      }
#ifdef SDCARD_DEBUG
			sprintf(tmpstr, "%s: successfully renamed to %s\n", name, newname);
      SerialPort.print(tmpstr);
#endif
    }
  }
	
  char newname[FILE_MAX_NAME];
  sprintf(newname, "%s.%d", log_name, fno+1);
#ifdef SDCARD_DEBUG
	sprintf(tmpstr, "%s : rename to %s\n", log_name, newname);
  SerialPort.print(tmpstr);
#endif

  if (SDHAL_SD.rename(log_name, newname) != true) {
#ifdef SDCARD_DEBUG
		char tmpstr[100];
		sprintf(tmpstr, "%s: cannot rename to %s\n", log_name, newname);
    SerialPort.print(tmpstr);
#endif
  }
	
#ifdef SDCARD_DEBUG
	sprintf(tmpstr, "%s: successfully renamed to %s\n", log_name, newname);
  SerialPort.print(tmpstr);
#endif
  return 0;
}

#endif

//********************************************
void VarioLog::send(const char *msg)
//********************************************
{
	char log_name[50];
  strcpy(log_name, log_dir);
	strcat(log_name, log_file);
	
#if not defined(SDFAT_LIB)
  File f = openLog();
  if (f) {
#else
  SdFile f = openLog();
  if (f.isOpen()) {
#endif
    size_t n = f.println(msg);
    f.close();
#ifdef SDCARD_DEBUG
		char tmpstr[100];
    size_t len = strlen(msg);
		sprintf(tmpstr, "%s: %s\n", log_name, msg);
    SerialPort.print(tmpstr);
		sprintf(tmpstr, "%s: %d/%d bytes written\n", log_name, n, len);
    SerialPort.print(tmpstr);
#endif
  }
}

//********************************************
void VarioLog::send(String msg)
//********************************************
{
	char tmpstr[100];
	msg.toCharArray(tmpstr,100);
	send(tmpstr);
}

//********************************************
void VarioLog::writeSd(const char *msg)
//********************************************
{
 char log_name[50];
  strcpy(log_name, log_dir);
	strcat(log_name, log_file);

#if not defined(SDFAT_LIB)
  if (logFile) {
#else
  if (logFile.isOpen()) {
#endif
    size_t n = logFile.println(msg);
		logFile.flush();
#ifdef SDCARD_DEBUG
		char tmpstr[100];
    size_t len = strlen(msg);
		sprintf(tmpstr, "%s: %s\n", log_name, msg);
    SerialPort.print(tmpstr);
		sprintf(tmpstr, "%s: %d/%d bytes written\n", log_name, n, len);
    SerialPort.print(tmpstr);
#endif
  }
}

//********************************************
void VarioLog::writeSd(String msg)
//********************************************
{
	char tmpstr[100];
	msg.toCharArray(tmpstr,100);
	writeSd(tmpstr);
}

//********************************************
void VarioLog::openSd(void)
//********************************************
{
  logFile = openLog();
}

/*
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
	
	      ESP_LOGE(TAG, "Erreur capteur MPU9250 introuvable");

*/
