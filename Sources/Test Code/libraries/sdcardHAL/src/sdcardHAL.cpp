/*********************************************************************************/
/*                                                                               */
/*                           Libraries SdcardHal                                 */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    06/05/19                                                            */
/*                                                                               */
/*********************************************************************************/

#include <sdcardHAL.h>
#include <Arduino.h>

#include <HardwareConfig.h>

#if defined(ESP8266)
//********************
// ESP8266
//********************

#elif defined(ESP32)
//********************
// ESP32
//********************

#include <mySD.h>

boolean SdCardHAL::begin(void){
	return(SDClass::begin(SDCARD_CS_PIN,SDCARD_MOSI_PIN,SDCARD_MISO_PIN,SDCARD_SCK_PIN));	
};

SdCardHAL SDHAL; 

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

boolean  SdCardHAL::begin(void){
	return(SDClass::begin(SDCARD_CS_PIN));
};

SdCardHAL SDHAL; 
#endif

