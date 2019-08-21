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
/*                                                                               */
/*********************************************************************************/

#include <VarioSettings.h>
#include <Arduino.h>
//#include <FlashAsEEPROM.h>
#include <eepromHAL.h>
#include <sdcardHAL.h>

#include <HardwareConfig.h>
#include <DebugConfig.h>

//SdFat SD;

boolean VarioSettings::initSettings() {
    if (!SDHAL.begin()) {
#ifdef SDCARD_DEBUG
      SerialPort.println("initialization failed!");
#endif //SDCARD_DEBUG
      return false;
    }
		
	EEPROMHAL.init(1024);
  if (!EEPROMHAL.isValid()) {
#ifdef EEPROM_DEBUG
      SerialPort.println("initialization failed!");
#endif //EEPROM_DEBUG
      return false;
    }
 		
  return true;
}

boolean VarioSettings::readSDSettings(){
  char character;
  String settingName;
  String settingValue;
  myFile = SDHAL.open(FileName);
  if (myFile) {
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
        applySetting(settingName,settingValue);
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
 
 /* Apply the value to the parameter by searching for the parameter name
 Using String.toInt(); for Integers
 toFloat(string); for Float
 toBoolean(string); for Boolean
 toLong(string); for Long
 */
 void VarioSettings::applySetting(String settingName, String settingValue) {
 
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
   else if(settingName == "VARIOMETER_ALTERNATE_PAGE_DURATION") {
	 VARIOMETER_ALTERNATE_PAGE_DURATION=settingValue.toInt();
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
 /*  else if(settingName == "VARIOMETER_SENT_LXNAV_SENTENCE") {
  /* What type of vario NMEA sentence is sent by bluetooth. */
  /* Possible values are :                                  */
  /*  - VARIOMETER_SENT_LXNAV_SENTENCE                      */
  /*  - VARIOMETER_SENT_LK8000_SENTENCE                     *
     VARIOMETER_SENT_LXNAV_SENTENCE=toBoolean(settingValue);
   }*/
   else if(settingName == "ALARM_SDCARD") {
     /* Alarm */
     /* Alarm SDCARD not insert */
     ALARM_SDCARD=toBoolean(settingValue);
   }
   else if(settingName == "ALARM_GPSFIX") {
	 /* Alarm GPS Fix */
     ALARM_GPSFIX=toBoolean(settingValue);
   }
   else if(settingName == "ALARM_FLYBEGIN") {
	 /* Alarm Fly begin */
     ALARM_FLYBEGIN=toBoolean(settingValue);
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
    else if(settingName == "SLEEP_TIMEOUT_SECONDS") {
		// Power-down timeout. Here we power down if the
		// vario does not see any climb or sink rate more than
		// 50cm/sec, for 20 minutes.
     SLEEP_TIMEOUT_SECONDS=settingValue.toInt(); // 20 minutes
   }
   else if(settingName == "SLEEP_THRESHOLD_CPS") {
	    // Power-down timeout. Here we power down if the
		// vario does not see any climb or sink rate more than
		// 50cm/sec, for 20 minutes.
     SLEEP_THRESHOLD_CPS=settingValue.toInt();
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
     VARIOMETER_SSID_1 = settingValue;
   }
   else if(settingName == "PASSWORD_1") {
     VARIOMETER_PASSWORD_1 = settingValue;
   }
   else if(settingName == "SSID_2") {
     VARIOMETER_SSID_2 = settingValue;
   }
   else if(settingName == "PASSWORD_2") {
     VARIOMETER_PASSWORD_2 = settingValue;
   }
   else if(settingName == "SSID_3") {
     VARIOMETER_SSID_3 = settingValue;
   }
   else if(settingName == "PASSWORD_3") {
     VARIOMETER_PASSWORD_3 = settingValue;
   }
   else if(settingName == "SSID_4") {
     VARIOMETER_SSID_4 = settingValue;
   }
   else if(settingName == "PASSWORD_4") {
     VARIOMETER_PASSWORD_4 = settingValue;
   }		
   else {       
   }  
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
void VarioSettings::writeFlashSDSettings() {
		
File myFile2;

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
  SDHAL.remove(FileFlashName);
  // Create new one
  myFile2 = SDHAL.open(FileFlashName, FILE_WRITE);
  if (myFile2) {
	  
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

boolean VarioSettings::readFlashSDSettings(){
  char character;
  String settingName;
  String settingValue;
  File myFile2;

#ifdef SDCARD_DEBUG

        //Debuuging Printing
        SerialPort.println("readFlashSDSettings");
#endif //SDCARD_DEBUG

  myFile2 = SDHAL.open(FileFlashName);
  if (myFile2) {
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
    SerialPort.println("EEPROM is empty, writing some example data:");
	  if (!readFlashSDSettings()) {
	    VARIOMETER_BEEP_VOLUME=5;
	    tmpValue = 5; 
	  }
	  else {
	    tmpValue = VARIOMETER_BEEP_VOLUME;	
	  }
  } else {
	  
    eepromTag = EEPROMHAL.read(SOUND_EEPROM_ADDR);
    eepromTag <<= 8;
    eepromTag += EEPROMHAL.read(SOUND_EEPROM_ADDR + 0x01);
  
    uint8_t TmpValue;
    if( eepromTag != SOUND_EEPROM_TAG ) { 
	    if (!readFlashSDSettings()) {
	      VARIOMETER_BEEP_VOLUME=5;
	      tmpValue = 5; 
	    }
	    else {
	      tmpValue = VARIOMETER_BEEP_VOLUME;	
	    }
    } else {
      /* read calibration settings */
      tmpValue =  EEPROMHAL.read(SOUND_EEPROM_ADDR + 0x02);
#ifdef SDCARD_DEBUG
      SerialPort.print("Read sound value : ");
      SerialPort.println(tmpValue);
#endif //SDCARD_DEBUG

    }
  }

#ifdef SDCARD_DEBUG
  SerialPort.print("Sound value : ");
  SerialPort.println(tmpValue);
#endif //SDCARD_DEBUG

  if ((tmpValue<0) || (tmpValue>10)) {tmpValue=5;}
  return tmpValue;
}

void VarioSettings::soundSettingWrite(uint8_t volume) {
#ifdef SDCARD_DEBUG
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

