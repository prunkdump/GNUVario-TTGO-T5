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
 *                                                                               *
 *********************************************************************************/
 
#include "VarioButton.h"
#include <DebugConfig.h>
#include "esp_log.h"

#include <HardwareConfig.h>
#include <sdcardHAL.h>
#include <toneHAL.h>
#include <varioscreenGxEPD.h>
#ifdef HAVE_WIFI
#include <wifiServer.h>
#endif //HAVE_WIFI

void VARIOButton::begin() {
    
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

void VARIOButton::update() {
	//Button update
	BtnA.read();
	BtnB.read();
	BtnC.read();
}


void VARIOButton::setWakeupButton(uint8_t button) {
  _wakeupPin = button;
}

VARIOButton VarioButton;


void VARIOButtonScheduleur::update() {

  // put your main code here, to run repeatedly:
  if(VarioButton.BtnA.wasPressed()) {
#ifdef BUTTON_DEBUG
    SerialPort.printf("wasPressed A \r\n");
#endif //BUTTON_DEBUG
		treatmentBtnA(false);
  }

  if(VarioButton.BtnA.wasReleased()) {
#ifdef BUTTON_DEBUG
    SerialPort.printf("wasReleased A \r\n");
#endif //BUTTON_DEBUG
  }

  if(VarioButton.BtnA.pressedFor(2000)) {
#ifdef BUTTON_DEBUG
    SerialPort.printf("pressedFor 2s A \r\n");
#endif //BUTTON_DEBUG
  }

  if(VarioButton.BtnB.wasPressed()) {
#ifdef BUTTON_DEBUG
    SerialPort.printf("wasPressed B \r\n");
#endif //BUTTON_DEBUG
		treatmentBtnB(false);
  }

  if(VarioButton.BtnB.wasReleased()) {
#ifdef BUTTON_DEBUG
    SerialPort.printf("wasReleased B \r\n");
#endif //BUTTON_DEBUG
  }

  if(VarioButton.BtnB.pressedFor(2000)) {
#ifdef BUTTON_DEBUG
    SerialPort.printf("pressedFor 2s B \r\n");
#endif //BUTTON_DEBUG
  }

  if(VarioButton.BtnC.wasPressed()) {
#ifdef BUTTON_DEBUG
    SerialPort.printf("wasPressed C \r\n");
#endif //BUTTON_DEBUG
 		treatmentBtnC(false);
 }

  if(VarioButton.BtnC.wasReleased()) {
#ifdef BUTTON_DEBUG
    SerialPort.printf("wasReleased C \r\n");
#endif //BUTTON_DEBUG
  }

  if(VarioButton.BtnC.pressedFor(2000)) {
#ifdef BUTTON_DEBUG
    SerialPort.printf("pressedFor 2s C \r\n");
#endif //BUTTON_DEBUG
  }
  
  VarioButton.update();
}

void VARIOButtonScheduleur::Set_StatePage(uint8_t state) {
	StatePage = state;
}

File root;
		
void VARIOButtonScheduleur::treatmentBtnA(bool Debounce) {
#ifdef BUTTON_DEBUG
      SerialPort.println("Bouton A");
#endif //BUTTON_DEBUG

#ifdef HAVE_WIFI 
	if (StatePage == STATE_PAGE_INIT) {
		WifiServeur();	
  }
	else if (StatePage == STATE_PAGE_WEBSERV) {
		SerialPort.println("RESTART ESP32");
		SerialPort.flush();
		ESP_LOGI("GnuVario-E", "RESTART ESP32");
		ESP.restart();
	}
#endif //HAVE_WIFI
}

void VARIOButtonScheduleur::treatmentBtnB(bool Debounce) {
	
#ifdef BUTTON_DEBUG
    SerialPort.println("Mute");
#endif //BUTTON_DEBUG
	
  if (StatePage == STATE_PAGE_VARIO) {
		toneHAL.mute(!toneHAL.isMute());
		screen.volLevel->mute(toneHAL.isMute());
	}		
	
}

void VARIOButtonScheduleur::treatmentBtnC(bool Debounce) {
	
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
}

#ifdef HAVE_WIFI
void VARIOButtonScheduleur::WifiServeur(void) {
#ifdef BUTTON_DEBUG
    SerialPort.println("liste des fichiers");
#endif //BUTTON_DEBUG

    /* Begin at the root "/" */
    root = SDHAL.open("/");
    if (root) {    
      printDirectory(root, 0);
      root.close();
    } else {
#ifdef BUTTON_DEBUG
      SerialPort.println("error Sdcard directory");
#endif //BUTTON_DEBUG
    }    
#ifdef BUTTON_DEBUG
    SerialPort.println("done!");
#endif //BUTTON_DEBUG	

/*START SERVEUR WEB */
		wifiServer.begin();
		wifiServer.connect();

		wifiServer.start();

		Set_StatePage(STATE_PAGE_WEBSERV);
		
		while(1) {
		  wifiServer.handleClient(); 
			update();
		}
}
#endif //HAVE_WIFI

void VARIOButtonScheduleur::printDirectory(File dir, int numTabs) {
  
	dir.rewindDirectory();
//#ifdef BUTTON_DEBUG
  while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       SerialPort.print('\t');   // we'll have a nice indentation
    }
     // Print the name
     SerialPort.print(entry.name());
     /* Recurse for directories, otherwise print the file size */
     if (entry.isDirectory()) {
       SerialPort.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       /* files have sizes, directories do not */
       SerialPort.print("\t\t");
       SerialPort.println(entry.size());
     }
     entry.close();
   }
//#endif //BUTTON_DEBUG
}	 

VARIOButtonScheduleur ButtonScheduleur;
