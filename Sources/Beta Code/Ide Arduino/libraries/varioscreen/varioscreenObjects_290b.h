/* varioscreenGxEPD_290b -- 
 *
 * Copyright 2019 Jean-philippe GOI / Pierre
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
 *                           VarioScreenObjects                                  *
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
 *    1.0.9  22/08/19   Ajout ScreenViewReboot																	 *
 *    1.0.10 22/08/19   Ajout Page1                                              *
 *    1.0.11 23/28/19   Correction bug previousPage                              *
 *                      Ajout TUnit                                              *
 *    1.0.12 24/08/19   Ajout ScreenViewSound(int volume)                        *
 *		1.0.13 25/08/19		Gestion de l'écran de config du son											 *	
 *    1.0.14 23/09/19   Modification page stat                                   *
 *    1.1.0  25/09/19   Ajout multi screen                                       *
 *    1.1.1  28/09/19   Changement de nom de la librairie                        *
 *    1.1.2  06/10/19   Mise à jour ratamuse                                     *
 *    1.1.3  13/10/19   Integration au Gnuvario                                  *
 *                      Ajout WindUnit                                           *
 *    1.1.4  16/10/19   Ajout taille maximal de l'object												 *
 *    1.1.5  15/10/19   Modification affichage des satellites                    *
 *    1.1.6  20/10/19   Suppression classe GxEPD2_BW_U													 *
 *    1.1.7  03/11/19   Modification de la position des titres                   *
 *                      Modification DISPLAY_OBJECT_ALTI                         *
 *    1.1.8  16/11/19   Ajout classe GxEPD2_BW_U                                 *
 *    1.1.9  11/01/20   Modif VARIOSCREEN_SIZE == 290                            *
 *    1.1.10 07/02/20   Ajout 290 et 291                                         *
 *                      Ajouter FONTNORMAL / FONTLARGE Digit                     *
 *    1.0.11 19/02/20   Ajout variolog                                           *
 *    1.0.12 21/02/20   Correction Bug d'affichage batterie                      *
 *    1.1.13 05/03/20   Ajout affichage AGL                                      *
 *    1.1.14 06/03/20   Ajout gestion icone DISPLAY_OBJECT_TREND                 *
 *    1.1.15 09/03/20   Modification de l'effacement digit left                  *
 *    1.1.16 08/04/20   Modification affichage des titres                        *
 *    1.1.17 13/04/20   Titre en mode texte                                      *
 *    1.2.0  29/04/20   Modification font screedigit                             *
 *    1.2.1  15/05/20   Modification screentime                                  *
 *    1.2.2  17/05/20   Ajout setPositionTitle                                   *
 *    1.2.3  25/05/20   Modification screendigit.setvalue                        *
 *    1.2.4  31/05/21   Compatibilité écran 292 (GDEW029M06)                     *
 *                                                                               *
 *                                                                               *
 *********************************************************************************/

#ifndef VARIOSCREENOBJECTS_290B_H
#define VARIOSCREENOBJECTS_290B_H

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#if ((VARIOSCREEN_SIZE == 290) || (VARIOSCREEN_SIZE == 292))

#include <digit.h>
#include <variostat.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
//#include <Fonts/FreeSansBold18pt7b.h>
//#include <Fonts/FreeSansBold24pt7b.h>
//#include <Fonts/SpaceMonoBold12pt7b.h>
//#include <Fonts/FreeSerifBold18pt7b.h>
//#include <Fonts/FreeMonoBold9pt7b.h>
//#include <Fonts/FreeMonoBold24pt7b.h>
//#include <Fonts/FreeMonoBold22pt7b.h>
//#include <Fonts/FreeMonoBold18pt7b.h>
//#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
//#include <Fonts/jersey22pt7b.h>
//#include <Fonts/digital22pt7b.h>
//#include <Fonts/MONOFONT22pt7b.h>
//#include <Fonts/Audimat24pt7b.h>
//#include <Fonts/Audimat26pt7b.h>
#include <FreeSansBold8pt7b.h>
//#include <Fonts/jersey18pt7b.h>
#include <gnuvarioe14pt7b.h>
#include <gnuvarioe18pt7b.h>
#include <gnuvarioe23pt7b.h>
#include <NotoSans6pt7b.h> //Picopixel.h>
#include <gnuvarioe12pt7b.h>

/* minimum drift to update digit */
#define VARIOSCREEN_DIGIT_DISPLAY_THRESHOLD 0.65

/*  lipo battery percentage to voltage linear interpolation */
/*    voltage = a * percent + b                             */ 
#define VARIOSCREEN_BAT_A_COEFF 0.0059
#define VARIOSCREEN_BAT_B_COEFF 3.5534
#define VARIOSCREEN_BAT_PIXEL_COUNT 10
#define VARIOSCREEN_BAT_MULTIPLIER 6

#define ColorScreen    GxEPD_WHITE
#define ColorText      GxEPD_BLACK

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
#define DISPLAY_OBJECT_TEMPERATURE					26
#define DISPLAY_OBJECT_TUNIT        				27
#define DISPLAY_OBJECT_LINE									28
#define DISPLAY_OBJECT_CIRCLE								29
#define DISPLAY_OBJECT_WIND                 30
#define DISPLAY_OBJECT_LAT                  31
#define DISPLAY_OBJECT_LONG                 32  
#define DISPLAY_OBJECT_BEARING							33
#define DISPLAY_OBJECT_BEARING_TEXT         34
#define DISPLAY_OBJECT_LAT_DIR              35
#define DISPLAY_OBJECT_LONG_DIR             36  
#define DISPLAY_OBJECT_HEIGHT							  37

#define MAXW_OBJECT_VARIO 								 	85
#define MAXW_OBJECT_TIME									 	50
#define MAXW_OBJECT_SPEED								 		50
#define MAXW_OBJECT_ALTI									 	10
#define MAXW_OBJECT_DURATION							 	20
#define MAXW_OBJECT_RATIO       						20
#define MAXW_OBJECT_TREND      							20
#define MAXW_OBJECT_TEMPERATURE							20
//#define MAXW_OBJECT_LAT 									 	160
//#define MAXW_OBJECT_LONG									 	160
#define MAXW_OBJECT_LAT_DIR                 32
#define MAXW_OBJECT_LONG_DIR                32
#define MAXW_OBJECT_BEARING_TEXT            110
#define MAXW_OBJECT_BEARING                 100
#define MAXW_OBJECT_LAT 									 	160
#define MAXW_OBJECT_LONG									 	160

#include <VarioSettings.h>
extern VarioSettings GnuSettings;
extern VarioStat flystat;

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

#define ALIGNRIGHT  	0
#define ALIGNLEFT   	1
#define ALIGNCENTER		2

#define FONTNORMAL    0
#define FONTSMALL			1
#define FONTLARGE			2

class ScreenDigit: public VarioScreenObject {
// anchorX			Position en X
// anchorY			Position en You
// width				Nombre de digit comprenant le . et les chiffres après le pointer
//            	125 -> 3, 12.36 -> 5
// precision  	nombre de chiffre après la virgule
// plusDisplay 	affichage du + et du -
// zero 				affichage des zero en début de nombre
// Align     		Alignement 
// showtitle 		Affichage du titre
// TypeDigit		Type d'affichege (DISPLAY_OBJECT_VARIO, DISPLAY_OBJECT_TIME, ....)
// TitleX       Position du titre eb X
// TitleY       Position du titre en Y

 public :
   ScreenDigit(uint16_t anchorX, uint16_t anchorY, uint16_t width, uint16_t precision, boolean plusDisplay = false, boolean zero = false, int8_t Align = ALIGNLEFT, boolean showtitle = true, 	int8_t displayTypeID = 0, int8_t large = FONTNORMAL, int nbCarTitle = 0);
 //  : VarioScreenObject(0), anchorX(anchorX), anchorY(anchorY), width(width), precision(precision), plusDisplay(plusDisplay), zero(zero), leftAlign(leftAlign), showtitle(showtitle)
 // { lastDisplayWidth = 0; }
  void show(void);
  void setValue(double value);
	void setPositionTitle(uint16_t X, uint16_t Y);
   
 private:
  int digitsBe4Decimal(double number);
  char * dtostrf2(double number, signed char width, unsigned char prec, char *s, boolean zero);
  double value;
  double oldvalue=-1;
  const uint16_t anchorX, anchorY, width, precision;
  boolean plusDisplay, zero;	//, showtitle
	int8_t Align;
  boolean showtitle;
 uint8_t lastDisplayWidth;
	int16_t oldw=0, oldh=0, oldx=0, oldy=0;
	uint16_t Zwidth, Zheight;
	uint16_t MaxWidth, MaxHeight;
	int8_t displayTypeID;
  int8_t large;
	int nbCarTitle;
	int MaxTitleWidth, MaxTitleHeight;
	uint16_t titleX, titleY;
};

class ScreenText: public VarioScreenObject {
// anchorX			Position en X
// anchorY			Position en Y
// width				Nombre de caractère
// large        normal / large
// Align     		Alignement 
// showtitle 		Affichage du titre
// TypeDigit		Type d'affichege (DISPLAY_OBJECT_VARIO, DISPLAY_OBJECT_TIME, ....)
// TitleX       Position du titre en X
// TitleY       Position du titre en Y

 public :
   ScreenText(uint16_t anchorX, uint16_t anchorY, uint16_t width, int8_t large = FONTNORMAL, int8_t Align = ALIGNLEFT, boolean showtitle = true, 	int8_t displayTypeID = 0, int nbCarTitle = 0);
 //  : VarioScreenObject(0), anchorX(anchorX), anchorY(anchorY), width(width), precision(precision), plusDisplay(plusDisplay), zero(zero), leftAlign(leftAlign), showtitle(showtitle)
 // { lastDisplayWidth = 0; }
  void show(void);
  void setValue(String value);
	void setPositionTitle(uint16_t X, uint16_t Y);
   
 private:
  String value;
  String oldvalue="";
  const uint16_t anchorX, anchorY, width;
  int8_t large;
	int8_t Align;
  boolean showtitle;
  uint8_t lastDisplayWidth;
	int16_t oldw=0, oldh=0, oldx=0, oldy=0;
	uint16_t Zwidth, Zheight;
	uint16_t MaxWidth, MaxHeight;
	int8_t displayTypeID;
	int nbCarTitle;
	int MaxTitleWidth, MaxTitleHeight;
	uint16_t titleX, titleY;
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

class TUnit : public VarioScreenObject {

 public :
  TUnit(uint8_t posX, uint8_t posY)
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
	void setPositionTitle(uint16_t X, uint16_t Y);

 protected:
  const uint8_t posX;
  const uint8_t posY;
  ScreenDigit& hour, minute;
  int8_t time[3];
  bool dot_or_h = false;
	uint16_t titleX, titleY;
	bool titlePosition = false;
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

class BGLine : public VarioScreenObject {

 public :
  BGLine(uint8_t posX1, uint8_t posY1, uint8_t posX2, uint8_t posY2)
   : VarioScreenObject(1), posX1(posX1), posY1(posY1), posX2(posX2), posY2(posY2) { }
  void show(void);
	void toDisplay(void);
  
 private :
  const uint8_t posX1, posY1;
  const uint8_t posX2, posY2;
};

class BGCircle : public VarioScreenObject {

 public :
  BGCircle(uint8_t posX, uint8_t posY, uint8_t R)
   : VarioScreenObject(1), posX(posX), posY(posY), R(R) { }
  void show(void);
	void toDisplay(void);
  
 private :
  const uint8_t posX;
  const uint8_t posY;
	const uint8_t R;
};

/* Wind */
class WIND : public VarioScreenObject {

 public :
 WIND(uint8_t posX, uint8_t posY)
   : VarioScreenObject(1), posX(posX), posY(posY) { }
  void show(void);
	void toDisplay(void);
  
 private :
  const uint8_t posX;
  const uint8_t posY;
};

#endif //VARIOSCREEN_SIZE

#endif