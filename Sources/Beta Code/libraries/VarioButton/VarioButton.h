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
 *********************************************************************************
 *                                                                               *
 *                           VarioScreenGxEPD                                    *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0                                                                        *
 *    1.0.1  15/0/19    Ajout WifiServeur                                        *
 *                      Modification déclaration printDirectory                  *
 *    1.0.2  16/08/19   Ajout #ifdef HAVE_WIFI																	 *
 *    1.0.3  17/08/19   changement wasPressed par isPressed                      *
 *                      Correction de la fonction update                         *
 *    1.0.4  22/08/19   Ajout affichage reboot																	 *
 *    1.0.5  22/08/19   Ajout gestion changement de page                         *
 *    1.0.6  26/08/19   Ajout gestion page config sound                          *
 *                      Augmentation debounce time                               *
 *                      Ajout _state button																			 *
 *    1.0.7  31/08/19		Correction bug reglage son															 *
 *    1.0.8  25/09/19   Ajout appuie 3 sec bouton central                        * 
 *    1.0.9  29/09/19	  Ajout gestion page de calibration                        *
 *    1.0.10 03/10/19   Ajout HAVE_SDCARD                                        *
 *    1.0.11 28/10/19		Ajout bip de control lors de la configuration du volume  *
 *    1.0.12 01/11/19   Modification de la configuration du volume               *
 *    1.0.13 03/01/19   Déplacement de la validation de la mise en veille sur le *
 *                      bouton gauche                                            *
 *    1.0.14 29/11/19   Ajout arduinotrace                                       *
 *                      Modif sdfat                                              *
 *                                                                               *
 *********************************************************************************/
 
#ifndef _VARIOBUTTON_H_
#define _VARIOBUTTON_H_

#if defined(ESP32)
#include <Arduino.h>
#include <DebugConfig.h>
#include "HardwareConfig.h"
#include "Button.h"

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif //HAVE_SDCARD

#define DEBOUNCE_MS 30

class VARIOButton {

 public:
    void begin();
    void update();

    void setWakeupButton(uint8_t button);

    // Button API
    Button BtnA = Button(BUTTON_A_PIN, true, DEBOUNCE_MS);
    Button BtnB = Button(BUTTON_B_PIN, true, DEBOUNCE_MS);
    Button BtnC = Button(BUTTON_C_PIN, true, DEBOUNCE_MS);

    // SPEAKER

 private:
    uint8_t _wakeupPin;
};

#define STATE_PAGE_INIT    				0
#define STATE_PAGE_VARIO   				1
#define STATE_PAGE_CONFIG  				2
#define STATE_PAGE_STAT    				3
#define STATE_PAGE_GPSCAL  				4
#define STATE_PAGE_WEBSERV 				5
#define STATE_PAGE_CONFIG_SOUND		6
#define STATE_PAGE_DEEP_SLEEP			7
#define STATE_PAGE_CALIBRATION    8
#define STATE_PAGE_CALIBRATE			9

class VARIOButtonScheduleur {
	public:
    void update();
		void Set_StatePage(uint8_t state);
		uint8_t Get_StatePage(void);
		
	private:
		uint8_t StatePage = STATE_PAGE_INIT;
		bool    _stateBA = false;
		bool    _stateBB = false;
		bool    _stateBC = false;
		void treatmentBtnA(bool Debounce);
		void treatmentBtnB(bool Debounce);
		void treatmentBtnC(bool Debounce);
		
		void treatmentBtnB3S(bool Debounce);
		
#ifdef HAVE_WIFI
		void WifiServeur(void);
#endif //HAVE_WIFI

#ifdef HAVE_SDCARD
#ifdef SDFAT_LIB
		void printDirectory(SdFile dir, int numTabs);
#else //SDFAT_LIB
		void printDirectory(File dir, int numTabs);
#endif
#endif //HAVE_SDCARD
		
};

extern VARIOButton VarioButton;
extern VARIOButtonScheduleur ButtonScheduleur;

#else
#error “This library only supports boards with ESP32 processor.”
#endif

#endif
