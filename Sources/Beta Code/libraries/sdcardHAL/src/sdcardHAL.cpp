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

	//********************
	// SDFAT
	//********************

#if defined(SDFAT_LIB)

//#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
//
// Pin numbers in templates must be constants.
boolean SdCardHAL::begin(void){
	const uint8_t SOFT_MISO_PIN = SDCARD_MISO_PIN;
	const uint8_t SOFT_MOSI_PIN = SDCARD_MOSI_PIN;
	const uint8_t SOFT_SCK_PIN  = SDCARD_SCK_PIN;
	//
	// Chip select may be constant or RAM variable.
	const uint8_t SD_CHIP_SELECT_PIN = SDCARD_CS_PIN;

	// SdFat software SPI template
	SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

  return(sd.begin(SD_CHIP_SELECT_PIN));
  }

SdCardHAL SDHAL; 

#else
	//********************
  // MYSD
	//********************
	
#include <mySD.h>

boolean SdCardHAL::begin(void){
	return(SDClass::begin(SDCARD_CS_PIN,SDCARD_MOSI_PIN,SDCARD_MISO_PIN,SDCARD_SCK_PIN));	
};

SdCardHAL SDHAL; 

#endif

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

