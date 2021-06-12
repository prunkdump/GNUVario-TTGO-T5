/* VarioSettings -- Gestion des parametres
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * .
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
/*                           VarioSettings                                       */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0                                                                        */
/*    1.0.1  24/06/19   suppression  VARIOMETER_POWER_ON_DELAY 									 */
/*    1.0.2  22/07/19   Ajout SETTINGS_CLIMB_PERIOD_COUNT et                     */
/*                            SETTINGS_GLIDE_RATIO_PERIOD_COUNT 								 */
/*    1.0.3  23/07/19   Ajout VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE           */
/*														RATIO_CLIMB_RATE																	 */
/*    1.0.4  25/07/19   Ajout default settings                           				 */ 
/*                      Ajout NO_RECORD                                          */
/*    1.0.5  05/08/19   Ajout paramettres Wifi                                   */
/*    1.0.6	 01/10/19		Ajout paramettres calibration                            */
/*    1.0.7  08/10/19   Ajout EnableBT																					 */
/*		1.0.8	 21/10/19   Ajout Compensation alti gps et température et enable bip */
/*                      au démarrage                                             */
/*    1.0.9  11/11/19   Ajout gestion params.jso                                 */
/*                      suppression statistique                                  */
/*											Modification applySetting																 */
/*                      Modification readSDSettings															 */
/*                      ajout  SLEEP_TIMEOUT_MINUTES														 */
/*														 SLEEP_THRESHOLD_CPS															 */
/*														 ALTERNATE_DATA_DURATION													 */
/*											Ajout saveConfigurationVario / loadConfigurationVario		 */
/* 		1.1.0	 13/11/19		Modification soundSettingRead / soundSettingWrite        */
/*                      Ajout lecture et sauvegarde fichier wifi.cfg 						 */
/*										  Ajout gestion automatique des version fichier params.jso */
/*										  Modif ALTERNATE_DATA_DURATION - MULTIDISPLAY_DURATION	   */
/*                      ajout gestion de plusieurs voiles                        */
/*    1.1.1  24/11/19		Modification SLEEP_THRESHOLD_CPS en float								 */
/*    1.2    29/11/19   Modification sdfat                                       */
/*    1.2.1  12/12/19   Ajout set get version et get screenmodel                 */
/*    1.3    28/12/19   Ajout read log.cfg                                       */ 
/*    1.3.1  11/01/20		Modif VARIOSCREEN_SIZE == 290														 */
/*    1.3.2  17/01/20   Ajout DISPLAY_STAT_DURATION - passage en v1.1 					 */
/*    1.3.3  19/01/20   Ajout DEEPSLEEP_DEBUG                                    */
/*    1.3.4  04/02/20   Ajout URL_UPDATE passage en version 1.2 de params.json   */
/*    1.3.5  05/03/20   Ajout DEFAULT_VARIOMETER_ENABLE_AGL											 */
/*    1.3.6  07/04/20   Ajout ACCELERATION_MEASURE_STANDARD_DEVIATION            */
/*                      Ajout LANGUAGE																					 */
/*                      Ajout VARIOMETER_INTEGRATED_CLIMB_RATE                   */
/*    1.3.7  09/06/20   Ajout VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE      */
/*                      Modification VARIOMETER_SENT_LXNAV_SENTENCE              */
/*    1.3.8  24/10/20   Ajout REF_VOLTAGE                                        */
/*    1.3.9  26/10/20   Correction Aleksandr Stroganov <a.stroganov@me.com>      */
/*    1.3.10 10/11/20   Ajout destructeur                                        */ 
/*                                                                               */
/*********************************************************************************/

#include <Arduino.h>

#include <HardwareConfig.h>
#include <DebugConfig.h>

#include <VarioSettings.h>

//#include <FlashAsEEPROM.h>
#include <eepromHAL.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

#include <ArduinoJson.h>

#include <VarioLog.h> 

//SdFat SD;

boolean VarioSettings::initSettings(bool initSD) {

#ifdef HAVE_SDCARD
	if (initSD) {
    if (!SDHAL.begin()) {
#ifdef SDCARD_DEBUG
      SerialPort.println("initialization failed!");
#endif //SDCARD_DEBUG
      return false;
    }
	}
#endif
	
	EEPROMHAL.init(800); //1024);
  if (!EEPROMHAL.isValid()) {
#ifdef EEPROM_DEBUG
      SerialPort.println("initialization failed!");
#endif //EEPROM_DEBUG
      return false;
    }
 		
  return true;
}

#ifdef HAVE_SDCARD
boolean VarioSettings::readSDSettings(char *FileName, boolean *ModifiedValue){
  char character;
  String settingName;
  String settingValue;
#ifdef SDFAT_LIB
  if (myFile.open(FileName, O_RDONLY)) {
#else //SDFAT_LIB
  myFile = SDHAL_SD.open(FileName);
  if (myFile) {
#endif //SDFAT_LIB		
    while (myFile.available()) {
      character = myFile.read();
      while((myFile.available()) && (character != '[')){
        character = myFile.read();
      }
      character = myFile.read();
      while((myFile.available()) && (character != '=')){
        settingName = settingName + character;
        character = myFile.read();
      }
      character = myFile.read();
      while((myFile.available()) && (character != ']')){
        settingValue = settingValue + character;
        character = myFile.read();
      }
      
      if(character == ']'){
 
#ifdef SDCARD_DEBUG

        //Debuuging Printing
        SerialPort.print("Name:");
        SerialPort.println(settingName);
        SerialPort.print("Value :");
        SerialPort.println(settingValue);
#endif //SDCARD_DEBUG

        // Apply the value to the parameter
        if (applySetting(settingName,settingValue)) *ModifiedValue = true;
        // Reset Strings
        settingName = "";
        settingValue = "";
      }
    }
 
    // close the file:
    myFile.close();
	return true;
  } else {
   // if the file didn't open, print an error:
#ifdef SDCARD_DEBUG
   SerialPort.println("error opening settings.txt");
#endif //SDCARD_DEBUG   
   return false;
  }
}
#endif 
 
 /* Apply the value to the parameter by searching for the parameter name
 Using String.toInt(); for Integers
 toFloat(string); for Float
 toBoolean(string); for Boolean
 toLong(string); for Long
 */
 boolean VarioSettings::applySetting(String settingName, String settingValue) {
 
	boolean	ValeurDifferente = false;
 
   if (settingName == "VARIOMETER_PILOT_NAME") {
#ifdef SDCARD_DEBUG
       SerialPort.println("Pilote : " + settingValue);
#endif //SDCARD_DEBUG
       VARIOMETER_PILOT_NAME=settingValue;
   }
#ifdef SDCARD_DEBUG  
   else if(settingName == "exINT") {
     exINT=settingValue.toInt();
   }
   else if(settingName == "exFloat") {
     exFloat=toFloat(settingValue);
   }
   else if(settingName == "exBoolean") {
     exBoolean=toBoolean(settingValue);
   }
   else if(settingName == "exLong") {
     exLong=toLong(settingValue);
   }
#endif //SDCARD_DEBUG  
   else if(settingName == "VARIOMETER_GLIDER_NAME") {
     VARIOMETER_GLIDER_NAME = settingValue;
   }
   else if(settingName == "VARIOMETER_BASE_PAGE_DURATION") {
//   if(settingName == "VARIOMETER_BASE_PAGE_DURATION") {
  /* the duration of the two screen pages in milliseconds */
     VARIOMETER_BASE_PAGE_DURATION = settingValue.toInt();
   }
   else if(settingName == "VARIOMETER_MULTIDISPLAY_DURATION") {
	 VARIOMETER_MULTIDISPLAY_DURATION=settingValue.toInt();
   }
   else if(settingName == "VARIOMETER_TIME_ZONE") {
	   VARIOMETER_TIME_ZONE=toFloat(settingValue);
   }
   else if(settingName == "VARIOMETER_BEEP_VOLUME") {
//	 VARIOMETER_BEEP_VOLUME=settingValue.toInt();
	 VARIOMETER_BEEP_VOLUME=soundSettingRead();
   }
   
  /* The variometer react like this according to vertical speed in m/s :        */
  /* (near climbing beep is not enabled by default)                             */
  /*                                                                            */
  /* <--LOW-BEEP--|------SILENT------|--NEAR-CLIMBING-BEEP--|--CLIMBING-BEEP--> */
  /*              |                  |                      |                   */
  /*           SINKING         CLIMBING-SENSITIVITY      CLIMBING               */
   
   
   else if(settingName == "VARIOMETER_SINKING_THRESHOLD") {
	 VARIOMETER_SINKING_THRESHOLD=toFloat(settingValue);
   }
   else if(settingName == "VARIOMETER_CLIMBING_THRESHOLD") {
	 VARIOMETER_CLIMBING_THRESHOLD=toFloat(settingValue);
   }
   else if(settingName == "VARIOMETER_NEAR_CLIMBING_SENSITIVITY") {
	 VARIOMETER_NEAR_CLIMBING_SENSITIVITY=toFloat(settingValue);
   }
   else if(settingName == "VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM") {
	     /* The near climbing alarm : signal that you enter or exit the near climbing zone */
     VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM=toBoolean(settingValue);
   }
   else if(settingName == "VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP") {
	     /* The near climbing beep : beep when you are in near climbing zone               */
     VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP=toBoolean(settingValue);
   }
   else if(settingName == "FLIGHT_START_MIN_TIMESTAMP") {
	     /* Flight start detection conditions :                      */
        /* -> Minimum time after poweron in milliseconds            */
    FLIGHT_START_MIN_TIMESTAMP=toFloat(settingValue);
   }
    else if(settingName == "FLIGHT_START_VARIO_LOW_THRESHOLD") {
		  /* Flight start detection conditions :                      */
		  /* -> Minimum vertical velocity in m/s (low/high threshold) */
    FLIGHT_START_VARIO_LOW_THRESHOLD=toFloat(settingValue);
   }
   else if(settingName == "FLIGHT_START_VARIO_HIGH_THRESHOLD") {
	     /* Flight start detection conditions :                      */
  /* -> Minimum vertical velocity in m/s (low/high threshold) */
    FLIGHT_START_VARIO_HIGH_THRESHOLD=toFloat(settingValue);
   }
   else if(settingName == "FLIGHT_START_MIN_SPEED") {
  /* Flight start detection conditions :                      */
 /* -> Minimum ground speed in km/h                          */
		FLIGHT_START_MIN_SPEED=toFloat(settingValue);
   }
   else if(settingName == "VARIOMETER_RECORD_WHEN_FLIGHT_START") {
  /* GPS track recording on SD card starting condition :  */ 
  /* -> As soon as possible (GPS fix)                     */
  /* -> When flight start is detected                     */
    VARIOMETER_RECORD_WHEN_FLIGHT_START=toBoolean(settingValue);
   }
   else if(settingName == "VARIOMETER_SENT_LXNAV_SENTENCE") {
  /* What type of vario NMEA sentence is sent by bluetooth. */
  /* Possible values are :                                  */
  /*  - VARIOMETER_SENT_LXNAV_SENTENCE                      */
  /*  - VARIOMETER_SENT_LK8000_SENTENCE                     */
    VARIOMETER_SENT_LXNAV_SENTENCE=settingValue.toInt(); //toBoolean(settingValue);
   }
   else if(settingName == "ALARM_SDCARD") {
     /* Alarm */
     /* Alarm SDCARD not insert */
    ALARM_SDCARD=toBoolean(settingValue);
   }
   else if(settingName == "ALARM_GPSFIX") {
	 /* Nip when GPS Fix */
    ALARM_GPSFIX=toBoolean(settingValue);
   }
   else if(settingName == "ALARM_FLYBEGIN") {
	 /* Bip when Fly begin */
    ALARM_FLYBEGIN=toBoolean(settingValue);
   }
   else if(settingName == "ALARM_VARIOBEGIN") {
	 /* Bip when vario begin */
    ALARM_VARIOBEGIN=toBoolean(settingValue);
   }
    else if(settingName == "KF_ZMEAS_VARIANCE") {
	 // Kalman filter configuration
		KF_ZMEAS_VARIANCE=toFloat(settingValue);
   }
   else if(settingName == "KF_ZACCEL_VARIANCE") {
	   // Kalman filter configuration
    KF_ZACCEL_VARIANCE=toFloat(settingValue);
   }
   else if(settingName == "KF_ACCELBIAS_VARIANCE") {
	   // Kalman filter configuration
     KF_ACCELBIAS_VARIANCE=toFloat(settingValue);
   }
    else if(settingName == "SLEEP_TIMEOUT_MINUTES") {
		// Power-down timeout. Here we power down if the
		// vario does not see any climb or sink rate more than
		// 50cm/sec, for 20 minutes.
     SLEEP_TIMEOUT_MINUTES=settingValue.toInt(); // 20 minutes
   }
   else if(settingName == "SLEEP_THRESHOLD_CPS") {
	    // Power-down timeout. Here we power down if the
		// vario does not see any climb or sink rate more than
		// 50cm/sec, for 20 minutes.
     SLEEP_THRESHOLD_CPS=toFloat(settingValue);
   }
   // vario thresholds in cm/sec for generating different
// audio tones. Between the sink threshold and the zero threshold,
// the vario is quiet
/*                                                                            */
/* <--LOW-BEEP--|------SILENT------|--NEAR-CLIMBING-BEEP--|--CLIMBING-BEEP--> */
/*              |                  |                      |                   */
/*             SINK              ZERO                   CLIMB                 */
   else if(settingName == "CLIMB_THRESHOLD") {
	 CLIMB_THRESHOLD=settingValue.toInt();
   }
   else if(settingName == "ZERO_THRESHOLD") {
	 ZERO_THRESHOLD=settingValue.toInt();
   }
   else if(settingName == "SINK_THRESHOLD") {
     SINK_THRESHOLD=settingValue.toInt();
   }
   else if(settingName == "VARIO_MAX_FREQHZ") {
// change these parameters based on the frequency bandwidth of the speaker
     VARIO_MAX_FREQHZ=settingValue.toInt();
   }
   else if(settingName == "VARIO_XOVER_FREQHZ") {
// change these parameters based on the frequency bandwidth of the speaker
	 VARIO_XOVER_FREQHZ=settingValue.toInt();
   }
   else if(settingName == "VARIO_MIN_FREQHZ") {
 // change these parameters based on the frequency bandwidth of the speaker
    VARIO_MIN_FREQHZ=settingValue.toInt();
   }
   else if(settingName == "VARIO_SINK_FREQHZ") {
     VARIO_SINK_FREQHZ=settingValue.toInt();
   }
   else if(settingName == "VARIO_TICK_FREQHZ") {
	 VARIO_TICK_FREQHZ=settingValue.toInt();
   }
   else if(settingName == "BATTERY_TONE_FREQHZ") {
 // audio feedback tones
    BATTERY_TONE_FREQHZ=settingValue.toInt();
   }
   else if(settingName == "CALIB_TONE_FREQHZ") {
 // audio feedback tones
    CALIB_TONE_FREQHZ=settingValue.toInt();
   }
   else if(settingName == "MPU9250_ERROR_TONE_FREQHZ") {
 // audio feedback tones
    MPU9250_ERROR_TONE_FREQHZ=settingValue.toInt();
   }
   else if(settingName == "MS5611_ERROR_TONE_FREQHZ") {
 // audio feedback tones
    MS5611_ERROR_TONE_FREQHZ=settingValue.toInt();
   }
   else if(settingName == "SDCARD_ERROR_TONE_FREQHZ") {
 // audio feedback tones
    SDCARD_ERROR_TONE_FREQHZ=settingValue.toInt();
   }  
   else if(settingName == "BEEP_FREQ") {
 // audio feedback tones
    BEEP_FREQ=settingValue.toInt();
   }  
   else if(settingName == "SETTINGS_CLIMB_PERIOD_COUNT") {
	   SETTINGS_CLIMB_PERIOD_COUNT=settingValue.toInt();
   }
   else if(settingName == "SETTINGS_GLIDE_RATIO_PERIOD_COUNT") {
	   SETTINGS_GLIDE_RATIO_PERIOD_COUNT=settingValue.toInt();
   }
  else if(settingName == "VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE") {
    VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE=toBoolean(settingValue);
   }  
  else if(settingName == "RATIO_CLIMB_RATE") {
    RATIO_CLIMB_RATE=settingValue.toInt();
   }  
   else if(settingName == "RATIO_MAX_VALUE") {
    RATIO_MAX_VALUE = toFloat(settingValue);
    }  
   else if(settingName == "RATIO_MIN_SPEED") {
    RATIO_MIN_SPEED = toFloat(settingValue);
    }  
   else if(settingName == "NO_RECORD") {
    NO_RECORD = toBoolean(settingValue);
    }  
   else if(settingName == "SSID_1") {
		 if (VARIOMETER_SSID_1 != settingValue) ValeurDifferente = true;
     VARIOMETER_SSID_1 = settingValue;
   }
   else if(settingName == "PASSWORD_1") {
 		if (VARIOMETER_PASSWORD_1 != settingValue) ValeurDifferente = true;
    VARIOMETER_PASSWORD_1 = settingValue;
   }
   else if(settingName == "SSID_2") {
		 if (VARIOMETER_SSID_2 != settingValue) ValeurDifferente = true;
     VARIOMETER_SSID_2 = settingValue;
   }
   else if(settingName == "PASSWORD_2") {
 		 if (VARIOMETER_PASSWORD_2 != settingValue) ValeurDifferente = true;
     VARIOMETER_PASSWORD_2 = settingValue;
   }
   else if(settingName == "SSID_3") {
		 if (VARIOMETER_SSID_3 != settingValue) ValeurDifferente = true;
     VARIOMETER_SSID_3 = settingValue;
   }
   else if(settingName == "PASSWORD_3") {
 		 if (VARIOMETER_PASSWORD_3 != settingValue) ValeurDifferente = true;
     VARIOMETER_PASSWORD_3 = settingValue;
   }
   else if(settingName == "SSID_4") {
		 if (VARIOMETER_SSID_4 != settingValue) ValeurDifferente = true;
     VARIOMETER_SSID_4 = settingValue;
   }
   else if(settingName == "PASSWORD_4") {
 		 if (VARIOMETER_PASSWORD_4 != settingValue) ValeurDifferente = true;
     VARIOMETER_PASSWORD_4 = settingValue;
   }		
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_00") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_00=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_01") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_01=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_02") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_02=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_03") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_03=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_04") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_04=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_05") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_05=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_06") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_06=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_07") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_07=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_08") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_08=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_09") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_09=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_10") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_10=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_GYRO_CAL_BIAS_11") {
	   VARIO_VERTACCEL_GYRO_CAL_BIAS_11=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_ACCEL_CAL_BIAS_00") {
	   VARIO_VERTACCEL_ACCEL_CAL_BIAS_00=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_ACCEL_CAL_BIAS_01") {
	   VARIO_VERTACCEL_ACCEL_CAL_BIAS_01=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_ACCEL_CAL_BIAS_02") {
	   VARIO_VERTACCEL_ACCEL_CAL_BIAS_02=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_ACCEL_CAL_SCALE") {
	   VARIO_VERTACCEL_ACCEL_CAL_SCALE=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_MAG_CAL_BIAS_00") {
	   VARIO_VERTACCEL_MAG_CAL_BIAS_00=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_MAG_CAL_BIAS_01") {
	   VARIO_VERTACCEL_MAG_CAL_BIAS_01=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_MAG_CAL_BIAS_02") {
	   VARIO_VERTACCEL_MAG_CAL_BIAS_02=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_MAG_CAL_PROJ_SCALE") {
	   VARIO_VERTACCEL_MAG_CAL_PROJ_SCALE=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER") {
	   VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_MAG_CAL_BIAS_MULTIPLIER") {
	   VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER=settingValue.toInt();
	 }
   else if(settingName == "VERTACCEL_MAG_CAL_BIAS_MULTIPLIER") {
	   VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER=settingValue.toInt();
	 }
   else if(settingName == "BT_ENABLE") {
	   VARIOMETER_ENABLE_BT=toBoolean(settingValue);
	 }
   else if(settingName == "COMPENSATION_TEMP") {
	   // Kalman filter configuration
     COMPENSATION_TEMP=toFloat(settingValue);
   }
   else if(settingName == "COMPENSATION_GPSALTI") {
	   // Kalman filter configuration
     COMPENSATION_GPSALTI=toFloat(settingValue);
   }	 
	 //Paramètre LOG 
   else if(settingName == "LOG_ENABLE") {
		 varioLog.setEnableDebug(toBoolean(settingValue));
    }	 
   else if(settingName == "LOG_SERIAL_ENABLE") {
		 varioLog.setEnableSerial(toBoolean(settingValue));
    }	 
   else if(settingName == "LOG_SD_ENABLE") {
		 varioLog.setEnableSdCard(toBoolean(settingValue));
    }	 
   else if(settingName == "LOG_DEBUG_ENABLE") {
		 varioLog.setEnableDebugESP32(toBoolean(settingValue));
    }	 
   else if(settingName == "MAIN_DEBUG") {
		 varioLog.setDebug(MAIN_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "SCREEN_DEBUG") {
		 varioLog.setDebug(SCREEN_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "GPS_DEBUG") {
		 varioLog.setDebug(GPS_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "BUTTON_DEBUG") {
		 varioLog.setDebug(BUTTON_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "MS5611_DEBUG") {
		 varioLog.setDebug(MS5611_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "MPU_DEBUG") {
		 varioLog.setDebug(MPU_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "SDCARD_DEBUG") {
		 varioLog.setDebug(SDCARD_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "BT_DEBUG") {
		 varioLog.setDebug(BT_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "WIFI_DEBUG") {
		 varioLog.setDebug(WIFI_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "DEEPSLEEP_DEBUG") {
		 varioLog.setDebug(DEEPSLEEP_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "VOLTAGE_DEBUG") {
		 varioLog.setDebug(VOLTAGE_DEBUG_LOG,toBoolean(settingValue));
    }	 
   else if(settingName == "DATA_DEBUG") {
		 varioLog.setDebug(DATA_DEBUG_LOG,toBoolean(settingValue));
    }	 
  else {       
   }  
	return ValeurDifferente;
}
 
 // converting string to Float
float VarioSettings::toFloat(String settingValue){
   char floatbuf[settingValue.length()+1];
   settingValue.toCharArray(floatbuf, sizeof(floatbuf));
   float f = atof(floatbuf);
   return f;
}
 
long VarioSettings::toLong(String settingValue){
   char longbuf[settingValue.length()+1];
   settingValue.toCharArray(longbuf, sizeof(longbuf));
   long l = atol(longbuf);
   return l;
}
 
 
 // Converting String to integer and then to boolean
 // 1 = true
 // 0 = false
boolean VarioSettings::toBoolean(String settingValue) {
  if(settingValue.toInt()==1){
    return true;
  } else {
    return false;
  }
}
 
 // Writes A Configuration file DEBBUG
#ifdef HAVE_SDCARD

void VarioSettings::writeFlashSDSettings() {
		
#ifdef SDFAT_LIB
SdFile myFile2;
#else //SDFAT_LIB
File myFile2;
#endif //SDFAT_LIB

/*  SD.remove("FLASH.TXT");
   myFile2 = SDHAL.open("FLASH.TXT", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile2) {
    SerialPort.print("Writing to FLASH.TXT...");
    myFile2.println("testing 1, 2, 3.");
    // close the file:
    myFile2.close();
    SerialPort.println("done.");
  } else {
    // if the file didn't open, print an error:
    SerialPort.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile2 = SDHAL.open("FLASH.TXT");
  if (myFile2) {
    SerialPort.println("FLASH.TXT:");

    // read from the file until there's nothing else in it:
    while (myFile2.available()) {
      SerialPort.write(myFile2.read());
    }
    // close the file:
    myFile2.close();
  } else {
    // if the file didn't open, print an error:
    SerialPort.println("error opening FLASH.TXT");
  }	*/

  
  // Delete the old One
  SDHAL_SD.remove(FileFlashName);
  // Create new one
#ifdef SDFAT_LIB
  if (myFile2.open(FileFlashName, O_WRONLY | O_CREAT)) {
#else //SDFAT_LIB
  myFile2 = SDHAL_SD.open(FileFlashName, FILE_WRITE);
  if (myFile2) {
#endif //SDFAT_LIB	  
#ifdef SDCARD_DEBUG
        //Debuuging Printing
    SerialPort.println("Write File SD");
#endif //SDCARD_DEBUG

     // writing in the file works just like regular print()/println() function
    myFile2.print("[");
    myFile2.print("SOUND=");
    myFile2.print(VARIOMETER_BEEP_VOLUME);
    myFile2.println("]");
    myFile2.print("[");
    myFile2.print("ACCELCALX=");
    myFile2.print(ACCELCALX,5);
    myFile2.println("]");
    myFile2.print("[");
    myFile2.print("ACCELCALY=");
    myFile2.print(ACCELCALY,5);
    myFile2.println("]");
    myFile2.print("[");
    myFile2.print("ACCELCALZ=");
    myFile2.print(ACCELCALZ,5);
    myFile2.println("]");
  
 /* myFile.print("[");
  myFile.print("exBoolean=");
  myFile.print(exBoolean);
  myFile.println("]");
  myFile.print("[");
  myFile.print("exLong=");
  myFile.print(exLong);
  myFile.println("]");*/
  // close the file:
   // myFile.flush();
    myFile2.close();
	
#ifdef SDCARD_DEBUG
        //Debuuging Printing
 	SerialPort.println("Writing done.");
#endif //SDCARD_DEBUG

   } else {
   // if the file didn't open, print an error:
#ifdef SDCARD_DEBUG
   SerialPort.print("error opening : ");
   SerialPort.println(FileFlashName);
#endif //SDCARD_DEBUG   
  }
}
#endif

 // Writes A Configuration file DEBBUG
#ifdef HAVE_SDCARD

//**********************************************************
void VarioSettings::writeWifiSDSettings(char *filename) {
//**********************************************************
		
#ifdef SDFAT_LIB
SdFile myFile2;
#else  //SDFAT_LIB
File myFile2;
#endif //SDFAT_LIB
  
  // Delete the old One
  SDHAL_SD.remove(filename);
  // Create new one

#ifdef SDFAT_LIB
  if (myFile2.open(filename, O_WRONLY | O_CREAT)) {
#else //SDFAT_LIB
  myFile2 = SDHAL_SD.open(filename, FILE_WRITE);
  if (myFile2) {
#endif //SDFAT_LIB	  
#ifdef SDCARD_DEBUG
        //Debuuging Printing
    SerialPort.println("Write File SD");
#endif //SDCARD_DEBUG

     // writing in the file works just like regular print()/println() function
    myFile2.print("[");
    myFile2.print("SSID_1=");
    myFile2.print(VARIOMETER_SSID_1);
    myFile2.println("]");
    myFile2.print("[");
    myFile2.print("PASSWORD_1=");
    myFile2.print( VARIOMETER_PASSWORD_1);
    myFile2.println("]");
		myFile2.println("");
    myFile2.print("[");
    myFile2.print("SSID_2=");
    myFile2.print(VARIOMETER_SSID_2);
    myFile2.println("]");
    myFile2.print("[");
    myFile2.print("PASSWORD_2=");
    myFile2.print( VARIOMETER_PASSWORD_2);
    myFile2.println("]");
		myFile2.println("");
    myFile2.print("[");
    myFile2.print("SSID_3=");
    myFile2.print(VARIOMETER_SSID_3);
    myFile2.println("]");
    myFile2.print("[");
    myFile2.print("PASSWORD_3=");
    myFile2.print( VARIOMETER_PASSWORD_3);
    myFile2.println("]");
		myFile2.println("");
    myFile2.print("[");
    myFile2.print("SSID_4=");
    myFile2.print(VARIOMETER_SSID_4);
    myFile2.println("]");
    myFile2.print("[");
    myFile2.print("PASSWORD_4=");
    myFile2.print( VARIOMETER_PASSWORD_4);
    myFile2.println("]");
		myFile2.println("");
  

  // close the file:
    myFile2.flush();
    myFile2.close();
	
#ifdef SDCARD_DEBUG
        //Debuuging Printing
 	SerialPort.println("Writing done.");
#endif //SDCARD_DEBUG

  } else {
   // if the file didn't open, print an error:
#ifdef SDCARD_DEBUG
   SerialPort.print("error opening : ");
   SerialPort.println(filename);
#endif //SDCARD_DEBUG   
  }
}
#endif

#ifdef HAVE_SDCARD
boolean VarioSettings::readFlashSDSettings(){
  char character;
  String settingName;
  String settingValue;
#ifdef SDFAT_LIB
  SdFile myFile2;
#else //SDFAT_LIB
  File myFile2;
#endif //SDFAT_LIB

#ifdef SDCARD_DEBUG

        //Debuuging Printing
        SerialPort.println("readFlashSDSettings");
#endif //SDCARD_DEBUG

#ifdef SDFAT_LIB
  if (myFile2.open(FileFlashName, O_RDONLY)) {
#else //SDFAT_LIB
  myFile2 = SDHAL_SD.open(FileFlashName);
  if (myFile2) {
#endif //SDFAT_LIB
    while (myFile2.available()) {
      character = myFile2.read();
      while((myFile2.available()) && (character != '[')){
        character = myFile2.read();
      }
      character = myFile2.read();
      while((myFile2.available()) && (character != '=')){
        settingName = settingName + character;
        character = myFile2.read();
      }
      character = myFile2.read();
      while((myFile2.available()) && (character != ']')){
        settingValue = settingValue + character;
        character = myFile2.read();
      }
      
      if(character == ']'){
 
#ifdef SDCARD_DEBUG

        //Debuuging Printing
        SerialPort.print("Name:");
        SerialPort.println(settingName);
        SerialPort.print("Value :");
        SerialPort.println(settingValue);
#endif //SDCARD_DEBUG

        // Apply the value to the parameter
        applyFlashSetting(settingName,settingValue);
        // Reset Strings
        settingName = "";
        settingValue = "";
      }
    }
 
    // close the file:
    myFile2.close();
	return true;
  } else {
   // if the file didn't open, print an error:
#ifdef SDCARD_DEBUG
   SerialPort.print("error opening : ");
   SerialPort.println(FileFlashName);
#endif //SDCARD_DEBUG   
   return false;
  }

/*File myFile2;

  SDHAL.remove("FLASH.TXT");
  myFile2 = SDHAL.open("FLASH.TXT", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile2) {
    SerialPort.print("Writing to FLASH.TXT...");
    myFile2.println("testing 1, 2, 3.");
    // close the file:
    myFile2.close();
    SerialPort.println("done.");
  } else {
    // if the file didn't open, print an error:
    SerialPort.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile2 = SDHAL.open("FLASH.TXT");
  if (myFile2) {
    SerialPort.println("FLASH.TXT:");

    // read from the file until there's nothing else in it:
    while (myFile2.available()) {
      SerialPort.write(myFile2.read());
    }
    // close the file:
    myFile2.close();
  } else {
    // if the file didn't open, print an error:
    SerialPort.println("error opening FLASH.TXT");
  }*/

}
#endif
 
 /* Apply the value to the parameter by searching for the parameter name
 Using String.toInt(); for Integers
 toFloat(string); for Float
 toBoolean(string); for Boolean
 toLong(string); for Long
 */
 void VarioSettings::applyFlashSetting(String settingName, String settingValue) {
 
   if (settingName == "SOUND") {
#ifdef SDCARD_DEBUG
       SerialPort.println("Sound Read File : " + settingValue);
#endif //SDCARD_DEBUG
     VARIOMETER_BEEP_VOLUME = settingValue.toInt();
   }
   else if(settingName == "ACCELCALX") {
     ACCELCALX=toFloat(settingValue);
   }
   else if(settingName == "ACCELCALY") {
     ACCELCALY=toFloat(settingValue);
   }
   else if(settingName == "ACCELCALZ") {
     ACCELCALZ=toFloat(settingValue);
   }
 }   
   
   
uint8_t VarioSettings::soundSettingRead(void) {
  /* check tag */
  uint16_t eepromTag;
  uint8_t tmpValue=0;
  
  if (!EEPROMHAL.isValid()) {
/*#ifdef HAVE_SDCARD		
    SerialPort.println("EEPROM is empty, writing some example data:");
	  if (!readFlashSDSettings()) {
	    VARIOMETER_BEEP_VOLUME=5;
	    tmpValue = 5; 
	  }
	  else {
	    tmpValue = VARIOMETER_BEEP_VOLUME;	
	  }
#endif*/
		//Erreur Memoire 
		tmpValue = DEFAULT_VARIOMETER_BEEP_VOLUME;	
  } else {
	  
    eepromTag = EEPROMHAL.read(SOUND_EEPROM_ADDR);
    eepromTag <<= 8;
    eepromTag += EEPROMHAL.read(SOUND_EEPROM_ADDR + 0x01);
  
//    uint8_t TmpValue;
    if( eepromTag != SOUND_EEPROM_TAG ) { 
/*#ifdef HAVE_SDCARD
	    if (!readFlashSDSettings()) {
	      VARIOMETER_BEEP_VOLUME=5;
	      tmpValue = 5; 
	    }
	    else {
	      tmpValue = VARIOMETER_BEEP_VOLUME;	
	    }
#endif*/
		//Memoire vide
			tmpValue = DEFAULT_VARIOMETER_BEEP_VOLUME;	
			soundSettingWrite(tmpValue);
#ifdef SOUND_DEBUG
      SerialPort.print("Memoire vide - sound value : ");
      SerialPort.println(tmpValue);
#endif //SOUND_DEBUG
    } else {
      /* read calibration settings */
      tmpValue =  EEPROMHAL.read(SOUND_EEPROM_ADDR + 0x02);
#ifdef SOUND_DEBUG
      SerialPort.print("Read sound value : ");
      SerialPort.println(tmpValue);
#endif //SOUND_DEBUG

    }
  }

#ifdef SOUND_DEBUG
  SerialPort.print("Sound value : ");
  SerialPort.println(tmpValue);
#endif //SOUND_DEBUG

  if ((tmpValue<0) || (tmpValue>10)) {tmpValue=5;}
  return tmpValue;
}

void VarioSettings::soundSettingWrite(uint8_t volume) {
#ifdef SOUND_DEBUG
  SerialPort.print("Write sound volume : ");
  SerialPort.println(volume);
#endif //SDCARD_DEBUG

  /* write tag */
  uint16_t eepromTag = SOUND_EEPROM_TAG;
  EEPROMHAL.write(SOUND_EEPROM_ADDR, (eepromTag>>8) & 0xff);
  EEPROMHAL.write(SOUND_EEPROM_ADDR + 0x01, eepromTag & 0xff);

  EEPROMHAL.write(SOUND_EEPROM_ADDR + 0x02 , volume);
  
  EEPROMHAL.commit();
  
  VARIOMETER_BEEP_VOLUME=volume;
 // writeFlashSDSettings();
}

//**********************************************************
void VarioSettings::loadConfigurationVario(char *filename) {
//**********************************************************
  // Open file for reading
	boolean MajFileParams = false;
#ifdef SDFAT_LIB
  SdFile file;
	if (!file.open(filename, O_RDONLY)) {
#else
  File file = SDHAL_SD.open(filename, FILE_READ);
  if (!file) {
#endif
    SerialPort.println(F("Failed to read file"));
    return;
  }

#ifdef SDCARD_DEBUG
	SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
#endif

//	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(11) + JSON_OBJECT_SIZE(12) + 790;
//	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(12) + JSON_OBJECT_SIZE(13) + 1090;
//  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(14) + JSON_OBJECT_SIZE(17) + 1090;
//  DynamicJsonDocument doc(capacity);

// Clearing Buffer
	doc.clear();

#ifdef SDCARD_DEBUG
	SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
#endif

#ifdef SDCARD_DEBUG
  SerialPort.println("deserialisation");
#endif

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    SerialPort.println(F("Failed to read file, using default configuration"));

  int    tmpValue;
  float  tmpValueFloat;
  long   tmpValueLong;
	String tmpValueString;

#ifdef SDCARD_DEBUG
  SerialPort.println("Paramètres : ");
#endif

	const char* GnuvarioE_version = doc["gnuvarioe"]["version"]; // "1.0"
	if (strcmp(GnuvarioE_version, PARAMS_VERSION) != 0) MajFileParams = true;

	//*****    SYSTEME *****

#ifdef SDCARD_DEBUG
  SerialPort.println("****** Systeme *******");
#endif

  JsonObject Systeme = doc["systeme"];
	
	if (Systeme.containsKey("BT_ENABLE")) {
	  tmpValue = Systeme["BT_ENABLE"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValue = DEFAULT_VARIOMETER_ENABLE_BT;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
  if (tmpValue == 1) VARIOMETER_ENABLE_BT = true;
  else               VARIOMETER_ENABLE_BT = false;
#ifdef SDCARD_DEBUG
  SerialPort.print("BT_ENABLE : ");
  SerialPort.println(VARIOMETER_ENABLE_BT);
#endif
    
	if (Systeme.containsKey("NO_RECORD")) {
		tmpValue = Systeme["NO_RECORD"]; 
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValue = DEFAULT_NO_RECORD;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
  if (tmpValue == 1) NO_RECORD = true;
  else               NO_RECORD = false;
#ifdef SDCARD_DEBUG
  SerialPort.print("NO_RECORD : ");
  SerialPort.println(NO_RECORD);
#endif

	if (Systeme.containsKey("ALARM_SDCARD")) {
		   tmpValue = Systeme["ALARM_SDCARD"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValue = DEFAULT_ALARM_SDCARD;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
  if (tmpValue == 1) ALARM_SDCARD = true;
  else               ALARM_SDCARD = false;
#ifdef SDCARD_DEBUG
  SerialPort.print("ALARM_SDCARD : ");
  SerialPort.println(ALARM_SDCARD);
#endif

	if (Systeme.containsKey("BEEP_GPSFIX")) {	
		   tmpValue = Systeme["BEEP_GPSFIX"]; 
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}	else {
		tmpValue = DEFAULT_ALARM_GPSFIX;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
  if (tmpValue == 1) ALARM_GPSFIX = true;
  else               ALARM_GPSFIX = false;
#ifdef SDCARD_DEBUG
  SerialPort.print("BEEP_GPSFIX : ");
  SerialPort.println(ALARM_GPSFIX);
#endif

  if (Systeme.containsKey("BEEP_FLYBEGIN")) {	
	   	 tmpValue = Systeme["BEEP_FLYBEGIN"]; 
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValue = DEFAULT_ALARM_FLYBEGIN;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
  if (tmpValue == 1) ALARM_FLYBEGIN = true;
  else               ALARM_FLYBEGIN = false;
#ifdef SDCARD_DEBUG
  SerialPort.print("BEEP_FLYBEGIN : ");
  SerialPort.println(ALARM_FLYBEGIN);
#endif

  if (Systeme.containsKey("BEEP_FLYBEGIN")) {	
		   tmpValue = Systeme["BEEP_VARIOBEGIN"]; 
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValue = DEFAULT_ALARM_VARIOBEGIN;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
  if (tmpValue == 1) ALARM_VARIOBEGIN = true;
  else               ALARM_VARIOBEGIN = false;
#ifdef SDCARD_DEBUG
  SerialPort.print("BEEP_VARIOBEGIN : ");
  SerialPort.println(ALARM_VARIOBEGIN);
#endif

  if (Systeme.containsKey("COMPENSATION_TEMP")) {	
	 tmpValueFloat = Systeme["COMPENSATION_TEMP"]; 
#ifdef SDCARD_DEBUG
	 SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValueFloat = DEFAULT_COMPENSATION_TEMP;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
  COMPENSATION_TEMP = tmpValueFloat;
#ifdef SDCARD_DEBUG
  SerialPort.print("COMPENSATION_TEMP : ");
  SerialPort.println(COMPENSATION_TEMP);
#endif

  if (Systeme.containsKey("COMPENSATION_GPSALTI")) {	
		   tmpValue = Systeme["COMPENSATION_GPSALTI"]; 
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValue = DEFAULT_COMPENSATION_GPSALTI;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
  COMPENSATION_GPSALTI = tmpValue;
#ifdef SDCARD_DEBUG
  SerialPort.print("COMPENSATION_GPSALTI : ");
  SerialPort.println(COMPENSATION_GPSALTI);
#endif

  if (Systeme.containsKey("SLEEP_TIMEOUT_MINUTES")) {	
		   tmpValue = Systeme["SLEEP_TIMEOUT_MINUTES"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValue = DEFAULT_SLEEP_TIMEOUT_MINUTES;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	SLEEP_TIMEOUT_MINUTES = tmpValue;
#ifdef SDCARD_DEBUG
  SerialPort.print("SLEEP_TIMEOUT_MINUTES : ");
  SerialPort.println(SLEEP_TIMEOUT_MINUTES);
#endif
	
  if (Systeme.containsKey("SLEEP_THRESHOLD_CPS")) {	
		   tmpValueFloat = Systeme["SLEEP_THRESHOLD_CPS"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValueFloat = DEFAULT_SLEEP_THRESHOLD_CPS;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	SLEEP_THRESHOLD_CPS = tmpValueFloat;
#ifdef SDCARD_DEBUG
  SerialPort.print("SLEEP_THRESHOLD_CPS : ");
  SerialPort.println(SLEEP_THRESHOLD_CPS);
#endif
	
	if (Systeme.containsKey("MULTIDISPLAY_DURATION")) {
	     tmpValue = Systeme["MULTIDISPLAY_DURATION"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	}
	else {
		tmpValue = VARIOMETER_MULTIDISPLAY_DURATION;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	VARIOMETER_MULTIDISPLAY_DURATION = tmpValue; 
#ifdef SDCARD_DEBUG
  SerialPort.print("MULTIDISPLAY_DURATION : ");
  SerialPort.println(VARIOMETER_MULTIDISPLAY_DURATION);
#endif

  if (Systeme.containsKey("DISPLAY_STAT_DURATION")) {	
		   tmpValue = Systeme["DISPLAY_STAT_DURATION"];
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif
	} else {
		tmpValue = DEFAULT_DISPLAY_STAT_DURATION;
		MajFileParams = true;
#ifdef SDCARD_DEBUG
		SerialPort.print("Defaut Recup - ");
#endif
	}
	DISPLAY_STAT_DURATION = tmpValue;
#ifdef SDCARD_DEBUG
  SerialPort.print("DISPLAY_STAT_DURATION : ");
  SerialPort.println(DISPLAY_STAT_DURATION);
#endif

  if (Systeme.containsKey("URL_UPDATE")) {	
		String Systeme_URL_UPDATE = Systeme["URL_UPDATE"];
		tmpValueString = Systeme_URL_UPDATE;
#ifdef SDCARD_DEBUG
		SerialPort.print("Json Recup - ");
#endif

//**************************************
//*****************************************

	} else {
		tmpValueString = DEFAULT_URL_UPDATE;
		MajFileParams = true;
		SerialPort.print("Defaut Recup - ");
	}
	URL_UPDATE = tmpValueString;
  SerialPort.print("URL_UPDATE : ");
  SerialPort.println(URL_UPDATE);

	if (Systeme.containsKey("LANGUAGE")) {
		     tmpValue = Systeme["LANGUAGE"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_LANGUAGE;
		MajFileParams = true;																		
		SerialPort.print("Defaut Recup - ");
	}
  LANGUAGE = tmpValue;
  SerialPort.print("LANGUAGE : ");
  SerialPort.println(LANGUAGE);

	if (Systeme.containsKey("REF_VOLTAGE")) {
		     tmpValue = Systeme["REF_VOLTAGE"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_REF_VOLTAGE;
		MajFileParams = true;																		
		SerialPort.print("Defaut Recup - ");
	}
  REF_VOLTAGE = tmpValue;
  SerialPort.print("REF_VOLTAGE : ");
  SerialPort.println(REF_VOLTAGE);

	//*****    GENERAL *****

  SerialPort.println("****** General *******");

  JsonObject General = doc["general"];
	
	if (General.containsKey("PILOT_NAME")) {
		String General_PILOT_NAME = General["PILOT_NAME"]; // "Magali"
		tmpValueString = General_PILOT_NAME;
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueString = DEFAULT_VARIOMETER_PILOT_NAME;
		MajFileParams = true;
		SerialPort.print("Defaut Recup - ");
	}
  VARIOMETER_PILOT_NAME = tmpValueString;
  SerialPort.print("Pilot Name : ");
  SerialPort.println(VARIOMETER_PILOT_NAME);
  
	JsonObject General_GLIDER = General["GLIDER"];

	if (General_GLIDER.containsKey("GLIDER_SELECT")) {
			 tmpValue = General_GLIDER["GLIDER_SELECT"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_VARIOMETER_GLIDER_SELECT; 
		MajFileParams = true;		
		SerialPort.print("Defaut Recup - ");
	}
  VARIOMETER_GLIDER_SELECT = tmpValue;
  SerialPort.print("Glider Select : ");
  SerialPort.println(VARIOMETER_GLIDER_SELECT);  
	
	if (General_GLIDER.containsKey("GLIDER_NAME1")) {
		String General_GLIDER_GLIDER_NAME = General_GLIDER["GLIDER_NAME1"]; // "MAC-PARA Muse 3"
		tmpValueString = General_GLIDER_GLIDER_NAME;
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueString = DEFAULT_VARIOMETER_GLIDER_NAME;
		MajFileParams = true;		
		SerialPort.print("Defaut Recup - ");
	}
	VARIOMETER_GLIDER_TAB[0] = tmpValueString;
  SerialPort.print("Glider Name 1 : ");
  SerialPort.println(VARIOMETER_GLIDER_TAB[0]);

	if (General_GLIDER.containsKey("GLIDER_NAME2")) {
		String General_GLIDER_GLIDER_NAME2 = General_GLIDER["GLIDER_NAME2"]; // "MAC-PARA Muse 3"
		tmpValueString = General_GLIDER_GLIDER_NAME2;
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueString = "";
		MajFileParams = true;		
		SerialPort.print("Defaut Recup - ");
	}
	VARIOMETER_GLIDER_TAB[1] = tmpValueString;
  SerialPort.print("Glider Name 2 : ");
  SerialPort.println(VARIOMETER_GLIDER_TAB[1]);
	
	if (General_GLIDER.containsKey("GLIDER_NAME3")) {
		String General_GLIDER_GLIDER_NAME3 = General_GLIDER["GLIDER_NAME3"]; // "MAC-PARA Muse 3"
		tmpValueString = General_GLIDER_GLIDER_NAME3;
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueString = "";
		MajFileParams = true;		
		SerialPort.print("Defaut Recup - ");
	}
	VARIOMETER_GLIDER_TAB[2] = tmpValueString;
  SerialPort.print("Glider Name 3 : ");
  SerialPort.println(VARIOMETER_GLIDER_TAB[2]);
	
	if (General_GLIDER.containsKey("GLIDER_NAME4")) {
		String General_GLIDER_GLIDER_NAME4 = General_GLIDER["GLIDER_NAME4"]; // "MAC-PARA Muse 3"
		tmpValueString = General_GLIDER_GLIDER_NAME4;
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueString = "";
		MajFileParams = true;		
		SerialPort.print("Defaut Recup - ");
	}
	VARIOMETER_GLIDER_TAB[3] = tmpValueString;
  SerialPort.print("Glider Name 4 : ");
  SerialPort.println(VARIOMETER_GLIDER_TAB[3]);
	
	VARIOMETER_GLIDER_NAME = VARIOMETER_GLIDER_TAB[VARIOMETER_GLIDER_SELECT];
  SerialPort.print("Glider Name : ");
  SerialPort.println(VARIOMETER_GLIDER_NAME);

	if (General.containsKey("TIME_ZONE")) {
			 tmpValue = General["TIME_ZONE"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_VARIOMETER_TIME_ZONE; 
		MajFileParams = true;		
		SerialPort.print("Defaut Recup - ");
	}
  VARIOMETER_TIME_ZONE = tmpValue;
  SerialPort.print("Time Zone : ");
  SerialPort.println(VARIOMETER_TIME_ZONE);  

	//*****  VARIO *****

  SerialPort.println("****** Vario *******");

  JsonObject Vario = doc["vario"];
  
	if (Vario.containsKey("SINKING_THRESHOLD")) {
    tmpValueFloat = Vario["SINKING_THRESHOLD"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueFloat = DEFAULT_VARIOMETER_SINKING_THRESHOLD;
		MajFileParams = true;				
		SerialPort.print("Defaut Recup - ");
	}
  VARIOMETER_SINKING_THRESHOLD = tmpValueFloat;
  SerialPort.print("VARIOMETER_SINKING_THRESHOLD : ");
  SerialPort.println(VARIOMETER_SINKING_THRESHOLD);

	if (Vario.containsKey("CLIMBING_THRESHOLD")) {
    tmpValueFloat = Vario["CLIMBING_THRESHOLD"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueFloat = DEFAULT_VARIOMETER_CLIMBING_THRESHOLD;
		MajFileParams = true;				
		SerialPort.print("Defaut Recup - ");
	}
  VARIOMETER_CLIMBING_THRESHOLD = tmpValueFloat;
  SerialPort.print("VARIOMETER_CLIMBING_THRESHOLD : ");
  SerialPort.println(VARIOMETER_CLIMBING_THRESHOLD);

	if (Vario.containsKey("NEAR_CLIMBING_SENSITIVITY")) {
		tmpValueFloat = Vario["NEAR_CLIMBING_SENSITIVITY"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueFloat = DEFAULT_VARIOMETER_NEAR_CLIMBING_SENSITIVITY;
		MajFileParams = true;						
		SerialPort.print("Defaut Recup - ");
	}
  VARIOMETER_NEAR_CLIMBING_SENSITIVITY = tmpValueFloat;
  SerialPort.print("NEAR_CLIMBING_SENSITIVITY : ");
  SerialPort.println(VARIOMETER_NEAR_CLIMBING_SENSITIVITY);

	if (Vario.containsKey("ENABLE_NEAR_CLIMBING_ALARM")) {
				 tmpValue = Vario["ENABLE_NEAR_CLIMBING_ALARM"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM;
		MajFileParams = true;								
		SerialPort.print("Defaut Recup - ");
	}
  if (tmpValue == 1) VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM = true;
  else              VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM = false;
  SerialPort.print("ENABLE_NEAR_CLIMBING_ALARM : ");
  SerialPort.println(VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM);

	if (Vario.containsKey("ENABLE_NEAR_CLIMBING_BEEP")) {
			   tmpValue = Vario["ENABLE_NEAR_CLIMBING_BEEP"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP;
		MajFileParams = true;										
		SerialPort.print("Defaut Recup - ");
	}
  if (tmpValue == 1) VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP = true;
  else              VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP = false;
  SerialPort.print("ENABLE_NEAR_CLIMBING_BEEP : ");
  SerialPort.println(VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP);

	if (Vario.containsKey("DISPLAY_INTEGRATED_CLIMB_RATE")) {
		     tmpValue = Vario["DISPLAY_INTEGRATED_CLIMB_RATE"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE;
		MajFileParams = true;												
		SerialPort.print("Defaut Recup - ");
	}
  if (tmpValue == 1) VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE = true;
  else              VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE = false;
  SerialPort.print("DISPLAY_INTEGRATED_CLIMB_RATE : ");
  SerialPort.println(VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE);

	if (Vario.containsKey("RATIO_CLIMB_RATE")) {
		     tmpValue = Vario["RATIO_CLIMB_RATE"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_RATIO_CLIMB_RATE;
		MajFileParams = true;																
		SerialPort.print("Defaut Recup - ");
	}
  RATIO_CLIMB_RATE = tmpValue;
  SerialPort.print("RATIO_CLIMB_RATE : ");
  SerialPort.println(RATIO_CLIMB_RATE);

	if (Vario.containsKey("CLIMB_PERIOD_COUNT")) {
		     tmpValue = Vario["CLIMB_PERIOD_COUNT"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_SETTINGS_CLIMB_PERIOD_COUNT;
		MajFileParams = true;																		
		SerialPort.print("Defaut Recup - ");
	}
  SETTINGS_CLIMB_PERIOD_COUNT = tmpValue;
  SerialPort.print("CLIMB_PERIOD_COUNT : ");
  SerialPort.println(SETTINGS_CLIMB_PERIOD_COUNT);

	if (Vario.containsKey("SETTINGS_GLIDE_RATIO_PERIOD_COUNT")) {
		     tmpValue = Vario["SETTINGS_GLIDE_RATIO_PERIOD_COUNT"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_SETTINGS_GLIDE_RATIO_PERIOD_COUNT;
		MajFileParams = true;																		
		SerialPort.print("Defaut Recup - ");
	}
  SETTINGS_GLIDE_RATIO_PERIOD_COUNT = tmpValue;
  SerialPort.print("SETTINGS_GLIDE_RATIO_PERIOD_COUNT : ");
  SerialPort.println(SETTINGS_GLIDE_RATIO_PERIOD_COUNT);

	if (Vario.containsKey("RATIO_MAX_VALUE")) {
		tmpValueFloat = Vario["RATIO_MAX_VALUE"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueFloat = DEFAULT_RATIO_MAX_VALUE;
		MajFileParams = true;																				
		SerialPort.print("Defaut Recup - ");
	}
  RATIO_MAX_VALUE = tmpValueFloat;
  SerialPort.print("RATIO_MAX_VALUE : ");
  SerialPort.println(RATIO_MAX_VALUE);

	if (Vario.containsKey("RATIO_MIN_SPEED")) {
		tmpValueFloat = Vario["RATIO_MIN_SPEED"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueFloat = DEFAULT_RATIO_MIN_SPEED;
		MajFileParams = true;																						
		SerialPort.print("Defaut Recup - ");
	}
  RATIO_MIN_SPEED = tmpValueFloat;
  SerialPort.print("RATIO_MIN_SPEED : ");
  SerialPort.println(RATIO_MIN_SPEED);

	if (Vario.containsKey("VARIOMETER_ENABLE_AGL")) {
		     tmpValue = Vario["VARIOMETER_ENABLE_AGL"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_VARIOMETER_ENABLE_AGL;
		MajFileParams = true;												
		SerialPort.print("Defaut Recup - ");
	}
  if (tmpValue == 1) VARIOMETER_ENABLE_AGL = true;
  else              VARIOMETER_ENABLE_AGL = false;
  SerialPort.print("VARIOMETER_ENABLE_AGL : ");
  SerialPort.println(VARIOMETER_ENABLE_AGL);

	if (Vario.containsKey("SENT_LXNAV_SENTENCE")) {
		tmpValue = Vario["SENT_LXNAV_SENTENCE"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_VARIOMETER_SENT_LXNAV_SENTENCE;
		MajFileParams = true;		
		SerialPort.print("Defaut Recup - ");
	}
	VARIOMETER_SENT_LXNAV_SENTENCE =  tmpValue;
  SerialPort.print("SENT_LXNAV_SENTENCE : ");
  SerialPort.println(VARIOMETER_SENT_LXNAV_SENTENCE);

	if (Vario.containsKey("ACCELERATION_MEASURE_STANDARD_DEVIATION")) {
		     tmpValueFloat = Vario["ACCELERATION_MEASURE_STANDARD_DEVIATION"]; 
				 //valeur entre 3.00 et 6.00
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueFloat = DEFAULT_ACCELERATION_MEASURE_STANDARD_DEVIATION;
		MajFileParams = true;																		
		SerialPort.print("Defaut Recup - ");
	}
  ACCELERATION_MEASURE_STANDARD_DEVIATION = tmpValueFloat;
  SerialPort.print("ACCELERATION_MEASURE_STANDARD_DEVIATION : ");
  SerialPort.println(ACCELERATION_MEASURE_STANDARD_DEVIATION);

	if (Vario.containsKey("VARIOMETER_INTEGRATED_CLIMB_RATE")) {
		     tmpValue = Vario["VARIOMETER_INTEGRATED_CLIMB_RATE"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_VARIOMETER_INTEGRATED_CLIMB_RATE;
		MajFileParams = true;												
		SerialPort.print("Defaut Recup - ");
	}
  if (tmpValue == 1) VARIOMETER_INTEGRATED_CLIMB_RATE = true;
  else              VARIOMETER_INTEGRATED_CLIMB_RATE = false;
  SerialPort.print("VARIOMETER_INTEGRATED_CLIMB_RATE : ");
  SerialPort.println(VARIOMETER_INTEGRATED_CLIMB_RATE);

	if (Vario.containsKey("SETTINGS_VARIO_PERIOD_COUNT")) {
				//valeur comprise entre 0 et 40 soit 0 à 2s avec un pas de 50ms
		     tmpValue = Vario["SETTINGS_VARIO_PERIOD_COUNT"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_SETTINGS_VARIO_PERIOD_COUNT;
		MajFileParams = true;																		
		SerialPort.print("Defaut Recup - ");
	}
  SETTINGS_VARIO_PERIOD_COUNT = tmpValue;
  SerialPort.print("SETTINGS_VARIO_PERIOD_COUNT : ");
  SerialPort.println(SETTINGS_VARIO_PERIOD_COUNT);

	if (Vario.containsKey("BLUETOOTH_SEND_CALIBRATED_ALTITUDE")) {
		     tmpValue = Vario["BLUETOOTH_SEND_CALIBRATED_ALTITUDE"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_BLUETOOTH_SEND_CALIBRATED_ALTITUDE;
		MajFileParams = true;												
		SerialPort.print("Defaut Recup - ");
	}
  if (tmpValue == 1) BLUETOOTH_SEND_CALIBRATED_ALTITUDE = true;
  else               BLUETOOTH_SEND_CALIBRATED_ALTITUDE = false;
  SerialPort.print("BLUETOOTH_SEND_CALIBRATED_ALTITUDE : ");
  SerialPort.println(BLUETOOTH_SEND_CALIBRATED_ALTITUDE);

	//*****  FLIGHT START *****

  SerialPort.println("****** Flight start *******");

  JsonObject Flight_start = doc["flightstart"];

	if  (Flight_start.containsKey("FLIGHT_START_MIN_TIMESTAMP")) {
		tmpValueLong = Flight_start["FLIGHT_START_MIN_TIMESTAMP"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueLong = DEFAULT_FLIGHT_START_MIN_TIMESTAMP;
		MajFileParams = true;																						
		SerialPort.print("Defaut Recup - ");
	}
  FLIGHT_START_MIN_TIMESTAMP = tmpValueLong;
  SerialPort.print("FLIGHT_START_MIN_TIMESTAMP : ");
  SerialPort.println(FLIGHT_START_MIN_TIMESTAMP);

	if   (Flight_start.containsKey("FLIGHT_START_VARIO_LOW_THRESHOLD")) {
		tmpValueFloat = Flight_start["FLIGHT_START_VARIO_LOW_THRESHOLD"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueFloat = DEFAULT_FLIGHT_START_VARIO_LOW_THRESHOLD;
		MajFileParams = true;																						
		SerialPort.print("Defaut Recup - ");
	}
  FLIGHT_START_VARIO_LOW_THRESHOLD = tmpValueFloat;
  SerialPort.print("FLIGHT_START_VARIO_LOW_THRESHOLD : ");
  SerialPort.println(FLIGHT_START_VARIO_LOW_THRESHOLD);

	if   (Flight_start.containsKey("FLIGHT_START_VARIO_HIGH_THRESHOLD")) {
		tmpValueFloat = Flight_start["FLIGHT_START_VARIO_HIGH_THRESHOLD"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueFloat = DEFAULT_FLIGHT_START_VARIO_HIGH_THRESHOLD;
		MajFileParams = true;																						
		SerialPort.print("Defaut Recup - ");
	}
  FLIGHT_START_VARIO_HIGH_THRESHOLD = tmpValueFloat;
  SerialPort.print("FLIGHT_START_VARIO_HIGH_THRESHOLD : ");
  SerialPort.println(FLIGHT_START_VARIO_HIGH_THRESHOLD);

	if   (Flight_start.containsKey("FLIGHT_START_MIN_SPEED")) {
		tmpValueFloat = Flight_start["FLIGHT_START_MIN_SPEED"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValueFloat = DEFAULT_FLIGHT_START_MIN_SPEED;
		MajFileParams = true;																						
		SerialPort.print("Defaut Recup - ");
	}
  FLIGHT_START_MIN_SPEED = tmpValueFloat;
  SerialPort.print("FLIGHT_START_MIN_SPEED : ");
  SerialPort.println(FLIGHT_START_MIN_SPEED);

	if (Flight_start.containsKey("RECORD_WHEN_FLIGHT_START")) {
		   tmpValue = Flight_start["RECORD_WHEN_FLIGHT_START"]; 
		SerialPort.print("Json Recup - ");
	} else {
		tmpValue = DEFAULT_VARIOMETER_RECORD_WHEN_FLIGHT_START;
		MajFileParams = true;																						
		SerialPort.print("Defaut Recup - ");
	}
  if (tmpValue == 1) VARIOMETER_RECORD_WHEN_FLIGHT_START = true;
  else               VARIOMETER_RECORD_WHEN_FLIGHT_START = false;
  SerialPort.print("RECORD_WHEN_FLIGHT_START : ");
  SerialPort.println(VARIOMETER_RECORD_WHEN_FLIGHT_START);

/*  SerialPort.println("****** Wifi *******");

  JsonObject Wifi = doc["Wifi"];

  String Wifi_WIFI_1_SSID = Wifi["WIFI_1"]["SSID"]; // "MAC-PARA Muse 3"
  VARIOMETER_SSID_1 = Wifi_WIFI_1_SSID;
  SerialPort.print("Wifi SSID 1 : ");
  SerialPort.println(VARIOMETER_SSID_1);

  String Wifi_WIFI_1_PASSWORD = Wifi["WIFI_1"]["PASSWORD"]; 
  VARIOMETER_PASSWORD_1 = Wifi_WIFI_1_PASSWORD;
  SerialPort.print("Wifi Password 1 : ");
  SerialPort.println(VARIOMETER_PASSWORD_1);

  String Wifi_WIFI_2_SSID = Wifi["WIFI_2"]["SSID"]; 
  VARIOMETER_SSID_2 = Wifi_WIFI_2_SSID;
  SerialPort.print("Wifi SSID 2 : ");
  SerialPort.println(VARIOMETER_SSID_2);

  String Wifi_WIFI_2_PASSWORD = Wifi["WIFI_2"]["PASSWORD"]; 
  VARIOMETER_PASSWORD_2 = Wifi_WIFI_2_PASSWORD;
  SerialPort.print("Wifi Password 2 : ");
  SerialPort.println(VARIOMETER_PASSWORD_2);

  String Wifi_WIFI_3_SSID = Wifi["WIFI_3"]["SSID"];
  VARIOMETER_SSID_3 = Wifi_WIFI_3_SSID;
  SerialPort.print("Wifi SSID 3 : ");
  SerialPort.println(VARIOMETER_SSID_3);

  String Wifi_WIFI_3_PASSWORD = Wifi["WIFI_3"]["PASSWORD"]; 
  VARIOMETER_PASSWORD_3 = Wifi_WIFI_3_PASSWORD;
  SerialPort.print("Wifi Password 3 : ");
  SerialPort.println(VARIOMETER_PASSWORD_3);

  String Wifi_WIFI_4_SSID = Wifi["WIFI_4"]["SSID"]; 
  VARIOMETER_SSID_4 = Wifi_WIFI_4_SSID;
  SerialPort.print("Wifi SSID 4 : ");
  SerialPort.println(VARIOMETER_SSID_4);

  String Wifi_WIFI_4_PASSWORD = Wifi["WIFI_4"]["PASSWORD"]; 
  VARIOMETER_PASSWORD_4 = Wifi_WIFI_4_PASSWORD;
  SerialPort.print("Wifi Password 4 : ");
  SerialPort.println(VARIOMETER_PASSWORD_4);*/

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
	
	SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
// Clearing Buffer
	doc.clear();

// Trying To Delete The Document
//	doc.delete();
//	doc.~BasicJsonDocument();
	SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
	
	//Mise à jour du fichier params.jso
	if (MajFileParams) {
		SerialPort.println("Sauvegarde de nouveaux paramètres");
		saveConfigurationVario(filename);
	}

}


// Saves the configuration to a file
//**********************************************************
void VarioSettings::saveConfigurationVario(char *filename) {
//**********************************************************
  // Delete existing file, otherwise the configuration is appended to the file
  SDHAL_SD.remove(filename);

  // Open file for writing
#ifdef SDFAT_LIB
  SdFile file;
  if (!file.open(filename, O_WRONLY | O_CREAT)) {
#else //SDFAT_LIB
  File file = SDHAL_SD.open(filename, FILE_WRITE);
  if (!file) {
#endif
    SerialPort.println(F("Failed to create file"));
    return;
  }

  SerialPort.println("****** SAUVEGARDE params.jso *******");
	SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
//	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(11) + JSON_OBJECT_SIZE(12) + 790;
//	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(12) + JSON_OBJECT_SIZE(13) + 1090;
//  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(14) + JSON_OBJECT_SIZE(17) + 1090;
//	DynamicJsonDocument doc(capacity);

// Clearing Buffer
	doc.clear();

  SerialPort.println("****** GnuvarioE *******");

  // Set the values in the document
	JsonObject GnuvarioE = doc.createNestedObject("gnuvarioe");
	GnuvarioE["version"] = PARAMS_VERSION;

	//*****    SYSTEME *****

  SerialPort.println("****** Systeme *******");

	JsonObject Systeme = doc.createNestedObject("systeme");
	 
	if (VARIOMETER_ENABLE_BT) Systeme["BT_ENABLE"] = 1;
	else 											Systeme["BT_ENABLE"] = 0;
	
  if (NO_RECORD) Systeme["NO_RECORD"] = 1;
  else           Systeme["NO_RECORD"] = 0;

  if (ALARM_SDCARD) Systeme["ALARM_SDCARD"] = 1; 
  else              Systeme["ALARM_SDCARD"] = 0; 

  if (ALARM_GPSFIX) Systeme["BEEP_GPSFIX"] = 1;
  else              Systeme["BEEP_GPSFIX"] = 0;

  if (ALARM_FLYBEGIN) Systeme["BEEP_FLYBEGIN"] = 1;
  else                Systeme["BEEP_FLYBEGIN"] = 0;

  if (ALARM_VARIOBEGIN) Systeme["BEEP_VARIOBEGIN"] = 1;
  else                  Systeme["BEEP_VARIOBEGIN"] = 0;

	
	Systeme["COMPENSATION_TEMP"] = COMPENSATION_TEMP;

  Systeme["COMPENSATION_GPSALTI"] = COMPENSATION_GPSALTI;
	
	Systeme["SLEEP_TIMEOUT_MINUTES"] = SLEEP_TIMEOUT_MINUTES;
	
	Systeme["SLEEP_THRESHOLD_CPS"] = SLEEP_THRESHOLD_CPS;
	
	Systeme["MULTIDISPLAY_DURATION"] = VARIOMETER_MULTIDISPLAY_DURATION;

	Systeme["DISPLAY_STAT_DURATION"] = DISPLAY_STAT_DURATION;
	
	Systeme["URL_UPDATE"] = URL_UPDATE;

	Systeme["LANGUAGE"] = LANGUAGE;

	Systeme["REF_VOLTAGE"] = REF_VOLTAGE;
		
	//*****    GENERAL *****

  SerialPort.println("****** General *******");

	JsonObject General = doc.createNestedObject("general");
	
	General["PILOT_NAME"] = VARIOMETER_PILOT_NAME;
  
//  General["GLIDER_NAME"] = VARIOMETER_GLIDER_NAME;

	JsonObject General_GLIDER = General.createNestedObject("GLIDER");
	General_GLIDER["GLIDER_SELECT"] = VARIOMETER_GLIDER_SELECT;
	General_GLIDER["GLIDER_NAME1"] = VARIOMETER_GLIDER_TAB[0];
	General_GLIDER["GLIDER_NAME2"] = VARIOMETER_GLIDER_TAB[1];
	General_GLIDER["GLIDER_NAME3"] = VARIOMETER_GLIDER_TAB[2];
	General_GLIDER["GLIDER_NAME4"] = VARIOMETER_GLIDER_TAB[3];


  General["TIME_ZONE"] = VARIOMETER_TIME_ZONE;

	//*****    VARIO *****

  SerialPort.println("****** Vario *******");

	JsonObject Vario = doc.createNestedObject("vario");
	
	Vario["SINKING_THRESHOLD"] =  VARIOMETER_SINKING_THRESHOLD;

  Vario["CLIMBING_THRESHOLD"] = VARIOMETER_CLIMBING_THRESHOLD;

  Vario["NEAR_CLIMBING_SENSITIVITY"] = VARIOMETER_NEAR_CLIMBING_SENSITIVITY;

  if (VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM) Vario["ENABLE_NEAR_CLIMBING_ALARM"] = 1;
  else              												 Vario["ENABLE_NEAR_CLIMBING_ALARM"] = 0;

  if (VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP) Vario["ENABLE_NEAR_CLIMBING_BEEP"] = 1;
  else              												Vario["ENABLE_NEAR_CLIMBING_BEEP"] = 0;

  if (VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE) Vario["DISPLAY_INTEGRATED_CLIMB_RATE"] = 1;
  else              														Vario["DISPLAY_INTEGRATED_CLIMB_RATE"] = 0;

  Vario["RATIO_CLIMB_RATE"] = RATIO_CLIMB_RATE;

  Vario["CLIMB_PERIOD_COUNT"] = SETTINGS_CLIMB_PERIOD_COUNT;

  Vario["SETTINGS_GLIDE_RATIO_PERIOD_COUNT"] = SETTINGS_GLIDE_RATIO_PERIOD_COUNT;

  Vario["RATIO_MAX_VALUE"] = RATIO_MAX_VALUE;

  Vario["RATIO_MIN_SPEED"] = RATIO_MIN_SPEED;

  if (VARIOMETER_ENABLE_AGL) Vario["VARIOMETER_ENABLE_AGL"] = 1;
  else              				 Vario["VARIOMETER_ENABLE_AGL"] = 0;

/*  
  if (VARIOMETER_SENT_LXNAV_SENTENCE) Systeme["SENT_LXNAV_SENTENCE"] = 1;
  else              									Systeme["SENT_LXNAV_SENTENCE"] = 0;
*/
  Vario["VARIOMETER_SENT_LXNAV_SENTENCE"] = VARIOMETER_SENT_LXNAV_SENTENCE;

  Vario["ACCELERATION_MEASURE_STANDARD_DEVIATION"] = ACCELERATION_MEASURE_STANDARD_DEVIATION;

  if (VARIOMETER_INTEGRATED_CLIMB_RATE) Vario["VARIOMETER_INTEGRATED_CLIMB_RATE"] = 1;
  else              				 						Vario["VARIOMETER_INTEGRATED_CLIMB_RATE"] = 0;

  Vario["SETTINGS_VARIO_PERIOD_COUNT"] = SETTINGS_VARIO_PERIOD_COUNT;
	
  if (BLUETOOTH_SEND_CALIBRATED_ALTITUDE) Vario["BLUETOOTH_SEND_CALIBRATED_ALTITUDE"] = 1;
  else              				 						  Vario["BLUETOOTH_SEND_CALIBRATED_ALTITUDE"] = 0;
	
	//*****    Flight_Start *****

  SerialPort.println("****** Flight start *******");
	
	JsonObject Flight_start = doc.createNestedObject("flightstart");
	
	
  Flight_start["FLIGHT_START_MIN_TIMESTAMP"] = FLIGHT_START_MIN_TIMESTAMP;
 /* SerialPort.print("FLIGHT_START_MIN_TIMESTAMP : ");
  SerialPort.println(Flight_start["FLIGHT_START_MIN_TIMESTAMP"]);*/

  Flight_start["FLIGHT_START_VARIO_LOW_THRESHOLD"] = FLIGHT_START_VARIO_LOW_THRESHOLD;
/*  SerialPort.print("FLIGHT_START_VARIO_LOW_THRESHOLD : ");
  SerialPort.println(FLIGHT_START_VARIO_LOW_THRESHOLD);*/

  Flight_start["FLIGHT_START_VARIO_HIGH_THRESHOLD"] = FLIGHT_START_VARIO_HIGH_THRESHOLD;
/*  SerialPort.print("FLIGHT_START_VARIO_HIGH_THRESHOLD : ");
  SerialPort.println(FLIGHT_START_VARIO_HIGH_THRESHOLD);*/

  Flight_start["FLIGHT_START_MIN_SPEED"] = FLIGHT_START_MIN_SPEED;
/*  SerialPort.print("FLIGHT_START_MIN_SPEED : ");
  SerialPort.println(FLIGHT_START_MIN_SPEED);	*/
	
  if (VARIOMETER_RECORD_WHEN_FLIGHT_START) Flight_start["RECORD_WHEN_FLIGHT_START"] = 1;
  else              											 Flight_start["RECORD_WHEN_FLIGHT_START"] = 0;
/*  SerialPort.print("RECORD_WHEN_FLIGHT_START : ");
  SerialPort.println(VARIOMETER_RECORD_WHEN_FLIGHT_START);*/
	
/*	JsonObject Wifi = doc.createNestedObject("Wifi");

	JsonObject Wifi_WIFI_1 = Wifi.createNestedObject("WIFI_1");
	Wifi_WIFI_1["SSID"] = VARIOMETER_SSID_1;
	Wifi_WIFI_1["PASSWORD"] = VARIOMETER_PASSWORD_1;

	JsonObject Wifi_WIFI_2 = Wifi.createNestedObject("WIFI_2");
	Wifi_WIFI_2["SSID"] = VARIOMETER_SSID_2;
	Wifi_WIFI_2["PASSWORD"] = VARIOMETER_PASSWORD_2;

	JsonObject Wifi_WIFI_3 = Wifi.createNestedObject("WIFI_3");
	Wifi_WIFI_3["SSID"] = VARIOMETER_SSID_3;
	Wifi_WIFI_3["PASSWORD"] = VARIOMETER_PASSWORD_3;

	JsonObject Wifi_WIFI_4 = Wifi.createNestedObject("WIFI_4");
	Wifi_WIFI_4["SSID"] = VARIOMETER_SSID_4;
	Wifi_WIFI_4["PASSWORD"] = VARIOMETER_PASSWORD_4;*/
	
  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    SerialPort.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
// Clearing Buffer
	doc.clear();
	SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
}


//**********************************************************
void VarioSettings::setVersion(uint8_t version, uint8_t subVersion, uint8_t betaVersion) {
//**********************************************************
  GnuvarioVersion = String(version) + "." + String(subVersion);
	if (betaVersion > 0) GnuvarioVersion += "b" + String(betaVersion);
}

//**********************************************************
String VarioSettings::getVersion(void) {
//**********************************************************
  return GnuvarioVersion;
}

//**********************************************************
String VarioSettings::getScreenModel(void) {
//**********************************************************
#if (VARIOSCREEN_SIZE == 154)
	return "154";
#elif (VARIOSCREEN_SIZE == 290)
	return "154";
#elif (VARIOSCREEN_SIZE == 213)
	return "154";
#else 
	return "";
#endif
}


/*
void Statistic::setTime(int8_t* timeValue) {

  for(uint8_t i = 0; i<3; i++) {
    time[i] = timeValue[i];
  }
}

int8_t* Statistic::getTime(void) {
  return time;	
}

int8_t* Statistic::getTime(int8_t* timeValue) {
  for(uint8_t i = 0; i<3; i++) {
    timeValue[i] = time[i];
  }	

  return time;	
}

void Statistic::setDuration(int8_t* durationValue) {
  for(uint8_t i = 0; i<3; i++) {
    duration[i] = durationValue[i];
  }	
}
   
int8_t* Statistic::getDuration(void) {
  return duration;
}

int8_t* Statistic::getDuration(int8_t* durationValue) {
  for(uint8_t i = 0; i<3; i++) {
    durationValue[i] = duration[i];
  }	

  return duration;	
}   

void Statistic::setAlti(double alti) {
  if (currentAlti == 0) {
	 currentAlti = alti;
	 altiDeco = alti;
	 maxAlti = alti;
	 minAlti = alti;
  }

  if (alti > maxAlti) {maxAlti = alti;}
  else if (alti < minAlti) {minAlti = alti;}
}
   
double Statistic::getMaxAlti(void) {
  return maxAlti;	
}
   
double Statistic::getMinAlti(void) {
  return minAlti;	
}
   
void Statistic::setVario(double vario) {
  if (vario > maxVario) {maxVario = vario;}
  else if (vario < minVario) {minVario = vario;}	
}
   
double Statistic::getMaxVario(void) {
  return maxVario;		
}
   
double Statistic::getMinVario(void) {
  return minVario;		
}
   
void Statistic::setSpeed(double speed) {
  if (speed > maxSpeed) {maxSpeed = speed;}
  else if (speed < minSpeed) {minSpeed = speed;}		
}
   
double Statistic::getMaxSpeed(void) {
  return maxSpeed;			
}
   
double Statistic::getMinSpeed(void) {
  return minSpeed;				
}
   
double Statistic::getAltiDeco(void) {
  return currentAlti;	
}
   
double Statistic::getGain(void) {
  return maxAlti - currentAlti;	
}

*/

/*
{
    "GnuvarioE": {
        "version": "1.0"
    },
    "Systeme": {
        "BT_ENABLE": 0,
        "NO_RECORD": 0,
        "ALARM_SDCARD": 1,
        "BEEP_GPSFIX": 1,
        "BEEP_FLYBEGIN": 1,
				"BEEP_VARIOBEGIN": 0,
				"COMPENSATION_TEMP": -6.1,
				"COMPENSATION_GPSALTI": -70,
				"SLEEP_TIMEOUT_MINUTES": 20,
				"SLEEP_THRESHOLD_CPS": 50,
				"ALTERNATE_DATA_DURATION": 2000,
				"URL_UPDATE": "http://gnuvario-e.yj.fr/webupdate/checkversion",
				"LANGUAGE": 0,
				"REF_VOLTAGE": 2280
    },
    "General": {
        "PILOT_NAME": "MagaliXXXXXXXXXXXXXX",
	"GLIDER": {
			"GLIDER_SELECT": 0,
			"GLIDER_NAME": "MAC-PARA Muse 3XXXXX",
			"GLIDER_NAME2": "MAC-PARA Muse 3XXXXX",
			"GLIDER_NAME3": "MAC-PARA Muse 3XXXXX",
			"GLIDER_NAME4": "MAC-PARA Muse 3XXXXX"
	},
        "TIME_ZONE": 1
    },
    "Vario": {
        "SINKING_THRESHOLD": -2.1,
        "CLIMBING_THRESHOLD": 0.2,
        "NEAR_CLIMBING_SENSITIVITY": 0.5,
        "ENABLE_NEAR_CLIMBING_ALARM": 0,
        "ENABLE_NEAR_CLIMBING_BEEP": 0,
        "DISPLAY_INTEGRATED_CLIMB_RATE": 0,
        "CLIMB_PERIOD_COUNT": 10,
        "SETTINGS_GLIDE_RATIO_PERIOD_COUNT": 20,
        "RATIO_MAX_VALUE": 30.1,
        "RATIO_MIN_SPEED": 10.1,
        "RATIO_CLIMB_RATE": 2,
        "SENT_LXNAV_SENTENCE": 1,
				"VARIOMETER_ENABLE_AGL": 1,
				"ACCELERATION_MEASURE_STANDARD_DEVIATION": 0.35,
				"VARIOMETER_INTEGRATED_CLIMB_RATE": 0,
				"SETTINGS_VARIO_PERIOD_COUNT":5,
				"BLUETOOTH_SEND_CALIBRATED_ALTITUDE":0
   },
    "Flight start": {
        "FLIGHT_START_MIN_TIMESTAMP": 15000,
        "FLIGHT_START_VARIO_LOW_THRESHOLD": -0.5,
        "FLIGHT_START_VARIO_HIGH_THRESHOLD": 0.5,
        "FLIGHT_START_MIN_SPEED": 8.1,
        "RECORD_WHEN_FLIGHT_START": 1
    }
}

PARSING

const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(11) + JSON_OBJECT_SIZE(12) + 940;
DynamicJsonDocument doc(capacity);

const char* json = "{\"GnuvarioE\":{\"version\":\"1.0\"},\"Systeme\":{\"BT_ENABLE\":0,\"NO_RECORD\":0,\"ALARM_SDCARD\":1,\"BEEP_GPSFIX\":1,\"BEEP_FLYBEGIN\":1,\"BEEP_VARIOBEGIN\":0,\"COMPENSATION_TEMP\":-6.1,\"COMPENSATION_GPSALTI\":-70,\"SLEEP_TIMEOUT_MINUTES\":20,\"SLEEP_THRESHOLD_CPS\":50,\"ALTERNATE_DATA_DURATION\":2000},\"General\":{\"PILOT_NAME\":\"MagaliXXXXXXXXXXXXXX\",\"GLIDER\":{\"GLIDER_SELECT\":0,\"GLIDER_NAME\":\"MAC-PARA Muse 3XXXXX\",\"GLIDER_NAME2\":\"MAC-PARA Muse 3XXXXX\",\"GLIDER_NAME3\":\"MAC-PARA Muse 3XXXXX\",\"GLIDER_NAME4\":\"MAC-PARA Muse 3XXXXX\"},\"TIME_ZONE\":1},\"Vario\":{\"SINKING_THRESHOLD\":-2.1,\"CLIMBING_THRESHOLD\":0.2,\"NEAR_CLIMBING_SENSITIVITY\":0.5,\"ENABLE_NEAR_CLIMBING_ALARM\":0,\"ENABLE_NEAR_CLIMBING_BEEP\":0,\"DISPLAY_INTEGRATED_CLIMB_RATE\":0,\"CLIMB_PERIOD_COUNT\":10,\"SETTINGS_GLIDE_RATIO_PERIOD_COUNT\":20,\"RATIO_MAX_VALUE\":30.1,\"RATIO_MIN_SPEED\":10.1,\"RATIO_CLIMB_RATE\":2,\"SENT_LXNAV_SENTENCE\":1},\"Flight start\":{\"FLIGHT_START_MIN_TIMESTAMP\":15000,\"FLIGHT_START_VARIO_LOW_THRESHOLD\":-0.5,\"FLIGHT_START_VARIO_HIGH_THRESHOLD\":0.5,\"FLIGHT_START_MIN_SPEED\":8.1,\"RECORD_WHEN_FLIGHT_START\":1}}";

deserializeJson(doc, json);

const char* GnuvarioE_version = doc["GnuvarioE"]["version"]; // "1.0"

JsonObject Systeme = doc["Systeme"];
int Systeme_BT_ENABLE = Systeme["BT_ENABLE"]; // 0
int Systeme_NO_RECORD = Systeme["NO_RECORD"]; // 0
int Systeme_ALARM_SDCARD = Systeme["ALARM_SDCARD"]; // 1
int Systeme_BEEP_GPSFIX = Systeme["BEEP_GPSFIX"]; // 1
int Systeme_BEEP_FLYBEGIN = Systeme["BEEP_FLYBEGIN"]; // 1
int Systeme_BEEP_VARIOBEGIN = Systeme["BEEP_VARIOBEGIN"]; // 0
float Systeme_COMPENSATION_TEMP = Systeme["COMPENSATION_TEMP"]; // -6.1
int Systeme_COMPENSATION_GPSALTI = Systeme["COMPENSATION_GPSALTI"]; // -70
int Systeme_SLEEP_TIMEOUT_MINUTES = Systeme["SLEEP_TIMEOUT_MINUTES"]; // 20
int Systeme_SLEEP_THRESHOLD_CPS = Systeme["SLEEP_THRESHOLD_CPS"]; // 50
int Systeme_ALTERNATE_DATA_DURATION = Systeme["ALTERNATE_DATA_DURATION"]; // 2000

JsonObject General = doc["General"];
const char* General_PILOT_NAME = General["PILOT_NAME"]; // "Magali"

JsonObject General_GLIDER = General["GLIDER"];
int General_GLIDER_GLIDER_SELECT = General_GLIDER["GLIDER_SELECT"]; // 0
const char* General_GLIDER_GLIDER_NAME = General_GLIDER["GLIDER_NAME"]; // "MAC-PARA Muse 3"
const char* General_GLIDER_GLIDER_NAME2 = General_GLIDER["GLIDER_NAME2"]; // ""
const char* General_GLIDER_GLIDER_NAME3 = General_GLIDER["GLIDER_NAME3"]; // ""
const char* General_GLIDER_GLIDER_NAME4 = General_GLIDER["GLIDER_NAME4"]; // ""

int General_TIME_ZONE = General["TIME_ZONE"]; // 1

JsonObject Vario = doc["Vario"];
float Vario_SINKING_THRESHOLD = Vario["SINKING_THRESHOLD"]; // -2.1
float Vario_CLIMBING_THRESHOLD = Vario["CLIMBING_THRESHOLD"]; // 0.2
float Vario_NEAR_CLIMBING_SENSITIVITY = Vario["NEAR_CLIMBING_SENSITIVITY"]; // 0.5
int Vario_ENABLE_NEAR_CLIMBING_ALARM = Vario["ENABLE_NEAR_CLIMBING_ALARM"]; // 0
int Vario_ENABLE_NEAR_CLIMBING_BEEP = Vario["ENABLE_NEAR_CLIMBING_BEEP"]; // 0
int Vario_DISPLAY_INTEGRATED_CLIMB_RATE = Vario["DISPLAY_INTEGRATED_CLIMB_RATE"]; // 0
int Vario_CLIMB_PERIOD_COUNT = Vario["CLIMB_PERIOD_COUNT"]; // 10
int Vario_SETTINGS_GLIDE_RATIO_PERIOD_COUNT = Vario["SETTINGS_GLIDE_RATIO_PERIOD_COUNT"]; // 20
float Vario_RATIO_MAX_VALUE = Vario["RATIO_MAX_VALUE"]; // 30.1
float Vario_RATIO_MIN_SPEED = Vario["RATIO_MIN_SPEED"]; // 10.1
int Vario_RATIO_CLIMB_RATE = Vario["RATIO_CLIMB_RATE"]; // 2
int Vario_SENT_LXNAV_SENTENCE = Vario["SENT_LXNAV_SENTENCE"]; // 1

JsonObject Flight_start = doc["Flight start"];
int Flight_start_FLIGHT_START_MIN_TIMESTAMP = Flight_start["FLIGHT_START_MIN_TIMESTAMP"]; // 15000
float Flight_start_FLIGHT_START_VARIO_LOW_THRESHOLD = Flight_start["FLIGHT_START_VARIO_LOW_THRESHOLD"]; // -0.5
float Flight_start_FLIGHT_START_VARIO_HIGH_THRESHOLD = Flight_start["FLIGHT_START_VARIO_HIGH_THRESHOLD"]; // 0.5
float Flight_start_FLIGHT_START_MIN_SPEED = Flight_start["FLIGHT_START_MIN_SPEED"]; // 8.1
int Flight_start_RECORD_WHEN_FLIGHT_START = Flight_start["RECORD_WHEN_FLIGHT_START"]; // 1

SERIALIZING

const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(11) + JSON_OBJECT_SIZE(12);
DynamicJsonDocument doc(capacity);

JsonObject GnuvarioE = doc.createNestedObject("GnuvarioE");
GnuvarioE["version"] = "1.0";

JsonObject Systeme = doc.createNestedObject("Systeme");
Systeme["BT_ENABLE"] = 0;
Systeme["NO_RECORD"] = 0;
Systeme["ALARM_SDCARD"] = 1;
Systeme["BEEP_GPSFIX"] = 1;
Systeme["BEEP_FLYBEGIN"] = 1;
Systeme["BEEP_VARIOBEGIN"] = 0;
Systeme["COMPENSATION_TEMP"] = -6.1;
Systeme["COMPENSATION_GPSALTI"] = -70;
Systeme["SLEEP_TIMEOUT_MINUTES"] = 20;
Systeme["SLEEP_THRESHOLD_CPS"] = 50;
Systeme["ALTERNATE_DATA_DURATION"] = 2000;

JsonObject General = doc.createNestedObject("General");
General["PILOT_NAME"] = "MagaliXXXXXXXXXXXXXX";

JsonObject General_GLIDER = General.createNestedObject("GLIDER");
General_GLIDER["GLIDER_SELECT"] = 0;
General_GLIDER["GLIDER_NAME"] = "MAC-PARA Muse 3XXXXX";
General_GLIDER["GLIDER_NAME2"] = "MAC-PARA Muse 3XXXXX";
General_GLIDER["GLIDER_NAME3"] = "MAC-PARA Muse 3XXXXX";
General_GLIDER["GLIDER_NAME4"] = "MAC-PARA Muse 3XXXXX";
General["TIME_ZONE"] = 1;

JsonObject Vario = doc.createNestedObject("Vario");
Vario["SINKING_THRESHOLD"] = -2.1;
Vario["CLIMBING_THRESHOLD"] = 0.2;
Vario["NEAR_CLIMBING_SENSITIVITY"] = 0.5;
Vario["ENABLE_NEAR_CLIMBING_ALARM"] = 0;
Vario["ENABLE_NEAR_CLIMBING_BEEP"] = 0;
Vario["DISPLAY_INTEGRATED_CLIMB_RATE"] = 0;
Vario["CLIMB_PERIOD_COUNT"] = 10;
Vario["SETTINGS_GLIDE_RATIO_PERIOD_COUNT"] = 20;
Vario["RATIO_MAX_VALUE"] = 30.1;
Vario["RATIO_MIN_SPEED"] = 10.1;
Vario["RATIO_CLIMB_RATE"] = 2;
Vario["SENT_LXNAV_SENTENCE"] = 1;

JsonObject Flight_start = doc.createNestedObject("Flight start");
Flight_start["FLIGHT_START_MIN_TIMESTAMP"] = 15000;
Flight_start["FLIGHT_START_VARIO_LOW_THRESHOLD"] = -0.5;
Flight_start["FLIGHT_START_VARIO_HIGH_THRESHOLD"] = 0.5;
Flight_start["FLIGHT_START_MIN_SPEED"] = 8.1;
Flight_start["RECORD_WHEN_FLIGHT_START"] = 1;

serializeJson(doc, Serial);

*/