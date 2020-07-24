#ifndef _HARDWARECONFIGMK0_H_
#define _HARDWARECONFIGMK0_H_

// CJMCU-117 MPU9250+MS5611 circuit interface
//
// VCC  VCC
// GND  GND
//
// MS5611 - MPU9250			WIRE/I2C (SERCOM0)
// SCL  D12 		SCL			WIRE/I2C - SERCOM0
// SDA  D11 		SDA			MS5611 / MPU9250
//
// SWITCHS
// 				A1,A2 				Switch
// 				D5    				Switch Droite - Power
//
// RST   	A4		 	Reset
//
// E-Ink								SPI (SERCOM1)
// CS    	D4
// BUSY  	D3
// RST   	A3
// DC    	D7
// DIN   	MOSI/D8
// CLK   	SCK/D9
//
// GPS	FGPMMOPA6C, construite autour du jeu de puces MTK3339		Serial1 (SERCOM5)
// TX    	D14						serial1  
// RX     D13 					Serial1     
//
// Bluetooth / Microship RN4871		SERIAL (SERCOM3)
// TX    	D0 						sercom3
// RX    	D1 						sercom3
// 				A5						RN4871:RST
//				A6						RN4871:INT		sleep / wake manager
// 
// SD-CARD		SPI (SERCOM2)
// MOSI		MOSI					SD SPI
// SCL		SCK						SD SPI
// SS			SS						SD SPI	
// MISO		MISO					SD SPI
// CD			CD						SD SPI
//
// Buzzer
//    		D2						PWM
//
//				A0/DAC0				DAC0
//				D6						Mute Max
//
// ADC_BATTERY					Battery voltage
// LED_BUILTIN					LED

/******************************/
/*            SCREEN          */
/******************************/

/* Set the pins used for Screen modules */

#if defined(ARDUINO_ARCH_SAMD)

// variant.h of MKR1000
//#define PIN_SPI_MISO  (10u)
//#define PIN_SPI_MOSI  (8u)
//#define PIN_SPI_SCK   (9u)
//#define PIN_SPI_SS    (24u) // should be 4?
// variant.h of MKRZERO
//#define PIN_SPI_MISO  (10u)
//#define PIN_SPI_MOSI  (8u)
//#define PIN_SPI_SCK   (9u)
//#define PIN_SPI_SS    (4u)

//GxIO_Class io(SPI, 4, 7, 6);
//GxEPD_Class display(io, 6, 5);

#define VARIOSCREEN_CS_PIN 4   
#define VARIOSCREEN_DC_PIN 7
#define VARIOSCREEN_RST_PIN A3
#define VARIOSCREEN_BUSY_PIN 3
//#define VARIOSCREEN_BUSY_PIN 5

/****************************/
/*           SDCARD         */
/****************************/

/*
  SD card read/write

 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 */

#define SDCARD_CS_PIN SDCARD_SS_PIN

/***************************/
/*        LED              */
/***************************/

#define pinLED (LED_BUILTIN)

/****************************/
/*           Buttons        */
/****************************/

/*int pinSDA = 11;
int pinSCL = 12;*/
//#define VARIODRDY_INT_PIN 3
//#define VARIOAUDIO_PWM1_PIN A3
//#define VARIOAUDIO_PWM2_PIN A4
#define VARIOBTN_LEFT_PIN A2
#define VARIOBTN_CENTER_PIN A1
#define VARIOBTN_RIGHT_PIN D5

/*interrupts in the Zero variant:

EXTERNAL_INT_2: A0, A5, 10
EXTERNAL_INT_4: A3, 6
EXTERNAL_INT_5: A4, 7
EXTERNAL_INT_6: 8, SDA
EXTERNAL_INT_7: 9, SCL
EXTERNAL_INT_9: A2, 3
EXTERNAL_INT_10: TX, MOSI
EXTERNAL_INT_11: RX, SCK*/

#define VARIOPOWER_INT_PIN 6
//const byte interruptPin = A5;

//#define VARIO_DETECT_USB A6
#define VARIO_PIN_ALIM   D5
#define VARIO_PIN_RST    A4
// A6    Detection de connection USB
// A5    Commande de l'alimentation des cartes

/***********************/
/* The voltage divisor */
/***********************/

#define VOLTAGE_DIVISOR_PIN ADC_BATTERY

/*************************/
/*        GPS            */
/*************************/

//#define pinGpsTXD  (21)
//#define pinGpsRXD  (22)
//#define pinGpsRTS  (-1)
//#define pinGpsCTS  (-1)

/*************************/
/*         AUDIO         */
/*************************/

#define PIN_AUDIO_AMP_ENA     6 			//Enabled ampli class D
#define SPEAKER_PIN 					25			//or 26
#define TONE_PIN_CHANNEL 			0				// or 1

#define AUDIO_TYPE_INTERFACE
#define TONE 													// 1 pin PWM
//#define	TONEAC 											// 2 pins Push-Pull PWM
//#define	TONEDAC
//#define TONEI2S

#endif 

#endif