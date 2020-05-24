/*********************************************************************************/
/*                                                                               */
/*                           Libraries SdCardHal                                 */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    19/04/19                                                            */
/*    1.1    26/11/19   Ajout SDFat                                              */
/*                                                                               */
/*********************************************************************************/
#ifndef sdcardHAL_h
#define sdcardHAL_h

#include <Arduino.h>

#include <HardwareConfig.h>

#ifndef SDFAT_LIB
//#define SDFAT_LIB
#endif

#if defined(ESP8266)
//********************
// ESP8266
//********************

#elif defined(ESP32)
//********************
// ESP32
//********************

#if defined(SDFAT_LIB)

#include <SPI.h>
#include "SdFat.h"

//#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
//
// Pin numbers in templates must be constants.

	const uint8_t SOFT_MISO_PIN = SDCARD_MISO_PIN;
	const uint8_t SOFT_MOSI_PIN = SDCARD_MOSI_PIN;
	const uint8_t SOFT_SCK_PIN  = SDCARD_SCK_PIN;
	//
	// Chip select may be constant or RAM variable.
	const uint8_t SD_CHIP_SELECT_PIN = SDCARD_CS_PIN;

class SdCardHAL {

  public:

	  boolean begin(void);
};

extern SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> SDHAL_SD;

#else

#include "FS.h"
#include "SD.h"
#include "SPI.h"

class SdCardHAL { //public SDClass {

  public:

	  boolean begin(void);
};

#define SDHAL_SD SD

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

class SdCardHAL : public SdFat {

  public:

	  boolean begin(void);
};

#endif

#define SDCARD_STATE_INITIAL 0
#define SDCARD_STATE_INITIALIZED 1
#define SDCARD_STATE_READY 2
#define SDCARD_STATE_ERROR -1

extern SdCardHAL SDHAL; 
//#define _fs SDHAL

#endif
	