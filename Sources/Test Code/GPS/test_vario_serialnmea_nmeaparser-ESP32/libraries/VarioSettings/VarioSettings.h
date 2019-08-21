#ifndef _VARIO_SETTINGS_H_
#define _VARIO_SETTINGS_H_

#include <Arduino.h>
#include <sdcardHAL.h>

#include "DebugConfig.h"
#include <HardwareConfig.h>

/*----------------------------*/
/*          SOFTWARE          */
/*      Vario parameters      */
/*                            */
/*----------------------------*/

#define VARIOMETER_MODEL "GNUVario"

/* calibration method */
// by EEPROM
//#define IMU_CALIBRATION_IN_EEPROM
// or by static value

/* Parametre par defaut */
#define IMU_GYRO_CAL_BIAS {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IMU_ACCEL_CAL_BIAS {0, 0, 0}
#define IMU_ACCEL_CAL_SCALE 0
#define IMU_MAG_CAL_BIAS {0, 0, 0}
#define IMU_MAG_CAL_PROJ_SCALE -166

//Version 1
//#define IMU_GYRO_CAL_BIAS {0x00, 0x00, 0x3f, 0xf0, 0xff, 0xff, 0xb8, 0x17, 0xff, 0xff, 0xa8, 0x2c}
//#define IMU_ACCEL_CAL_BIAS {-1943, 4749, -15216}
//#define IMU_ACCEL_CAL_SCALE -140
//#define IMU_MAG_CAL_BIAS {45, 3697, 2482}
//#define IMU_MAG_CAL_PROJ_SCALE -9714


/********************/
/* Measure behavior */
/********************/

/* Speed filtering :                                               */
/* Greater values give smoother speed. The base unit is 2 seconds  */
/* so size = 5 use the last 10 seconds to average speed.           */
#define VARIOMETER_SPEED_FILTER_SIZE 5

/* Set the GPS precision needed to use the GPS altitude value  */
/* to calibrate the barometric altitude.                       */
/*      !!! the best possible precision is 100 !!!             */ 
#define VARIOMETER_GPS_ALTI_CALIBRATION_PRECISION_THRESHOLD 200

/* time needed to power on all the devices */
/* Version 1 et 2                          */
#define VARIOMETER_POWER_ON_DELAY 2000

/******************************************************/
/******************************************************/


class VarioSettings {

 public:
  boolean initSettings();
  boolean readSDSettings();
  boolean readFlashSDSettings();
  void writeFlashSDSettings();
  uint8_t soundSettingRead(void);
  void soundSettingWrite(uint8_t volume);

#ifdef IMU_DEBUG 
  int exINT = 15;
  float exFloat = 1.12345;
  boolean exBoolean = true;
  long exLong = 2123456789;
#endif //IMU_DEBUG
  
  String VARIOMETER_PILOT_NAME = "Magali";
  String VARIOMETER_GLIDER_NAME = "MAC-PARA Muse 3";
  
  /* time zone relative to UTC */
  int8_t VARIOMETER_TIME_ZONE = (+2); 

  /*******************/
/* Screen behavior */
/*******************/

/* the duration of the two screen pages in milliseconds */
  int16_t VARIOMETER_BASE_PAGE_DURATION = 3000;
  int16_t VARIOMETER_ALTERNATE_PAGE_DURATION = 3000;
	int16_t VARIOMETER_MULTIDISPLAY_DURATION = 2000;

  /*********/
  /* Beeps */
  /*********/

  /* The volume of the beeps, max = 10 */
  uint8_t VARIOMETER_BEEP_VOLUME = 3;

  /* The variometer react like this according to vertical speed in m/s :        */
  /* (near climbing beep is not enabled by default)                             */
  /*                                                                            */
  /* <--LOW-BEEP--|------SILENT------|--NEAR-CLIMBING-BEEP--|--CLIMBING-BEEP--> */
  /*              |                  |                      |                   */
  /*           SINKING         CLIMBING-SENSITIVITY      CLIMBING               */
  float VARIOMETER_SINKING_THRESHOLD =-2.0;
  float VARIOMETER_CLIMBING_THRESHOLD=0.2;
  float VARIOMETER_NEAR_CLIMBING_SENSITIVITY=0.5; 
  
  /* The near climbing alarm : signal that you enter or exit the near climbing zone */
  /* The near climbing beep : beep when you are in near climbing zone               */
  boolean VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM = false;
  boolean VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP  = false;

  /********************/
  /* Measure behavior */
  /********************/

  /* Flight start detection conditions :                      */
  /* -> Minimum time after poweron in milliseconds            */
  /* -> Minimum vertical velocity in m/s (low/high threshold) */
  /* -> Minimum ground speed in km/h                          */
  long FLIGHT_START_MIN_TIMESTAMP = 15000;
  float FLIGHT_START_VARIO_LOW_THRESHOLD = (-0.5);
  float FLIGHT_START_VARIO_HIGH_THRESHOLD = 0.5;
  float FLIGHT_START_MIN_SPEED = 8.0;

  /* GPS track recording on SD card starting condition :  */ 
  /* -> As soon as possible (GPS fix)                     */
  /* -> When flight start is detected                     */
   boolean VARIOMETER_RECORD_WHEN_FLIGHT_START = true;

/* Display integrated climb rate or instantaneous values if disabled     */
/* If enabled set the integration time in ms.                            */
/* ! Climb rate integration time must not be more than glide ratio one ! */
/* **************************  boolean VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE = false;
  int VARIOMETER_CLIMB_RATE_INTEGRATION_TIME       = 2000;
  float VARIOMETER_INTEGRATED_CLIMB_RATE_DISPLAY_FREQ = 2.0;  ******************** */

/* Glide ratio display parameters  */
/* Integration time in ms.         */
// **********************************  long VARIOMETER_GLIDE_RATIO_INTEGRATION_TIME = 15000;

  /* What type of vario NMEA sentence is sent by bluetooth. */
  /* Possible values are :                                  */
  /*  - VARIOMETER_SENT_LXNAV_SENTENCE                      */
  /*  - VARIOMETER_SENT_LK8000_SENTENCE                     */
  //boolean VARIOMETER_SENT_LXNAV_SENTENCE = true;

  /* Alarm */
  /* Alarm SDCARD not insert */
  boolean ALARM_SDCARD = true;
  /* Alarm GPS Fix */
  boolean ALARM_GPSFIX = true;
  /* Alarm Fly begin */
  boolean ALARM_FLYBEGIN = true;

// Kalman filter configuration
  float KF_ZMEAS_VARIANCE  =     400.0f;
  float KF_ZACCEL_VARIANCE =     1000.0f;
  float KF_ACCELBIAS_VARIANCE   = 1.0f;

// Power-down timeout. Here we power down if the
// vario does not see any climb or sink rate more than
// 50cm/sec, for 20 minutes.
   uint16_t SLEEP_TIMEOUT_SECONDS   = 1200; // 20 minutes
   uint8_t  SLEEP_THRESHOLD_CPS		= 50;

// vario thresholds in cm/sec for generating different
// audio tones. Between the sink threshold and the zero threshold,
// the vario is quiet
/*                                                                            */
/* <--LOW-BEEP--|------SILENT------|--NEAR-CLIMBING-BEEP--|--CLIMBING-BEEP--> */
/*              |                  |                      |                   */
/*             SINK              ZERO                   CLIMB                 */
   uint8_t CLIMB_THRESHOLD   =   50;
   int8_t ZERO_THRESHOLD	 =    5;
   int16_t SINK_THRESHOLD    =   -250;

// change these parameters based on the frequency bandwidth of the speaker

    uint16_t VARIO_MAX_FREQHZ   =   4000;
    uint16_t VARIO_XOVER_FREQHZ =   2000;
    uint16_t VARIO_MIN_FREQHZ   =   200;

    uint16_t VARIO_SINK_FREQHZ  =   400;
    uint16_t VARIO_TICK_FREQHZ  =   200;

		float 	 RATIO_MAX_VALUE 		=		30.0;
		float		 RATIO_MIN_SPEED 		=		10.0;
		

// audio feedback tones
    uint16_t BATTERY_TONE_FREQHZ	=	400;
    uint16_t CALIB_TONE_FREQHZ		=	800;
    uint16_t MPU9250_ERROR_TONE_FREQHZ	= 200;
    uint16_t MS5611_ERROR_TONE_FREQHZ	= 2500;
    uint16_t SDCARD_ERROR_TONE_FREQHZ	= 2000;  
		uint16_t BEEP_FREQ                  = 800;
	
//Setting accelerometer
    double ACCELCALX = 0.0;
		double ACCELCALY = 0.0;
		double ACCELCALZ = 0.0;
  
 protected:
		File myFile;
//  File myFile2;
		char FileName[15] = "SETTINGS.TXT";
		char FileFlashName[15] = "FLASH.TXT";
  
		void applySetting(String settingName, String settingValue);
		void applyFlashSetting(String settingName, String settingValue);
		float toFloat(String settingValue);
		long toLong(String settingValue);
		boolean toBoolean(String settingValue);
};

extern VarioSettings GnuSettings;

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

#endif
