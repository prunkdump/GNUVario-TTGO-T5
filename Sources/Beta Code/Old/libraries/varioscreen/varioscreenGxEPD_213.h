/* varioscreenGxEPD_213 -- 
 *
 * Copyright 2019 Jean-philippe GOI
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
 *                           VarioScreenGxEPD_213                                *
 *                               Ecran 2.13''                                    *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    22/10/19                                                            *
 *                                                                               *
 *********************************************************************************/

#ifndef VARIOSCREENGXEPD_213_H
#define VARIOSCREENGXEPD_213_H

#include <HardwareConfig.h>
#include <DebugConfig.h>

#if (VARIOSCREEN_SIZE == 213)

#include <varioscreenObjects_213.h>

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
  
  void displayStep(void);
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
	
/*	WIND* wind;
	BGLine* bgline1;
	BGLine* bgline2;
	BGLine* bgline3;
	BGLine* bgline4;
	BGCircle* bgcircle; */
	
//object page 10 - calibrate GPS 	
	
	ScreenDigit*  gpsPeriodDigit; 
	ScreenDigit*  gpsMeanPeriodDigit; 
	ScreenDigit*  gpsDurationDigit;
	
//object page 1  	
	
	ScreenDigit*  tempDigit; 	
	TUnit* tunit;
		
	
//  ScreenSchedulerObject* displayList;
	ScreenSchedulerObject displayList[30];  //17];
	ScreenScheduler* schedulerScreen; 
	uint8_t MaxObjectList = 0;
	
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
  void clearScreen(void); 

  void ScreenViewInit(uint8_t Version, uint8_t Sub_Version, String Author, uint8_t Beta_Code);
	void ScreenViewStat(void);
	void ScreenViewStatPage(int PageStat);
	void ScreenViewPage(int8_t page, boolean clear);
	void ScreenViewWifi(String SSID, String IP);
	void ScreenViewReboot(void);
  void ScreenViewSound(int volume);	
	void ScreenViewMessage(String message, int delai);
		
	void CreateObjectDisplay(int8_t ObjectDisplayTypeID, VarioScreenObject* object, int8_t page, int8_t multiDisplayID, boolean actif); 
	void updateData(int8_t ObjectDisplayTypeID, double data);
		
 /* void beginClear(void); //multi step clear
  bool clearStep(void); //return true while clearing*/
  
 private:
   unsigned long timerShow = 0;
//  uint8_t clearingStep;
};

extern VarioScreen screen;
extern volatile uint8_t stateDisplay;
extern GxEPD2_BW_U<GxEPD2_213, GxEPD2_213::HEIGHT> display;

#endif
#endif