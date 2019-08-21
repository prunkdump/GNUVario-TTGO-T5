/* varioscreenGxEPD -- 
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
 *                           VarioScreenGxEPD                                    *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    20/06/19                                                            *
 *    1.0.1  25/06/19   Correction affichage batterie / affichage vitesse        *
 *    1.0.2  20/07/19   Correction bug d'affiche screedigit                      *
 *    1.0.3  21/07/19   Correction affichage statistique                         *
 *    1.0.4  23/07/19   Ajout trendDigit                                         *
 *                      Modification ratioDigit / trendDigit                     *
 *    1.0.5  06/08/19   Ajout icon noRecord                                      *
 *                      Ajout raffrachissement ALL toutes les 30 secs            *
 *    1.0.6  12/08/19   Ajout gestion écran de config GPS                        *
 *    1.0.7  15/08/19   Ajout gestion bouton dans screeninit                     *
 *    1.0.8  15/09/19   Ajout écran connection Wifi - ScreenViewWifi             *
 *                                                                               *
 *********************************************************************************/

#ifndef VARIOSCREENGXEPD_H
#define VARIOSCREENGXEPD_H

#include <Arduino.h>
#include <digit.h>
#include <variostat.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

/* minimum drift to update digit */
#define VARIOSCREEN_DIGIT_DISPLAY_THRESHOLD 0.65

/*  lipo battery percentage to voltage linear interpolation */
/*    voltage = a * percent + b                             */ 
#define VARIOSCREEN_BAT_A_COEFF 0.0059
#define VARIOSCREEN_BAT_B_COEFF 3.5534
#define VARIOSCREEN_BAT_PIXEL_COUNT 10
#define VARIOSCREEN_BAT_MULTIPLIER 6

#define ColorScreen    GxEPD_BLACK
#define ColorText      GxEPD_WHITE

#define STATE_OK									0x00
#define STATE_BUSY                0x01
#define STATE_BUSYMEMORY					0x01
#define STATE_BUSYUPDATE					0x02

#define DISPLAY_OBJECT_VARIO 								 1
#define DISPLAY_OBJECT_TIME									 2
#define DISPLAY_OBJECT_TENSION							 3
#define DISPLAY_OBJECT_TEMPRATURE						 4
#define DISPLAY_OBJECT_SPEED								 5
#define DISPLAY_OBJECT_ALTI									 6
#define DISPLAY_OBJECT_DURATION							 7
#define DISPLAY_OBJECT_MUNIT        				 8
#define DISPLAY_OBJECT_MSUNIT       				 9
#define DISPLAY_OBJECT_KMHUNIT     					10
#define DISPLAY_OBJECT_RATIO       					11
#define DISPLAY_OBJECT_INFOLEVEL   					12
#define DISPLAY_OBJECT_VOLLEVEL    					13
#define DISPLAY_OBJECT_RECORDIND	 					14
#define DISPLAY_OBJECT_TRENDLEVEL  					15
#define DISPLAY_OBJECT_BATLEVEL    					16
#define DISPLAY_OBJECT_SATLEVEL    					17
#define DISPLAY_OBJECT_FIXGPSINFO  					18
#define DISPLAY_OBJECT_BTINFO      					19
#define DISPLAY_OBJECT_SCREENTIME           20
#define DISPLAY_OBJECT_SCREENELAPSEDTIME    21
#define DISPLAY_OBJECT_TREND      					22
#define DISPLAY_OBJECT_GPS_PERIOD           23
#define DISPLAY_OBJECT_GPS_MEAN_PERIOD			24
#define DISPLAY_OBJECT_GPS_DURATION					25

#include <VarioSettings.h>
extern VarioSettings GnuSettings;

template<typename GxEPD2_Type, const uint16_t page_height>
class GxEPD2_BW_U : public GxEPD2_BW<GxEPD2_Type, page_height>
{
  public:
    GxEPD2_BW_U(GxEPD2_Type epd2_instance) : GxEPD2_BW<GxEPD2_Type, page_height>(epd2_instance){}
		
		unsigned int GetState(void);
	//	void updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation = true);
//    void displayScreen(bool partial_update_mode = false);
		
/*		void charWidth(char c, uint16_t *width, uint16_t *height);
		void getTextWidth(const char *str, uint16_t *w, uint16_t *h);
		void getTextWidth(const __FlashStringHelper *s, uint16_t *w, uint16_t *h);
		void getTextWidth(const String &str, uint16_t *w, uint16_t *h);*/
		
	private:
    unsigned int state = STATE_OK;

};


/* the main abstract class for all the objects */ 
/*----------------------------*/
class VarioScreenObject {

 public:
  bool update(bool force = false);  //return true if an update was done
  void reset(void);   //force redisplay 
  virtual void show(void) = 0;
//	virtual void setValue(double value) = 0;
  
 protected:
  VarioScreenObject(uint8_t state) : state(state) { }
  uint8_t state; //the first byte is used to know is redisplay is needed
                 //the other can be used freely	
};

/* screen digit */

#define ALIGNNONE  0
#define ALIGNSPACE 1
#define ALIGNZERO  2

#define ALIGNLEFT  true
#define ALIGNRIGHT false

class ScreenDigit: public VarioScreenObject {

 public :
   ScreenDigit(uint16_t anchorX, uint16_t anchorY, uint16_t width, uint16_t precision, boolean plusDisplay = false, boolean zero = false, boolean leftAlign = false);
 //  : VarioScreenObject(0), anchorX(anchorX), anchorY(anchorY), width(width), precision(precision), plusDisplay(plusDisplay), zero(zero), leftAlign(leftAlign)
 // { lastDisplayWidth = 0; }
  void show(void);
  void setValue(double value);
   
 private:
  int digitsBe4Decimal(double number);
  char * dtostrf2(double number, signed char width, unsigned char prec, char *s, boolean zero);
  double value;
  double oldvalue=-1;
  const uint16_t anchorX, anchorY, precision, width;
  boolean plusDisplay, zero, leftAlign;
  uint8_t lastDisplayWidth;
	int16_t oldw=0, oldh=0, oldx=0, oldy=0;
	uint16_t Zwidth, Zheight;
};

/* meters unit */
class MUnit : public VarioScreenObject {

 public :
  MUnit(uint8_t posX, uint8_t posY)
   : VarioScreenObject(1), posX(posX), posY(posY) { }
  void show(void);
	void toDisplay(void);
  
 private :
  const uint8_t posX;
  const uint8_t posY;
};

/* meters per second unit */
class MSUnit : public VarioScreenObject {

 public :
 MSUnit(uint8_t posX, uint8_t posY)
   : VarioScreenObject(1), posX(posX), posY(posY) { }
  void show(void);
	void toDisplay(void);
  
 private :
  const uint8_t posX;
  const uint8_t posY;
};

/* kilometers per hour unit */
class KMHUnit : public VarioScreenObject {

 public :
  KMHUnit(uint8_t posX, uint8_t posY)
   : VarioScreenObject(1), posX(posX), posY(posY) { }
  void show(void);
	void toDisplay(void);

 private :
  const uint8_t posX;
  const uint8_t posY;
};

/* battery level */
class BATLevel : public VarioScreenObject {

 public :
 BATLevel(uint8_t posX, uint8_t posY, double divisor, double refVoltage)
   : VarioScreenObject(0), posX(posX), posY(posY),
    base((uint16_t)((VARIOSCREEN_BAT_B_COEFF * VOLTAGE_RESOLUTION * (1<<VARIOSCREEN_BAT_MULTIPLIER))/(divisor*refVoltage))),
    inc((uint16_t)((VARIOSCREEN_BAT_A_COEFF * VOLTAGE_RESOLUTION * 100 * (1<<VARIOSCREEN_BAT_MULTIPLIER))/(divisor*refVoltage*(VARIOSCREEN_BAT_PIXEL_COUNT+1))))
    { }


  void setVoltage(int voltage = 0);
  void show(void);

 private :
  uint16_t Voltage;
	float	   uVoltage;
	uint8_t  pVoltage;
  const uint8_t posX;
  const uint8_t posY;
  const uint16_t base;
  const uint16_t inc;
};

/* battery level */
class VOLLevel : public VarioScreenObject {

 public :
 VOLLevel(uint8_t posX, uint8_t posY)
   : VarioScreenObject(0), posX(posX), posY(posY)   { }

  void setVolume(int Volume = 0);
	void mute(bool newMuteState);

  void show(void);

 private :
  uint8_t volume=0;
  const uint8_t posX;
  const uint8_t posY;
	bool _mute = false;
};

/* satellite level */
class SATLevel : public VarioScreenObject {

 public:
 SATLevel(uint8_t posX, uint8_t posY)
   : VarioScreenObject(0), posX(posX), posY(posY) { }

  void setSatelliteCount(uint8_t count);
  void show(void);
  
 private:
  const uint8_t posX;
  const uint8_t posY;
  uint8_t satelliteCount;
};


/* info level */

#define INFO_NONE  0
#define INFO_USB   1


class INFOLevel : public VarioScreenObject {

 public:
 INFOLevel(uint8_t posX, uint8_t posY)
   : VarioScreenObject(0), posX(posX), posY(posY) { }

  void set(uint8_t value);
  void show(void);
  
 private:
  const uint8_t posX;
  const uint8_t posY;
  uint8_t InfoValue = INFO_USB;
};

/* record indicator */

#define STATE_SCAN   		0
#define STATE_RECORD 		1
#define STATE_GPSFIX 		2
#define STATE_NORECORD 	3


class RECORDIndicator : public VarioScreenObject {

 public:
 RECORDIndicator(uint8_t posX, uint8_t posY)
   : VarioScreenObject(0), posX(posX), posY(posY) { }

  void stateRECORD(void);
  void setActifSCAN(void);
  void setActifRECORD(void);
  void setActifGPSFIX(void);
	void setNoRECORD(void); 
  void show(void);
  
 private:
  const uint8_t posX;
  const uint8_t posY;
  bool  displayRecord = false;
  uint8_t  recordState = STATE_SCAN;
  unsigned long lastFreqUpdate=0;
};


/* trend */
class TRENDLevel : public VarioScreenObject {

 public :
  TRENDLevel(uint8_t posX, uint8_t posY)
   : VarioScreenObject(1), posX(posX), posY(posY) { }
  void stateTREND(int8_t state);
  void show(void);

 private :
  const uint8_t posX;
  const uint8_t posY;
  int8_t trendState = 0;
};

/* time */
class ScreenTime : public VarioScreenObject {

 public:
  ScreenTime(uint8_t posX, uint8_t posY, ScreenDigit& hour, ScreenDigit& minute, bool dot_or_h = false)
    : VarioScreenObject(0), posX(posX), posY(posY), hour(hour), minute(minute), dot_or_h(dot_or_h) { }

  void setTime(uint32_t newTime);
  void setTime(int8_t* newTime);
  void correctTimeZone(int8_t UTCDrift);
  int8_t* getTime(void);
  void show(void);

 protected:
  const uint8_t posX;
  const uint8_t posY;
  int8_t time[3];
  bool dot_or_h = false;
  ScreenDigit& hour, minute;
};


class ScreenElapsedTime : public ScreenTime {

 public:
  ScreenElapsedTime(uint8_t posX, uint8_t posY, ScreenDigit& hour, ScreenDigit& minute) 
  : ScreenTime(posX, posY, hour, minute, true) { }
  
  void setBaseTime(int8_t* time);
  void setCurrentTime(int8_t* time);

 protected:
  int8_t baseTime[3];
  
};

class FIXGPSInfo : public VarioScreenObject {

 public :
  FIXGPSInfo(uint8_t posX, uint8_t posY)
   : VarioScreenObject(1), posX(posX), posY(posY) { }
  void setFixGps(void);
  void unsetFixGps(void);
  boolean getFixGps(void);

  void show(void);

 private :
  boolean FixGPS = false;
  const uint8_t posX;
  const uint8_t posY;
};

class BTInfo : public VarioScreenObject {

 public :
  BTInfo(uint8_t posX, uint8_t posY)
   : VarioScreenObject(1), posX(posX), posY(posY) { }

  void setBT(void);
  void unsetBT(void);
  boolean getBT(void);

  void show(void);

 private :
  boolean bt = false;
  const uint8_t posX;
  const uint8_t posY;
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
	
//object page 10 - calibrate GPS 	
	
	ScreenDigit*  gpsPeriodDigit; 
	ScreenDigit*  gpsMeanPeriodDigit; 
	ScreenDigit*  gpsDurationDigit;
	
	
//  ScreenSchedulerObject* displayList;
	ScreenSchedulerObject displayList[30];  //17];
	ScreenScheduler* schedulerScreen; 
	uint8_t MaxObjectList = 0;
	
	virtual ~VarioScreen();

  void init(void);
  void createScreenObjects(void);
	void createScreenObjectsPage0(void);
	void createScreenObjectsDisplayPage0(void);
	void createScreenObjectsPage10(void);
	void createScreenObjectsDisplayPage10(void);

  void begin(void);
	
//  void  getTextBounds(char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
  void updateScreen (void);
  void clearScreen(void); 

  void ScreenViewInit(uint8_t Version, uint8_t Sub_Version, String Author, uint8_t Beta_Code);
	void ScreenViewStat(VarioStat flystat);
	void ScreenViewPage(int8_t page, boolean clear);
	void ScreenViewWifi(String SSID, String IP);
		
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

#endif