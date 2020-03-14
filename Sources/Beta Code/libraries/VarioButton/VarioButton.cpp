/* varioButton -- 
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * This file is part of Gnuvario-E.
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

/* 
 ************************************************************************************
 *                                                                              	*
 *                           VarioScreenGxEPD                                    	*
 *                                                                               	*
 *  version    Date     Description                                              	*
 *    1.0                                                                        	*
 *    1.0.1  15/0/19    Ajout WifiServeur                                        	*
 *                      Modification déclaration printDirectory                  	*
 *    1.0.2  16/08/19   Ajout #ifdef HAVE_WIFI										*
 *    1.0.3  17/08/19   changement wasPressed par isPressed                      	*
 *                      Correction de la fonction update                         	*
 *    1.0.4  22/08/19   Ajout affichage reboot										*
 *    1.0.5  22/08/19   Ajout gestion changement de page                         	*
 *    1.0.6  26/08/19   Ajout gestion page config sound                          	*
 *                      Augmentation debounce time                               	*
 *                      Ajout _state button																				*
 *    1.0.7  31/08/19		Correction bug reglage son																*
 *    1.0.8  25/09/19   Ajout appuie 3 sec bouton central                        	* 
 *    1.0.9  29/09/19	  Ajout gestion page de calibration                        	*
 *    1.0.9  29/09/19	  Ajout gestion page de calibration                        	*
 *    1.0.10 03/10/19   Ajout HAVE_SDCARD                                        	*
 *    1.0.11 28/10/19		Ajout bip de control lors de la configuration du volume 	*
 *    1.0.12 01/11/19   Modification de la configuration du volume               	*
 *    1.0.13 03/11/19   Déplacement de la validation de la mise en veille sur le 	*
 *                      bouton gauche                                            	*
 *    1.0.14 29/11/19   Ajout arduinotrace                                       	*
 *                      Modif sdfat                                              	*
 *    1.0.15 09/03/20   Modification ScreenViewSound                             	*
 *    1.0.16 10/03/20   Ajout Bouton A 2sec calibration via AGL         					*
 *                      Ajout déclenchement debut du vol (appuie sur bouton A     *
 *                                                                               	*
 ************************************************************************************/

#include "VarioButton.h"
#include <DebugConfig.h>
#include "esp_log.h"

#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

#include <toneHAL.h>
#include <beeper.h>
#include <varioscreenGxEPD.h>
#ifdef HAVE_WIFI
#include <VarioWifiServer.h>
#endif //HAVE_WIFI

#include <Utility.h>
#include <VarioCalibration.h>

#ifdef BUTTON_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include <VarioLog.h>

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
#include <kalmanvert.h>
#include <AglManager.h>
#endif //HAVE_SDCARD && HAVE_GPS

uint8_t RegVolume;

#define VARIOMETER_STATE_INITIAL 0
#define VARIOMETER_STATE_DATE_RECORDED 1
#define VARIOMETER_STATE_CALIBRATED 2
#define VARIOMETER_STATE_FLIGHT_STARTED 3

void VARIOButton::begin()
{

	// Setup the button with an internal pull-up
	pinMode(BUTTON_A_PIN, INPUT_PULLUP);
	pinMode(BUTTON_B_PIN, INPUT_PULLUP);
	pinMode(BUTTON_C_PIN, INPUT_PULLUP);

	// Set wakeup button
	//    setWakeupButton(BUTTON_A_PIN);

#ifdef BUTTON_DEBUG
	SerialPort.println("OK");
#endif
}

/**********************************************************/
void VARIOButton::update()
{
	/**********************************************************/
	//Button update
	BtnA.read();
	BtnB.read();
	BtnC.read();
}

/**********************************************************/
void VARIOButton::setWakeupButton(uint8_t button)
{
	/**********************************************************/
	_wakeupPin = button;
}

VARIOButton VarioButton;

/**********************************************************/
void VARIOButtonScheduleur::update()
{
	/**********************************************************/

	VarioButton.update();

	/////////////////////////////////////////////////
	/////////////////// BOUTON A  ///////////////////
	/////////////////////////////////////////////////

	if (VarioButton.BtnA.isPressed())
	{
#ifdef BUTTON_DEBUG
		SerialPort.printf("isPressed A \r\n");
#endif //BUTTON_DEBUG
		_stateBA = true;
	}

	if (VarioButton.BtnA.pressedFor(2000))
	{
#ifdef BUTTON_DEBUG
		SerialPort.printf("pressedFor 2s A \r\n");
#endif //BUTTON_DEBUG
#if defined(HAVE_SDCARD) && defined(HAVE_GPS) && defined(AGL_MANAGER_H)
		int groundLevel = aglManager.getGroundLevel();
		if (groundLevel != -1)
		{
			kalmanvert.calibratePosition(groundLevel);
			
			int aglLevel = aglManager.getAgl();
			GnuSettings.COMPENSATION_GPSALTI = aglLevel;
			
			char tmpchar[20] = "params.jso";
			GnuSettings.saveConfigurationVario(tmpchar);

			beeper.generateTone(523, 250);
			beeper.generateTone(659, 250);
			beeper.generateTone(784, 250);
			beeper.generateTone(1046, 250);
		}
#endif
		_stateBA = false;
	}

	if (VarioButton.BtnA.wasReleased())
	{
#ifdef BUTTON_DEBUG
		SerialPort.printf("wasReleased A \r\n");
#endif //BUTTON_DEBUG
		if (_stateBA == true)
		{
			treatmentBtnA(false);
			_stateBA = false;
		}
	}

	/////////////////////////////////////////////////
	/////////////////// BOUTON B  ///////////////////
	/////////////////////////////////////////////////

	if (VarioButton.BtnB.pressedFor(3000))
	{
#ifdef BUTTON_DEBUG
		SerialPort.println("*********************************************");
		SerialPort.printf("pressedFor 3s B \r\n");
		SerialPort.println("*********************************************");
#endif //BUTTON_DEBUG

		TRACE();
		SDUMP("pressed For 3s B");
		_stateBB = false;
		treatmentBtnB3S(false);
	}

	if (VarioButton.BtnB.isPressed())
	{
#ifdef BUTTON_DEBUG
		SerialPort.printf("isPressed B \r\n");
#endif //BUTTON_DEBUG
		_stateBB = true;
	}

	if (VarioButton.BtnB.wasReleased())
	{
#ifdef BUTTON_DEBUG
		SerialPort.printf("wasReleased B \r\n");
#endif //BUTTON_DEBUG
		if (_stateBB == true)
		{
			treatmentBtnB(false);
			_stateBB = false;
		}
	}

	/////////////////////////////////////////////////
	/////////////////// BOUTON C  ///////////////////
	/////////////////////////////////////////////////

	if (VarioButton.BtnC.isPressed())
	{ //wasPressed()) {
#ifdef BUTTON_DEBUG
		SerialPort.printf("isPressed C \r\n");
#endif //BUTTON_DEBUG
		if (_stateBC == false)
		{
			treatmentBtnC(false);
			_stateBC = true;
		}
	}

	if (VarioButton.BtnC.wasReleased())
	{
#ifdef BUTTON_DEBUG
		SerialPort.printf("wasReleased C \r\n");
#endif //BUTTON_DEBUG
		_stateBC = false;
	}

	if (VarioButton.BtnC.pressedFor(2000))
	{
#ifdef BUTTON_DEBUG
		SerialPort.printf("pressedFor 2s C \r\n");
#endif //BUTTON_DEBUG
	}
}

/**********************************************************/
void VARIOButtonScheduleur::Set_StatePage(uint8_t state)
{
	/**********************************************************/
	StatePage = state;
}

/**********************************************************/
uint8_t VARIOButtonScheduleur::Get_StatePage(void)
{
	/**********************************************************/
	return StatePage;
}

#ifdef HAVE_SDCARD
#ifdef SDFAT_LIB
SdFile root;
#else  //SDFAT_LIB
File root;
#endif //SDFATLIB
#endif

/**********************************************************/
void VARIOButtonScheduleur::treatmentBtnA(bool Debounce)
{
/**********************************************************/
#ifdef BUTTON_DEBUG
	SerialPort.println("Bouton A");
#endif //BUTTON_DEBUG

#ifdef HAVE_WIFI
	if (StatePage == STATE_PAGE_INIT)
	{
		WifiServeur();
	}
	else if (StatePage == STATE_PAGE_VARIO)
	{
		
		if (variometerState == VARIOMETER_STATE_CALIBRATED) 
		{     
			if (GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START) {
        GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START = false;
			  createSDCardTrackFile();
			}
		} else {
			screen.schedulerScreen->previousPage();
		}
	}
	else if (StatePage == STATE_PAGE_WEBSERV)
	{
		SerialPort.println("RESTART ESP32");
		SerialPort.flush();
		ESP_LOGI("GnuVario-E", "RESTART ESP32");
		screen.ScreenViewReboot();
		ESP.restart();
	}
	else
#endif //HAVE_WIFI
		if (StatePage == STATE_PAGE_CONFIG_SOUND)
	{
		uint8_t tmpvol;
		tmpvol = RegVolume; //toneHAL.getVolume();
		if (tmpvol > 0)
		{
			tmpvol--;
			//			toneHAL.setVolume(tmpvol);
			//			beeper.setVolume(tmpvol);
			RegVolume = tmpvol;
			beeper.generateTone(2000, 300, RegVolume);
			screen.SetViewSound(RegVolume); //toneHAL.getVolume());
		}
	}
	else if (StatePage == STATE_PAGE_DEEP_SLEEP)
	{
		indicatePowerDown();
		//    TRACELOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG);
		MESSLOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG, "Deep sleep - Bouton");
		deep_sleep("En veille");
	}
	else if (StatePage == STATE_PAGE_CALIBRATE)
	{
		//sortie calibration
		SerialPort.println("RESTART ESP32");
		SerialPort.flush();
		ESP_LOGI("GnuVario-E", "RESTART ESP32");
		screen.ScreenViewReboot();
		ESP.restart();
	}
}

/**********************************************************/
void VARIOButtonScheduleur::treatmentBtnB(bool Debounce)
{
	/***********************************************************/

	if (StatePage == STATE_PAGE_VARIO)
	{

		if (screen.schedulerScreen->getPage() == screen.schedulerScreen->getMaxPage() + 1)
		{
			StatePage = STATE_PAGE_CONFIG_SOUND;
			RegVolume = toneHAL.getVolume();
			screen.SetViewSound(RegVolume);
		}
		else
		{

#ifdef BUTTON_DEBUG
			SerialPort.println("Mute");
#endif //BUTTON_DEBUG

			toneHAL.mute(!toneHAL.isMute());
			screen.volLevel->mute(toneHAL.isMute());
		}
	}
	else if (StatePage == STATE_PAGE_CONFIG_SOUND)
	{
		StatePage = STATE_PAGE_VARIO;
		screen.SetViewSound(RegVolume);
		toneHAL.setVolume(RegVolume);
		GnuSettings.soundSettingWrite(RegVolume);
		screen.volLevel->setVolume(RegVolume);
		screen.volLevel->mute(toneHAL.isMute());
	}
	else if (StatePage == STATE_PAGE_CALIBRATION)
	{
		//lancement de la calibration
		StatePage = STATE_PAGE_CALIBRATE;
		screen.ScreenViewMessage("en cours", 0);
		Calibration.Begin();
	}
}

/************************************************************/
void VARIOButtonScheduleur::treatmentBtnC(bool Debounce)
{
	/************************************************************/

	/*SerialPort.println("Read test.txt");

    // after writing then reopen the file and read it
    root = SDHAL.open("test.txt");
    if (root) {    
      // read from the file until there's nothing else in it 
      while (root.available()) {
        // read the file and print to Terminal 
        SerialPort.write(root.read());
      }
      root.close();
    } else {
      SerialPort.println("error opening test.txt");
    }
    SerialPort.println("done!");     	*/
	if (StatePage == STATE_PAGE_VARIO)
	{
		screen.schedulerScreen->nextPage();
	}
	else if (StatePage == STATE_PAGE_CONFIG_SOUND)
	{
		uint8_t tmpvol;
		tmpvol = RegVolume; //toneHAL.getVolume();
		if (tmpvol < 10)
		{
			tmpvol++;
			//			toneHAL.setVolume(tmpvol);
			//			beeper.setVolume(tmpvol);
			RegVolume = tmpvol;
			beeper.generateTone(2000, 300, RegVolume);
			screen.SetViewSound(RegVolume); //toneHAL.getVolume());
		}
	}
	else if (StatePage == STATE_PAGE_INIT)
	{
		StatePage = STATE_PAGE_CALIBRATION;
	}
}

#ifdef HAVE_WIFI
void VARIOButtonScheduleur::WifiServeur(void)
{
#ifdef BUTTON_DEBUG
	SerialPort.println("liste des fichiers");
#endif //BUTTON_DEBUG

#ifdef HAVE_SDCARD

	/* Begin at the root "/" */
#ifdef SDFAT_LIB
	if (root.open("/"))
	{
#else
	root = SDHAL_SD.open("/");
	if (root)
	{
#endif //SDFAT_LIB
		printDirectory(root, 0);
		root.close();
	}
	else
	{
#ifdef BUTTON_DEBUG
		SerialPort.println("error Sdcard directory");
#endif //BUTTON_DEBUG
	}
#endif //HAVE_SDCARD
#ifdef BUTTON_DEBUG
	SerialPort.println("done!");
#endif //BUTTON_DEBUG

	/*START SERVEUR WEB */
	varioWifiServer.begin();
	varioWifiServer.connect();

	varioWifiServer.start();

	Set_StatePage(STATE_PAGE_WEBSERV);

	while (1)
	{
		varioWifiServer.handleClient();
		update();
	}
}
#endif //HAVE_WIFI

#ifdef HAVE_SDCARD
#ifdef SDFAT_LIB
void VARIOButtonScheduleur::printDirectory(SdFile dir, int numTabs)
{

	char fBuffer[15];
	SdFile entry;

	dir.rewind();
	//#ifdef BUTTON_DEBUG
	while (true)
	{
		if (!entry.openNext(&dir, O_RDONLY))
		{

			break;
		}
		for (uint8_t i = 0; i < numTabs; i++)
		{
			SerialPort.print('\t'); // we'll have a nice indentation
		}
		// Print the name
		entry.getName(fBuffer, 13);
		SerialPort.print(fBuffer);
		/* Recurse for directories, otherwise print the file size */
		if (entry.isDir())
		{
			SerialPort.println("/");
			printDirectory(entry, numTabs + 1);
		}
		else
		{
			/* files have sizes, directories do not */
			SerialPort.print("\t\t");
			SerialPort.println(entry.fileSize());
		}
		entry.close();
	}
	//#endif //BUTTON_DEBUG
}
#else  //SDFAT_LIB
void VARIOButtonScheduleur::printDirectory(File dir, int numTabs)
{

	dir.rewindDirectory();
	//#ifdef BUTTON_DEBUG
	while (true)
	{
		File entry = dir.openNextFile();
		if (!entry)
		{
			break;
		}
		for (uint8_t i = 0; i < numTabs; i++)
		{
			SerialPort.print('\t'); // we'll have a nice indentation
		}
		// Print the name
		SerialPort.print(entry.name());
		/* Recurse for directories, otherwise print the file size */
		if (entry.isDirectory())
		{
			SerialPort.println("/");
			printDirectory(entry, numTabs + 1);
		}
		else
		{
			/* files have sizes, directories do not */
			SerialPort.print("\t\t");
			SerialPort.println(entry.size());
		}
		entry.close();
	}
	//#endif //BUTTON_DEBUG
}
#endif //SDFATLIB
#endif //HAVE_SDCARD

void VARIOButtonScheduleur::treatmentBtnB3S(bool Debounce)
{

	if (StatePage == STATE_PAGE_VARIO)
	{
		StatePage = STATE_PAGE_DEEP_SLEEP;
		screen.ScreenViewMessage("ARRET", 5);
		StatePage = STATE_PAGE_VARIO;
		screen.ScreenViewPage(screen.schedulerScreen->getPage(), true);
		screen.updateScreen();
	}
}

VARIOButtonScheduleur ButtonScheduleur;
