#ifndef _HARDWARECONFIG_H_
#define _HARDWARECONFIG_H_

#include <HardwareConfigPRO.h>
#include <HardwareConfigESP32.h>
#include <HardwareConfigMK0.h>

/***********************/
/* The voltage divisor */
/***********************/

#define VOLTAGE_DIVISOR_VALUE 1.27
#define VOLTAGE_DIVISOR_REF_VOLTAGE 3.3

/*****************************/
/*  EEPROM SOUND             */
/*****************************/

/* eeprom sound setting adresses */
#define SOUND_EEPROM_TAG 9806
#define SOUND_EEPROM_ADDR 0x30

/* ms5611 parameters */
/* You can set the calibration coefficients if known */
#define MS5611_STATIC_ADDRESS 0x77
//#define MS5611_STATIC_CALIBRATION {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}

/* We advice using the ms5611 barometer */
/* but if you want to use the BMP280 you can enable it here */
//#define HAVE_BMP280
#define BMP280_STATIC_ADDRESS 0x76
//#define BMP280_STATIC_CALIBRATION {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}


/* If you embed an accelerometer set the model here. */
/* Possible values are :                             */
/*   MPU6050, MPU6500, MPU9150, MPU9250              */
#define MPU9250
#define MPU_STATIC_ADDRESS 0x68

#define VERTACCEL_STATIC_CALIBRATION

#define VERTACCEL_GYRO_CAL_BIAS {0x00, 0x00, 0x29, 0xdf, 0xff, 0xff, 0x9f, 0x4a, 0xff, 0xff, 0xb5, 0xe9}
#define VERTACCEL_ACCEL_CAL_BIAS {-3416, 9992, -31522}
#define VERTACCEL_ACCEL_CAL_SCALE -165
#define VERTACCEL_MAG_CAL_BIAS {-5097, 4319, 22227}
#define VERTACCEL_MAG_CAL_PROJ_SCALE 9247
#define VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER 7
#define VERTACCEL_MAG_CAL_BIAS_MULTIPLIER 5


/*----------------------------*/
/*          HARDWARE          */
/*      Vario parameters      */
/*                            */
/*----------------------------*/

/* Comment or uncomment according to  */
/* what you embed in the variometer   */ 
#define HAVE_SPEAKER
#define HAVE_ACCELEROMETER
//#define HAVE_SCREEN
//#define HAVE_GPS
#define HAVE_SDCARD
//#define HAVE_BLUETOOTH
//#define HAVE_VOLTAGE_DIVISOR

#endif
