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
/*  version    Date        Description                                           */
/*    1.0      29/09/19                                                          */
/*    1.0.1    01/10/19    Ajout detection bouton                                */
/*                                                                               */
/*********************************************************************************/
#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#include <VarioCalibration.h>

#include <SPI.h>
#include "eepromHAL.h"

#include <VarioSettings.h>

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

#include <toneHAL.h>
#include <digit.h>

#ifdef HAVE_SDCARD
#include <SdCardHAL.h>
#endif //HAVE_SDCARD

#include <VarioButton.h>

/* need beeps */
#define MAKE_BEEP
#define BEEP_DURATION 300
#define BEEP_VOLUME 3
#define BEEP_START_FREQ 500
#define BEEP_RECORD_FREQ 1000

/* to compute the standard deviation of the accelerometer */
#define ACCEL_SD_WAIT_DURATION 2000
#define ACCEL_SD_MEASURE_DURATION 8000

/* to make averaged measure */
#define MEASURE_DURATION 500
#define STABILIZE_DURATION 500

/* movement detection */
#define PREDICTION_INTERVAL_COEFFICIENT 1.96

/* orientation change detection */
#define NEW_MEASURE_MINIMAL_DEVIATION_COEFF 20.0

VarioCalibration Calibration;

#if defined(SDCARD_OUTPUT) && defined(HAVE_SDCARD)
//****************************************************************************************************************************
void VarioCalibration::writeNumber(int16_t number) {
//****************************************************************************************************************************
  valueDigit.begin((long)number);
  while( valueDigit.available() ) {
    file.write( valueDigit.get() );
  }
}
#endif //SDCARD_OUTPUT

/**************************/
void VarioCalibration::startMeasure(void) {
/**************************/

  /* stabilize */
  unsigned long stabilizeTimestamp = millis();
  while( millis() - stabilizeTimestamp < STABILIZE_DURATION ) {
    makeMeasureStep();
		
/*******************************/
/*  Compute button             */
/*******************************/

	#ifdef BUTTON_DEBUG
		SerialPort.println("Lecture boutons");
	#endif //PROG_DEBUG
		ButtonScheduleur.update();
		
    delay(100);
  }

#ifdef PROG_DEBUG
  SerialPort.println("init Var");
#endif //PROG_DEBUG

  /* init vars */
  measureTimestamp = millis();
  accelCount = 0;
#ifdef AK89xx_SECONDARY
  magCount = 0;
#endif

  for( int i = 0; i<3; i++) {
    accelMean[i] = 0.0;
    accelSD[i] = 0.0;
#ifdef AK89xx_SECONDARY
    magMean[i] = 0.0;
    magSD[i] = 0.0;
#endif
  }
}

/******************************************************/
uint8_t VarioCalibration::readRawAccel(int16_t* accel, int32_t* quat) {
/******************************************************/

  uint8_t haveValue = 0;

#ifdef PROG_DEBUG
  SerialPort.println("readRawAccel");
#endif //PROG_DEBUG

  delay(2000);
#ifdef TWOWIRESCHEDULER  
  while( fastMPUReadFIFO(NULL, accel, quat) >= 0 ) {
#else
  while( imu.fastMPUReadFIFO(NULL, accel, quat) != INV_SUCCESS) {
    
#endif
    haveValue = 1;
    delay(100);
  }

#ifdef PROG_DEBUG
  SerialPort.println("readRawAccel - exit");
#endif //PROG_DEBUG

  return haveValue;
}

#ifdef AK89xx_SECONDARY
/*******************************/
uint8_t VarioCalibration::readRawMag(int16_t* mag) {
/*******************************/

  uint8_t haveValue = 0;

#ifdef PROG_DEBUG
  SerialPort.println("readRawMag");
#endif //PROG_DEBUG

  if( imu.fastMPUMagReady() ) {
#ifdef VERTACCEL_USE_MAG_SENS_ADJ

#ifdef PROG_DEBUG
     SerialPort.println("fastMPUReadMag");
#endif //PROG_DEBUG

    imu.fastMPUReadMag(mag);
#else
#ifdef PROG_DEBUG
     SerialPort.println("fastMPUReadRawMag");
#endif //PROG_DEBUG

    fastMPUReadRawMag(mag);
#endif //VERTACCEL_USE_MAG_SENS_ADJ
    haveValue = 1;
  }

#ifdef PROG_DEBUG
  SerialPort.println("readRawMag - exit");
#endif //PROG_DEBUG

  return haveValue;
}
#endif //AK89xx_SECONDARY

/*******************************/
void VarioCalibration::makeMeasureStep(void) {
/*******************************/  
  /* accel */
  int16_t accel[3];
  int32_t quat[4];
  if( readRawAccel(accel, quat) ) {

#ifdef PROG_DEBUG
    SerialPort.println("makeMeasureStep : readRawAccel OK");
#endif //PROG_DEBUG

    accelCount++;
    for( int i = 0; i<3; i++) {
      
      accelMean[i] += (double)accel[i];
      accelSD[i]   += ((double)accel[i])*((double)accel[i]);

#ifdef PROG_DEBUG
      SerialPort.print("accelMean : ");
      SerialPort.println( accelMean[i]);
      SerialPort.print("accelSD : ");
      SerialPort.println( accelSD[i]);
#endif //PROG_DEBUG

    }
  }

#ifdef AK89xx_SECONDARY
  /* mag */
  int16_t mag[3];
  if( readRawMag(mag) ) {
#ifdef PROG_DEBUG
    SerialPort.println("makeMeasureStep : readRawMag OK");
#endif //PROG_DEBUG

   magCount++;
    for( int i = 0; i<3; i++) {
      magMean[i] += (double)mag[i];
      magSD[i] += ((double)mag[i])*((double)mag[i]);

#ifdef PROG_DEBUG
      SerialPort.print("magMean : ");
      SerialPort.println( magMean[i]);
      SerialPort.print("magSD : ");
      SerialPort.println( magSD[i]);
#endif //PROG_DEBUG
      
    }
  }
#endif //AK89xx_SECONDARY
}


/* return standard deviation */
double VarioCalibration::getAccelMeasure(int16_t* accelMeasure) {
/*****************************/  
  double accelMeasureSD = 0.0; 

#ifdef PROG_DEBUG
    SerialPort.println("GetAccelMeasure");
#endif //PROG_DEBUG

  for( int i = 0; i<3; i++) {
    accelMeasureSD += accelSD[i]/(double)accelCount;
    accelMeasureSD -= (accelMean[i]/(double)accelCount) * (accelMean[i]/(double)accelCount);
    
    accelMeasure[i] = (int16_t)(accelMean[i]/(double)accelCount);

#ifdef PROG_DEBUG
    SerialPort.print("accelMeasure : ");
    SerialPort.println(accelMeasure[i]);
#endif //PROG_DEBUG
    
  }

  return sqrt(accelMeasureSD);
}

#ifdef AK89xx_SECONDARY
/* return standard deviation */
/*******************************/
double VarioCalibration::getMagMeasure(int16_t* magMeasure) {
/*******************************/

#ifdef PROG_DEBUG
    SerialPort.println("GetMagMeasure");
#endif //PROG_DEBUG
  
  double magMeasureSD = 0.0; 
    
  for( int i = 0; i<3; i++) {
    magMeasureSD += magSD[i]/(double)magCount;
    magMeasureSD -= (magMean[i]/(double)magCount) * (magMean[i]/(double)magCount);
    
    magMeasure[i] = (int16_t)(magMean[i]/(double)magCount);
 #ifdef PROG_DEBUG
    SerialPort.print("magMeasure : ");
    SerialPort.println(magMeasure[i]);
#endif //PROG_DEBUG
 }

  return sqrt(magMeasureSD);
}
#endif //AK89xx_SECONDARY


//****************************************************************************************************************************
void VarioCalibration::Begin(void)
//****************************************************************************************************************************
{
#if defined(SDCARD_OUTPUT) && defined(HAVE_SDCARD)
	sdcardFound = true;
#endif

  /* reset variables */
#ifdef PROG_DEBUG
  SerialPort.println("Start Mesure");
#endif //PROG_DEBUG
  
  startMeasure();
  accelSDRecordTimestamp = millis();  

#ifdef PROG_DEBUG
  SerialPort.println("Loop");
#endif //PROG_DEBUG

  while (1) {
/*******************************/
/*  Compute button             */
/*******************************/

	#ifdef BUTTON_DEBUG
			 SerialPort.println("Lecture boutons");
	#endif //PROG_DEBUG
		ButtonScheduleur.update();

		delay(1);
		/*------------------------------------------------------------*/
		/*  wait, record standard deviation, and get gyro calibration */
		/*------------------------------------------------------------*/

		/* first  wait */
		if( recordInitState == RECORD_STATE_INITIAL ) {

	#ifdef PROG_DEBUG
			 SerialPort.println("First Wait");
	#endif //PROG_DEBUG

			/* wait finished ? */
			if( millis() - accelSDRecordTimestamp > ACCEL_SD_WAIT_DURATION ) {

				/* start measure */
	#ifdef MAKE_BEEP
				toneHAL.tone(BEEP_START_FREQ, BEEP_VOLUME);
				delay(BEEP_DURATION);
				toneHAL.tone(0);
	#endif //MAKE_BEEP     
	
	/*******************************/
/*  Compute button             */
/*******************************/

	#ifdef BUTTON_DEBUG
				SerialPort.println("Lecture boutons");
	#endif //PROG_DEBUG
				ButtonScheduleur.update();

				startMeasure();

				/* next step */
				recordInitState = RECORD_STATE_WAIT_DONE;
			}
		}

		/* next record accel SD */
		else if( recordInitState == RECORD_STATE_WAIT_DONE ) {

			/* accel SD recording finished ? */
			if( millis() - measureTimestamp > ACCEL_SD_MEASURE_DURATION ) {

				/* get result */
				int16_t measure[3];
				rawAccelSD = getAccelMeasure(measure);

				/* next step */
				recordInitState = RECORD_STATE_ACCEL_SD_RECORDED;
			}

			/* else measure */
			else {
				delay(200);
				makeMeasureStep();
			}
		}

		/* next get gyro calibration */
		else if( recordInitState == RECORD_STATE_ACCEL_SD_RECORDED ) {

			/* check if gyro calibrated */
			unsigned char gyroCal[12];
			fastMPUReadGyroBias(gyroCal);

			bool gyroCalibrated = false;
			for(int i = 0; i<12; i++) {
				if(gyroCal[i] != 0 ) {
					gyroCalibrated = true;
					break;
				}
			}

			if( ! gyroCalibrated ) {
				delay(MEASURE_DURATION);
			}

			/* output gyro calibration start measures */
			else {
				
#if defined(SDCARD_OUTPUT) && defined(HAVE_SDCARD)
				if( sdcardFound ) {
					file = SDHAL.open(filename, FILE_WRITE);  //, FILENAME_SIZE);
				}

				writeNumber(gyroCal[0]);
				for(int i = 1; i<12; i++) {
					file.write(',');
					file.write(' ');
					writeNumber(gyroCal[i]);
				}
				file.write('\n');
	#endif //SDCARD_OUTPUT

	#ifdef SERIAL_OUTPUT
				SerialPort.print(gyroCal[0], DEC);
				for(int i = 1; i<12; i++) {
					SerialPort.print(", ");
					SerialPort.print(gyroCal[i], DEC);
				}
				SerialPort.print("\n");
	#endif //SERIAL_OUTPUT

				/* start recording */
				recordInitState = RECORD_STATE_GYRO_CALIBRATED;
				startMeasure();
			}
		}

		/*-----------------*/
		/* record measures */
		/*-----------------*/
		else {

			/****************/
			/* make measure */
			/****************/
			if( millis() - measureTimestamp < MEASURE_DURATION ) {
				makeMeasureStep();
			}

			/******************/
			/* output measure */
			/******************/
			else {

				int16_t accelMeasure[3];
				double accelMeasureSD = getAccelMeasure(accelMeasure);

	#ifdef AK89xx_SECONDARY
				int16_t magMeasure[3];
				double magMeasureSD = getMagMeasure(magMeasure);
	#endif //AK89xx_SECONDARY
			
				/**************************/
				/* check measure validity */
				/**************************/

				/* check measure stability */
				if( accelMeasureSD < PREDICTION_INTERVAL_COEFFICIENT * rawAccelSD ) {

					/* check deviation with last measure */
					double measureDeviation = 0.0;
					for( int i = 0; i<3; i++) {
						double s = (double)accelMeasure[i] - (double)lastAccelMeasure[i];
						measureDeviation += s * s;
					}
					measureDeviation = sqrt(measureDeviation);

					if( measureDeviation > NEW_MEASURE_MINIMAL_DEVIATION_COEFF*PREDICTION_INTERVAL_COEFFICIENT*rawAccelSD/sqrt((double)accelCount) ) {

						/* save measure */
						for( int i = 0; i<3; i++) {
							lastAccelMeasure[i] = accelMeasure[i];
						}


	#ifdef SERIAL_OUTPUT
						/*****************/
						/* serial output */
						/*****************/
						SerialPort.print(accelMeasure[0], DEC);
						SerialPort.print(", ");
						SerialPort.print(accelMeasure[1], DEC);
						SerialPort.print(", ");
						SerialPort.print(accelMeasure[2], DEC);
	#ifdef AK89xx_SECONDARY
						SerialPort.print(", ");
						SerialPort.print(magMeasure[0], DEC);
						SerialPort.print(", ");
						SerialPort.print(magMeasure[1], DEC);
						SerialPort.print(", ");
						SerialPort.print(magMeasure[2], DEC);
	#endif //AK89xx_SECONDARY
						SerialPort.print("\n");
	#endif //SERIAL_OUTPUT

#if defined(SDCARD_OUTPUT) && defined(HAVE_SDCARD)
						/*****************/
						/* SDcard output */
						/*****************/
						writeNumber(accelMeasure[0]);
						file.write(',');
						file.write(' ');
						writeNumber(accelMeasure[1]);
						file.write(',');
						file.write(' ');
						writeNumber(accelMeasure[2]);
	#ifdef AK89xx_SECONDARY
						file.write(',');
						file.write(' ');
						writeNumber(magMeasure[0]);
						file.write(',');
						file.write(' ');
						writeNumber(magMeasure[1]);
						file.write(',');
						file.write(' ');
						writeNumber(magMeasure[2]);
	#endif //AK89xx_SECONDARY
						file.write('\n');
						
						file.flush();
	#endif //SDCARD_OUTPUT

	#ifdef MAKE_BEEP
						toneHAL.tone(BEEP_RECORD_FREQ, BEEP_VOLUME);
						delay(BEEP_DURATION);
						toneHAL.tone(0);
	#endif //MAKE_BEEP
					}
				}
				
/*******************************/
/*  Compute button             */
/*******************************/

#ifdef BUTTON_DEBUG
				SerialPort.println("Lecture boutons");
#endif //PROG_DEBUG
				ButtonScheduleur.update();
				
				/* next */
				delay(100);
				startMeasure();
			}
		}
	}
}