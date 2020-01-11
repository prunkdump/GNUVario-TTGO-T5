/* vertaccel -- Compute vertical acceleration from IMU 
 *
 * Copyright 2016-2019 Baptiste PELLEGRIN
 * 
 * This file is part of GNUVario.
 *
 * GNUVario is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNUVario is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
 
/*********************************************************************************/
/*                                                                               */
/*                           Vertaccel2                                          */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0                                                                        */
/*    1.0.1  01/10/19   Ajout configuration via varioSettings 									 */
/*                                                                               */
/*********************************************************************************/

#include <vertaccel2.h>

#include <Arduino.h>
//#include <LightInvensense.h>

#include <DebugConfig.h>
#include <HardwareConfig.h>

extern "C" {
#include "util/inv_mpu.h"
}

#ifdef VERTACCEL_ENABLE_EEPROM
#include <eepromHAL.h>
#endif

/* static variables /
#ifdef VERTACCEL_STATIC_CALIBRATION
constexpr uint8_t Vertaccel::gyroCalArray[12];
constexpr int32_t Vertaccel::accelCalArray[3];
#endif*/


#ifdef VERTACCEL_ENABLE_EEPROM
/******************/
/* EEPROM methods */
/******************/
void readEEPROMValues(int address, uint16_t eepromTag, int length, uint8_t* data) {

  /* read tag */
  uint16_t tag = EEPROMHAL.read(address);
  address++;
  tag = (tag<<8) + EEPROMHAL.read(address);
  address++;

  /* read values */
  for( uint8_t i = 0; i<length; i++) {
    if( tag == eepromTag ) {
      *data = EEPROMHAL.read(address);
    } else {
      *data = 0;
    }
    data++;
    address++;
  }
}


void writeEEPROMValues(int address, uint16_t eepromTag, int length, const uint8_t* data) {

  /* write tag */
  EEPROMHAL.write(address, (eepromTag>>8) & 0xff);
  EEPROMHAL.write(address + 0x01, eepromTag & 0xff);
  address += 2;
  
  /* write values */
  for( uint8_t i = 0; i<length; i++) {
    EEPROMHAL.write(address, *data);
    data++;
    address++;
  }
}


VertaccelSettings Vertaccel::readSettings(void) {

  VertaccelSettings eepromSettings;
  readGyroCalibration(eepromSettings.gyroCal);
  readAccelCalibration(eepromSettings.accelCal);
#ifdef AK89xx_SECONDARY 
  readMagCalibration(eepromSettings.magCal);
#endif //AK89xx_SECONDARY

  return eepromSettings;
}


void Vertaccel::saveGyroCalibration(const uint8_t* gyroCal) {

  writeEEPROMValues(VERTACCEL_GYRO_CAL_EEPROM_ADDR, VERTACCEL_GYRO_CAL_EEPROM_TAG, 12, gyroCal);
}

void Vertaccel::readGyroCalibration(uint8_t* gyroCal) {

  readEEPROMValues(VERTACCEL_GYRO_CAL_EEPROM_ADDR, VERTACCEL_GYRO_CAL_EEPROM_TAG, 12, gyroCal);
}

void Vertaccel::saveAccelCalibration(const VertaccelCalibration& accelCal) {

  writeEEPROMValues(VERTACCEL_ACCEL_CAL_EEPROM_ADDR, VERTACCEL_ACCEL_CAL_EEPROM_TAG, sizeof(VertaccelCalibration), (uint8_t*)(&accelCal));
}

void Vertaccel::readAccelCalibration(VertaccelCalibration& accelCal) {

  readEEPROMValues(VERTACCEL_ACCEL_CAL_EEPROM_ADDR, VERTACCEL_ACCEL_CAL_EEPROM_TAG, sizeof(VertaccelCalibration), (uint8_t*)(&accelCal));
}

#ifdef AK89xx_SECONDARY
void Vertaccel::saveMagCalibration(const VertaccelCalibration& magCal) {

  writeEEPROMValues(VERTACCEL_MAG_CAL_EEPROM_ADDR, VERTACCEL_MAG_CAL_EEPROM_TAG, sizeof(VertaccelCalibration), (uint8_t*)(&magCal));
}

void Vertaccel::readMagCalibration(VertaccelCalibration& magCal) {

  readEEPROMValues(VERTACCEL_MAG_CAL_EEPROM_ADDR, VERTACCEL_MAG_CAL_EEPROM_TAG, sizeof(VertaccelCalibration), (uint8_t*)(&magCal));
}
#endif //AK89xx_SECONDARY
#else if defined(VERTACCEL_ENABLE_SD_SETTINGS)
VertaccelSettings Vertaccel::readSettings(void) {

  VertaccelSettings SDSettings;
  readGyroCalibration(SDSettings.gyroCal);
  readAccelCalibration(SDSettings.accelCal);
#ifdef AK89xx_SECONDARY 
  readMagCalibration(SDSettings.magCal);
#endif //AK89xx_SECONDARY

  return SDSettings;
}

void Vertaccel::readGyroCalibration(uint8_t* gyroCal) {

	gyroCal[0] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_00;
	gyroCal[1] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_01;
	gyroCal[2] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_02;
	gyroCal[3] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_03;
	gyroCal[4] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_04;
	gyroCal[5] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_05;
	gyroCal[6] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_06;
	gyroCal[7] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_07;
	gyroCal[8] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_08;
	gyroCal[9] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_09;
	gyroCal[10] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_10;
	gyroCal[11] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_11;
}

void Vertaccel::readAccelCalibration(VertaccelCalibration& accelCal) {

//  readEEPROMValues(VERTACCEL_ACCEL_CAL_EEPROM_ADDR, VERTACCEL_ACCEL_CAL_EEPROM_TAG, sizeof(VertaccelCalibration), (uint8_t*)(&accelCal));

	accelCal.bias[0] = GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_00;
	accelCal.bias[1] = GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_01;
	accelCal.bias[2] = GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_02;
	accelCal.scale 	 = GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_SCALE;
}

#ifdef AK89xx_SECONDARY

void Vertaccel::readMagCalibration(VertaccelCalibration& magCal) {

//  readEEPROMValues(VERTACCEL_MAG_CAL_EEPROM_ADDR, VERTACCEL_MAG_CAL_EEPROM_TAG, sizeof(VertaccelCalibration), (uint8_t*)(&magCal));
	magCal.bias[0] = GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_00;
	magCal.bias[1] = GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_01;
	magCal.bias[2] = GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_02;
	magCal.scale 	 = GnuSettings.VARIO_VERTACCEL_MAG_CAL_PROJ_SCALE;
}
#endif //AK89xx_SECONDARY
#endif //VERTACCEL_ENABLE_EEPROM


/***************/
/* init device */
/***************/
void Vertaccel::init(void) {


#if defined(VERTACCEL_ENABLE_SD_SETTINGS)
	settings.gyroCal[0] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_00;
	settings.gyroCal[1] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_01;
	settings.gyroCal[2] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_02;
	settings.gyroCal[3] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_03;
	settings.gyroCal[4] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_04;
	settings.gyroCal[5] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_05;
	settings.gyroCal[6] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_06;
	settings.gyroCal[7] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_07;
	settings.gyroCal[8] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_08;
	settings.gyroCal[9] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_09;
	settings.gyroCal[10] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_10;
	settings.gyroCal[11] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_11;

	settings.accelCal.bias[0] = GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_00;
	settings.accelCal.bias[1] = GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_01;
	settings.accelCal.bias[2] = GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_02;
	settings.accelCal.scale 	= GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_SCALE;

#ifdef AK89xx_SECONDARY
	settings.magCal.bias[0] = GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_00;
	settings.magCal.bias[1] = GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_01;
	settings.magCal.bias[2] = GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_02;
	settings.magCal.scale 	 = GnuSettings.VARIO_VERTACCEL_MAG_CAL_PROJ_SCALE;
#endif

	gyroCalArray[0] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_00;
	gyroCalArray[1] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_01;
	gyroCalArray[2] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_02;
	gyroCalArray[3] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_03;
	gyroCalArray[4] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_04;
	gyroCalArray[5] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_05;
	gyroCalArray[6] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_06;
	gyroCalArray[7] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_07;
	gyroCalArray[8] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_08;
	gyroCalArray[9] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_09;
	gyroCalArray[10] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_10;
	gyroCalArray[11] = GnuSettings.VARIO_VERTACCEL_GYRO_CAL_BIAS_11;
  accelCalArray[0] = (int32_t)vertaccelSettings.accelCal.bias[0] * ((int32_t)1 << (15 - GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER));
	accelCalArray[1] = (int32_t)vertaccelSettings.accelCal.bias[1] * ((int32_t)1 << (15 - GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER));
	accelCalArray[1] = (int32_t)vertaccelSettings.accelCal.bias[2] * ((int32_t)1 << (15 - GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER)); //passed as pointer
#endif //VERTACCEL_ENABLE_SD_SETTINGS

  /* init MPU */
//  fastMPUInit(false);

#ifndef VERTACCEL_STATIC_CALIBRATION
  /* read calibration coeff from EEPROM if needed */
  settings = readSettings();

  /* set gyro calibration in the DMP */
//  fastMPUSetGyroBias8(settings.gyroCal);
#else
  /* set gyro calibration in the DMP */
//  fastMPUSetGyroBias8(gyroCalArray);
#endif

  

#ifndef VERTACCEL_STATIC_CALIBRATION
  /* set accel calibration in the DMP */
  int32_t accelBias[3];

#if defined(VERTACCEL_ENABLE_SD_SETTINGS)
  for(int i = 0; i<3; i++) 
    accelBias[i] = (int32_t)settings.accelCal.bias[i] << (15 - GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER);
#else
  for(int i = 0; i<3; i++) 
    accelBias[i] = (int32_t)settings.accelCal.bias[i] << (15 - VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER);
#endif //VERTACCEL_ENABLE_SD_SETTINGS

//  fastMPUSetAccelBiasQ15(accelBias);
#else
  /* set accel calibration in the DMP */
//  fastMPUSetAccelBiasQ15(accelCalArray);
#endif
    
  /* start DMP */
//  fastMPUStart();

#ifdef AK89xx_SECONDARY
  readMagSensAdj(); //also reset FIFO
#endif

}


#ifdef AK89xx_SECONDARY
static uint8_t magSensAdj[3];

void Vertaccel::readMagSensAdj(void) {

  /* get access *
  disableDMP();

  /************
  /* read adj *
  /************
  uint8_t tmp[3];

  /* bypass *
  tmp[0] = BIT_BYPASS_EN;
  intTW.writeBytes(INV_HW_ADDR, INV_REG_INT_PIN_CFG, 1, tmp); 

  /* get fuse access *
  tmp[0] = AKM_POWER_DOWN;
  intTW.writeBytes(LIGHT_INVENSENSE_COMPASS_ADDR, AKM_REG_CNTL, 1, tmp);
  delay(1);

  tmp[0] = AKM_FUSE_ROM_ACCESS;
  intTW.writeBytes(LIGHT_INVENSENSE_COMPASS_ADDR, AKM_REG_CNTL, 1, tmp);
  delay(1);
  
  /* read values *
  intTW.readBytes(LIGHT_INVENSENSE_COMPASS_ADDR, AKM_REG_ASAX, 3, magSensAdj);
  
  /* stop bypass *
  tmp[0] = 0;
  intTW.writeBytes(INV_HW_ADDR, INV_REG_INT_PIN_CFG, 1, tmp);   

  /* enable DMP *
  enableDMP();*/
}
#endif

/* compute vertical vector and vertical accel from IMU data */
void Vertaccel::compute(int16_t *imuAccel, int32_t *imuQuat, double* vertVector, double& vertAccel) {

  /*---------------------------------------*/
  /*   vertical acceleration computation   */
  /*---------------------------------------*/

  /***************************/
  /* normalize and calibrate */
  /***************************/
  double accel[3]; 
  double quat[4];
  
#ifndef VERTACCEL_STATIC_CALIBRATION
  for(int i = 0; i<3; i++) {
#if defined(VERTACCEL_ENABLE_SD_SETTINGS)
    int64_t calibratedAccel = (int64_t)imuAccel[i] << GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER;
    calibratedAccel -= (int64_t)settings.accelCal.bias[i];
    calibratedAccel *= ((int64_t)settings.accelCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
    accel[i] = ((double)calibratedAccel)/((double)((int64_t)1 << (GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_ACCEL_SCALE_SHIFT)));
#else
    int64_t calibratedAccel = (int64_t)imuAccel[i] << VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER;
    calibratedAccel -= (int64_t)settings.accelCal.bias[i];
    calibratedAccel *= ((int64_t)settings.accelCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
    accel[i] = ((double)calibratedAccel)/((double)((int64_t)1 << (VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_ACCEL_SCALE_SHIFT)));
#endif //VERTACCEL_ENABLE_SD_SETTINGS
  }
#else
  /* inline for optimization */
  int64_t calibratedAccel;
#if defined(VERTACCEL_ENABLE_SD_SETTINGS)
  calibratedAccel = (int64_t)imuAccel[0] << GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER;
  calibratedAccel -= (int64_t)settings.accelCal.bias[0];
  calibratedAccel *= ((int64_t)settings.accelCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
  accel[0] = ((double)calibratedAccel)/((double)((int64_t)1 << (GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_ACCEL_SCALE_SHIFT)));

  calibratedAccel = (int64_t)imuAccel[1] << GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER;
  calibratedAccel -= (int64_t)settings.accelCal.bias[1];
  calibratedAccel *= ((int64_t)settings.accelCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
  accel[1] = ((double)calibratedAccel)/((double)((int64_t)1 << (GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_ACCEL_SCALE_SHIFT)));

  calibratedAccel = (int64_t)imuAccel[2] << GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER;
  calibratedAccel -= (int64_t)settings.accelCal.bias[2];
  calibratedAccel *= ((int64_t)settings.accelCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
  accel[2] = ((double)calibratedAccel)/((double)((int64_t)1 << (GnuSettings.VARIO_VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_ACCEL_SCALE_SHIFT)));
#else		//VERTACCEL_ENABLE_SD_SETTINGS
  calibratedAccel = (int64_t)imuAccel[0] << VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER;
  calibratedAccel -= (int64_t)settings.accelCal.bias[0];
  calibratedAccel *= ((int64_t)settings.accelCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
  accel[0] = ((double)calibratedAccel)/((double)((int64_t)1 << (VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_ACCEL_SCALE_SHIFT)));

  calibratedAccel = (int64_t)imuAccel[1] << VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER;
  calibratedAccel -= (int64_t)settings.accelCal.bias[1];
  calibratedAccel *= ((int64_t)settings.accelCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
  accel[1] = ((double)calibratedAccel)/((double)((int64_t)1 << (VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_ACCEL_SCALE_SHIFT)));

  calibratedAccel = (int64_t)imuAccel[2] << VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER;
  calibratedAccel -= (int64_t)settings.accelCal.bias[2];
  calibratedAccel *= ((int64_t)settings.accelCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
  accel[2] = ((double)calibratedAccel)/((double)((int64_t)1 << (VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_ACCEL_SCALE_SHIFT)));
#endif //VERTACCEL_ENABLE_SD_SETTINGS
#endif //VERTACCEL_STATIC_CALIBRATION
  
  for(int i = 0; i<4; i++)
    quat[i] = ((double)imuQuat[i])/LIGHT_INVENSENSE_QUAT_SCALE;
  
  
  /******************************/
  /* real and vert acceleration */
  /******************************/
  
  /* compute vertical direction from quaternions */
  vertVector[0] = 2*(quat[1]*quat[3]-quat[0]*quat[2]);
  vertVector[1] = 2*(quat[2]*quat[3]+quat[0]*quat[1]);
  vertVector[2] = 2*(quat[0]*quat[0]+quat[3]*quat[3])-1;
  
  /* compute real acceleration (without gravity) */
  double ra[3];
  for(int i = 0; i<3; i++) 
    ra[i] = accel[i] - vertVector[i];
  
  /* compute vertical acceleration */
  vertAccel = (vertVector[0]*ra[0] + vertVector[1]*ra[1] + vertVector[2]*ra[2]) * VERTACCEL_G_TO_MS;
}


#ifdef AK89xx_SECONDARY
void Vertaccel::computeNorthVector(double* vertVector, int16_t* mag, double* northVector) {

  /*-------------------------------*/
  /*   north vector computation    */
  /*-------------------------------*/
  
  double n[3];

#ifndef VERTACCEL_STATIC_CALIBRATION
  for(int i = 0; i<3; i++) {
#if defined(VERTACCEL_ENABLE_SD_SETTINGS)
    int64_t calibratedMag = ((int64_t)mag[i]) << GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER;
    calibratedMag -= (int64_t)settings.magCal.bias[i];
    calibratedMag *= ((int64_t)settings.magCal.scale + ((int64_t)1 << GnuSettings.VARIO_VERTACCEL_CAL_SCALE_MULTIPLIER));
    n[i] = ((double)calibratedMag)/((double)((int64_t)1 << (GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_MAG_PROJ_SCALE_SHIFT)));
#else //VERTACCEL_ENABLE_SD_SETTINGS
    int64_t calibratedMag = ((int64_t)mag[i]) << VERTACCEL_MAG_CAL_BIAS_MULTIPLIER;
    calibratedMag -= (int64_t)settings.magCal.bias[i];
    calibratedMag *= ((int64_t)settings.magCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
    n[i] = ((double)calibratedMag)/((double)((int64_t)1 << (VERTACCEL_MAG_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_MAG_PROJ_SCALE_SHIFT)));
#endif //VERTACCEL_ENABLE_SD_SETTINGS
  }
#else
  /* inline for optimization */
  int64_t calibratedMag;
#if defined(VERTACCEL_ENABLE_SD_SETTINGS)
  calibratedMag = ((int64_t)mag[0]) << GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER;
  calibratedMag -= (int64_t)settings.magCal.bias[0];
  calibratedMag *= ((int64_t)settings.magCal.scale + ((int64_t)1 << GnuSettings.VARIO_VERTACCEL_CAL_SCALE_MULTIPLIER));
  n[0] = ((double)calibratedMag)/((double)((int64_t)1 << (GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_MAG_PROJ_SCALE_SHIFT)));

  calibratedMag = ((int64_t)mag[1]) << GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER;
  calibratedMag -= (int64_t)settings.magCal.bias[1];
  calibratedMag *= ((int64_t)settings.magCal.scale + ((int64_t)1 << GnuSettings.VARIO_VERTACCEL_CAL_SCALE_MULTIPLIER));
  n[1] = ((double)calibratedMag)/((double)((int64_t)1 << (GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_MAG_PROJ_SCALE_SHIFT)));

  calibratedMag = ((int64_t)mag[2]) << GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER;
  calibratedMag -= (int64_t)settings.magCal.bias[2];
  calibratedMag *= ((int64_t)settings.magCal.scale + ((int64_t)1 << GnuSettings.VARIO_VERTACCEL_CAL_SCALE_MULTIPLIER));
  n[2] = ((double)calibratedMag)/((double)((int64_t)1 << (GnuSettings.VARIO_VERTACCEL_MAG_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_MAG_PROJ_SCALE_SHIFT)));
#else  //VERTACCEL_ENABLE_SD_SETTINGS
  calibratedMag = ((int64_t)mag[0]) << VERTACCEL_MAG_CAL_BIAS_MULTIPLIER;
  calibratedMag -= (int64_t)settings.magCal.bias[0];
  calibratedMag *= ((int64_t)settings.magCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
  n[0] = ((double)calibratedMag)/((double)((int64_t)1 << (VERTACCEL_MAG_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_MAG_PROJ_SCALE_SHIFT)));

  calibratedMag = ((int64_t)mag[1]) << VERTACCEL_MAG_CAL_BIAS_MULTIPLIER;
  calibratedMag -= (int64_t)settings.magCal.bias[1];
  calibratedMag *= ((int64_t)settings.magCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
  n[1] = ((double)calibratedMag)/((double)((int64_t)1 << (VERTACCEL_MAG_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_MAG_PROJ_SCALE_SHIFT)));

  calibratedMag = ((int64_t)mag[2]) << VERTACCEL_MAG_CAL_BIAS_MULTIPLIER;
  calibratedMag -= (int64_t)settings.magCal.bias[2];
  calibratedMag *= ((int64_t)settings.magCal.scale + ((int64_t)1 << VERTACCEL_CAL_SCALE_MULTIPLIER));
  n[2] = ((double)calibratedMag)/((double)((int64_t)1 << (VERTACCEL_MAG_CAL_BIAS_MULTIPLIER + VERTACCEL_CAL_SCALE_MULTIPLIER + LIGHT_INVENSENSE_MAG_PROJ_SCALE_SHIFT)));
#endif	//VERTACCEL_ENABLE_SD_SETTINGS
#endif
        
  /* compute north vector by applying rotation from v to z to vector n */
  vertVector[2] = -1.0 - vertVector[2];
  northVector[0] = (1+vertVector[0]*vertVector[0]/vertVector[2])*n[0] + (vertVector[0]*vertVector[1]/vertVector[2])*n[1] - vertVector[0]*n[2];
  northVector[1] = (vertVector[0]*vertVector[1]/vertVector[2])*n[0] + (1+vertVector[1]*vertVector[1]/vertVector[2])*n[1] - vertVector[1]*n[2];
}

#endif //AK89xx_SECONDARY

