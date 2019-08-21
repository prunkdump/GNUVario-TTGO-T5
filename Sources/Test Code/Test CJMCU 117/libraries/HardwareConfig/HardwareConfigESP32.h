/* HardwareConfig -- 
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * HardwareConfig pour ESP32.
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
/*    1.0.1  24/06/19   Ajout define VARIOMETER_POWER_ON_DELAY 									 */
/*                                                                               */
/*********************************************************************************/


/*
*****************
MPU 9250 / MS5611
*****************
 
SDA_PIN 	IO21
SCL_PIN 	IO22

*******************
Ecran EPaper 1.54''
*******************

CS/SS 		IO5
DC 				IO17
RST 			IO16
BUSY 			IO4

MOSI			15
MISO			2
SCK				14

****
LED
**** 

LED_BUILTIN 	22

*******
SDCard
*******

mySD.h

M5Stack.h

CS 			13
MOSI		15
MISO		2
SCK			14

*********
Boutons
*********

M4Stack.h

Bouton A	38
Bouton B	37
Bouton C	39

************
Speaker
************

PWM/DAC		IO25

 NS4148, http://www.chipsourcetek.com/Uploads/file/20151207193630_0605.pdf amplificateur audio mono 3W - Classe D connecté sur la pin GPIO 25

(XT_DAC_Audio) for ESP32  http://www.xtronical.com/the-dacaudio-library-download-and-installation/ 

PWM/DAC	IO26		optional ampli class D
Enable	IO34	enable optional ampli class D

******************
GPS
******************

RX	IO19

******************************
Lecture de la charge batterie
******************************

tension  IO35	

            vbat
             |
            270k
             |
      IO35 ---
             |
            1M
             |
            GND



EEprom

IGC Header			0x00		195+2		0xC8
Sounnd volume   0xC8    1+2     0xD0
Fly stat        0xD0		26+2

*/


#ifndef _HARDWARECONFIGESP32_H_
#define _HARDWARECONFIGESP32_H_

#if defined(ESP32)

/******************************/
/*            SCREEN          */
/******************************/

/* Set the pins used for Screen modules */

#define VARIOSCREEN_CS_PIN SS
#define VARIOSCREEN_DC_PIN 17
#define VARIOSCREEN_RST_PIN 16
#define VARIOSCREEN_BUSY_PIN 4


/****************************/
/*           SDCARD         */
/****************************/

#define SDCARD_CS_PIN   13
#define SDCARD_MOSI_PIN 15
#define SDCARD_MISO_PIN 2
#define SDCARD_SCK_PIN  14

/***************************/
/*        LED              */
/***************************/

#define pinLED 22

#define LED_ON() 		   {GPIO.out_w1ts = (1 << (pinLED));}
#define LED_OFF()		   {GPIO.out_w1tc = (1 << (pinLED));}

#ifndef LED_BUILTIN
#define LED_BUILTIN pinLED
#endif


/****************************/
/*           Buttons        */
/****************************/

#define VARIOBTN_LEFT_PIN 38
#define VARIOBTN_CENTER_PIN 37
#define VARIOBTN_RIGHT_PIN 39

#define BTN_A 0
#define BTN_B 1
#define BTN_C 2
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_C 2

#define BUTTON_A_PIN 38
#define BUTTON_B_PIN 37
#define BUTTON_C_PIN 39


/****************/
/*     TENSION  */
/****************/

// The voltage divisor 

#define VOLTAGE_DIVISOR_PIN 35  //ADC1_CH7 (GPIO 35)
#define VOLTAGE_RESOLUTION  4096.0f


/*************************/
/*        GPS            */
/*************************/

#define pinGpsTXD  (-1)
#define pinGpsRXD  (19)
#define pinGpsRTS  (-1)
#define pinGpsCTS  (-1)

/* The bauds rate used by the GPS and Bluetooth modules. */
/* GPS and bluetooth need to have the same bauds rate.   */
#define GPS_BLUETOOTH_BAUDS 9600

#define GPS_UART_NUM          UART_NUM_1
//#define UART_RX_BUFFER_SIZE   512
#define UART_RX_BUFFER_SIZE   256

/* GPS / bluetooth pins */
#define SERIAL_NMEA_RX_PIN 19
#define SERIAL_NMEA_TX_PIN 21


/*************************/
/*         AUDIO         */
/*************************/

#define PIN_AUDIO_AMP_ENA     34			//Enabled ampli class D
#define SPEAKER_PIN 					25			//or 26
#define TONE_PIN_CHANNEL 			0				// or 1

#define AUDIO_TYPE_INTERFACE
#define TONE 													// 1 pin PWM
//#define	TONEAC 											// 2 pins Push-Pull PWM
//#define	TONEDAC
//#define TONEI2S

/*********************/
/* TWO WIRE settings */
/* MPU 9250 / MS5611 */
/*********************/

#define VARIO_TW_SDA_PIN 21
#define VARIO_TW_SCL_PIN 22

/* Set the freq */
#define VARIO_TW_FREQ 400000UL

/*************************/
/*         POWER         */
/*************************/

#define POWER_PIN 27
#define POWER_PIN_STATE HIGH

/* time needed to power on all the devices */
#define VARIOMETER_POWER_ON_DELAY 2000

/*****************************/
/*         EEPROM            */
/*****************************/

/*****************************/
/*  IGC HEADER EEPROM        */
/*****************************/

#define IGC_SENTENCE_HEADER_EEPROM_ADDRESS 0x0 

/*****************************/
/*  EEPROM SOUND             */
/*****************************/

/* eeprom sound setting adresses */
#define SOUND_EEPROM_ADDR 0xC8

/*****************************/
/*  EEPROM STAT              */
/*****************************/
#define FLY_STAT_HEADER_EEPROM_ADDRESS 0xD0

#endif
#endif