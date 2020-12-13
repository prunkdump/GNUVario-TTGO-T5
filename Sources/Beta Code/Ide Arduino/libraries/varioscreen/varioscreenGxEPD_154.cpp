/* varioscreenGxEPD_154 -- 
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
 *********************************************************************************
 *                                                                               *
 *                           VarioScreenGxEPD_154                                *
 *                               Ecran 1.54''                                    *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    24/09/19                                                            *
 *    1.0.1  28/09/19   Modification des noms des librairies                     *
 *    1.0.2  09/10/19   Ajout showtitle                                          *
 *    1.0.3	 13/10/19		Ajout Wind																							 *	
 *    1.0.4  07/11/19   Modif updateScreen																			 *
 *    1.0.5  11/01/20   Modif ScreenViewPage																		 *
 *                      Modif effacement de zone +1 à gauche et +3 pour vitesse  *
 *    1.0.6  17/01/20   Desactivation effacement ligne 1427                      *
 *    1.0.7  20/01/20   Modif ScreenViewReboot																	 *
 *    1.0.8  28/01/20   Modification écran 1 - ajout info gps                    *
 *    1.0.9  09/02/20   Modif écran 1 - font normal / coordonné GPS en degrés    *
 *    1.0.10 17/02/20   Ajout large (font) varioscreenDigit                      *
 *    1.0.11 25/02/20   Ajout ScreenBackground                                   *
 *    1.0.12 04/03/20   Ajout affichage alti agl                                 *
 *    1.0.13 07/03/20   Correction xSemaphore                                    *
 *    1.0.14 09/03/20   Modification ScreenViewSound                             *
 *    1.0.15 08/04/20   Modification affichage des titres                        *
 *    1.0.16 08/04/20   Modification affichage des titres                        *
 *    1.1.0  29/04/20   Changement de font - repositionnement                    *
 *    1.1.1  10/05/20   Correction affichage screenTime (:/h)                    *
 *    1.1.2  11/05/20   Effacement zones multi                                   *
 *    1.1.3  17/05/20   Ajout position titre avac setPositionTitle               *
 *		1.1.4  23/05/20   Passage vario en -XX.X								  								 *
 *    1.1.5  27/07/20   Affichage de la batterie au démarrage                    *
 *    1.1.6  27/09/20   Ajout DISPLAY_LIGHT                                      *
 *                                                                               *
 *********************************************************************************/ 


 /*
 *********************************************************************************
 *                    conversion image to cpp code                               *
 *                                                                               *
 *      https://javl.github.io/image2cpp/                                        *
 *                                                                               *
 *********************************************************************************/

#include <HardwareConfig.h>
#include <DebugConfig.h>

#if (VARIOSCREEN_SIZE == 154)

#include <varioscreenObjects_154.h>

#include <varioscreenGxEPD_154.h>
#include <Arduino.h>

#if defined(ESP32)
//static const char* TAG = "VarioScreen";
#include "esp_log.h"
#endif //ESP32

//#include <avr\dtostrf.h>
#include <stdlib.h>

/* http://javl.github.io/image2cpp/ */

#include <VarioButton.h>

#include <imglib/gridicons_sync.h>
#include <varioscreenIcone_154.h>

#include <VarioSettings.h>
#include <toneHAL.h>

#include <Utility.h>

// #include <SysCall.h>

#include <VarioData.h>
#include <AglManager.h>

#include <VarioLanguage.h>

#include <VarioHardwareManager.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
  #include <pgmspace.h>
#endif

// Many (but maybe not all) non-AVR board installs define macros
// for compatibility with existing PROGMEM-reading AVR code.
// Do our own checks and defines here for good measure...

#ifndef pgm_read_byte
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
 #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
 #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
 #define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
 #define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

volatile uint8_t stateDisplay;
volatile unsigned long oldtime;
volatile uint8_t led1stat = 0; 
volatile uint8_t stateMulti = 0;

/*********************/
/* static class data */
/*********************/
SemaphoreHandle_t VarioScreen::screenMutex;
TaskHandle_t VarioScreen::screenTaskHandler;

#define VARIOSCREEN_DOT_WIDTH 6
#define VARIOSCREEN_DIGIT_WIDTH 11

#ifndef ColorScreen
#define ColorScreen    GxEPD_WHITE
#endif

#ifndef ColorText
#define ColorText      GxEPD_BLACK
#endif

#define VARIOSCREEN_TENSION_ANCHOR_X 140
#define VARIOSCREEN_TENSION_ANCHOR_Y 150

#define VARIOSCREEN_AUTONOMIE_ANCHOR_X 90
#define VARIOSCREEN_AUTONOMIE_ANCHOR_Y 235

/*****************************************/
/* screen objets Page 0                  */
/*****************************************/

#define VARIOSCREEN_ALTI_ANCHOR_X 100  //110
#define VARIOSCREEN_ALTI_ANCHOR_Y 80
#define VARIOSCREEN_ALTI_UNIT_ANCHOR_X    100 //110
#define VARIOSCREEN_VARIO_ANCHOR_X 90 //98//#define VARIOSCREEN_VARIO_ANCHOR_X 5
#define VARIOSCREEN_VARIO_ANCHOR_Y 135
#define VARIOSCREEN_VARIO_UNIT_ANCHOR_X   92 //100
#define VARIOSCREEN_VARIO_UNIT_ANCHOR_Y   110
#define VARIOSCREEN_SPEED_ANCHOR_X 45 //49
#define VARIOSCREEN_SPEED_ANCHOR_Y 190
#define VARIOSCREEN_SPEED_UNIT_ANCHOR_X 51
#define VARIOSCREEN_SPEED_UNIT_ANCHOR_Y 165
#define VARIOSCREEN_GR_ANCHOR_X 120
#define VARIOSCREEN_GR_ANCHOR_Y 135
#define VARIOSCREEN_INFO_ANCHOR_X 17
#define VARIOSCREEN_INFO_ANCHOR_Y 0
#define VARIOSCREEN_VOL_ANCHOR_X 44
#define VARIOSCREEN_VOL_ANCHOR_Y 0
#define VARIOSCREEN_RECCORD_ANCHOR_X 164 //84
#define VARIOSCREEN_RECCORD_ANCHOR_Y 0
#define VARIOSCREEN_BAT_ANCHOR_X 84 //124
#define VARIOSCREEN_BAT_ANCHOR_Y 0
#define VARIOSCREEN_SAT_ANCHOR_X 124 //164
#define VARIOSCREEN_SAT_ANCHOR_Y 0
#define VARIOSCREEN_SAT_FIX_ANCHOR_X 176
#define VARIOSCREEN_SAT_FIX_ANCHOR_Y 38    //32
#define VARIOSCREEN_TIME_ANCHOR_X 198
#define VARIOSCREEN_TIME_ANCHOR_Y 190
#define VARIOSCREEN_ELAPSED_TIME_ANCHOR_X 198
#define VARIOSCREEN_ELAPSED_TIME_ANCHOR_Y 190
#define VARIOSCREEN_BT_ANCHOR_X 0
#define VARIOSCREEN_BT_ANCHOR_Y 4
#define VARIOSCREEN_TREND_ANCHOR_X 147 //57 //120
#define VARIOSCREEN_TREND_ANCHOR_Y 52  //111
#define VARIOSCREEN_WIND_ANCHOR_X 200
#define VARIOSCREEN_WIND_ANCHOR_Y 33

#define VARIOSCREEN_ALTI_TITLE_X 5  
#define VARIOSCREEN_ALTI_TITLE_Y 40
#define VARIOSCREEN_VARIO_TITLE_X 5
#define VARIOSCREEN_VARIO_TITLE_Y 95
#define VARIOSCREEN_SPEED_TITLE_X 5
#define VARIOSCREEN_SPEED_TITLE_Y 150
#define VARIOSCREEN_GR_TITLE_X 120
#define VARIOSCREEN_GR_TITLE_Y 95
#define VARIOSCREEN_TIME_TITLE_X 80
#define VARIOSCREEN_TIME_TITLE_Y 150
#define VARIOSCREEN_ELAPSED_TIME_TITLE_X 80
#define VARIOSCREEN_ELAPSED_TIME_TITLE_Y 150

/*****************************************/
/* screen objets Page 1                  */
/*****************************************/
//#define VARIOSCREEN_TEMP_ANCHOR_X 115
//#define VARIOSCREEN_TEMP_ANCHOR_Y 190
//#define VARIOSCREEN_TEMP_UNIT_ANCHOR_X 170
#define VARIOSCREEN_LONG_ANCHOR_X 10 //150
//#define VARIOSCREEN_LONGDIR_ANCHOR_X 165
#define VARIOSCREEN_LONG_ANCHOR_Y 135 //90
#define VARIOSCREEN_LAT_ANCHOR_X 10 //150
//#define VARIOSCREEN_LATDIR_ANCHOR_X 165
#define VARIOSCREEN_LAT_ANCHOR_Y 85 //135
#define VARIOSCREEN_BEARING_TEXT_ANCHOR_X 10
#define VARIOSCREEN_BEARING_TEXT_ANCHOR_Y 190
#define VARIOSCREEN_BEARING_ANCHOR_X 190 //115
#define VARIOSCREEN_BEARING_ANCHOR_Y 192

#define VARIOSCREEN_LAT_TITLE_X 5
#define VARIOSCREEN_LAT_TITLE_Y 52
#define VARIOSCREEN_LONG_TITLE_X 5
#define VARIOSCREEN_LONG_TITLE_Y 102
#define VARIOSCREEN_BEARING_TEXT_TITLE_X 5
#define VARIOSCREEN_BEARING_TEXT_TITLE_Y 152
#define VARIOSCREEN_BEARING_TITLE_X 120
#define VARIOSCREEN_BEARING_TITLE_Y 152

/*****************************************/
/* screen objets Page 10 - Calibrate GPS */
/*****************************************/
#define VARIOSCREEN_GPS_PERIOD_ANCHOR_X 190
#define VARIOSCREEN_GPS_PERIOD_ANCHOR_Y 80
#define VARIOSCREEN_GPS_MEAN_PERIOD_ANCHOR_X 190
#define VARIOSCREEN_GPS_MEAN_PERIOD_ANCHOR_Y 120
#define VARIOSCREEN_GPS_DURATION_ANCHOR_X 190
#define VARIOSCREEN_GPS_DURATION_ANCHOR_Y 160

VarioScreen screen;

/**********/
/* screen */
/**********/

//****************************************************************************************************************************
//****************************************************************************************************************************
//								VARIOSCREEN
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
VarioScreen::~VarioScreen() {
//****************************************************************************************************************************
//	free(tensionDigit);
	free(displayList);
	free(schedulerScreen);
}

#define ITEMS_IN_ARRAY(array)   (sizeof(array) / sizeof(*array))

//****************************************************************************************************************************
void VarioScreen::init(void)
//****************************************************************************************************************************
{
#ifdef SCREEN_DEBUG
	SerialPort.println("init");	
#endif //SCREEN_DEBUG

  display.init(0);   ///115200);  pour affichage debug Screen GxEPD2

//  setRotation(2);

#ifdef SCREEN_DEBUG
	SerialPort.println("fillScreen");	
#endif //SCREEN_DEBUG

  display.setFullWindow();

  display.fillScreen(GxEPD_WHITE);
	
#ifdef SCREEN_DEBUG
	SerialPort.println("setTextColor");	
#endif //SCREEN_DEBUG
	
  display.setTextColor(GxEPD_BLACK);
	
  screenMutex = xSemaphoreCreateBinary();
  xSemaphoreGive(screenMutex);

#ifdef SCREEN_DEBUG2
		SerialPort.println("int : Give");	
#endif //SCREEN_DEBUG

  xTaskCreatePinnedToCore(screenTask, "TaskDisplay", SCREEN_STACK_SIZE, NULL, SCREEN_PRIORITY, &screenTaskHandler,SCREEN_CORE);
	
}
	
//****************************************************************************************************************************
void VarioScreen::createScreenObjects(void)
//****************************************************************************************************************************
{
	
#ifdef SCREEN_DEBUG
	SerialPort.println("Create screen objects");	
#endif //SCREEN_DEBUG
	
	/* création des champs d'affichage */
	
	
//	ScreenDigit tensionDigit(TENSION_DISPLAY_POSX /*VARIOSCREEN_TENSION_ANCHOR_X*/, VARIOSCREEN_TENSION_ANCHOR_Y, 5, 2, false, false, ALIGNRIGHT);

/*	tensionDigit = new ScreenDigit(VARIOSCREEN_TENSION_ANCHOR_X, VARIOSCREEN_TENSION_ANCHOR_Y, 5, 2, false, false, ALIGNRIGHT);	
	tempratureDigit = new ScreenDigit(VARIOSCREEN_TENSION_ANCHOR_X, VARIOSCREEN_TENSION_ANCHOR_Y, 5, 2, false, false, ALIGNRIGHT);*/

	MaxZoneList = 0;
	
  MaxZoneList++;
		
	ZoneMultiList[MaxZoneList-1].x			= VARIOSCREEN_GR_ANCHOR_X-4;
	ZoneMultiList[MaxZoneList-1].y   		= VARIOSCREEN_GR_ANCHOR_Y-42;
	ZoneMultiList[MaxZoneList-1].width 	= 65;
	ZoneMultiList[MaxZoneList-1].height	= 49;
	ZoneMultiList[MaxZoneList-1].page   = 0;
	
#ifdef DISPLAY_LIGHT
  createScreenObjectsPageLight();
#else
	createScreenObjectsPage0();
#endif
	createScreenObjectsPage1();
//	createScreenObjectsPage10();
	
//	displayList  = new ScreenSchedulerObject[3];
	MaxObjectList = 0;
	
#ifdef SCREEN_DEBUG
	SerialPort.println("schedulerScreen : createObjectDisplay");	
#endif //SCREEN_DEBUG

#ifdef DISPLAY_LIGHT	
	createScreenObjectsDisplayPageLight();
#else	
	createScreenObjectsDisplayPage0();
#endif
	createScreenObjectsDisplayPage1();

//	createScreenObjectsDisplayPage10();
}	

#define TAILLE_FONT FONTNORMAL
	
#ifndef DISPLAY_LIGHT	
//****************************************************************************************************************************
void VarioScreen::createScreenObjectsPage0(void) {
//****************************************************************************************************************************
	altiDigit = new ScreenDigit(VARIOSCREEN_ALTI_ANCHOR_X, VARIOSCREEN_ALTI_ANCHOR_Y, 4, 0, false, false, ALIGNRIGHT, true, DISPLAY_OBJECT_ALTI, TAILLE_FONT, MAX_CAR_TITRE_ALTI);
  altiDigit->setPositionTitle(VARIOSCREEN_ALTI_TITLE_X,VARIOSCREEN_ALTI_TITLE_Y);
	heightDigit = new ScreenDigit(VARIOSCREEN_ALTI_ANCHOR_X, VARIOSCREEN_ALTI_ANCHOR_Y, 4, 0, false, false, ALIGNRIGHT, true, DISPLAY_OBJECT_HEIGHT, TAILLE_FONT, MAX_CAR_TITRE_AGL);
  heightDigit->setPositionTitle(VARIOSCREEN_ALTI_TITLE_X,VARIOSCREEN_ALTI_TITLE_Y);
	munit = new MUnit(VARIOSCREEN_ALTI_UNIT_ANCHOR_X, VARIOSCREEN_ALTI_ANCHOR_Y);

	trendLevel = new TRENDLevel(VARIOSCREEN_TREND_ANCHOR_X, VARIOSCREEN_TREND_ANCHOR_Y);
	fixgpsinfo = new FIXGPSInfo(VARIOSCREEN_SAT_FIX_ANCHOR_X, VARIOSCREEN_SAT_FIX_ANCHOR_Y);

	varioDigit = new ScreenDigit(VARIOSCREEN_VARIO_ANCHOR_X, VARIOSCREEN_VARIO_ANCHOR_Y, 4, 1, true, false,  ALIGNRIGHT, true, DISPLAY_OBJECT_VARIO, TAILLE_FONT, MAX_CAR_TITRE_VARIO);
  varioDigit->setPositionTitle(VARIOSCREEN_VARIO_TITLE_X,VARIOSCREEN_VARIO_TITLE_Y);
//	varioDigit = new ScreenDigit(VARIOSCREEN_VARIO_ANCHOR_X, VARIOSCREEN_VARIO_ANCHOR_Y, 4, 1, true, false,  ALIGNLEFT, true, DISPLAY_OBJECT_VARIO, TAILLE_FONT);
	msunit = new MSUnit(VARIOSCREEN_VARIO_UNIT_ANCHOR_X, VARIOSCREEN_VARIO_UNIT_ANCHOR_Y);

	speedDigit = new ScreenDigit(VARIOSCREEN_SPEED_ANCHOR_X, VARIOSCREEN_SPEED_ANCHOR_Y, 2, 0, false, false, ALIGNRIGHT, true, DISPLAY_OBJECT_SPEED, TAILLE_FONT, MAX_CAR_TITRE_SPEED);
	speedDigit->setPositionTitle(VARIOSCREEN_SPEED_TITLE_X,VARIOSCREEN_SPEED_TITLE_Y);
	kmhunit = new KMHUnit(VARIOSCREEN_SPEED_UNIT_ANCHOR_X, VARIOSCREEN_SPEED_UNIT_ANCHOR_Y);

	ratioDigit = new ScreenDigit(VARIOSCREEN_GR_ANCHOR_X, VARIOSCREEN_GR_ANCHOR_Y, 2, 0, false, true, ALIGNLEFT, true, DISPLAY_OBJECT_RATIO, TAILLE_FONT, MAX_CAR_TITRE_TCHUTE);
	ratioDigit->setPositionTitle(VARIOSCREEN_GR_TITLE_X,VARIOSCREEN_GR_TITLE_Y);
	trendDigit = new ScreenDigit(VARIOSCREEN_GR_ANCHOR_X, VARIOSCREEN_GR_ANCHOR_Y, 3, 1, false, true, ALIGNLEFT, true, DISPLAY_OBJECT_TREND, TAILLE_FONT, MAX_CAR_TITRE_FINESSE);
	trendDigit->setPositionTitle(VARIOSCREEN_GR_TITLE_X,VARIOSCREEN_GR_TITLE_Y);

	btinfo = new BTInfo(VARIOSCREEN_BT_ANCHOR_X, VARIOSCREEN_BT_ANCHOR_Y);
	volLevel = new VOLLevel(VARIOSCREEN_VOL_ANCHOR_X, VARIOSCREEN_VOL_ANCHOR_Y);
	batLevel = new BATLevel(VARIOSCREEN_BAT_ANCHOR_X, VARIOSCREEN_BAT_ANCHOR_Y, VOLTAGE_DIVISOR_VALUE, VOLTAGE_DIVISOR_REF_VOLTAGE);
	infoLevel = new INFOLevel(VARIOSCREEN_INFO_ANCHOR_X, VARIOSCREEN_INFO_ANCHOR_Y);
	recordIndicator = new RECORDIndicator(VARIOSCREEN_RECCORD_ANCHOR_X, VARIOSCREEN_RECCORD_ANCHOR_Y);
	satLevel = new SATLevel(VARIOSCREEN_SAT_ANCHOR_X, VARIOSCREEN_SAT_ANCHOR_Y);

	timeMDigit = new ScreenDigit (VARIOSCREEN_TIME_ANCHOR_X-55, VARIOSCREEN_TIME_ANCHOR_Y, 2, 0, false, true, ALIGNLEFT, false, DISPLAY_OBJECT_DURATION, TAILLE_FONT, MAX_CAR_TITRE_TIME);
	timeHDigit = new ScreenDigit (VARIOSCREEN_TIME_ANCHOR_X-73, VARIOSCREEN_TIME_ANCHOR_Y, 2, 0, false, true, ALIGNRIGHT, true, DISPLAY_OBJECT_TIME, TAILLE_FONT, MAX_CAR_TITRE_TIME);

	screenTime = new ScreenTime(VARIOSCREEN_TIME_ANCHOR_X, VARIOSCREEN_TIME_ANCHOR_Y, *timeHDigit, *timeMDigit,false);
	screenTime->setPositionTitle(VARIOSCREEN_TIME_TITLE_X,VARIOSCREEN_TIME_TITLE_Y);
	screenElapsedTime = new ScreenElapsedTime(VARIOSCREEN_ELAPSED_TIME_ANCHOR_X, VARIOSCREEN_ELAPSED_TIME_ANCHOR_Y, *timeHDigit, *timeMDigit);
	screenElapsedTime->setPositionTitle(VARIOSCREEN_ELAPSED_TIME_TITLE_X,VARIOSCREEN_ELAPSED_TIME_TITLE_Y);

	/*
  display.drawLine(130, 26, 295, 26, GxEPD_BLACK);
  display.drawLine(130, 26, 130, 0, GxEPD_BLACK);
  display.drawLine(295, 26, 295, 0, GxEPD_BLACK);
  display.drawLine(190, 26, 190, 127, GxEPD_BLACK);
  display.drawCircle(245, 80, 40, GxEPD_BLACK);*/
	
/*	bgline1 = new BGLine(130, 26, 295, 26);
	bgline2 = new BGLine(130, 26, 130, 0);
	bgline3 = new BGLine(295, 26, 295, 0);
	bgline4 = new BGLine(190, 26, 190, 127);	
	
	bgcircle = new BGCircle(245, 80, 40);*/

//	wind = new WIND(VARIOSCREEN_WIND_ANCHOR_X, VARIOSCREEN_WIND_ANCHOR_Y);
}
#endif
	
//****************************************************************************************************************************
void VarioScreen::createScreenObjectsPage10(void) {
//****************************************************************************************************************************
/*	gpsPeriodDigit 			= new ScreenDigit(VARIOSCREEN_GPS_PERIOD_ANCHOR_X, VARIOSCREEN_GPS_PERIOD_ANCHOR_Y, 6, 1, false, false, ALIGNRIGHT, true, TAILLE_FONT);
	gpsMeanPeriodDigit 	= new ScreenDigit(VARIOSCREEN_GPS_MEAN_PERIOD_ANCHOR_X, VARIOSCREEN_GPS_MEAN_PERIOD_ANCHOR_Y, 6, 1, false, false,  ALIGNRIGHT, true, TAILLE_FONT);
	gpsDurationDigit	 	= new ScreenDigit(VARIOSCREEN_GPS_DURATION_ANCHOR_X, VARIOSCREEN_GPS_DURATION_ANCHOR_Y, 6, 1, false, false,  ALIGNRIGHT, true, TAILLE_FONT);*/
}

//****************************************************************************************************************************
void VarioScreen::createScreenObjectsPage1(void) {
//****************************************************************************************************************************	
//  gpsLatDir 					= new ScreenText(VARIOSCREEN_LATDIR_ANCHOR_X, VARIOSCREEN_LAT_ANCHOR_Y, 1, FONTLARGE, ALIGNLEFT, false, DISPLAY_OBJECT_LAT_DIR);
//	gpsLongDir					= new ScreenText(VARIOSCREEN_LONGDIR_ANCHOR_X, VARIOSCREEN_LONG_ANCHOR_Y, 1, FONTLARGE, ALIGNLEFT, false, DISPLAY_OBJECT_LONG_DIR);
	gpsBearingText			= new ScreenText(VARIOSCREEN_BEARING_TEXT_ANCHOR_X, VARIOSCREEN_BEARING_TEXT_ANCHOR_Y, 3, FONTNORMAL, ALIGNLEFT, true, DISPLAY_OBJECT_BEARING_TEXT, MAX_CAR_TITRE_CAP);
	gpsBearingText->setPositionTitle(VARIOSCREEN_BEARING_TEXT_TITLE_X,VARIOSCREEN_BEARING_TEXT_TITLE_Y);
	gpsBearing 					= new ScreenDigit(VARIOSCREEN_BEARING_ANCHOR_X, VARIOSCREEN_BEARING_ANCHOR_Y, 3, 0, false, false, ALIGNRIGHT, true, DISPLAY_OBJECT_BEARING, TAILLE_FONT, MAX_CAR_TITRE_CAP);
	gpsBearing->setPositionTitle(VARIOSCREEN_BEARING_TITLE_X,VARIOSCREEN_BEARING_TITLE_Y);
//	gpsLat    					= new ScreenDigit(VARIOSCREEN_LAT_ANCHOR_X, VARIOSCREEN_LAT_ANCHOR_Y, 6, 3, false, false, ALIGNRIGHT, false, DISPLAY_OBJECT_LAT);
//	gpsLong   					= new ScreenDigit(VARIOSCREEN_LONG_ANCHOR_X, VARIOSCREEN_LONG_ANCHOR_Y, 6, 3, false, false, ALIGNRIGHT, false, DISPLAY_OBJECT_LONG);
  gpsLat 						  = new ScreenText(VARIOSCREEN_LAT_ANCHOR_X, VARIOSCREEN_LAT_ANCHOR_Y, 11, FONTNORMAL, ALIGNLEFT, true, DISPLAY_OBJECT_LAT, MAX_CAR_TITRE_LAT);
  gpsLat->setPositionTitle(VARIOSCREEN_LAT_TITLE_X,VARIOSCREEN_LAT_TITLE_Y);
	gpsLong							= new ScreenText(VARIOSCREEN_LONG_ANCHOR_X, VARIOSCREEN_LONG_ANCHOR_Y, 11, FONTNORMAL, ALIGNLEFT, true, DISPLAY_OBJECT_LONG, MAX_CAR_TITRE_LONG);
	gpsLong->setPositionTitle(VARIOSCREEN_LONG_TITLE_X,VARIOSCREEN_LONG_TITLE_Y);

//	tempDigit 					= new ScreenDigit(VARIOSCREEN_TEMP_ANCHOR_X, VARIOSCREEN_TEMP_ANCHOR_Y, 2, 0, false, false, ALIGNLEFT, false, DISPLAY_OBJECT_TEMPERATURE);
//	tunit 							= new TUnit(VARIOSCREEN_TEMP_UNIT_ANCHOR_X, VARIOSCREEN_TEMP_ANCHOR_Y);
}
	
#ifdef DISPLAY_LIGHT	
//****************************************************************************************************************************
void VarioScreen::createScreenObjectsPageLight(void) {
//****************************************************************************************************************************
	altiDigit = new ScreenDigit(VARIOSCREEN_ALTI_ANCHOR_X, VARIOSCREEN_ALTI_ANCHOR_Y, 4, 0, false, false, ALIGNRIGHT, true, DISPLAY_OBJECT_ALTI, FONTNORMAL, MAX_CAR_TITRE_ALTI);
  altiDigit->setPositionTitle(VARIOSCREEN_ALTI_TITLE_X,VARIOSCREEN_ALTI_TITLE_Y);

	heightDigit = new ScreenDigit(VARIOSCREEN_ALTI_ANCHOR_X, VARIOSCREEN_ALTI_ANCHOR_Y, 4, 0, false, false, ALIGNRIGHT, true, DISPLAY_OBJECT_HEIGHT, TAILLE_FONT, MAX_CAR_TITRE_AGL);
  heightDigit->setPositionTitle(VARIOSCREEN_ALTI_TITLE_X,VARIOSCREEN_ALTI_TITLE_Y);

	munit = new MUnit(VARIOSCREEN_ALTI_UNIT_ANCHOR_X, VARIOSCREEN_ALTI_ANCHOR_Y);

	trendLevel = new TRENDLevel(VARIOSCREEN_TREND_ANCHOR_X, VARIOSCREEN_TREND_ANCHOR_Y);
	fixgpsinfo = new FIXGPSInfo(VARIOSCREEN_SAT_FIX_ANCHOR_X, VARIOSCREEN_SAT_FIX_ANCHOR_Y);

	varioDigit = new ScreenDigit(VARIOSCREEN_VARIO_ANCHOR_X, VARIOSCREEN_VARIO_ANCHOR_Y, 4, 1, true, false,  ALIGNRIGHT, true, DISPLAY_OBJECT_VARIO, FONTNORMAL, MAX_CAR_TITRE_VARIO);
  varioDigit->setPositionTitle(VARIOSCREEN_VARIO_TITLE_X,VARIOSCREEN_VARIO_TITLE_Y);
//	varioDigit = new ScreenDigit(VARIOSCREEN_VARIO_ANCHOR_X, VARIOSCREEN_VARIO_ANCHOR_Y, 4, 1, true, false,  ALIGNLEFT, true, DISPLAY_OBJECT_VARIO, TAILLE_FONT);
	msunit = new MSUnit(VARIOSCREEN_VARIO_UNIT_ANCHOR_X, VARIOSCREEN_VARIO_UNIT_ANCHOR_Y);

	speedDigit = new ScreenDigit(VARIOSCREEN_SPEED_ANCHOR_X, VARIOSCREEN_SPEED_ANCHOR_Y, 2, 0, false, false, ALIGNRIGHT, true, DISPLAY_OBJECT_SPEED, FONTNORMAL, MAX_CAR_TITRE_SPEED);
	speedDigit->setPositionTitle(VARIOSCREEN_SPEED_TITLE_X,VARIOSCREEN_SPEED_TITLE_Y);
	kmhunit = new KMHUnit(VARIOSCREEN_SPEED_UNIT_ANCHOR_X, VARIOSCREEN_SPEED_UNIT_ANCHOR_Y);

	ratioDigit = new ScreenDigit(VARIOSCREEN_GR_ANCHOR_X, VARIOSCREEN_GR_ANCHOR_Y, 2, 0, false, true, ALIGNLEFT, true, DISPLAY_OBJECT_RATIO, FONTNORMAL, MAX_CAR_TITRE_TCHUTE);
	ratioDigit->setPositionTitle(VARIOSCREEN_GR_TITLE_X,VARIOSCREEN_GR_TITLE_Y);
	trendDigit = new ScreenDigit(VARIOSCREEN_GR_ANCHOR_X, VARIOSCREEN_GR_ANCHOR_Y, 3, 1, false, true, ALIGNLEFT, true, DISPLAY_OBJECT_TREND, FONTNORMAL, MAX_CAR_TITRE_FINESSE);
	trendDigit->setPositionTitle(VARIOSCREEN_GR_TITLE_X,VARIOSCREEN_GR_TITLE_Y);

	btinfo = new BTInfo(VARIOSCREEN_BT_ANCHOR_X, VARIOSCREEN_BT_ANCHOR_Y);
	volLevel = new VOLLevel(VARIOSCREEN_VOL_ANCHOR_X, VARIOSCREEN_VOL_ANCHOR_Y);
	batLevel = new BATLevel(VARIOSCREEN_BAT_ANCHOR_X, VARIOSCREEN_BAT_ANCHOR_Y, VOLTAGE_DIVISOR_VALUE, VOLTAGE_DIVISOR_REF_VOLTAGE);
	infoLevel = new INFOLevel(VARIOSCREEN_INFO_ANCHOR_X, VARIOSCREEN_INFO_ANCHOR_Y);
	recordIndicator = new RECORDIndicator(VARIOSCREEN_RECCORD_ANCHOR_X, VARIOSCREEN_RECCORD_ANCHOR_Y);
	satLevel = new SATLevel(VARIOSCREEN_SAT_ANCHOR_X, VARIOSCREEN_SAT_ANCHOR_Y);

	timeMDigit = new ScreenDigit (VARIOSCREEN_TIME_ANCHOR_X-55, VARIOSCREEN_TIME_ANCHOR_Y, 2, 0, false, true, ALIGNLEFT, false, DISPLAY_OBJECT_DURATION, FONTNORMAL, MAX_CAR_TITRE_TIME);
	timeHDigit = new ScreenDigit (VARIOSCREEN_TIME_ANCHOR_X-73, VARIOSCREEN_TIME_ANCHOR_Y, 2, 0, false, true, ALIGNRIGHT, true, DISPLAY_OBJECT_TIME, FONTNORMAL, MAX_CAR_TITRE_TIME);

	screenTime = new ScreenTime(VARIOSCREEN_TIME_ANCHOR_X, VARIOSCREEN_TIME_ANCHOR_Y, *timeHDigit, *timeMDigit,false);
	screenTime->setPositionTitle(VARIOSCREEN_TIME_TITLE_X,VARIOSCREEN_TIME_TITLE_Y);
	screenElapsedTime = new ScreenElapsedTime(VARIOSCREEN_ELAPSED_TIME_ANCHOR_X, VARIOSCREEN_ELAPSED_TIME_ANCHOR_Y, *timeHDigit, *timeMDigit);
	screenElapsedTime->setPositionTitle(VARIOSCREEN_ELAPSED_TIME_TITLE_X,VARIOSCREEN_ELAPSED_TIME_TITLE_Y);
}
#endif	
	
#ifndef DISPLAY_LIGHT	
//****************************************************************************************************************************
void VarioScreen::createScreenObjectsDisplayPage0(void) {
//****************************************************************************************************************************
//	CreateObjectDisplay(DISPLAY_OBJECT_TENSION, tensionDigit, 0, 0, true); 
//	CreateObjectDisplay(DISPLAY_OBJECT_TEMPRATURE, tempratureDigit, 0, 2, true); 

/*detection dossier AGL et AGL enable*/

		if (GnuSettings.VARIOMETER_ENABLE_AGL && varioData.aglManager.IsOk()) {
			CreateObjectDisplay(DISPLAY_OBJECT_ALTI							, altiDigit					, 0, 1, true);
			CreateObjectDisplay(DISPLAY_OBJECT_HEIGHT						, heightDigit				, 0, 2, true);
		} else {
			CreateObjectDisplay(DISPLAY_OBJECT_ALTI							, altiDigit					, 0, 0, true);
		}
		
		CreateObjectDisplay(DISPLAY_OBJECT_MUNIT						, munit							, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_VARIO						, varioDigit				, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_MSUNIT						, msunit						, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_KMHUNIT					, kmhunit						, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_SPEED						, speedDigit				, 0, 0, true); 
	

#ifdef SCREEN_DEBUG
		SerialPort.print("RATIO_CLIMB_RATE : ");	
		SerialPort.println(GnuSettings.RATIO_CLIMB_RATE);	
#endif //SCREEN_DEBUG
	
    if (GnuSettings.RATIO_CLIMB_RATE == 1) {	
		  CreateObjectDisplay(DISPLAY_OBJECT_RATIO					, ratioDigit				, 0, 0, true); 
		} else if (GnuSettings.RATIO_CLIMB_RATE == 2) {	
		  CreateObjectDisplay(DISPLAY_OBJECT_TREND					, trendDigit				, 0, 0, true); 
		} else {
		  CreateObjectDisplay(DISPLAY_OBJECT_RATIO					, ratioDigit				, 0, 1, true); 
		  CreateObjectDisplay(DISPLAY_OBJECT_TREND					, trendDigit				, 0, 2, true); 			
    }
		CreateObjectDisplay(DISPLAY_OBJECT_INFOLEVEL				, infoLevel					, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_VOLLEVEL					, volLevel					, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_RECORDIND				, recordIndicator		, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_TRENDLEVEL				, trendLevel				, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_BATLEVEL					, batLevel					, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_SATLEVEL					, satLevel					, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_SCREENTIME				, screenTime				, 0, 1, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_SCREENELAPSEDTIME, screenElapsedTime	, 0, 2, true); 		
		CreateObjectDisplay(DISPLAY_OBJECT_FIXGPSINFO				, fixgpsinfo				, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_BTINFO						, btinfo						, 0, 0, true); 
		
/*		CreateObjectDisplay(DISPLAY_OBJECT_LINE	          	, bgline1           , 0, 0, true);
		CreateObjectDisplay(DISPLAY_OBJECT_LINE	          	, bgline2           , 0, 0, true);
		CreateObjectDisplay(DISPLAY_OBJECT_LINE	          	, bgline3           , 0, 0, true);
		CreateObjectDisplay(DISPLAY_OBJECT_LINE	          	, bgline4           , 0, 0, true);

		CreateObjectDisplay(DISPLAY_OBJECT_CIRCLE         	, bgcircle          , 0, 0, true);		*/		
		
//		CreateObjectDisplay(DISPLAY_OBJECT_WIND         		, wind          		, 0, 0, true);				
}
#endif	
	
//****************************************************************************************************************************
void VarioScreen::createScreenObjectsDisplayPage10(void) {
//****************************************************************************************************************************
/*		CreateObjectDisplay(DISPLAY_OBJECT_GPS_PERIOD				, gpsPeriodDigit		, 10, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_GPS_MEAN_PERIOD	, gpsMeanPeriodDigit, 10, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_GPS_DURATION			, gpsDurationDigit	, 10, 0, true); 

		CreateObjectDisplay(DISPLAY_OBJECT_VOLLEVEL					, volLevel					, 10, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_BATLEVEL					, batLevel					, 10, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_SATLEVEL					, satLevel					, 10, 0, true); */
}	

//****************************************************************************************************************************
void VarioScreen::createScreenObjectsDisplayPage1(void) {
//****************************************************************************************************************************
//		CreateObjectDisplay(DISPLAY_OBJECT_ALTI							, altiDigit					, 1, 0, true); 
//		CreateObjectDisplay(DISPLAY_OBJECT_MUNIT						, munit							, 1, 0, true); 
    CreateObjectDisplay(DISPLAY_OBJECT_LAT							, gpsLat					  , 1, 0, true);    
    CreateObjectDisplay(DISPLAY_OBJECT_LONG							, gpsLong						, 1, 0, true);
    //CreateObjectDisplay(DISPLAY_OBJECT_LAT_DIR					, gpsLatDir				  , 1, 0, true);    
    //CreateObjectDisplay(DISPLAY_OBJECT_LONG_DIR 				, gpsLongDir				, 1, 0, true);
    CreateObjectDisplay(DISPLAY_OBJECT_BEARING_TEXT			, gpsBearingText		, 1, 0, true);       
    CreateObjectDisplay(DISPLAY_OBJECT_BEARING    			, gpsBearing    		, 1, 0, true);       
//		CreateObjectDisplay(DISPLAY_OBJECT_TEMPERATURE			, tempDigit					, 1, 0, true); 
//		CreateObjectDisplay(DISPLAY_OBJECT_TUNIT						, tunit							, 1, 0, true); 

		CreateObjectDisplay(DISPLAY_OBJECT_INFOLEVEL				, infoLevel					, 1, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_VOLLEVEL					, volLevel					, 1, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_RECORDIND				, recordIndicator		, 1, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_BATLEVEL					, batLevel					, 1, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_SATLEVEL					, satLevel					, 1, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_FIXGPSINFO				, fixgpsinfo				, 1, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_BTINFO						, btinfo						, 1, 0, true); 

}	

#ifdef DISPLAY_LIGHT	
//****************************************************************************************************************************
void VarioScreen::createScreenObjectsDisplayPageLight(void) {
//****************************************************************************************************************************
		CreateObjectDisplay(DISPLAY_OBJECT_ALTI							, altiDigit					, 0, 0, true);
		CreateObjectDisplay(DISPLAY_OBJECT_MUNIT						, munit							, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_VARIO						, varioDigit				, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_MSUNIT						, msunit						, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_KMHUNIT					, kmhunit						, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_SPEED						, speedDigit				, 0, 0, true); 
	
		CreateObjectDisplay(DISPLAY_OBJECT_INFOLEVEL				, infoLevel					, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_VOLLEVEL					, volLevel					, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_RECORDIND				, recordIndicator		, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_TRENDLEVEL				, trendLevel				, 0, 0, false); 
		CreateObjectDisplay(DISPLAY_OBJECT_BATLEVEL					, batLevel					, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_SATLEVEL					, satLevel					, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_SCREENTIME				, screenTime				, 0, 1, false); 
		CreateObjectDisplay(DISPLAY_OBJECT_SCREENELAPSEDTIME, screenElapsedTime	, 0, 2, false); 		
		CreateObjectDisplay(DISPLAY_OBJECT_FIXGPSINFO				, fixgpsinfo				, 0, 0, true); 
		CreateObjectDisplay(DISPLAY_OBJECT_BTINFO						, btinfo						, 0, 0, true); 		
}

#endif

//****************************************************************************************************************************
void VarioScreen::ScreenBackground(int8_t page)
//****************************************************************************************************************************
{
	switch (page) {
	  case 0:
//			display.drawLine(0, 35, 200, 35, GxEPD_BLACK);
			break;
	  case 1:
			display.drawLine(0, 35, 200, 35, GxEPD_BLACK);
			break;
	  default:
			break;
	}
}


















//****************************************************************************************************************************
void VarioScreen::begin(void)
//****************************************************************************************************************************
{
	
#ifdef SCREEN_DEBUG
	SerialPort.println("schedulerScreen : create");	
		
	SerialPort.print("begin - objectCount : ");	
	SerialPort.println(ITEMS_IN_ARRAY(displayList));	
	SerialPort.print("begin - MaxobjectCount : ");	
	SerialPort.println(MaxObjectList);	

//	SerialPort.println(sizeof(displayList);	
#endif //SCREEN_DEBUG
	
//	schedulerScreen = new ScreenScheduler(displayList, ITEMS_IN_ARRAY(displayList) -1, 0, 0);   //ITEMS_IN_ARRAY(displayList), 0, 0);
	schedulerScreen = new ScreenScheduler(displayList, MaxObjectList -1, 0, 1);   //ITEMS_IN_ARRAY(displayList), 0, 0);

  schedulerScreen->setPage(0,false);
  stateDisplay = STATE_OK;
	
//  display.update();  

/*  updateWindow(0, 0, display.width(), display.height(), false);  
  while (GetState() != STATE_OK) {
    updateWindow(0, 0, display.width(), display.height(), false);
  }*/
}

//****************************************************************************************************************************
void VarioScreen::CreateObjectDisplay(int8_t ObjectDisplayTypeID, VarioScreenObject* object, int8_t page, int8_t multiDisplayID, boolean actif) { 
//****************************************************************************************************************************

  MaxObjectList++;
	
#ifdef SCREEN_DEBUG
	SerialPort.print("MaxObjectList : ");	
	SerialPort.println(MaxObjectList);	
#endif //SCREEN_DEBUG
	
//  displayList = (ScreenSchedulerObject*) realloc (displayList, MaxObjectList * sizeof(ScreenSchedulerObject ) );
	
	displayList[MaxObjectList-1].object				 				= object;
	displayList[MaxObjectList-1].page   							= page;
	displayList[MaxObjectList-1].multiDisplayID 			= multiDisplayID;
	displayList[MaxObjectList-1].ObjectDisplayTypeID	= ObjectDisplayTypeID;
	displayList[MaxObjectList-1].actif  							= actif;
	
	
}

/*TaskHandle_t taskDisplay;

// ****************************************************************************************************************************
void genericTask( void * parameter ){
// ****************************************************************************************************************************
  stateDisplay = STATE_BUSY;
#ifdef SCREEN_DEBUG
    SerialPort.print("Created task: Executing on core ");
    SerialPort.println(xPortGetCoreID());
#endif //SCREEN_DEBUG

	display.display(true); // partial update
	display.epd2.powerOff();
*	for(int i=0;i<200;i++) {
		delay(1);
// Passes control to other tasks when called
		SysCall::yield();
	}*
//	display.
  stateDisplay = STATE_OK;
  vTaskDelete(taskDisplay);
}*/

// ****************************************************************************************************************************
void VarioScreen::screenTask( void * parameter ){
// ****************************************************************************************************************************

  while( true ) {

    /* wait */
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
 
#ifdef SCREEN_DEBUG2
	SerialPort.println("screenTask : wake");	
#endif //SCREEN_DEBUG
 
    /* launch interrupt */
	 xSemaphoreTake(screenMutex, portMAX_DELAY);	

#ifdef SCREEN_DEBUG2
		SerialPort.println("screenTask : take");	
#endif //SCREEN_DEBUG

	 display.setFullWindow();
	 display.display(true); // partial update
	 
#ifdef SCREEN_DEBUG2
		SerialPort.println("screenTask : display");	
#endif //SCREEN_DEBUG
	 
	 display.epd2.powerOff();
	 
#ifdef SCREEN_DEBUG2
		SerialPort.println("screenTask : poweroff");	
#endif //SCREEN_DEBUG

   xSemaphoreGive(screenMutex);
#ifdef SCREEN_DEBUG2
		SerialPort.println("screenTask : Give");	
#endif //SCREEN_DEBUG

  }
}

// ****************************************************************************************************************************
void VarioScreen::updateScreen (void)
// ****************************************************************************************************************************
{
#ifdef SCREEN_DEBUG2
	SerialPort.println("screen update");	
#endif //SCREEN_DEBUG

//  BaseType_t xHigherPriorityTaskWoken = 0;

	if( xSemaphoreTake( screen.screenMutex, ( TickType_t ) 0 )  == pdTRUE) {
    xSemaphoreGive(screen.screenMutex);
    xTaskNotify(screenTaskHandler, 0, eNoAction);
	 
#ifdef SCREEN_DEBUG2
		SerialPort.println("updateScreen : wake");	
#endif //SCREEN_DEBUG
	}
}
	
/*	
  if (stateDisplay != STATE_OK) {
#ifdef SCREEN_DEBUG2
		SerialPort.println("Task en cours");	
#endif //SCREEN_DEBUG

		if (millis() - timerShow > 1500) {
			stateDisplay = STATE_OK;
			vTaskDelete(taskDisplay);
//			display.powerOff();
			display.epd2.reset();
#ifdef SCREEN_DEBUG2
			SerialPort.println("Task reset");	
#endif //SCREEN_DEBUG

		}
	  return;
	}
	
	timerShow = millis();
	display.setFullWindow();
#ifdef SCREEN_DEBUG
	SerialPort.println("screen update : setFullWindows");	
#endif //SCREEN_DEBUG
	
//	xTaskCreate(
//							genericTask,       // Task function. 
//							"genericTask",     // String with name of task. 
//							10000,             // Stack size in words. 
//							NULL,              // Parameter passed as input of the task 
//							2,                 // Priority of the task. 
//							NULL);             // Task handle. 
	
	xTaskCreatePinnedToCore(
							genericTask,       // Task function. 
							"TaskDisplay",     // String with name of task. 
							10000,             // Stack size in words. 
							NULL,              // Parameter passed as input of the task 
							2,                 // Priority of the task.
							&taskDisplay,			 // Task handle
							1);             	 // pin task to core 1	
							
//	display.display(true); // partial update
#ifdef SCREEN_DEBUG
	SerialPort.println("screen update : create task");	
#endif //SCREEN_DEBUG

//	display.updateWindow(0, 0, display.width(), display.height(), false);
	//display.display(true); // partial update

}*/

// ****************************************************************************************************************************
void VarioScreen::updateScreenNB (void)
// ****************************************************************************************************************************
{
#ifdef SCREEN_DEBUG2
	SerialPort.println("screen updateNB");	
#endif //SCREEN_DEBUG
	
	display.display(true); // partial update

}

// ****************************************************************************************************************************
void VarioScreen::updateData(int8_t ObjectDisplayTypeID, double data) {
// ****************************************************************************************************************************
	
	#ifdef SCREEN_DEBUG
	SerialPort.println("updateData");	
#endif //SCREEN_DEBUG

	for(int i=0;i<=sizeof(displayList)/sizeof(ScreenSchedulerObject);i++) {
		
#ifdef SCREEN_DEBUG
		SerialPort.print("i = ");	
		SerialPort.println(i);	
		SerialPort.print("ObjectDisplayTypeID = ");	
		SerialPort.println(displayList[i].ObjectDisplayTypeID);			
#endif //SCREEN_DEBUG
		
		if (displayList[i].ObjectDisplayTypeID	== ObjectDisplayTypeID) {
	//		displayList[i].object->setValue(data);
			
#ifdef SCREEN_DEBUG
		  SerialPort.print("i = ");	
		  SerialPort.println(i);	
		  SerialPort.print("data = ");	
		  SerialPort.println(data);	
#endif //SCREEN_DEBUG
		
			
		}
	}
}

// ****************************************************************************************************************************
void VarioScreen::clearScreen() {
// ****************************************************************************************************************************
  display.clearScreen();
}

// ****************************************************************************************************************************
void VarioScreen::ScreenViewInit(uint8_t Version, uint8_t Sub_Version, String Author, uint8_t Beta_Code)
// ****************************************************************************************************************************
{
  char tmpbuffer[100];
	
  display.setFullWindow();
  display.firstPage();
  do
  {
// 	  display.fillScreen(ColorScreen);
//		display.clearScreen(ColorScreen);

		display.drawBitmap(0, 10, logo_gnuvario, 102, 74, ColorText); //94

		display.setFont(&FreeSansBold12pt7b);
		display.setTextColor(ColorText);
		display.setTextSize(1);

		display.setCursor(100, 30);
		display.println("Version");
		sprintf(tmpbuffer,"%02d.%02d-", Version, Sub_Version);
		display.setCursor(105, 50);
		display.print(tmpbuffer);
		display.print(Author);
		if (Beta_Code > 0) {
			sprintf(tmpbuffer," Beta %01d", Beta_Code);
  		display.setCursor(105, 70);
		  display.print(tmpbuffer);
		}
		
		sprintf(tmpbuffer,"%s", __DATE__);
		display.setCursor(25, 110);
		display.print(tmpbuffer);

		display.setFont(&FreeSansBold12pt7b);
		display.setTextSize(1);
		display.setCursor(20, VARIOSCREEN_TENSION_ANCHOR_Y);
		display.print("GNUVARIO-E");
		
		sprintf(tmpbuffer,"%03d", varioHardwareManager.varioPower.getCapacite());
		String tmpstr = String(tmpbuffer) +"% (";
		sprintf(tmpbuffer,"%.2f", varioHardwareManager.varioPower.getTension());
		tmpstr = tmpstr + String(tmpbuffer) + "v)";
		display.setCursor(25, 190);
		display.print(tmpstr);
		
  }
  while (display.nextPage());
	
//	display.powerOff();
	
	unsigned long TmplastDisplayTimestamp = millis();
	int compteur = 0;
	while (compteur < 3) {

		ButtonScheduleur.update();

		if (ButtonScheduleur.Get_StatePage() == STATE_PAGE_CALIBRATION) break;
		
		if( millis() - TmplastDisplayTimestamp > 1000 ) {

			TmplastDisplayTimestamp = millis();
			compteur++;
		
		  display.fillRect(19, 150, 180, -30, GxEPD_WHITE);

		  if ((compteur % 2) == 0) {
				display.setCursor(20, VARIOSCREEN_TENSION_ANCHOR_Y);
				display.print("GNUVARIO-E");
#ifdef SCREEN_DEBUG
				SerialPort.println("Gnuvario-E");	
#endif //SCREEN_DEBUG

			} else {
#ifdef SCREEN_DEBUG
				SerialPort.println("Startup");	
#endif //SCREEN_DEBUG

			}
			updateScreen ();
		}
	}
	
// 	display.fillScreen(ColorScreen);
	
//  display..update();
/*		display..updateWindow(0, 0, display.width(), display.height(), false);
  while (display..GetState() != STATE_OK) {
    display..updateWindow(0, 0, display.width(), display.height(), false);
  }*/	
}

// ****************************************************************************************************************************
void VarioScreen::ScreenViewPage(int8_t page, boolean clear, boolean refresh)
// ****************************************************************************************************************************
{
	
#ifdef SCREEN_DEBUG
	SerialPort.println("fillScreen");	
#endif //SCREEN_DEBUG

//setPage(int8_t page, boolean force = false);

  if (clear) {
	  display.setFullWindow();	
		display.clearScreen(ColorScreen);
		display.fillScreen(ColorScreen);
	}
	
#ifdef SCREEN_DEBUG
	SerialPort.println("setTextColor");	
#endif //SCREEN_DEBUG
	display.clearScreen(ColorScreen);
  display.fillScreen(ColorScreen);
  display.setTextColor(GxEPD_BLACK);//display.setTextColor(GxEPD_BLACK);
	
#ifdef SCREEN_DEBUG
	SerialPort.println("update");	
#endif //SCREEN_DEBUG	

	if (refresh) {
		altiDigit->update(true);
		heightDigit->update(true);
		varioDigit->update(true);
		speedDigit->update(true);
		ratioDigit->update(true);
		trendDigit->update(true);
		infoLevel->update(true);
		volLevel->update(true);
	
		recordIndicator->update(true);
		recordIndicator->update(true);
		trendLevel->update(true);
		batLevel->update(true);
		satLevel->update(true);
		screenTime->update(true);
		screenElapsedTime->update(true);

		fixgpsinfo->update(true);
		btinfo->update(true);
		
		gpsLat->update(true);
		gpsLong->update(true);
		gpsBearing->update(true);
		gpsBearingText->update(true);
		
//		gpsLatDir->update(true);
//		gpsLongDir->update(true);

	}
	else {		
		altiDigit->setValue(9999);
		heightDigit->setValue(9999);
		varioDigit->setValue(0.0);
		speedDigit->setValue(0);
		ratioDigit->setValue(0);
		trendDigit->setValue(0);
		infoLevel->set(INFO_NONE);
		volLevel->setVolume(10);
	
		recordIndicator->setActifSCAN();
		recordIndicator->stateRECORD();
		trendLevel->stateTREND(0);
		batLevel->setVoltage(0);
		satLevel->setSatelliteCount(0);

		int8_t tmptime[] = {0,0,0};
		screenTime->setTime(tmptime);

		screenElapsedTime->setBaseTime(screenTime->getTime());
		screenElapsedTime->setCurrentTime(screenTime->getTime());

		fixgpsinfo->unsetFixGps();
		btinfo->unsetBT();
		
	//	gpsLat->setValue(0.0);
	//	gpsLatDir->setValue("N");
	//	gpsLong->setValue(0.0);
	//	gpsLongDir->setValue("E");
		gpsLat->setValue("000*00'00'' X");
		gpsLong->setValue("000*00'00'' X");
		gpsBearingText->setValue("XXX");
		gpsBearing->setValue(0);
	}
		
	munit->toDisplay();
	msunit->toDisplay();
	kmhunit->toDisplay();
}	

/*void VarioScreen::clearScreen(void) 
{
  eraseDisplay();	
}*/

/*void VarioScreen::beginClear(void) {
  clearingStep = 0;
}

bool VarioScreen::clearStep(void) {

  // * check if clear is needed *
  if( clearingStep == LCDHEIGHT ) {
    return false;
  }

  // * clear one line *

  // * next *
  clearingStep++;
  return true;
}*/


//****************************************************************************************************************************
void VarioScreen::ScreenViewStat(void)
//****************************************************************************************************************************
{
	delay(100);
  display.setFullWindow();
  display.clearScreen(ColorScreen);
  display.fillScreen(ColorScreen);
  display.firstPage();
  do
  {
// 	  display.fillScreen(ColorScreen);
//		display.clearScreen(ColorScreen);

		ScreenViewStatPage(0);
  }
  while (display.nextPage());
	
//	display.powerOff();
	
/*	unsigned long TmplastDisplayTimestamp = millis();
	int compteur = 0;
	while (compteur < 3) {
		if( millis() - TmplastDisplayTimestamp > 1000 ) {

			TmplastDisplayTimestamp = millis();
			compteur++;
		
		}
	}

// *  altiDigit.setValue(flystat.GetAlti());
  altiDigit.update();
  altiDigit.display();
  varioDigit.setValue(flystat.GetVarioMin());
  varioDigit.update();
  varioDigit.display();
  double tmpspeed = flystat.GetSpeed();
  if (tmpspeed > 99) tmpspeed = 99;
  speedDigit.setValue(tmpspeed);
  speedDigit.update();
  speedDigit.display();
  kmhunit.update();
  kmhunit.display();
  msunit.update();
  msunit.display();
  munit.update();  
  munit.display();*/
}

//****************************************************************************************************************************
void VarioScreen::ScreenViewStatPage(int PageStat)
//****************************************************************************************************************************
{

  char tmpbuffer[100];

	display.setFont(&FreeSansBold12pt7b);
	display.setTextColor(ColorText);
	display.setTextSize(1);

	display.setCursor(10, 30);
	display.print(varioLanguage.getText(TITRE_STAT));   //"STATISTIQUES");

	uint8_t tmpDate[3];
	int8_t  tmpTime[3];
	varioData.flystat.GetDate(tmpDate);
		
#ifdef SCREEN_DEBUG
    for (int i=0; i< 3; i++) {
		  SerialPort.print(tmpDate[i]);
      SerialPort.print(" ");
    }
    SerialPort.println("");		
#endif //SCREEN_DEBUG
		
		sprintf(tmpbuffer,"Date: %02d.%02d.%02d", tmpDate[0],tmpDate[1],tmpDate[2]);
		display.setCursor(1, 60);
		display.print(tmpbuffer);

#ifdef SCREEN_DEBUG
 		  SerialPort.println(tmpbuffer);
#endif //SCREEN_DEBUG

		varioData.flystat.GetTime(tmpTime);
		sprintf(tmpbuffer,"%s: %02d:%02d",varioLanguage.getText(TITRE_TIME),tmpTime[2],tmpTime[1]); 
		display.setCursor(1, 80);
		display.print(tmpbuffer);

#ifdef SCREEN_DEBUG
 		  SerialPort.println(tmpbuffer);
#endif //SCREEN_DEBUG

		varioData.flystat.GetDuration(tmpTime);	
		sprintf(tmpbuffer,"Duree: %02d:%02d",tmpTime[2],tmpTime[1]); 
		display.setCursor(1, 100);
		display.print(tmpbuffer);

#ifdef SCREEN_DEBUG
 		  SerialPort.println(tmpbuffer);
#endif //SCREEN_DEBUG

    double tmpAlti = varioData.flystat.GetAlti();
		if (tmpAlti > 9999) tmpAlti = 9999;
		sprintf(tmpbuffer,"Alti Max: %3.0f",tmpAlti); 
		display.setCursor(1, 120);
		display.print(tmpbuffer);
	 
   double tmpVarioMin = varioData.flystat.GetVarioMin();
	 if (tmpVarioMin > 10) tmpVarioMin = 9.9;
	 if (tmpVarioMin < -10) tmpVarioMin = -9.9;
	 sprintf(tmpbuffer,"Vario Min: %1.1f",tmpVarioMin); 
	 display.setCursor(1, 140);
	 display.print(tmpbuffer);

   double tmpVarioMax = varioData.flystat.GetVarioMax();
	 if (tmpVarioMax > 10) tmpVarioMax = 9.9;
	 if (tmpVarioMax < -10) tmpVarioMax = -9.9;
	 sprintf(tmpbuffer,"Vario Max: %1.1f",tmpVarioMax); 
	 display.setCursor(1, 160);
	 display.print(tmpbuffer);
	 
   double tmpSpeed = varioData.flystat.GetSpeed();
	 if (tmpSpeed > 1000) tmpSpeed = 999;
	 sprintf(tmpbuffer,"%s: %3.0f",varioLanguage.getText(TITRE_SPEED),tmpSpeed); //%02d.%02d.%02d", tmpDate[0],tmpDate[1],tmpDate[2]);
	 display.setCursor(1, 180);
	 display.print(tmpbuffer);
}






//****************************************************************************************************************************
void VarioScreen::ScreenViewWifi(String SSID, String IP)
//****************************************************************************************************************************
{
//  char tmpbuffer[100];
	
	if ((SSID == "") && (IP == "")) {
		display.setFullWindow();
		display.firstPage();
		do
		{

			display.setFont(&FreeSansBold12pt7b);
			display.setTextColor(ColorText);
			display.setTextSize(2);

			display.setCursor(40, 40);
			display.print("WIFI");

			display.setTextSize(1);
			display.setCursor(5, 80);
			display.print(varioLanguage.getText(TITRE_CONNECT));   //"Connection ...");
		}
		while (display.nextPage());
		
#ifdef SCREEN_DEBUG
		SerialPort.println("ScreenViewWifi : Connecting");	
#endif //SCREEN_DEBUG


	} else if ((SSID != "") && (IP != "")) {
		display.setCursor(5, 120);
		display.print(varioLanguage.getText(TITRE_CONNECTA)); //"Connection a ");

		display.setCursor(5, 140);
		display.print(SSID);

		display.setCursor(5, 160);
		display.print(IP);
		
#ifdef SCREEN_DEBUG
		SerialPort.println("ScreenViewWifi : Connected");	
#endif //SCREEN_DEBUG

		updateScreen ();
	} else {
		display.setCursor(5, 180);
		display.print(varioLanguage.getText(TITRE_DEMAR)); //"Demarrage");
		
#ifdef SCREEN_DEBUG
		SerialPort.println("ScreenViewWifi : start");	
#endif //SCREEN_DEBUG

		updateScreen ();
		
	}
}

//****************************************************************************************************************************
void VarioScreen::ScreenViewReboot(String message)
//****************************************************************************************************************************
{
//  char tmpbuffer[100];
	
  display.setFullWindow();
  display.firstPage();
  do
  {
// 	  display.fillScreen(ColorScreen);
//		display.clearScreen(ColorScreen);

		display.drawBitmap(0, 10, logo_gnuvario, 102, 74, ColorText); //94

		display.setFont(&FreeSansBold12pt7b);
		display.setTextColor(ColorText);
		display.setTextSize(1);

		display.setCursor(95, 80);
		display.print("Vario-E");

		display.setFont(&FreeSansBold12pt7b);
		display.setTextSize(1);
		display.setCursor(20, 140);
		if (message == "")
			display.print(varioLanguage.getText(TITRE_REDEMAR)); //"Redemarrage");
		else 
			display.print(message);
			
 		display.setCursor(40, 170);
		display.print(varioLanguage.getText(TITRE_ENCOURS)); //"en cours");
  }
  while (display.nextPage());
}

//****************************************************************************************************************************
void VarioScreen::ScreenViewMessage(String message, int delai)
//****************************************************************************************************************************
{
//  char tmpbuffer[100];
	
  display.setFullWindow();
  display.firstPage();
  do
  {
// 	  display.fillScreen(ColorScreen);
//		display.clearScreen(ColorScreen);

		display.drawBitmap(0, 10, logo_gnuvario, 102, 74, ColorText); //94

		display.setFont(&FreeSansBold12pt7b);
		display.setTextColor(ColorText);
		display.setTextSize(1);

/*		display.setCursor(100, 30);
		display.println("Version");
		if (Beta_Code > 0) {
			sprintf(tmpbuffer," Beta %01d", Beta_Code);
  		display.setCursor(105, 50);
		  display.print(tmpbuffer);
		}
		sprintf(tmpbuffer,"%02d.%02d-", Version, Sub_Version);
		display.setCursor(105, 70);
		display.print(tmpbuffer);
		display.print(Author);
		sprintf(tmpbuffer,"%s", __DATE__);
		display.setCursor(25, 110);
		display.print(tmpbuffer);*/

		display.setCursor(20, 110);
		display.print("GNUVario-E");

		display.setFont(&FreeSansBold12pt7b);
		display.setTextSize(1);
		
		
		int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
		display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh); // it works for origin 0, 0, fortunately (negative tby!)
  // center bounding box by transposition of origin:
		uint16_t x = ((display.width() - tbw) / 2) - tbx;
//		uint16_t y = ((display.height() - tbh) / 2) - tby;
    display.setCursor(x, VARIOSCREEN_TENSION_ANCHOR_Y); // set the postition to start printing text
		display.print(message);
  }
  while (display.nextPage());
	
//	display.powerOff();
	
	delay(500);
	unsigned long TmplastDisplayTimestamp = millis();
	int compteur = 0;
	
	int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
	display.getTextBounds(message, 0, 0, &tbx, &tby, &tbw, &tbh); // it works for origin 0, 0, fortunately (negative tby!)
// center bounding box by transposition of origin:
	uint16_t x = ((display.width() - tbw) / 2) - tbx;
//	uint16_t y = ((display.height() - tbh) / 2) - tby;
	
	while (compteur < delai) {
		ButtonScheduleur.update();
		
		if( millis() - TmplastDisplayTimestamp > 1000 ) {

			TmplastDisplayTimestamp = millis();
			compteur++;
		
		  display.fillRect(19, 170, 180, -30, GxEPD_WHITE);

		  if ((compteur % 2) == 0) {
				display.setCursor(x, VARIOSCREEN_TENSION_ANCHOR_Y);
				display.print(message);
#ifdef SCREEN_DEBUG
				SerialPort.println(message);	
#endif //SCREEN_DEBUG

			} else {
#ifdef SCREEN_DEBUG
//				SerialPort.println("");	
#endif //SCREEN_DEBUG

			}
			updateScreen ();
		}
	}
}

// ****************************************************************************************************************************
void VarioScreen::ScreenViewBattery(boolean clear)
// ****************************************************************************************************************************
{
	
#ifdef SCREEN_DEBUG
	SerialPort.println("Charge batterie");	
#endif //SCREEN_DEBUG

//setPage(int8_t page, boolean force = false);

  if (clear) {
	  display.setFullWindow();	
		display.clearScreen(ColorScreen);
		display.fillScreen(ColorScreen);
	}
	
  display.setTextColor(GxEPD_BLACK);//display.setTextColor(GxEPD_BLACK);
	display.setFont(&FreeSansBold12pt7b);
	
	display.setTextSize(1);
	display.setCursor(55, 20);
	display.print(varioLanguage.getText(TITRE_BATTERIE));
		
  display.setCursor(40, 40);
	display.print(varioLanguage.getText(TITRE_CHARGE));
	
//	display.setFont(&FreeSansBold12pt7b);
	char tmpbuffer[100];
	sprintf(tmpbuffer,"%03d", varioHardwareManager.varioPower.getCapacite());
	String tmpstr = String(tmpbuffer) +"%";
	display.fillRect(0, 55, 190, 40, GxEPD_WHITE);
	display.setCursor(40, 90);
	display.setTextSize(2);
	display.print(tmpstr);

	display.setFont(&FreeSansBold12pt7b);
	display.setTextSize(1);
	tmpstr = "(";
	sprintf(tmpbuffer,"%.2f", varioHardwareManager.varioPower.getTension());
	tmpstr = tmpstr + String(tmpbuffer) + "v)";
  display.fillRect(0, 110, 190, 30, GxEPD_WHITE);
	display.setCursor(60, 130);
	display.print(tmpstr);
	sprintf(tmpbuffer,"%04d / %04d", varioHardwareManager.varioPower.getVoltage(), varioHardwareManager.varioPower.getRefVoltage());
	tmpstr = String(tmpbuffer);
  display.fillRect(0, 145, 190, 30, GxEPD_WHITE);
	display.setCursor(30, 160);
	display.setTextSize(1);
	display.print(tmpstr);

/*	
	
	display.setCursor(30, 50);
	display.print(varioLanguage.getText(TITRE_BATTERIE));
	
	display.setCursor(20, 100);
	display.print(varioLanguage.getText(TITRE_CHARGE));
	
	char tmpbuffer[100];
	display.setTextSize(2);
	sprintf(tmpbuffer,"%03d", varioHardwareManager.varioPower.getCapacite());
	String tmpstr = String(tmpbuffer) +"%";
  display.fillRect(0, 150, 190, 50, GxEPD_WHITE);
	display.setCursor(20, 180);
	display.print(tmpstr);
	tmpstr = "(";
	sprintf(tmpbuffer,"%.2f", varioHardwareManager.varioPower.getTension());
	tmpstr = tmpstr + String(tmpbuffer) + "v)";
  display.fillRect(0, 210, 190, 50, GxEPD_WHITE);
	display.setCursor(5, 240);
	display.print(tmpstr);
	sprintf(tmpbuffer,"%04d / %04d", varioHardwareManager.varioPower.getVoltage(), varioHardwareManager.varioPower.getRefVoltage());
	tmpstr = String(tmpbuffer);
  display.fillRect(0, 260, 190, 50, GxEPD_WHITE);
	display.setCursor(15, 280);
	display.setTextSize(1);
	display.print(tmpstr);
*/
	
	updateScreen ();
}	

const unsigned char volume75_1_icons[] = { 
// '750px-Speaker_Icon_1', 75x75px
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,
0xe0, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x3f,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xf8, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0x80,
0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x3e,
0x7f, 0xff, 0xff, 0xe0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x3f, 0xff, 0xff, 0xe0, 0xf0, 0x00,
0x00, 0x00, 0x00, 0x38, 0x1f, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x1f, 0xff,
0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x0f, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x3e, 0x0f, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x07, 0xff, 0xff, 0xe0,
0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x07, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f,
0x07, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x07, 0xff, 0xff, 0xe0, 0xe0, 0x00,
0x00, 0x00, 0x00, 0x3f, 0x03, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x03, 0xff,
0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x83, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x3f, 0x83, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x83, 0xff, 0xff, 0xe0,
0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x03, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f,
0x03, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x07, 0xff, 0xff, 0xe0, 0xe0, 0x00,
0x00, 0x00, 0x00, 0x3f, 0x07, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x07, 0xff,
0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x07, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x3e, 0x0f, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x0f, 0xff, 0xff, 0xe0,
0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x1f, 0xff, 0xff, 0xe0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3c,
0x1f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x3c, 0x3f, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xfc, 0x00, 0x00, 0x3e, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0x80, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xc0,
0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x3f,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xff, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,
0xe0, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0
};

const unsigned char volume75_2_icons[] = { 
// '750px-Speaker_Icon_2', 75x75px
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,
0xe0, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x3f,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x3f, 0xfc, 0x7f, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xf8, 0x00, 0x3f, 0xf8, 0x3f, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x3f, 0xf0, 0x3f,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x3f, 0xf8, 0x1f, 0xff, 0xe0, 0xff, 0xff, 0xff, 0x80,
0x00, 0x3f, 0xf8, 0x1f, 0xff, 0xe0, 0xff, 0xff, 0xff, 0x00, 0x00, 0x3f, 0xfc, 0x0f, 0xff, 0xe0,
0xff, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xfe, 0x0f, 0xff, 0xe0, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x3e,
0x7e, 0x07, 0xff, 0xe0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x3f, 0x07, 0xff, 0xe0, 0xf0, 0x00,
0x00, 0x00, 0x00, 0x38, 0x1f, 0x03, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x0f, 0x03,
0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x0f, 0x83, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x3e, 0x0f, 0x81, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x07, 0x81, 0xff, 0xe0,
0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x07, 0xc1, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f,
0x07, 0xc1, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x07, 0xc1, 0xff, 0xe0, 0xe0, 0x00,
0x00, 0x00, 0x00, 0x3f, 0x03, 0xc1, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x03, 0xc0,
0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x83, 0xc0, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x3f, 0x83, 0xc0, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x83, 0xc0, 0xff, 0xe0,
0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x03, 0xc0, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f,
0x03, 0xc1, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x07, 0xc1, 0xff, 0xe0, 0xe0, 0x00,
0x00, 0x00, 0x00, 0x3f, 0x07, 0xc1, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x07, 0xc1,
0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x07, 0x81, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x3e, 0x0f, 0x81, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x0f, 0x83, 0xff, 0xe0,
0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x1f, 0x03, 0xff, 0xe0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3c,
0x1f, 0x03, 0xff, 0xe0, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x3c, 0x3f, 0x07, 0xff, 0xe0, 0xff, 0xff,
0xfc, 0x00, 0x00, 0x3e, 0x7e, 0x07, 0xff, 0xe0, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xfe, 0x0f,
0xff, 0xe0, 0xff, 0xff, 0xff, 0x80, 0x00, 0x3f, 0xfc, 0x0f, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xc0,
0x00, 0x3f, 0xf8, 0x1f, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xf8, 0x1f, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x3f, 0xf8, 0x3f, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x3f,
0xf8, 0x7f, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x3f, 0xfc, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xff, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,
0xe0, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0
};

const unsigned char volume75_3_icons[] = { 
 // 'basic1-095_volume_loud-32'
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x03, 0x80, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x0f,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1f, 0xc0, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x0f, 0xc0, 0x00,
0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xc0,
0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x03, 0x83, 0xf0, 0x00, 0x00, 0x00,
0x00, 0x07, 0xff, 0xc0, 0x07, 0xc3, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x0f, 0xc1,
0xf8, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xc0, 0x07, 0xe1, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x7f,
0xff, 0xc0, 0x07, 0xe0, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0xfc, 0x00,
0x00, 0x00, 0x01, 0xff, 0xff, 0xc0, 0x01, 0xf0, 0x7c, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xc1,
0x81, 0xf8, 0x7e, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xc0, 0xf8, 0x3e, 0x00, 0x0f, 0xff,
0xff, 0xff, 0xff, 0xc7, 0xe0, 0xfc, 0x3e, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xf0, 0xfc,
0x3f, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xf0, 0x7c, 0x3f, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xc1, 0xf0, 0x7e, 0x1f, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xf8, 0x7e, 0x1f, 0x00,
0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xf8, 0x3e, 0x1f, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0,
0xf8, 0x3e, 0x1f, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xf8, 0x3e, 0x1f, 0x80, 0x1f, 0xff,
0xff, 0xff, 0xff, 0xc0, 0xfc, 0x3e, 0x1f, 0x80, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xfc, 0x3f,
0x1f, 0x80, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x7c, 0x3f, 0x1f, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xc0, 0x7c, 0x3f, 0x1f, 0x80, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x7c, 0x3f, 0x1f, 0x80,
0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xfc, 0x3f, 0x1f, 0x80, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0,
0xfc, 0x3e, 0x1f, 0x80, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xf8, 0x3e, 0x1f, 0x80, 0x1f, 0xff,
0xff, 0xff, 0xff, 0xc0, 0xf8, 0x3e, 0x1f, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xf8, 0x3e,
0x1f, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xf8, 0x7e, 0x1f, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xc1, 0xf0, 0x7e, 0x1f, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xf0, 0x7c, 0x3f, 0x00,
0x1f, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xe0, 0xfc, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xc3,
0xe0, 0xfc, 0x3e, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc3, 0xc0, 0xf8, 0x7e, 0x00, 0x00, 0x00,
0x03, 0xff, 0xff, 0xc1, 0x81, 0xf8, 0x7e, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xc0, 0x01, 0xf0,
0x7c, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xc0, 0x03, 0xf0, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x3f,
0xff, 0xc0, 0x07, 0xe0, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xc0, 0x07, 0xe1, 0xf8, 0x00,
0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x07, 0xc1, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xc0,
0x07, 0x83, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x03, 0x03, 0xf0, 0x00, 0x00, 0x00,
0x00, 0x00, 0xff, 0xc0, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x0f,
0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1f, 0xc0, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x1f, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char volume75_4_icons[] = { 
/// '750px-Speaker_Icon_4', 76x76px
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff,
0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xff, 0xff, 0xff, 0xe0, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x80, 0x1f,
0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff,
0xff, 0xfe, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0xff,
0xff, 0xf0, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xf0,
0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xff, 0xff, 0x80, 0x00, 0x1f, 0xbf, 0xff, 0xfd, 0xf0, 0xff, 0xff, 0xff, 0x00, 0x00, 0x1f,
0x0f, 0xff, 0xf0, 0xf0, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x1e, 0x07, 0xff, 0xe0, 0x70, 0xfc, 0x00,
0x00, 0x00, 0x00, 0x1e, 0x07, 0xff, 0xe0, 0x70, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x03, 0xff,
0xc0, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x81, 0xff, 0x81, 0xf0, 0xf8, 0x00, 0x00, 0x00,
0x00, 0x1f, 0x80, 0xff, 0x01, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x7e, 0x03, 0xf0,
0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x3c, 0x07, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f,
0xf0, 0x1c, 0x0f, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x18, 0x1f, 0xf0, 0xf8, 0x00,
0x00, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x3f, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0x00,
0x7f, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0x00, 0x7f, 0xf0, 0xf8, 0x00, 0x00, 0x00,
0x00, 0x1f, 0xff, 0x00, 0xff, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x00, 0xff, 0xf0,
0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x00, 0xff, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f,
0xfe, 0x00, 0x7f, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x3f, 0xf0, 0xf8, 0x00,
0x00, 0x00, 0x00, 0x1f, 0xf8, 0x10, 0x1f, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x18,
0x0f, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x3c, 0x07, 0xf0, 0xf8, 0x00, 0x00, 0x00,
0x00, 0x1f, 0xc0, 0x7e, 0x03, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0xff, 0x03, 0xf0,
0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x81, 0xff, 0x81, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f,
0x03, 0xff, 0xc0, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x07, 0xff, 0xc0, 0x70, 0xff, 0xff,
0xfc, 0x00, 0x00, 0x1e, 0x07, 0xff, 0xe0, 0x70, 0xff, 0xff, 0xff, 0x00, 0x00, 0x1e, 0x0f, 0xff,
0xf0, 0xf0, 0xff, 0xff, 0xff, 0x80, 0x00, 0x1f, 0x9f, 0xff, 0xf9, 0xf0, 0xff, 0xff, 0xff, 0xc0,
0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x1f,
0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff,
0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x80, 0x1f, 0xff, 0xff,
0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff,
0xe0, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f,
0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0
};


//****************************************************************************************************************************
boolean VarioScreen::ScreenViewSound(void)
//****************************************************************************************************************************
{
	
/*  display.setFullWindow();
  display.firstPage();
  do
  {*/
// 	  display.fillScreen(ColorScreen);
//		display.clearScreen(ColorScreen);

	int volume = viewSound;
#ifdef SCREEN_DEBUG2
		SerialPort.println("Show : VolLevel");
#endif //SCREEN_DEBUG

#ifdef SCREEN_DEBUG2
		SerialPort.print("Volume : ");
		SerialPort.println(volume);
#endif //SCREEN_DEBUG

	if( xSemaphoreTake( screen.screenMutex, ( TickType_t ) 0 )  == pdTRUE) {

#ifdef SCREEN_DEBUG2
		SerialPort.println("ScreenViewSound : Take");	
#endif //SCREEN_DEBUG
  
		display.fillRect(60, 30, 60+75, 30+75, GxEPD_WHITE);
		
		if (volume == 0) display.drawInvertedBitmap(60, 30, volume75_4_icons, 75, 75, GxEPD_BLACK);
		else if (volume < 5) display.drawInvertedBitmap(60, 30, volume75_1_icons, 75, 75, GxEPD_BLACK);
		else if (volume < 8) display.drawInvertedBitmap(60, 30, volume75_2_icons, 75, 75, GxEPD_BLACK);
		else  display.drawBitmap(60, 30, volume75_3_icons, 75, 75, GxEPD_BLACK);

		display.fillRect(20, 117, 145, 55, GxEPD_WHITE);

		display.setFont(&FreeSansBold12pt7b);
		display.setTextColor(ColorText);
		display.setTextSize(2);

		if (volume == 10) display.setCursor(65, 160);
		else 							display.setCursor(65+12, 160);
		display.print(volume);
		
		if (ButtonScheduleur.Get_StatePage() == STATE_PAGE_CONFIG_SOUND) {
			display.drawRect(55, 120, 80, 50, GxEPD_BLACK);
			display.drawRect(54, 119, 80, 50, GxEPD_BLACK);
			display.drawRect(53, 118, 80, 50, GxEPD_BLACK);
			
			display.fillTriangle(140,130,160,145,140,160,GxEPD_BLACK);
			display.fillTriangle(45,130,25,145,45,160,GxEPD_BLACK);
		}
		
    xSemaphoreGive(screen.screenMutex);
#ifdef SCREEN_DEBUG2
		SerialPort.println("ScreenViewSound : Give");	
#endif //SCREEN_DEBUG
		return true;
	}
	else {
		return false;
	}			
 /* }
  while (display.nextPage());*/
}

//****************************************************************************************************************************
void VarioScreen::SetViewSound(int volume) {
//****************************************************************************************************************************
	viewSound = volume;
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//                          VARIOSCREEMULTI
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void ScreenZoneMulti::update(void) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
	SerialPort.print("VarioScreenMulti");	
#endif //SCREEN_DEBUG
  
//	display.drawRect(x, y, width, height, GxEPD_BLACK);
	display.fillRect(x, y, width, height, GxEPD_WHITE);
}


/************************/
/* The screen scheduler */
/************************/

//****************************************************************************************************************************
//****************************************************************************************************************************
//									SCREENSCHEDULER
//****************************************************************************************************************************
//****************************************************************************************************************************

//#define TIMER_DISPLAY

#ifdef TIMER_DISPLAY

hw_timer_t * timerScreenScheduler = NULL;
portMUX_TYPE timerMuxScreenScheduler = portMUX_INITIALIZER_UNLOCKED;

//****************************************************************************************************************************
void IRAM_ATTR onTimerScreenScheduler() {
//****************************************************************************************************************************
  portENTER_CRITICAL_ISR(&timerMuxScreenScheduler);

	if (millis() - oldtime >= GnuSettings.VARIOMETER_MULTIDISPLAY_DURATION)
	{
		oldtime  = millis();
		stateMulti = 1 - stateMulti;

#ifdef SCREEN_DEBUG
		led1stat = 1 - led1stat;
//		digitalWrite(pinLED, led1stat);   // turn the LED on or off
#endif //SCREEN_DEBUG
	}

//  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMuxScreenScheduler);
}

#endif //TIMER_DISPLAY

//****************************************************************************************************************************
ScreenScheduler::ScreenScheduler(ScreenSchedulerObject* displayList, uint8_t objectCount, int8_t startPage, int8_t endPage)
: displayList(displayList), objectCount(objectCount), pos(0), currentPage(startPage), endPage(endPage) {
//****************************************************************************************************************************
	
#ifdef SCREEN_DEBUG
/*	pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, LOW); */
	led1stat    = 0;  
#endif //SCREEN_DEBUG
	
	oldtime     = 0;
	stateMulti  = 0;

#ifdef TIMER_DISPLAY
	
	timerScreenScheduler = timerBegin(1, 80, true);
	timerAttachInterrupt(timerScreenScheduler, &onTimerScreenScheduler, true);
	timerAlarmWrite(timerScreenScheduler, 10000, true);  //100Hz
	timerAlarmEnable(timerScreenScheduler);
#endif //TIMER_DISPLAY
}
		 
//****************************************************************************************************************************
boolean ScreenScheduler::displayStep(void) {
//****************************************************************************************************************************

 /* if (stateDisplay != STATE_OK) {
#ifdef SCREEN_DEBUG2
		SerialPort.println("Task en cours");	
#endif //SCREEN_DEBUG
	  return;
	}*/

#ifdef SCREEN_DEBUG2
	SerialPort.println("displayStep");	
#endif //SCREEN_DEBUG

	if (currentPage == endPage+1) 
		return(screen.ScreenViewSound());

#ifndef TIMER_DISPLAY

	if (millis() - oldtime >= GnuSettings.VARIOMETER_MULTIDISPLAY_DURATION)
	{
		oldtime  = millis();
		stateMulti = 1 - stateMulti;

#ifdef SCREEN_DEBUG
		led1stat = 1 - led1stat;
//		digitalWrite(pinLED, led1stat);   // turn the LED on or off
#endif //SCREEN_DEBUG
	}

#endif //TIMER_DISPLAY

  /* next try to find something to display */
  /* for the current page                  */
#ifdef SCREEN_DEBUG2
  SerialPort.print("displaystep - objectCount  : ");
  SerialPort.println(objectCount);
#endif //SCREEN_DEBUG

  if( xSemaphoreTake( screen.screenMutex, ( TickType_t ) 0 )  == pdTRUE) {

#ifdef SCREEN_DEBUG2
		SerialPort.println("DisplayStep : Take");	
#endif //SCREEN_DEBUG

		display.setFullWindow();
 	
		if (millis() - oldtimeAllDisplay >= 900000)	{
		// refresh toutes les 15min
			oldtimeAllDisplay  = millis();	
			ShowDisplayAll = true;
//		display.fillRect(0, 0, display.width(), display.height(), GxEPD_WHITE);
			display.clearScreen();
//			display.writeScreenBuffer();
		
#ifdef SCREEN_DEBUG2
			SerialPort.println("displaystep - showDisplayAll");
#endif //SCREEN_DEBUG
		
		}
// Efface les zones multiScreen
	for(int i=0; i<screen.MaxZoneList; i++) {
		if (screen.ZoneMultiList[i].page == currentPage) screen.ZoneMultiList[i].update();
	}

		uint8_t n = 0;
		while( n <= objectCount ) {
/*   if( displayList[pos].page == currentPage && displayList[pos].actif == true && displayList[pos].object->update() ) {
	return;
}*/
#ifdef SCREEN_DEBUG
		SerialPort.print("displaylist number : ");
		SerialPort.println(n);
		SerialPort.print("displaylist / Page : ");
//  SerialPort.println(displayList[pos].page);
		SerialPort.print(displayList[n].page);
		SerialPort.print(" - Pageactive : ");
		SerialPort.println(currentPage);
	
#endif //SCREEN_DEBUG

/*	if( displayList[pos].page == currentPage && displayList[pos].actif == true) {
		ScreenDigit* tmppointeur = (ScreenDigit*)displayList[pos].object;
		tmppointeur->update(); 
	}*/

		currentMultiDisplay = 1 + stateMulti;
	
#ifdef SCREEN_DEBUG
		SerialPort.print("Current Multidisplay : ");
		SerialPort.println(currentMultiDisplay);
#endif //SCREEN_DEBUG

		if (ShowDisplayAll && displayList[n].page == currentPage && displayList[n].actif == true ) {
			displayList[n].object->reset();
			displayList[n].object->update(true);
#ifdef SCREEN_DEBUG
			SerialPort.println("displaystep - reset");
#endif //SCREEN_DEBUG
		}
		else if ( displayList[n].page == currentPage && displayList[n].actif == true && (displayList[n].multiDisplayID == 0 || displayList[n].multiDisplayID == currentMultiDisplay))	displayList[n].object->update();

/* next */
			pos++;
			if( pos == objectCount) {
				pos = 0;
			}
			n++;
		}

		if (ShowDisplayAll == true) {
			ShowDisplayAll = false;
//		display.fillRect(0, 0, display.width(), display.height(), GxEPD_WHITE);
		//.clearScreen();
		}

	
		screen.ScreenBackground(currentPage);
	
#ifdef SCREEN_DEBUG
		SerialPort.println("displayStep : Display");
#endif //SCREEN_DEBUG

/*  display.setCursor(10, 150);
  display.print("HelloWorld");*/

//    display.displayWindow(box_x, box_y, box_w, box_h);

//  display.display(true); // partial update
    xSemaphoreGive(screen.screenMutex);
#ifdef SCREEN_DEBUG2
		SerialPort.println("DisplayStep : Give");	
#endif //SCREEN_DEBUG
		return true;
	}
	else {
		return false;
	}
}

//****************************************************************************************************************************
void ScreenScheduler::displayAll(void) {
//****************************************************************************************************************************
  IsDisplayAll = false;
}

//****************************************************************************************************************************
int8_t ScreenScheduler::getPage(void) {
//****************************************************************************************************************************

  return currentPage;
}

//****************************************************************************************************************************
int8_t ScreenScheduler::getMaxPage(void) {
//****************************************************************************************************************************

  return endPage;
}

bool displayStat = true;

//****************************************************************************************************************************
void ScreenScheduler::setPage(int8_t page, boolean forceUpdate)  {
//****************************************************************************************************************************

  /* check if page change is needed */
  if ((forceUpdate == false) && ( page == currentPage )) return;

  /* set the new page */
  currentPage = page;

  /* screen need to by cleared */
//  display.clearScreen();
 // display.eraseDisplay();
  display.fillRect(0, 0, display.width(), display.height(), GxEPD_WHITE);

	if (currentPage == endPage+1) {
		displayStat = true;
		screen.SetViewSound(toneHAL.getVolume());
//		  screen.updateScreen ();
	} else if ((currentPage == endPage+2) && (displayStat)) {
		displayStat = false;
		screen.ScreenViewStatPage(0);
		screen.updateScreen ();

	} else {
		/* all the page object need to be redisplayed */
		/* but no problem to reset all the objects */
		displayStat = true;
		for(uint8_t i = 0; i<objectCount; i++) {
			displayList[i].object->reset();
		}
	}
}

//****************************************************************************************************************************
void ScreenScheduler::nextPage(void) {
//****************************************************************************************************************************
  
  uint8_t newPage = currentPage + 1;
  if( newPage > endPage + 2 ) {
    newPage = 0;
  }

  setPage(newPage);
}
 
//****************************************************************************************************************************
void ScreenScheduler::previousPage(void) {
//****************************************************************************************************************************
  
  int8_t newPage = currentPage - 1;
	
  if( newPage < 0 ) {
    newPage = endPage+2;
  }

  setPage(newPage);
} 

//****************************************************************************************************************************
void ScreenScheduler::enableShow(void) {
//****************************************************************************************************************************
	oldtime = millis();
#ifdef TIMER_DISPLAY
  timerAlarmEnable(timerScreenScheduler);
#endif //TIMER_DISPLAY
} 

void ScreenScheduler::disableShow(void) {
#ifdef TIMER_DISPLAY
  timerAlarmDisable(timerScreenScheduler);
#endif //TIMERDISPLAY
} 

//****************************************************************************************************************************
//****************************************************************************************************************************
//							MULTIDISPLAY
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void MultiDisplay::displayStep(void) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
	SerialPort.println("display step - Multidisplay");	
#endif //SCREEN_DEBUG

  if (lastFreqUpdate == 0) lastFreqUpdate = millis();
  unsigned long FreqDuration = millis() - lastFreqUpdate;
  if( FreqDuration > 1000 ) {
    lastFreqUpdate = millis();

    for(uint8_t i = 0; i<multiObjectCount; i++) {
      multiDisplayList[i].countTime--;
      if (multiDisplayList[i].countTime <= 0) {
         multiDisplayList[i].countTime = multiDisplayList[i].seconde;
         multiDisplayList[i].oldDisplayActif = multiDisplayList[i].displayActif;
         multiDisplayList[i].displayActif++;
         if (multiDisplayList[i].displayActif > multiObjectCount) multiDisplayList[i].displayActif = 0;
		 
#ifdef SCREEN_DEBUG
		 SerialPort.print("old display  : ");
         SerialPort.println(objectCount);
#endif //SCREEN_DEBUG
		 
         displayList[multiDisplayList[i].objectList[multiDisplayList[i].oldDisplayActif]].actif = false;       
         displayList[multiDisplayList[i].objectList[multiDisplayList[i].displayActif]].actif = true;  
 /*        displayList[8].actif = false;       
         displayList[9].actif = true;  */
		 
			}		 
		}
	}
}

#endif //VARIOSCREEN_SIZE

/*

500/portTICK_PERIOD_MS  = 500ms

    if (xSemaphore! = NULL)
    {
        / * Voyez si nous pouvons obtenir le sémaphore. Si le sémaphore n'est pas
        disponible attendez 10 ticks pour voir s'il devient gratuit. * /
        if (xSemaphoreTake (xSemaphore, (TickType_t) 10) == pdTRUE)
        {
            / * Nous avons pu obtenir le sémaphore et pouvons maintenant accéder au
            ressource partagée. * /

            / * ... * /

            / * Nous avons fini d'accéder à la ressource partagée. Relâchez le
            sémaphore. * /
            xSemaphoreGive (xSemaphore);
        }
        autre
        {
            / * Nous n'avons pas pu obtenir le sémaphore et ne pouvons donc pas accéder
            la ressource partagée en toute sécurité. * /
        }
    }
*/