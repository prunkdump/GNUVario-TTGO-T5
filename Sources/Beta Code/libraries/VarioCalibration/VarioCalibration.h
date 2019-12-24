/* VarioCalibration -- 
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
/*                           VarioCalibration                                    */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0      29/09/19                                                          */
/*    1.0.1    01/10/19    Ajout detection bouton                                */
/*    1.0.2    28/10/19    Ajout enable ampli avant chaque appelle à tone        */
/*    1.0.3    29/11/19    Modif sdfat                                           */
/*                                                                               */
/*********************************************************************************/


#ifndef VARIOCALIBRATION_H
#define VARIOCALIBRATION_H

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#define TWOWIRESCHEDULER

#ifdef TWOWIRESCHEDULER
#include <IntTW.h>
#include <ms5611TW.h>
#include <vertaccel.h>
#include <LightInvensense.h>
#include <TwoWireScheduler.h>
#else
#include <MS5611-Ext.h>
#include <Wire.h>
#include <vertaccel2.h>
//#include <SparkFunMPU9250-DMP.h>
#include <MPU9250-DMP_SF_EXT.h>
#endif

#include <digit.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif	//HAVE_SDCARD

#include <VarioSettings.h>

/* where output ? */
#define SERIAL_OUTPUT
#define SDCARD_OUTPUT

#define RECORD_STATE_INITIAL 0
#define RECORD_STATE_WAIT_DONE 1
#define RECORD_STATE_ACCEL_SD_RECORDED 2
#define RECORD_STATE_GYRO_CALIBRATED 3

/*
extern MS5611 ms5611;
extern MPU9250_DMP imu;
extern Vertaccel vertaccel;
*/


class VarioCalibration {
	
public:
   
  void Begin(void);
  
private:

/********************************/
/* Standard deviation recording */
/********************************/
	unsigned long accelSDRecordTimestamp;
	double rawAccelSD;

	int recordInitState = RECORD_STATE_INITIAL;

	double referencePressure;

/*****************/
/* measures data */
/*****************/
	unsigned long measureTimestamp;

/* accel measures */
	int16_t lastAccelMeasure[3];
	long accelCount;
	double accelMean[3];
	double accelSD[3];

#ifdef AK89xx_SECONDARY
/* mag measures */
	int16_t lastMagMeasure[3];
	long magCount;
	double magMean[3];
	double magSD[3];
#endif //AK89xx_SECONDARY

#if defined(SDCARD_OUTPUT) && defined(HAVE_SDCARD)
#ifdef SDFAT_LIB
	SdFile file;
#else //SDFAT_LIB
	File file;
#endif //SDFAT_LIB

	boolean sdcardFound = false;
	char filename[15] = "RECORD00.CAL";
#define FILENAME_SIZE 8
	Digit valueDigit;

	void writeNumber(int16_t number);
#endif //SDCARD_OUTPUT
	
	void startMeasure(void);
	uint8_t readRawAccel(int16_t* accel, int32_t* quat);

#ifdef AK89xx_SECONDARY
/*******************************/
	uint8_t readRawMag(int16_t* mag);
#endif //AK89xx_SECONDARY

	void makeMeasureStep(void);
	double getAccelMeasure(int16_t* accelMeasure);

#ifdef AK89xx_SECONDARY
/* return standard deviation */
/*******************************/
	double getMagMeasure(int16_t* magMeasure);
#endif //AK89xx_SECONDARY

};

extern VarioCalibration Calibration;
#endif
