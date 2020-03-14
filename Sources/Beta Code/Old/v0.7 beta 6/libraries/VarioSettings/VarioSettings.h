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
/* 		1.1.0	13/11/19		Modification soundSettingRead / soundSettingWrite        */
/*                      Ajout lecture et sauvegarde fichier wifi.cfg 						 */
/*										  Ajout gestion automatique des version fichier params.jso */
/*										  Modif ALTERNATE_DATA_DURATION - MULTIDISPLAY_DURATION	   */
/*                      ajout gestion de plusieurs voiles                        */
/*    1.1.1 24/11/19    Modification DEFAULT_SLEEP_THRESHOLD_CPS                 */
/*    1.2   29/11/19    Modification sdfat                                       */
/*    1.2.1 12/12/19    Ajout set get version et get screenmodel                 */ 
/*    1.3.2 17/01/20    Ajout DISPLAY_STAT_DURATION - passage en v1.1 					 */
/*    1.3.3 19/01/20    Ajout DEEPSLEEP_DEBUG                                    */
/*    1.3.4 04/02/20    Ajout URL_UPDATE passage en version 1.2 de params.json   */
/*                                                                               */
/*********************************************************************************/

#ifndef _VARIO_SETTINGS_H_
#define _VARIO_SETTINGS_H_

#include <Arduino.h>

#include "DebugConfig.h"
#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif


#define PARAMS_VERSION "1.2"

/*----------------------------*/
/*          DEFAULT           */
/*      Vario parameters      */
/*                            */
/*----------------------------*/

#define DEFAULT_VARIOMETER_PILOT_NAME  										"Magali"
#define DEFAULT_VARIOMETER_GLIDER_NAME 										"MAC-PARA Muse 3"
#define DEFAULT_VARIOMETER_GLIDER_SELECT									0
#define DEFAULT_VARIOMETER_TIME_ZONE  										(+2) 
#define DEFAULT_VARIOMETER_BASE_PAGE_DURATION 						3000
#define DEFAULT_VARIOMETER_MULTIDISPLAY_DURATION 			  	2000
#define DEFAULT_VARIOMETER_BEEP_VOLUME 										3
#define DEFAULT_VARIOMETER_SINKING_THRESHOLD 							-2.0
#define DEFAULT_VARIOMETER_CLIMBING_THRESHOLD							0.2
#define DEFAULT_VARIOMETER_NEAR_CLIMBING_SENSITIVITY			0.5
#define DEFAULT_VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM   	false
#define DEFAULT_VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP    	false
#define DEFAULT_FLIGHT_START_MIN_TIMESTAMP 								15000
#define DEFAULT_FLIGHT_START_VARIO_LOW_THRESHOLD 					(-0.5)
#define DEFAULT_FLIGHT_START_VARIO_HIGH_THRESHOLD 				0.5
#define DEFAULT_FLIGHT_START_MIN_SPEED 										8.0
#define DEFAULT_VARIOMETER_RECORD_WHEN_FLIGHT_START 			true
#define DEFAULT_ALARM_SDCARD 															true
#define DEFAULT_ALARM_GPSFIX 															true
#define DEFAULT_ALARM_FLYBEGIN 														true
#define DEFAULT_SETTINGS_CLIMB_PERIOD_COUNT  							10
#define DEFAULT_SETTINGS_GLIDE_RATIO_PERIOD_COUNT 				20
#define DEFAULT_VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE	false
#define DEFAULT_RATIO_CLIMB_RATE 													1
#define DEFAULT_NO_RECORD 																false
#define DEFAULT_BATTERY_TONE_FREQHZ												400
#define DEFAULT_CALIB_TONE_FREQHZ													800
#define DEFAULT_MPU9250_ERROR_TONE_FREQHZ									200
#define DEFAULT_MS5611_ERROR_TONE_FREQHZ									2500
#define DEFAULT_SDCARD_ERROR_TONE_FREQHZ									2000
#define DEFAULT_BEEP_FREQ                  								800
#define DEFAULT_VARIOMETER_SENT_LXNAV_SENTENCE						1

#define DEFAULT_VARIOMETER_SSID_1													"your_SSID1"
#define DEFAULT_VARIOMETER_PASSWORD_1											"your_PASSWORD_for SSID1"

#define DEFAULT_VARIOMETER_SSID_2													"your_SSID2"
#define DEFAULT_VARIOMETER_PASSWORD_2											"your_PASSWORD_for SSID2"

#define DEFAULT_VARIOMETER_SSID_3													"your_SSID3"
#define DEFAULT_VARIOMETER_PASSWORD_3											"your_PASSWORD_for SSID3"

#define DEFAULT_VARIOMETER_SSID_4													"your_SSID4"
#define DEFAULT_VARIOMETER_PASSWORD_4											"your_PASSWORD_for SSID4"

#define DEFAULT_URL_UPDATE																"http://gnuvario-e.yj.fr/update/firmware.json"

#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_00								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_01								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_02								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_03								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_04								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_05								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_06								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_07								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_08								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_09								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_10								0x00
#define DEFAULT_VERTACCEL_GYRO_CAL_BIAS_11								0x00
#define DEFAULT_VERTACCEL_ACCEL_CAL_BIAS_00								0x00
#define DEFAULT_VERTACCEL_ACCEL_CAL_BIAS_01								0x00
#define DEFAULT_VERTACCEL_ACCEL_CAL_BIAS_02								0x00
#define DEFAULT_VERTACCEL_ACCEL_CAL_SCALE 								0
#define DEFAULT_VERTACCEL_MAG_CAL_BIAS_00									0
#define DEFAULT_VERTACCEL_MAG_CAL_BIAS_01									0
#define DEFAULT_VERTACCEL_MAG_CAL_BIAS_02									0
#define DEFAULT_VERTACCEL_MAG_CAL_PROJ_SCALE 							-16689
#define DEFAULT_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER 			7
#define DEFAULT_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER 				5

#define DEFAULT_RATIO_MAX_VALUE														30.0
#define DEFAULT_RATIO_MIN_SPEED														10.0

#define DEFAULT_VARIOMETER_ENABLE_BT										  0
#define DEFAULT_ALARM_VARIOBEGIN                          1

#define DEFAULT_COMPENSATION_TEMP													-6
#define DEFAULT_COMPENSATION_GPSALTI											-50

#define DEFAULT_SLEEP_TIMEOUT_MINUTES 										20
#define DEFAULT_SLEEP_THRESHOLD_CPS												0.5
#define DEFAULT_ALTERNATE_DATA_DURATION										2000

#define DEFAULT_DISPLAY_STAT_DURATION                     6

  
/*----------------------------*/
/*          SOFTWARE          */
/*      Vario parameters      */
/*                            */
/*----------------------------*/

#define VARIOMETER_MODEL "GNUVario"
#define VARIOMETER_MODEL_NAME "GnuVario-E"

/********************/
/* Measure behavior */
/********************/

/* Speed filtering :                                               */
/* Greater values give smoother speed. The base unit is 2 seconds  */
/* so size = 5 use the last 10 seconds to average speed.           */
#define VARIOMETER_SPEED_FILTER_SIZE 5


#define SCREEN_MODEL_154 "154"
#define SCREEN_MODEL_290 "290"
#define SCREEN_MODEL_213 "213"

/******************************************************/
/******************************************************/

class VarioSettings {

 public:
  boolean initSettings(bool initSD);
#ifdef HAVE_SDCARD
  boolean readSDSettings(char *FileName, boolean *ModifiedValue);
  boolean readFlashSDSettings();
  void writeFlashSDSettings();
	void loadConfigurationVario(char *filename);
	void saveConfigurationVario(char *filename);
	void writeWifiSDSettings(char *filename);
	
	void setVersion(uint8_t version, uint8_t subVersion, uint8_t betaVersion);
	String getVersion(void);
	String getScreenModel(void);
	
#endif
  uint8_t soundSettingRead(void);
  void soundSettingWrite(uint8_t volume);

#ifdef SDCARD_DEBUG 
  int exINT = 15;
  float exFloat = 1.12345;
  boolean exBoolean = true;
  long exLong = 2123456789;
#endif //SDCARD_DEBUG
  
  String VARIOMETER_PILOT_NAME 	= DEFAULT_VARIOMETER_PILOT_NAME;
	String VARIOMETER_GLIDER_TAB[4] = {DEFAULT_VARIOMETER_GLIDER_NAME, "", "", ""};
	uint8_t VARIOMETER_GLIDER_SELECT = DEFAULT_VARIOMETER_GLIDER_SELECT;
  String VARIOMETER_GLIDER_NAME = DEFAULT_VARIOMETER_GLIDER_NAME;
  
  /* time zone relative to UTC */
  int8_t VARIOMETER_TIME_ZONE 	= DEFAULT_VARIOMETER_TIME_ZONE; 

  /*******************/
/* Screen behavior */
/*******************/

/* the duration of the two screen pages in milliseconds */
  int16_t VARIOMETER_BASE_PAGE_DURATION 				= DEFAULT_VARIOMETER_BASE_PAGE_DURATION;
	int16_t VARIOMETER_MULTIDISPLAY_DURATION 			= DEFAULT_VARIOMETER_MULTIDISPLAY_DURATION;

  /*********/
  /* Beeps */
  /*********/

  /* The volume of the beeps, max = 10 */
  uint8_t VARIOMETER_BEEP_VOLUME								= DEFAULT_VARIOMETER_BEEP_VOLUME;

  /* The variometer react like this according to vertical speed in m/s :        */
  /* (near climbing beep is not enabled by default)                             */
  /*                                                                            */
  /* <--LOW-BEEP--|------SILENT------|--NEAR-CLIMBING-BEEP--|--CLIMBING-BEEP--> */
  /*              |                  |                      |                   */
  /*           SINKING         CLIMBING-SENSITIVITY      CLIMBING               */
  float VARIOMETER_SINKING_THRESHOLD 						= DEFAULT_VARIOMETER_SINKING_THRESHOLD;
  float VARIOMETER_CLIMBING_THRESHOLD						=	DEFAULT_VARIOMETER_CLIMBING_THRESHOLD;
  float VARIOMETER_NEAR_CLIMBING_SENSITIVITY		= DEFAULT_VARIOMETER_NEAR_CLIMBING_SENSITIVITY; 
  
  /* The near climbing alarm : signal that you enter or exit the near climbing zone */
  /* The near climbing beep : beep when you are in near climbing zone               */
  boolean VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM = DEFAULT_VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM;
  boolean VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP  = DEFAULT_VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP;

  /********************/
  /* Measure behavior */
  /********************/

  /* Flight start detection conditions :                      */
  /* -> Minimum time after poweron in milliseconds            */
  /* -> Minimum vertical velocity in m/s (low/high threshold) */
  /* -> Minimum ground speed in km/h                          */
  long FLIGHT_START_MIN_TIMESTAMP 							= DEFAULT_FLIGHT_START_MIN_TIMESTAMP;
  float FLIGHT_START_VARIO_LOW_THRESHOLD 				= DEFAULT_FLIGHT_START_VARIO_LOW_THRESHOLD;
  float FLIGHT_START_VARIO_HIGH_THRESHOLD 			= DEFAULT_FLIGHT_START_VARIO_HIGH_THRESHOLD;
  float FLIGHT_START_MIN_SPEED 									= DEFAULT_FLIGHT_START_MIN_SPEED;

  /* GPS track recording on SD card starting condition :  */ 
  /* -> As soon as possible (GPS fix)                     */
  /* -> When flight start is detected                     */
  boolean VARIOMETER_RECORD_WHEN_FLIGHT_START 	= DEFAULT_VARIOMETER_RECORD_WHEN_FLIGHT_START;

  /* What type of vario NMEA sentence is sent by bluetooth. */
  /* Possible values are :                                  */
  /*  - VARIOMETER_SENT_LXNAV_SENTENCE                      */
  /*  - VARIOMETER_SENT_LK8000_SENTENCE                     */
  //boolean VARIOMETER_SENT_LXNAV_SENTENCE 				= DEFAULT_VARIOMETER_SENT_LXNAV_SENTENCE;

  /* Alarm */
  /* Alarm SDCARD not insert */
  boolean ALARM_SDCARD 														= DEFAULT_ALARM_SDCARD;
  /* Nip when GPS Fix */
  boolean ALARM_GPSFIX 														= DEFAULT_ALARM_GPSFIX;
  /* Bip when Fly begin */
  boolean ALARM_FLYBEGIN 													= DEFAULT_ALARM_FLYBEGIN;
	
	/* Bip when vario begin  */
	boolean ALARM_VARIOBEGIN    										= DEFAULT_ALARM_VARIOBEGIN;                     

//*****************************************************************************
//*****************************************************************************

// Kalman filter configuration
  float KF_ZMEAS_VARIANCE  			=   400.0f;
  float KF_ZACCEL_VARIANCE 			=   1000.0f;
  float KF_ACCELBIAS_VARIANCE   = 	1.0f;

// Power-down timeout. Here we power down if the
// vario does not see any climb or sink rate more than
// 50cm/sec, for 20 minutes.
//   uint16_t SLEEP_TIMEOUT_SECONDS = 1200; // 20 minutes
//   uint8_t  SLEEP_THRESHOLD_CPS		= 50;

// vario thresholds in cm/sec for generating different
// audio tones. Between the sink threshold and the zero threshold,
// the vario is quiet
/*                                                                            */
/* <--LOW-BEEP--|------SILENT------|--NEAR-CLIMBING-BEEP--|--CLIMBING-BEEP--> */
/*              |                  |                      |                   */
/*             SINK              ZERO                   CLIMB                 */
   uint8_t CLIMB_THRESHOLD   			=   50;
   int8_t ZERO_THRESHOLD	 				=    5;
   int16_t SINK_THRESHOLD    			=   -250;

// change these parameters based on the frequency bandwidth of the speaker

    uint16_t VARIO_MAX_FREQHZ   	=   4000;
    uint16_t VARIO_XOVER_FREQHZ 	=   2000;
    uint16_t VARIO_MIN_FREQHZ   	=   200;

    uint16_t VARIO_SINK_FREQHZ  	=   400;
    uint16_t VARIO_TICK_FREQHZ  	=   200;

		float 	 RATIO_MAX_VALUE 			=		DEFAULT_RATIO_MAX_VALUE;
		float		 RATIO_MIN_SPEED 			=		DEFAULT_RATIO_MIN_SPEED;
		
//Setting accelerometer
    double ACCELCALX = 0.0;
		double ACCELCALY = 0.0;
		double ACCELCALZ = 0.0;
		
//********************************************************
//********************************************************		

// audio feedback tones
    uint16_t BATTERY_TONE_FREQHZ												=	DEFAULT_BATTERY_TONE_FREQHZ;
    uint16_t CALIB_TONE_FREQHZ													=	DEFAULT_CALIB_TONE_FREQHZ;
    uint16_t MPU9250_ERROR_TONE_FREQHZ									= DEFAULT_MPU9250_ERROR_TONE_FREQHZ;
    uint16_t MS5611_ERROR_TONE_FREQHZ										= DEFAULT_MS5611_ERROR_TONE_FREQHZ;
    uint16_t SDCARD_ERROR_TONE_FREQHZ										= DEFAULT_SDCARD_ERROR_TONE_FREQHZ;  
		uint16_t BEEP_FREQ                  								= DEFAULT_BEEP_FREQ ;
  
//Setting FightHistory	
//!!!!!!!!!!!!
// need to be moved to settings 
// unit is 500ms
		int8_t SETTINGS_CLIMB_PERIOD_COUNT  								= DEFAULT_SETTINGS_CLIMB_PERIOD_COUNT;
		int8_t SETTINGS_GLIDE_RATIO_PERIOD_COUNT 						= DEFAULT_SETTINGS_GLIDE_RATIO_PERIOD_COUNT;


		boolean VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE 		= DEFAULT_VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE;
		uint8_t RATIO_CLIMB_RATE 														= DEFAULT_RATIO_CLIMB_RATE;
		
		boolean NO_RECORD 																	= DEFAULT_NO_RECORD;
  
		String VARIOMETER_SSID_1 														= DEFAULT_VARIOMETER_SSID_1;
		String VARIOMETER_PASSWORD_1 												= DEFAULT_VARIOMETER_PASSWORD_1;

		String VARIOMETER_SSID_2 														= DEFAULT_VARIOMETER_SSID_2;
		String VARIOMETER_PASSWORD_2 												= DEFAULT_VARIOMETER_PASSWORD_2;

		String VARIOMETER_SSID_3 														= DEFAULT_VARIOMETER_SSID_3;
		String VARIOMETER_PASSWORD_3 												= DEFAULT_VARIOMETER_PASSWORD_3;

		String VARIOMETER_SSID_4 														= DEFAULT_VARIOMETER_SSID_4;
		String VARIOMETER_PASSWORD_4 												= DEFAULT_VARIOMETER_PASSWORD_4;
		
		uint8_t VARIO_VERTACCEL_GYRO_CAL_BIAS_00						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_00;
		uint8_t VARIO_VERTACCEL_GYRO_CAL_BIAS_01						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_01;
		uint8_t VARIO_VERTACCEL_GYRO_CAL_BIAS_02					  = DEFAULT_VERTACCEL_GYRO_CAL_BIAS_02;
		uint8_t VARIO_VERTACCEL_GYRO_CAL_BIAS_03						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_03;
		uint8_t VARIO_VERTACCEL_GYRO_CAL_BIAS_04						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_04;
		uint8_t VARIO_VERTACCEL_GYRO_CAL_BIAS_05						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_05;
		uint8_t VARIO_VERTACCEL_GYRO_CAL_BIAS_06						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_06;
		uint8_t  VARIO_VERTACCEL_GYRO_CAL_BIAS_07						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_07;
		uint8_t  VARIO_VERTACCEL_GYRO_CAL_BIAS_08						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_08;
		uint8_t  VARIO_VERTACCEL_GYRO_CAL_BIAS_09						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_09;
		uint8_t  VARIO_VERTACCEL_GYRO_CAL_BIAS_10						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_10;
		uint8_t  VARIO_VERTACCEL_GYRO_CAL_BIAS_11						= DEFAULT_VERTACCEL_GYRO_CAL_BIAS_11;
		uint16_t VARIO_VERTACCEL_ACCEL_CAL_BIAS_00					= DEFAULT_VERTACCEL_ACCEL_CAL_BIAS_00;
		uint16_t VARIO_VERTACCEL_ACCEL_CAL_BIAS_01					= DEFAULT_VERTACCEL_ACCEL_CAL_BIAS_01;
		uint16_t VARIO_VERTACCEL_ACCEL_CAL_BIAS_02					= DEFAULT_VERTACCEL_ACCEL_CAL_BIAS_02;
		uint16_t VARIO_VERTACCEL_ACCEL_CAL_SCALE    				= DEFAULT_VERTACCEL_ACCEL_CAL_SCALE;
		uint16_t VARIO_VERTACCEL_MAG_CAL_BIAS_00						= DEFAULT_VERTACCEL_MAG_CAL_BIAS_00;
		uint16_t VARIO_VERTACCEL_MAG_CAL_BIAS_01						= DEFAULT_VERTACCEL_MAG_CAL_BIAS_01;
		uint16_t VARIO_VERTACCEL_MAG_CAL_BIAS_02						= DEFAULT_VERTACCEL_MAG_CAL_BIAS_02;
		uint16_t VARIO_VERTACCEL_MAG_CAL_PROJ_SCALE 				= DEFAULT_VERTACCEL_MAG_CAL_PROJ_SCALE;
		uint16_t VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER	= DEFAULT_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER;
		uint16_t VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER 	 	= DEFAULT_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER;
		
		boolean  VARIOMETER_ENABLE_BT 											= DEFAULT_VARIOMETER_ENABLE_BT;

		float    COMPENSATION_TEMP													= DEFAULT_COMPENSATION_TEMP;
		int16_t  COMPENSATION_GPSALTI												= DEFAULT_COMPENSATION_GPSALTI;

		uint8_t	 SLEEP_TIMEOUT_MINUTES											= DEFAULT_SLEEP_TIMEOUT_MINUTES;
		float    SLEEP_THRESHOLD_CPS												= DEFAULT_SLEEP_THRESHOLD_CPS;
		uint16_t ALTERNATE_DATA_DURATION										= DEFAULT_ALTERNATE_DATA_DURATION;
		
		uint8_t  DISPLAY_STAT_DURATION											= DEFAULT_DISPLAY_STAT_DURATION;
		
		String   URL_UPDATE																	= DEFAULT_URL_UPDATE;
		
 protected:
#ifdef HAVE_SDCARD
#ifdef SDFAT_LIB
		SdFile myFile;
#else //SDFAT_LIB
		File myFile;
#endif //SDFAT_LIB
#endif
//  File myFile2;
//		char FileName[15] = "SETTINGS.TXT";
		char FileFlashName[15] = "FLASH.TXT";
		String GnuvarioVersion;
  
		boolean applySetting(String settingName, String settingValue);
		void applyFlashSetting(String settingName, String settingValue);
		float toFloat(String settingValue);
		long toLong(String settingValue);
		boolean toBoolean(String settingValue);
};

extern VarioSettings GnuSettings;


/*
class Statistic {

 public:
   void setTime(int8_t* timeValue);
   int8_t* getTime(void);
   int8_t* getTime(int8_t* timeValue);
   void setDuration(int8_t* durationValue);
   int8_t* getDuration(void);
   int8_t* getDuration(int8_t* durationValue);
   void setAlti(double alti);
   double getMaxAlti(void);
   double getMinAlti(void);
   void setVario(double vario);
   double getMaxVario(void);
   double getMinVario(void);
   void setSpeed(double speed);
   double getMaxSpeed(void);
   double getMinSpeed(void);
   double getAltiDeco(void);
   double getGain(void);

  private:
    int8_t time[3];
	int8_t duration[3];

    double currentSpeed=0;
    double maxSpeed;
    double minSpeed;
    double currentAlti=0;
    double maxAlti;
    double minAlti;
	double currentVario=0;
	double maxVario;
	double minVario;
	
	double altiDeco;
	double gain;
};
*/

#endif
