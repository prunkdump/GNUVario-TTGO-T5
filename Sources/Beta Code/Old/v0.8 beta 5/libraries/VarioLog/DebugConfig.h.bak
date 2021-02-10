/* DebugConfig -- Gestion du debuggage
 *
 * Copyright 2019 Jean-philippe GOI
 * 
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
/*                           DebugConfig                                         */
/*                                                                               */
/*  version    Date       Description                                            */
/*    1.0                                                                        */
/*    1.0.1    16/08/19   Ajout BT_DEBUG          							              	 */
/*    1.0.2    17/08/19   Ajout WIFI_DEBUG                                       */
/*    1.0.3    30/08/19   Ajout SOUND_DEBUG																			 */
/*                                                                               */
/*********************************************************************************/

#ifndef _DEBUGCONFIG_H_
#define _DEGUGCONFIG_H_

//Monitor Port 
#if defined(ESP8266)
#define SerialPort Serial
#elif defined(ESP32)
#define SerialPort Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialPort SerialUSB
#elif defined(_BOARD_GENERIC_STM32F103C_H_)

#elif defined(ARDUINO_AVR_PRO)
#define SerialPort Serial
#else
#define SerialPort Serial
#endif

/**********************************/
/*     ACTIVATION/DESACTIVATION   */
/*      des messages de debbug    */
/*                                */
/* Ne pas desactiver la ligne     */
/* #define PROG_DEBUG             */
/*                                */
/* Commenter #define ENABLE_DEBUG */
/* pour désactiver les messages   */
/**********************************/

#define ENABLE_DEBUG

#if defined(ENABLE_DEBUG)


// 							OUTPUT SERIALNMEA
//#define SERIAL_NMEA_SERIAL_OUTPUT
#define SERIAL_NMEA_BLUETOOTH_OUTPUT


//              DEBUGING MODE
#define PROG_DEBUG			  //debug principal program
//#define HARDWARE_DEBUG
//#define IMU_DEBUG			  //debug IMU
//#define I2CDEV_SERIAL_DEBUG   //debug I2Cdev
//#define DEBUG_SERIAL_NMEA_1
//#define SCREEN_DEBUG
//#define SCREEN_DEBUG2
//#define GPS_DEBUG
//#define BUTTON_DEBUG
//#define TONEDAC_DEBUG
//#define MS5611_DEBUG
//#define KALMAN_DEBUG
//#define ACCEL_DEBUG
//#define EEPROM_DEBUG
//#define NMEAPARSER_DEBUG
//#define VOLTAGE_DIVISOR_DEBUG
//#define SDCARD_DEBUG
//#define IGC_DEBUG
#define DATA_DEBUG
//#define BT_DEBUG
//#define WIFI_DEBUG
//#define SOUND_DEBUG
//#define AGL_DEBUG
//#define SQL_DEBUG
//#define BEARING_DEBUG
//#define TWOWIRESCH_DEBUG
//#define POWER_DEBUG
//#define MEMORY_DEBUG
#endif //ENABLE_DEBUG
#endif
