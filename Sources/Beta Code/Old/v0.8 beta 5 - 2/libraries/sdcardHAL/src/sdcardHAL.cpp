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

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> SDHAL_SD; //sd;

boolean SdCardHAL::begin(void)
{

	return (SDHAL_SD.begin(SD_CHIP_SELECT_PIN));
}

SdCardHAL SDHAL;

#else
//********************
// SD
//********************

//#include <mySD.h>

#include "FS.h"
#include "SD.h"
#include <SPIFFS.h>
SPIClass SpiSdCard(HSPI);

boolean SdCardHAL::begin(void)
{
	//	return(SDClass::begin(SDCARD_CS_PIN,SDCARD_MOSI_PIN,SDCARD_MISO_PIN,SDCARD_SCK_PIN));

	SpiSdCard.begin(SDCARD_SCK_PIN, SDCARD_MISO_PIN, SDCARD_MOSI_PIN, SDCARD_CS_PIN);
	return (SDHAL_SD.begin(SDCARD_CS_PIN, SpiSdCard));
};

boolean SdCardHAL::end(void)
{
	SpiSdCard.end();
}

void SdCardHAL::deleteRecursive(String path)
{
	/***********************************/

	File fileSD;

	fileSD = SDHAL_SD.open((char *)path.c_str(), FILE_READ);
	if (!fileSD.isDirectory())
	{
		fileSD.close();
		SDHAL_SD.remove((char *)path.c_str());
		return;
	}

	fileSD.rewindDirectory();

	while (true)
	{
		File entry;
		if (!(entry = fileSD.openNextFile(FILE_READ)))

		{
			break;
		}

		String entryPath = entry.name();
		if (entry.isDirectory())
		{
			entry.close();
			deleteRecursive(entryPath);
		}
		else
		{
			entry.close();
			SDHAL_SD.remove((char *)entryPath.c_str());
#ifdef WIFI_DEBUG
			SerialPort.print("[HTTP] suppression du fichier : <");
			SerialPort.print(entryPath);
			SerialPort.println(">");
#endif
		}
	}

	SDHAL_SD.rmdir((char *)path.c_str());
	fileSD.close();
}

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

boolean SdCardHAL::begin(void)
{
	return (SDClass::begin(SDCARD_CS_PIN));
};

SdCardHAL SDHAL;
#endif
