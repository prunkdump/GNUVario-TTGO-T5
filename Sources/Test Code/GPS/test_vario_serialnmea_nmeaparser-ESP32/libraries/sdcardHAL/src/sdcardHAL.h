/*********************************************************************************/
/*                                                                               */
/*                           Libraries SdCardHal                                 */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    19/04/19                                                            */
/*                                                                               */
/*********************************************************************************/
#ifndef sdcardHAL_h
#define sdcardHAL_h

#include <Arduino.h>

#if defined(ESP8266)
//********************
// ESP8266
//********************

#elif defined(ESP32)
//********************
// ESP32
//********************

#include <mySD.h>

class SdCardHAL : public SDClass {

  public:

	  boolean begin(void);
};

#elif defined(ARDUINO_AVR_PRO)
//********************
// ARDUINO PRO MINI
//********************

#elif defined(ARDUINO_ARCH_SAMD)
//********************
// MKR ZERO
//********************
#include <SPI.h>
#include <SdFat.h>

class SdCardHAL : public SdFat {

  public:

	  boolean begin(void);
};

#endif

extern SdCardHAL SDHAL; 

#endif
	