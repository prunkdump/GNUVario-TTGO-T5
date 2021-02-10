#ifndef _HARDWARECONFIG_H_
#define _HARDWARECONFIG_H_

/* HardwareConfig -- 
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
/*                           HardwareConfig                                      */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    03/06/19                                                            */
/*    1.0.1  24/06/19   Ajout profil GPS - choix entre 2 versions 							 */
/*    1.0.2  23/07/19   suppression des parametres CLIMB_RATE                    */
/*    1.0.3  04/08/19   Ajout config Wifi                                        */
/*    1.0.4  16/08/19   Ajout HAVE_WIFI                                          */
/*		1.0.5	 05/09/19		Ajout MPU_COMP_TEMP																			 */
/*    1.0.6  19/09/19   Ajout NB_ACQUISITION_FIX_GPS														 */
/*    1.0.7  06/10/19   Ajout R�solution de l'ecran                              */
/*    1.0.8  22/10/19   Ajout �cran 2.13''                                       */
/*    1.0.9  11/01/20   Ajout DISPLAY_LOW_UPDATE et DISPLAY_UPDATE               */
/*    1.0.10 27/09/20   Ajout DISPLAY_LIGHT                                      */
/*    1.0.11 05/10/20   Ajout gestion PCB3																			 */
/*    1.0.12 07/12/20   Ajout PCB V3.5                                           */
/*    1.0.13 10/12/20   Ajout display orientation                                */
/*                                                                               */
/*********************************************************************************/

//VERSION
//#define VARIOVERSION 154     //Version 1 avec ecran 1.54
//#define VARIOVERSION 254     //Version 2 avec ecran 1.54
//#define VARIOVERSION 290     //Version 2 avec ecran 2.90 paysage
//#define VARIOVERSION 291       //Version 2 avec ecran 2.90 portrait
//#define VARIOVERSION 292     //Version 2 avec ecran 2.90 V2 paysage     
#define VARIOVERSION 293     //Version 2 avec ecran 2.90 V2 portrait
//#define VARIOVERSION 354     //Version 3.1 avec ecran 1.54
//#define VARIOVERSION 390     //Version 3 avec ecran 2.90 paysage
//#define VARIOVERSION 391     //Version 3 avec ecran 2.90 portrait
//#define VARIOVERSION 395     //Version 3.5 avec ecran 2.90 paysage
//#define VARIOVERSION 396     //Version 3.5 avec ecran 2.90 portrait

#include <HardwareConfigPRO.h>
#include <HardwareConfigESP32.h>
#include <HardwareConfigMK0.h>

/******************************/
/*            SCREEN          */
/******************************/

#if ((VARIOVERSION == 154) || (VARIOVERSION == 254) || (VARIOVERSION == 354)) 
#define VARIOSCREEN_SIZE 	154		//Ecran 1.54''
#define DISPLAY_PORTRAIT
#elif ((VARIOVERSION == 290) || (VARIOVERSION == 390) || (VARIOVERSION == 395)) 
#define VARIOSCREEN_SIZE	290 	//Ecran 2.90'' Paysage
#define DISPLAY_LANDSCAPE
#elif ((VARIOVERSION == 291) || (VARIOVERSION == 391) || (VARIOVERSION == 396)) 
#define VARIOSCREEN_SIZE 	291 	//Ecran 2.90'' Portrait
#define DISPLAY_PORTRAIT
#elif ((VARIOVERSION == 292)) 
#define VARIOSCREEN_SIZE 	292 	//Ecran 2.90'' V2 Portrait
#define DISPLAY_LANDSCAPE
#elif ((VARIOVERSION == 293)) 
#define VARIOSCREEN_SIZE 	293 	//Ecran 2.90'' V2 Portrait
#define DISPLAY_PORTRAIT
#else
#define VARIOSCREEN_SIZE 	154		//Ecran 1.54''
#define DISPLAY_PORTRAIT
#endif

#if (VARIOSCREEN_SIZE == 154)
#define DISPLAY_LOW_UPDATE 50
#define DISPLAY_UPDATE    10
//#define DISPLAY_LIGHT
#elif ((VARIOSCREEN_SIZE == 290) || (VARIOVERSION == 390) || (VARIOVERSION == 395) || (VARIOVERSION == 292))
#define DISPLAY_LOW_UPDATE 40
#define DISPLAY_UPDATE    10
#elif ((VARIOSCREEN_SIZE == 291) || (VARIOVERSION == 391) || (VARIOVERSION == 396) || (VARIOVERSION == 293))
#define DISPLAY_LOW_UPDATE 40 //40
#define DISPLAY_UPDATE    10 //10
#endif

/***********************/
/* The voltage divisor */
/***********************/

#ifndef VOLTAGE_DIVISOR_VALUE
#define VOLTAGE_DIVISOR_VALUE 1.27  //270k et 1M
#endif

#ifndef VOLTAGE_DIVISOR_REF_VOLTAGE
#define VOLTAGE_DIVISOR_REF_VOLTAGE 3.3
#endif

/*****************************/
/*         EEPROM            */
/*****************************/
//  IGC Header			0x00		195		0xC8
//  Sound volume		0xC8    1     0xD0
//  Fly Stat        0xD0    48X10 0x2B0     
//  Calibration     0x2C0   56 		0x2FA				(762)

/*****************************/
/*  IGC HEADER EEPROM        */
/*****************************/

#ifndef IGC_SENTENCE_HEADER_EEPROM_ADDRESS
#define IGC_SENTENCE_HEADER_EEPROM_ADDRESS 0x30 
#define IGC_SENTENCE_HEADER_MAX_SIZE (0x200 - 0x30)
#endif

#define IGC_SENTENCE_EEPROM_TAG 2568

/*****************************/
/*  EEPROM SOUND             */
/*****************************/

/* eeprom sound setting adresses */
#ifndef SOUND_EEPROM_ADDR
#define SOUND_EEPROM_ADDR 0x201
#endif

#define SOUND_EEPROM_TAG 9806

/*****************************/
/*  EEPROM STAT              */
/*****************************/
#ifndef FLY_STAT_HEADER_EEPROM_ADDRESS
#define FLY_STAT_HEADER_EEPROM_ADDRESS 0x210
#endif
 
#define FLY_STAT_EEPROM_TAG 2025

/*****************************/
/*  EEPROM CALIBRATION       */
/*****************************/
#ifndef CAL_MPU_HEADER_EEPROM_ADDRESS
#define CAL_MPU_HEADER_EEPROM_ADDRESS 0x3F0
#endif
 
#define CAL_MPU_EEPROM_TAG 3030

/*****************************/
/* Baro parameters           */
/*****************************/

/* You can set the calibration coefficients if known */
#define MS5611_STATIC_ADDRESS 0x77
//#define MS5611_STATIC_CALIBRATION {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}

/* We advice using the ms5611 barometer */
/* but if you want to use the BMP280 you can enable it here */
//#define HAVE_BMP280
#define BMP280_STATIC_ADDRESS 0x76
//#define BMP280_STATIC_CALIBRATION {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}

/**********************/
/*  MPU parameters    */
/**********************/

/*****************************************************/
/*    choix de la librairie MPU/MS5611               */
/*****************************************************/

#if ((VARIOVERSION == 395) || (VARIOVERSION == 396)) 
#define BNO085
#define MPU_STATIC_ADDRESS 0x4A

/* calibration method */
// by EEPROM
//#define IMU_CALIBRATION_IN_EEPROM
// or by static value

/* Parametre par defaut */
#define VERTACCEL_GYRO_CAL_BIAS {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VERTACCEL_ACCEL_CAL_BIAS {0, 0, 0}
#define VERTACCEL_ACCEL_CAL_SCALE 0
#define VERTACCEL_MAG_CAL_BIAS {0, 0, 0}
#define VERTACCEL_MAG_CAL_PROJ_SCALE -16689
#define VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER 7
#define VERTACCEL_MAG_CAL_BIAS_MULTIPLIER 5

#define MPU_ENABLE_INT_PIN

//TEMPORAIRE POUR COMPILATION
#define MPU9250
#define MPU_STATIC_ADDRESS 0x68
#define MPU_COMP_TEMP -6

#else

#define TWOWIRESCHEDULER

/* If you embed an accelerometer set the model here. */
/* Possible values are :                             */
/*   MPU6050, MPU6500, MPU9150, MPU9250              */
#define MPU9250
#define MPU_STATIC_ADDRESS 0x68

/* Compensation temperature                          */
#define MPU_COMP_TEMP -6

/*#define VERTACCEL_GYRO_CAL_EEPROM_ADDR 0x00
#define VERTACCEL_GYRO_CAL_EEPROM_TAG 0xf4e2
#define VERTACCEL_ACCEL_CAL_EEPROM_ADDR 0x0C
#define VERTACCEL_ACCEL_CAL_EEPROM_TAG 0xee54
#define VERTACCEL_MAG_CAL_EEPROM_ADDR 0x14
#define VERTACCEL_MAG_CAL_EEPROM_TAG 0x49f2
#define VERTACCEL_MAG_PROJ_EEPROM_ADDR 0x1C
#define VERTACCEL_MAG_PROJ_EEPROM_TAG 0x67fa*/

/* calibration method */
// by EEPROM
//#define IMU_CALIBRATION_IN_EEPROM
// or by static value

/* Parametre par defaut */
#define VERTACCEL_GYRO_CAL_BIAS {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VERTACCEL_ACCEL_CAL_BIAS {0, 0, 0}
#define VERTACCEL_ACCEL_CAL_SCALE 0
#define VERTACCEL_MAG_CAL_BIAS {0, 0, 0}
#define VERTACCEL_MAG_CAL_PROJ_SCALE -16689
#define VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER 7
#define VERTACCEL_MAG_CAL_BIAS_MULTIPLIER 5

//#define VERTACCEL_STATIC_CALIBRATION

/*#define VERTACCEL_GYRO_CAL_BIAS {0x00, 0x00, 0x29, 0xdf, 0xff, 0xff, 0x9f, 0x4a, 0xff, 0xff, 0xb5, 0xe9}
#define VERTACCEL_ACCEL_CAL_BIAS {-3416, 9992, -31522}
#define VERTACCEL_ACCEL_CAL_SCALE -165
#define VERTACCEL_MAG_CAL_BIAS {-5097, 4319, 22227}
#define VERTACCEL_MAG_CAL_PROJ_SCALE 9247*/

/*#define VERTACCEL_GYRO_CAL_BIAS {0x00, 0x00, 0x53, 0x5a, 0xff, 0xff, 0x77, 0x56, 0xff, 0xff, 0xc1, 0x76}
#define VERTACCEL_ACCEL_CAL_BIAS {-356, -293, -23473}
#define VERTACCEL_ACCEL_CAL_SCALE -33169
#define VERTACCEL_MAG_CAL_BIAS {260, 26368, -24674}
#define VERTACCEL_MAG_CAL_PROJ_SCALE -32495
#define VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER 5
#define VERTACCEL_MAG_CAL_BIAS_MULTIPLIER 5*/

/* enable if you want to use the MPU INT pin */
#define MPU_ENABLE_INT_PIN
#endif

/*****************************/
/* SDCard/Bluetooth behavior */
/*****************************/

/* What type of barometric altitude is sent :           */
/* -> Based on international standard atmosphere        */
/* -> Calibrated with GPS altitude                      */
#define VARIOMETER_SDCARD_SEND_CALIBRATED_ALTITUDE
#define VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE

/* What type of vario NMEA sentence is sent by bluetooth. */
/* Possible values are :                                  */
/*  - VARIOMETER_SENT_LXNAV_SENTENCE                      */
/*  - VARIOMETER_SENT_LK8000_SENTENCE                     */
// #define VARIOMETER_SENT_LXNAV_SENTENCE

/* When there is no GPS to sync variometer bluetooth sentences */
/* set the delay between sendings in milliseconds.             */ 
#define VARIOMETER_SENTENCE_DELAY 2000

/*****************************/
/*           SDCard          */
/*****************************/

#define LF16_FILE_NAME_NUMBER_SIZE 2
#define LF16_FILE_NAME_NUMBER_LIMIT 100


/******************************/
/*             GPS            */
/******************************/

/* The GPS period in ms                             */
/* use the gps-time-analysis sketch to determine it */
#define GPS_PERIOD 997.5  //default 1000

/* Set the GPS precision needed to use the GPS altitude value  */
/* to calibrate the barometric altitude.                       */
/*      !!! the best possible precision is 100 !!!             */ 
#define VARIOMETER_GPS_ALTI_CALIBRATION_PRECISION_THRESHOLD 250

/* The GPS RMC and GGA sentences parameters */
/* Check the SD card ouput or bluetooth output of gps-time-analysis */
#define NMEA_TAG_SIZE 5

/* Nombre de mesure GPS avant la mise à jour de l'altitude baro      */
#define NB_ACQUISITION_FIX_GPS 20

// GPS MODEL
#if (PCB_VERSION == 3)
	// PCB 3
#define L86
#else
#define ATGM336H
//#define NEO_6M
//#define L86
#endif

#if defined(ATGM336H)
#undef NEO_6M
#undef L86
#endif

#if defined(NEO_6M)
#undef ATGM336H
#undef L86
#endif

#if defined(L86)
#undef ATGM336H
#undef NEO_6M
#endif

#if defined(ATGM336H)
//ATGM336H

#define NMEA_RMC_TAG "GNRMC"
#define NMEA_GGA_TAG "GNGGA"

/* precision = 10^(number of digit after dot in gps ouput) */
/* check the gps-time-analysis output to check the precision */
/* RMC speed is at 8th position */
/* GGA time is at 2th position */
/* GGA Alti is at 10th position */
/* be carefull, time precision is an int */
#define NMEA_RMC_SPEED_PRECISION 100.0
#define NMEA_GGA_TIME_PRECISION 1000
#define NMEA_GGA_ALTI_PRECISION 10.0

#elif defined(NEO_6M)

//NEO-6M
#define NMEA_RMC_TAG "GPRMC"
#define NMEA_GGA_TAG "GPGGA"

#define NMEA_RMC_SPEED_PRECISION 1000.0
#define NMEA_GGA_TIME_PRECISION 100
#define NMEA_GGA_ALTI_PRECISION 10.0

#elif defined(L86)
//L86

#define NMEA_RMC_TAG "GNRMC"
#define NMEA_GGA_TAG "GPGGA"

/* precision = 10^(number of digit after dot in gps ouput) */
/* check the gps-time-analysis output to check the precision */
/* RMC speed is at 8th position */
/* GGA time is at 2th position */
/* GGA Alti is at 10th position */
/* be carefull, time precision is an int */
#define NMEA_RMC_SPEED_PRECISION 100.0
#define NMEA_GGA_TIME_PRECISION 1000
#define NMEA_GGA_ALTI_PRECISION 10.0
#endif 

/******************************/
/*            WIFI            */
/******************************/

#define servername "GNUVARIO-E"  // Set your server's logical name here e.g. if 'myserver' then address is http://myserver.local/
//#define IP_FIXE
#define LOCAL_IP 192,168,1,150 // Set your server's fixed IP address here
#define GATEWAY  192,168,1,1   // Set your network Gateway usually your Router base address
#define SUBNET   255,255,255,0 // Set your network sub-network mask here
#define DNS      192,168,1,1   // Set your network DNS usually your Router base address

/*************************/
/*         AUDIO         */
/*************************/

#ifndef  AUDIO_AMPLI_LOWPOWER
// #define AUDIO_AMPLI_LOWPOWER
#endif //AUDIO_AMPLI_LOWPOWER

/******************************/
/*        SERVEUR WEB         */
/******************************/
#define WEBSERVER_SDCARD

/*----------------------------*/
/*          HARDWARE          */
/*      Vario parameters      */
/*                            */
/*----------------------------*/

/* Comment or uncomment according to  */
/* what you embed in the variometer   */ 
#define HAVE_SPEAKER
#define HAVE_ACCELEROMETER
#define HAVE_SCREEN
#define HAVE_GPS
#define HAVE_SDCARD
#define HAVE_VOLTAGE_DIVISOR
//#define HAVE_AUDIO_AMPLI	
#define HAVE_POWER_ALIM	
#define HAVE_BUTTON
#define HAVE_WIFI
#define HAVE_BLUETOOTH
#define HAVE_BLE

#if defined(HAVE_WIFI) && defined(HAVE_BLUETOOTH) && !defined(HAVE_BLE)
#undef HAVE_BLUETOOTH
#endif

#if defined(HAVE_BLE) && !defined(HAVE_BLUETOOTH)
#define HAVE_BLUETOOTH
#endif

#if defined(HAVE_WIFI) && defined(HAVE_BLUETOOTH) && defined(HAVE_BLE)
// 							OUTPUT SERIALNMEA
//#define SERIAL_NMEA_SERIAL_OUTPUT
#define SERIAL_NMEA_BLUETOOTH_OUTPUT
#endif

#endif
