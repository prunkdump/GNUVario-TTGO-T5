/* varioscreenGxEPD_291b -- 
 *
 * Copyright 2019 Jean-philippe GOI / Pierre FRANCIA
 * 
 * This file is part of GnuVario-E.
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
 *                           VarioScreenGxEPD_291                                *
 *                               Ecran 2,90''  portrait                          *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    24/09/19                                                            *
 *    1.0.1  28/09/19   Modification des noms des librairies                     *
 *    1.0.2  06/10/19   Mise à jour ratamuse                                     *
 *    1.0.3  13/10/19   Integration au GnuVario                                  *
 *                      Ajout Wind                                               *
 *    1.0.4  16/11/19   Modif updateScreen										 *
 *    1.0.5  11/01/20   Modif ScreenViewPage									 *
 *                      Modif VARIOSCREEN_SIZE == 290                            *
 *    1.0.6  17/01/20   Desactivation effacement ligne 1427                      *
 *    1.0.7  20/01/20   Modif ScreenViewReboot									 *
 *    1.0.8  28/01/20   Modification écran 1 - ajout info gps                    *
 *    1.0.9  03/02/20   changement de nom passage de 29 à 290                    *
 *    1.0.10 09/02/20   Modif écran 1 - font normal / coordonné GPS en degrés    *
 *    1.0.11 17/02/20   Ajout 2.90 et 2.91                                       *
 *                      Ajout FONTLARGE / FONTNORMAL                             *
 *    1.0.12 25/02/20   Ajout ScreenBackground                                   *
 *    1.0.13 05/03/20   Ajout affichage alti agl                                 *
 *    1.0.14 07/03/20   Correction xSemaphore                                    *
 *    1.0.15 09/03/20   Modification ScreenViewSound                             *
 *    1.0.16 08/04/20   Modification affichage des titres                        *
 *    1.1.0  29/04/20   Changement de font - repositionnement                    *
 *    1.1.1  10/05/20   Correction affichage screenTime (:/h)                    *
 *    1.1.2  11/05/20   Effacement zones multi                                   *
 *    1.1.3  14/05/20   Raffraichissement de l'écran toutes les 15min            *
 *    1.1.4  17/05/20   Ajout position titre avac setPositionTitle               *
 *	  1.1.5  23/05/20   Passage vario en -XX.X								  	 *
 *    1.1.6  27/07/20   Affichage de la batterie au démarrage                    *
 *    1.1.7  04/10/20   Modification position finesse                            *
 *                      Modification position titre finesse                      *
 *    1.1.8  19/10/20   Ajout ScreenViewBattery(boolean clear)                   *
 *    1.1.10 19/12/20   Modification affichage des titres P. FRANCIA             *
 *    1.2.11 10/02/20   Compatibilité écran 291 et 293                           *
 *                                                                               *
 *********************************************************************************/

#ifndef VARIOSCREENGXEPD_291B_H
#define VARIOSCREENGXEPD_291B_H

#include <Arduino.h>
#include <VarioSettings.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <HardwareConfig.h>
#include <DebugConfig.h>

#if ((VARIOSCREEN_SIZE == 291) || (VARIOSCREEN_SIZE == 293))

#include <varioscreenObjects_291b.h>

/* task parameters */
#define SCREEN_STACK_SIZE 2000
#define SCREEN_CORE 1
#define SCREEN_PRIORITY 10

#define MAX_CAR_TITRE_TIME 					15
#define MAX_CAR_TITRE_TDV 					15 
#define MAX_CAR_TITRE_ALTI 					8 
#define MAX_CAR_TITRE_AGL 					8 
#define MAX_CAR_TITRE_VARIO 				5
#define MAX_CAR_TITRE_SPEED					7
#define MAX_CAR_TITRE_FINESSE   		7
#define MAX_CAR_TITRE_TCHUTE				7 
#define MAX_CAR_TITRE_CAP					  3
#define MAX_CAR_TITRE_LAT					  3
#define MAX_CAR_TITRE_LONG 					4
#define MAX_CAR_TITRE_COMPAS 				6
#define MAX_CAR_TITRE_STAT          12
#define MAX_CAR_TITRE_DATE 					4
#define MAX_CAR_TITRE_HEURE					5
#define MAX_CAR_TITRE_DUREE 			  11
#define MAX_CAR_TITRE_VITESSE 			7
#define MAX_CAR_TITRE_CONNECT   	  10
#define MAX_CAR_TITRE_CONNECTA	  	13
#define MAX_CAR_TITRE_DEMAR     		9
#define MAX_CAR_TITRE_REDEMAR     	11
#define MAX_CAR_TITRE_ENCOURS 			11				
#define MAX_CAR_TITRE_CALIBR   			12
#define MAX_CAR_TITRE_VEILLE				14			
#define MAX_CAR_TITRE_CHARGE 				9
#define MAX_CAR_TITRE_CHARGER 			7
#define MAX_CAR_TITRE_BATTERIE 			8

/******************************/
/* The screen zone multi      */ 
/*----------------------------*/
class ScreenZoneMulti {
  public:
		uint8_t x, y, width, height, page;
		void update();  
};

/************************/
/* The screen scheduler */
/************************/
 
/* une negative page index for pages displayed once */
struct ScreenSchedulerObject {
  
  VarioScreenObject* object;
  int8_t page;
	int8_t multiDisplayID;
	int8_t ObjectDisplayTypeID;
  boolean actif;
};

/* the scheduler loop on 0 <--> endPage */
class ScreenScheduler {

 public:
 ScreenScheduler(ScreenSchedulerObject* displayList, uint8_t objectCount, int8_t startPage, int8_t endPage);
//   : displayList(displayList), objectCount(objectCount), pos(0), currentPage(startPage), endPage(endPage); // {};
  
  boolean displayStep(void);
  int8_t getPage(void);
  int8_t getMaxPage(void);
  void setPage(int8_t page, boolean forceUpdate = false);
  void nextPage(void);
  void previousPage(void);
	void enableShow(void);
	void disableShow(void);
	void displayAll(void);
	
 private:
  //const 
//  ScreenSchedulerObject* displayList = NULL;
  ScreenSchedulerObject* 	displayList;
  uint8_t 								objectCount;
  uint8_t 								pos;
  int8_t 									currentPage;
	int8_t 									currentMultiDisplay = 1;
  const int8_t 						endPage;
	bool   									IsDisplayAll = false;
	bool  									ShowDisplayAll = false;
	unsigned long						oldtimeAllDisplay = 0;
};

/*
  if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;
 
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
 
  }
*/

struct MultiDisplayObject {
  
  int8_t* objectList;
  const uint8_t objectCount;
  int8_t  displayActif; 
  int8_t  oldDisplayActif;
  uint8_t seconde;
  uint8_t countTime;
};

class MultiDisplay {
	
 public:

 MultiDisplay(ScreenSchedulerObject* displayList, uint8_t objectCount, MultiDisplayObject* multiDisplayList, uint8_t multiObjectCount)
   : displayList(displayList), objectCount(objectCount), multiDisplayList(multiDisplayList), multiObjectCount(multiObjectCount) { }
   
  void displayStep(void);
  
 private:
  ScreenSchedulerObject* displayList;
  const uint8_t objectCount;
  MultiDisplayObject* multiDisplayList;
  const uint8_t multiObjectCount;
  unsigned long lastFreqUpdate=0;  
};

/********************/
/* The screen class */
/********************/

class VarioScreen {
 public:

  ScreenDigit* tensionDigit; 
	ScreenDigit* tempratureDigit; 
	
	ScreenDigit* altiDigit;
	ScreenDigit* heightDigit;
	
	MUnit* munit;
	ScreenDigit* varioDigit;
	MSUnit* msunit;
	KMHUnit* kmhunit;
	ScreenDigit* speedDigit;
	ScreenDigit* ratioDigit;
	ScreenDigit* trendDigit;

	INFOLevel* infoLevel;
	VOLLevel* volLevel;
	RECORDIndicator* recordIndicator;
	TRENDLevel* trendLevel;

	BATLevel* batLevel;

	SATLevel* satLevel;

	ScreenDigit* timeHDigit;
	ScreenDigit* timeMDigit;

	ScreenTime* screenTime;
	ScreenElapsedTime* screenElapsedTime;

	FIXGPSInfo* fixgpsinfo;
	BTInfo* btinfo;
	
	WIND* wind;

	BGLine* bgline1;
	BGLine* bgline2;
	BGLine* bgline3;
	BGLine* bgline4;
	
//	BGCircle* bgcircle; 

//object page 10 - calibrate GPS 	
	
	ScreenDigit*  gpsPeriodDigit; 
	ScreenDigit*  gpsMeanPeriodDigit; 
	ScreenDigit*  gpsDurationDigit;
	
//object page 1  	
	
//	ScreenDigit*  tempDigit; 	
//	TUnit* tunit;
//	ScreenText*   gpsLatDir;
//	ScreenText*   gpsLongDir;
	ScreenText*   gpsBearingText;
//	ScreenDigit*  gpsLat; 
//	ScreenDigit*  gpsLong; 
	ScreenDigit*  gpsBearing;
	ScreenText*   gpsLat;
	ScreenText*   gpsLong;
	
	ScreenText*   bearingText;
	ScreenDigit*  bearing;
	
//  ScreenSchedulerObject* displayList;
	ScreenSchedulerObject displayList[40];  //17];
	ScreenZoneMulti				ZoneMultiList[5];

	ScreenScheduler* schedulerScreen; 
	uint8_t MaxObjectList = 0;
	uint8_t MaxZoneList   = 0;
	
	virtual ~VarioScreen();

  void init(void);
  void createScreenObjects(void);
	void createScreenObjectsPage0(void);
	void createScreenObjectsDisplayPage0(void);
	void createScreenObjectsPage1(void);
	void createScreenObjectsDisplayPage1(void);
	void createScreenObjectsPage10(void);
	void createScreenObjectsDisplayPage10(void);

  void begin(void);
	
//  void  getTextBounds(char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
  void updateScreen (void);
  void updateScreenNB (void);
  void clearScreen(void); 

  void ScreenViewInit(uint8_t Version, uint8_t Sub_Version, String Author, uint8_t Beta_Code);
	void ScreenViewStat(void);
	void ScreenViewStatPage(int PageStat);
	void ScreenViewPage(int8_t page, boolean clear, boolean refresh = false);
	void ScreenViewWifi(String SSID, String IP);
	void ScreenViewReboot(String message = "");
  boolean ScreenViewSound(void);	
	void SetViewSound(int volume);	
	void ScreenViewMessage(String message, int delai);
	void ScreenBackground(int8_t page);
	void ScreenViewBattery(boolean clear);
		
	void CreateObjectDisplay(int8_t ObjectDisplayTypeID, VarioScreenObject* object, int8_t page, int8_t multiDisplayID, boolean actif); 
	void updateData(int8_t ObjectDisplayTypeID, double data);
		
 /* void beginClear(void); //multi step clear
  bool clearStep(void); //return true while clearing*/

	 static SemaphoreHandle_t screenMutex;
  
 private:
   unsigned long timerShow = 0;
   static uint8_t volatile status;
   static TaskHandle_t screenTaskHandler;
   static void screenTask(void* param);
	 int    viewSound;

//  uint8_t clearingStep;
};

extern VarioScreen screen;
extern volatile uint8_t stateDisplay;

#if (VARIOSCREEN_SIZE == 291)
extern GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display;
#else
extern GxEPD2_BW<GxEPD2_290_T94, GxEPD2_290_T94::HEIGHT> display;
#endif 

#endif
#endif