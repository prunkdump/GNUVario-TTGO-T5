/* TwoWireScheduler -- Interrupt driven Two Wire devices scheduler
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

/***************************************************************************************************/
/*                                TWOWIRESCHEDULER                                                 */
/*                                                                                                 */
/*  Ver     Date                                                                                   */
/*  1.0                                                                                            */
/*  1.1     21/08/19     Ajout getTempAlti(double& temp, double& alti)                             */
/*  1.2     04/09/19		 Modification nom bibliothèque MS5611                                      */
/*  1.3     15/11/19     modif delayMicroseconds(400);                                             */
/*                                                                                                 */
/***************************************************************************************************/

#ifndef TWO_WIRE_SCHEDULER_H
#define TWO_WIRE_SCHEDULER_H

#include <Arduino.h>
#include <VarioSettings.h>
#include <esp32-hal-timer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <DebugConfig.h>
#include <HardwareConfig.h>

/* scheduler task parameters */
#define TWO_WIRE_SCHEDULER_STACK_SIZE 2000
#define TWO_WIRE_SCHEDULER_CORE 1
#define TWO_WIRE_SCHEDULER_PRIORITY 10

/* timer used */
#define TWO_WIRE_SCHEDULER_TIMER_NUM 3

#ifdef HAVE_BMP280
#include <bmp280.h>
#else
#include <ms5611TW.h>
#endif

#ifdef HAVE_ACCELEROMETER
#include <vertaccel.h>
#endif

/* clock settings */
#define TWO_WIRE_SCHEDULER_INTERRUPT_PRESCALE 80

/******************************************/
/* The barometer is the reference freq    */
/* read take max 0.3 ms                   */
/*                                        */
/* bmp280                                 */ 
/* -> max measurement time : 43.2 ms      */
/* -> 43.5 ms period ( approx 20 Hz )     */
/* -> ref period = 40 ( more than 20 Hz ) */
/* -> real unit = 1.088 ms                */
/*                                        */
/* ms5611                                 */
/* -> max measurement time : 9.04 ms      */
/* -> 9.34 ms period ( approx 100 Hz )    */
/* -> ref period = 8 ( more than 100 Hz ) */
/* -> real unit = 1.168 ms                */
/*                                        */
/* mpu fifo                               */
/* -> freq = 100 Hz but checked at 200 Hz */
/* -> period = 4 ( more than 200 Hz )     */
/*                                        */
/* magnetometer                           */
/* -> freq = 10 Hz but checked at 20 Hz   */
/* -> period = 40 ( more than 20 Hz )     */
/******************************************/
#ifdef HAVE_BMP280
#define TWO_WIRE_SCHEDULER_INTERRUPT_COMPARE 1088
#else
#define TWO_WIRE_SCHEDULER_INTERRUPT_COMPARE 1168
#endif

/* The scheduler                                          */
/* see how TW request are shifted                         */
/* so they don't interact each other                      */
/* !! mpu fifo interrupt take 1.4 ms, more than 1 unit !! */ 
#define TWO_WIRE_SCHEDULER_MS5611_PERIOD 8
#define TWO_WIRE_SCHEDULER_MS5611_SHIFT 0
#define TWO_WIRE_SCHEDULER_BMP280_PERIOD 40
#define TWO_WIRE_SCHEDULER_BMP280_SHIFT 0
#define TWO_WIRE_SCHEDULER_IMU_PERIOD 4
#define TWO_WIRE_SCHEDULER_IMU_SHIFT 1
#define TWO_WIRE_SCHEDULER_MAG_PERIOD 40
#define TWO_WIRE_SCHEDULER_MAG_SHIFT 3


/*******************************************************/
/* !!!                                             !!! */
/* To use TWScheduler, init static variables :         */
/*                                                     */
/* Ms5611 TWScheduler::ms5611;                         */
/* Vertaccel TWScheduler::vertaccel;                   */
/* !!!                                             !!! */
/*******************************************************/
class TWScheduler {

 public:
  /* static class devices to define */
#ifdef HAVE_BMP280
  static Bmp280 bmp280;
#else  
  static Ms5611 ms5611;
#endif
  
#ifdef HAVE_ACCELEROMETER
  static Vertaccel vertaccel;
#endif

  /* init both devices but not the TW bus */
  static void init(void);

  /* barometer part */
  static bool havePressure(void);
  static double getAlti(void);	
  static void getTempAlti(double& temp, double& alti);

#ifdef HAVE_ACCELEROMETER
  /* IMU part */
  /* tap callback is triggered by getRawAccel and getAccel */
  static bool haveAccel(void);
  static void getRawAccel(int16_t* rawAccel, int32_t* quat);
  static double getAccel(double* vertVector); //vertVector = NULL if not needed
#ifdef AK89xx_SECONDARY
  static bool haveMag(void);
  static void getRawMag(int16_t* rawMag);
  static void getNorthVector(double* vertVector, double* northVector); //give the vertVector obtained previously
#endif //AK89xx_SECONDARY
#endif //HAVE_ACCELEROMETER
  
  /* the main interrupt */
  static void mainInterrupt(void);

 private:
  static uint8_t volatile status;
#ifdef HAVE_BMP280 
  static uint8_t volatile bmp280Output[2*3];  //two bmp280 output measures
  static uint8_t volatile bmp280Count;
  static SemaphoreHandle_t bmp280Mutex;
#else
  static int8_t volatile ms5611Step; 
  static uint8_t volatile ms5611Output[3*3];  //three ms5611 output measures
  static uint8_t volatile ms5611Count;
  static SemaphoreHandle_t ms5611Mutex;
#endif
#ifdef HAVE_ACCELEROMETER
  static uint8_t volatile checkOutput[2];
  static uint8_t volatile imuOutput[LIGHT_INVENSENSE_COMPRESSED_DMP_PAQUET_LENGTH]; //imu dmp fifo output
#ifdef MPU_ENABLE_INT_PIN
  static uint8_t volatile imuIntCount;
  static SemaphoreHandle_t imuIntCountMutex; 
#endif //MPU_ENABLE_INT_PIN
  static uint8_t volatile imuCount;
  static SemaphoreHandle_t imuMutex;
#ifdef AK89xx_SECONDARY
  static uint8_t volatile magOutput[8];    //magnetometer output
  static uint8_t volatile magCount;
  static SemaphoreHandle_t magMutex;
#endif //AK89xx_SECONDARY
#endif //HAVE_ACCELEROMETER
  static TaskHandle_t schedulerTaskHandler;
  static hw_timer_t* timer;
  
  /* private interrupt methods */
#ifdef HAVE_BMP280
  static void bmp280Interrupt(void);
  static void bmp280OutputCallback(void);
#else
  static void ms5611Interrupt(void);
  static void ms5611TempCallback(void);
  static void errorRelaunch(void);
  static void errorRelaunchCallback(void);
  static void ms5611OutputCallback(void);
#endif
#ifdef HAVE_ACCELEROMETER
  static void imuInterrupt(void);
  static void imuCheckFifoCountCallBack(void);
  static void imuReadFifoData(void);
  static void imuHaveFifoDataCallback(void);
#ifdef MPU_ENABLE_INT_PIN
  static void imuIntPinInterrupt(void);
#endif //MPU_ENABLE_INT_PIN
#ifdef AK89xx_SECONDARY
  static void magInterrupt(void);
  static void magCheckStatusCallback(void);
  static void magHaveDataCallback(void);
#endif //AK89xx_SECONDARY
#endif //HAVE_ACCELEROMETER
  static void interruptScheduler(void* param);
  static void timerCallback(void);
};

extern TWScheduler twScheduler;

#endif //TWO_WIRE_SCHEDULER_H
