#ifndef HARDWARECONFIGPRO_H_
#define HARDWARECONFIGPRO_H_

#if defined(ARDUINO_AVR_PRO)

/******************************/
/*            SCREEN          */
/******************************/

/* Set the pins used for Screen modules */


/****************************/
/*           SDCARD         */
/****************************/

#define SDCARD_CS_PIN 14

/***************************/
/*        LED              */
/***************************/

#define pinLED (13)

#ifndef LED_BUILTIN
#define LED_BUILTIN 22
#endif

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

#define VOLTAGE_DIVISOR_PIN 16

#define VOLTAGE_DIVISOR_VALUE 1.27
#define VOLTAGE_DIVISOR_REF_VOLTAGE 3.3

/*************************/
/*        GPS            */
/*************************/


#define pinGpsTXD  (21)
#define pinGpsRXD  (22)
#define pinGpsRTS  (-1)
#define pinGpsCTS  (-1)


/*************************/
/*         AUDIO         */
/*************************/

#define pinAudio          (25)


/*****************************/
/*  EEPROM SOUND             */
/*****************************/

/* eeprom sound setting adresses */
#define SOUND_EEPROM_TAG 9806
#define SOUND_EEPROM_ADDR 0x30

/*********************/
/* MPU 9250 / MS5611 */
/*********************/

#define VARIO_TW_SDA_PIN 21
#define VARIO_TW_SCL_PIN 22

#endif

#endif