/* varioscreenObjects -- 
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
 *    1.0.9  22/08/19   Ajout ScreenViewReboot									 *
 *    1.0.10 22/08/19   Ajout Page1                                              *
 *    1.0.11 23/08/19   Correction bug previousPage                              *
 *                      Ajout TUnit                                              *
 *    1.0.12 24/08/19   Ajout ScreenViewSound(int volume)                        *
 *	  1.0.13 25/08/19	Gestion de l'écran de config du son						 *	
 *    1.0.14 23/09/19   Modification page stat                                   *
 *                      Modification de l'affichage de la charge de la betterie  *
 *                      Ajout d'un deep-sleep en cas de batterie trop faible     *
 *    1.1.0  25/09/19   Ajout multi screen                                       *
 *    1.1.1  28/09/19   Changement de nom de la librairie                        *
 *    1.1.2  06/10/19   Correction bug fleche par Pierre                         *
 *                      Ajout nom des champs par Pierre                          *
 *    1.1.3  08/10/19   Ajout affichage ou non des champs de données             *
 *                      Déplacement de l'affichage des titres aux niveau de      *
 *                      varioDigit                                               *
 *                      Ajout affichage ou pas des titres                        *
 *    1.1.4  14/10/19   Modification affichage titre champs screendigit          *
 *    1.1.5  15/10/19   Modification affichage des satellites                    *
 *    1.1.6  03/11/19	Modification d'TUnit     			     				 *
 *    1.1.7  28/01/20   Correction leftAlign                                     *
 *                      Ajout ALIGNCENTER                                        *
 *                      Ajout Objet ScreenText                                   *
 *    1.1.8  09/02/20   Modif font screenText                                    *
 *    1.1.9  17/02/20   Ajout large (font) varioscreenDigit                      *
 *    1.1.10 19/02/20   Ajout variolog                                           *
 *    1.1.11 21/02/20   Correction Bug d'affichage batterie                      *
 *    1.1.12 05/03/20   Ajout affichage AGL                                      *
 *    1.1.13 06/03/20   Ajout gestion icone DISPLAY_OBJECT_TREND                 *
 *    1.1.14 09/03/20   Modification de l'effacement digit left                  *
 *    1.1.15 08/04/20   Modification affichage des titres                        *
 *    1.1.16 13/04/20   Titre en mode texte                                      *
 *    1.2.0  29/04/20   Modification font screedigit                             *
 *    1.2.1  17/05/20   Ajout setPositionTitle                                   *
 *    1.2.2  25/05/20   Modification screendigit.setvalue                        *
 *                                                                               *
 *********************************************************************************/
 
 /*
 *********************************************************************************
 *                    conversion image to cpp code                               *
 *                                                                               *
 *      https://javl.github.io/image2cpp/                                        *
 *                                                                               *
 *********************************************************************************/

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#if (VARIOSCREEN_SIZE == 154)

#include <varioscreenObjects_154.h>

#if defined(ESP32)
//static const char* TAG = "VarioScreen";
#include "esp_log.h"
#endif //ESP32

#ifdef SCREEN_DEBUG2
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include <VarioLog.h>

//#include <avr\dtostrf.h>
#include <stdlib.h>
#include <math.h>

/* http://javl.github.io/image2cpp/ */

#include <VarioButton.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "./GxEPD2_boards.h"
#include "./GxEPD2_boards_added.h"
//#include "GxEPD2_more_boards_added.h" // private

#include <imglib/gridicons_sync.h>
#include <varioscreenIcone_154.h>

#include <VarioSettings.h>
#include <toneHAL.h>

#include <Utility.h>

#include <VarioLanguage.h>

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

#define VARIOSCREEN_DOT_WIDTH 6
#define VARIOSCREEN_DIGIT_WIDTH 11

#ifndef ColorScreen
#define ColorScreen    GxEPD_WHITE
#endif

#ifndef ColorText
#define ColorText      GxEPD_BLACK
#endif

//***********************
//     GxEPD2_BW_U  
//***********************


/*template<typename GxEPD2_Type, const uint16_t page_height> void GxEPD2_BW_U<GxEPD2_Type, page_height>::updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
  if (state == STATE_OK) { 
#ifdef SCREEN_DEBUG
	  SerialPort.println("Screen State OK");
#endif 
    if (GxEPD2_BW<GxEPD2_Type, page_height>::_current_page != -1) return;
    if (GxEPD2_BW<GxEPD2_Type, page_height>::using_rotation) GxEPD2_BW<GxEPD2_Type, page_height>::_rotate(x, y, w, h);
  }
  
  if (x >= GxEPD2_Type::WIDTH) return;
  if (y >= GxEPD2_Type::HEIGHT) return;
  uint16_t xe = gx_uint16_min(GxEPD2_Type::WIDTH, x + w) - 1;
  uint16_t ye = gx_uint16_min(GxEPD2_Type::HEIGHT, y + h) - 1;
  uint16_t xs_d8 = x / 8;
  uint16_t xe_d8 = xe / 8;

  if (state == STATE_OK) { 
#ifdef SCREEN_DEBUG
	  SerialPort.println("Screen State OK");
#endif //SCREEN_DEBUG
    GxEPD2_Type::_Init_Part(0x03);
    _SetRamArea(xs_d8, xe_d8, y % 256, y / 256, ye % 256, ye / 256); // X-source area,Y-gate area
    _SetRamPointer(xs_d8, y % 256, y / 256); // set ram
//  _waitWhileBusy(0, 100); // needed ?
//  _waitWhileBusy2();
	state = STATE_BUSYMEMORY;
  }
  
  if ((state == STATE_BUSYMEMORY) && (digitalRead(_busy))) return;
  
  if (state == STATE_BUSYMEMORY) {
#ifdef SCREEN_DEBUG
	  SerialPort.println("Screen State BUSY Memory");
#endif //SCREEN_DEBUG
    _writeCommand(0x24);
    for (int16_t y1 = y; y1 <= ye; y1++)
    {
      for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++)
      {
        uint16_t idx = y1 * (GxGDEP015OC1_WIDTH / 8) + x1;
        uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
        _writeData(~data);
      }
    }
#ifdef SCREEN_DEBUG
	  SerialPort.println("Screen State BUSY Memory - Update Part");
#endif //SCREEN_DEBUG
		
    _Update_Part();
    delay(GxGDEP015OC1_PU_DELAY);
    // update erase buffer
#ifdef SCREEN_DEBUG		
	  SerialPort.println("Screen State BUSY Memory - Set Ram Area");
#endif //SCREEN_DEBUG
		
    _SetRamArea(xs_d8, xe_d8, y % 256, y / 256, ye % 256, ye / 256); // X-source area,Y-gate area
    _SetRamPointer(xs_d8, y % 256, y / 256); // set ram
//    _waitWhileBusy2();
	state = STATE_BUSYUPDATE;
#ifdef SCREEN_DEBUG	
	  SerialPort.println("Screen State BUSY Memory - State Busy Update");
#endif //SCREEN_DEBUG
  }
  
#ifdef SCREEN_DEBUG
	SerialPort.println("STATE_BUSYUPDATE - digital read");
#endif //SCREEN_DEBUG
	
  if ((state == STATE_BUSYUPDATE) && (digitalRead(_busy))) return;
  
  if (state == STATE_BUSYUPDATE) {
#ifdef SCREEN_DEBUG
	 SerialPort.println("Screen State Busy Update");
#endif //SCREEN_DEBUG

	_writeCommand(0x24);
    for (int16_t y1 = y; y1 <= ye; y1++)
    {
      for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++)
      {
        uint16_t idx = y1 * (GxGDEP015OC1_WIDTH / 8) + x1;
        uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
        _writeData(~data);
      }
    }
  delay(GxGDEP015OC1_PU_DELAY);
  state = STATE_OK;
  _PowerOff();
} */

//****************************************************************************************************************************
template<typename GxEPD2_Type, const uint16_t page_height> unsigned int GxEPD2_BW_U<GxEPD2_Type, page_height>::GetState(void){
//****************************************************************************************************************************
	return state;
}	

/*template<typename GxEPD2_Type, const uint16_t page_height> void GxEPD2_BW_U<GxEPD2_Type, page_height>::displayScreen(bool partial_update_mode = false) {

    // display buffer content to screen, useful for full screen buffer
#ifdef SCREEN_DEBUG
	SerialPort.println("display");	
#endif //SCREEN_DEBUG

	GxEPD2_BW::epd2.writeImage(GxEPD2_GFX::_buffer, 0, 0, GxEPD2_EPD::WIDTH, GxEPD2_GFX::_page_height);
	
#ifdef SCREEN_DEBUG
	SerialPort.println("display : writeImage");	
#endif //SCREEN_DEBUG
	
	GxEPD2_BW::epd2.refresh(partial_update_mode);
	
#ifdef SCREEN_DEBUG
	SerialPort.println("display : refresh");	
#endif //SCREEN_DEBUG

	if (epd2.hasFastPartialUpdate)
	{
		GxEPD2_BW::epd2.writeImageAgain(GxEPD2_BW::_buffer, 0, 0, GxEPD2_EPD::WIDTH, GxEPD2_BW::_page_height);
#ifdef SCREEN_DEBUG
		SerialPort.println("display : writeimageagain");	
#endif //SCREEN_DEBUG

	}
}*/

/**********/
/* screen */
/**********/



//****************************************************************************************************************************
//****************************************************************************************************************************
//                          VARIOSCREENOBJECT
//****************************************************************************************************************************
//****************************************************************************************************************************


/*****************************************/
/*  methods common to all screen objects */
/*****************************************/
#define display_needed() (state & 0x01)
#define display_done() state &= ~(0x01)
#define setDisplayFlag() state |= (0x01)

//****************************************************************************************************************************
bool VarioScreenObject::update(bool force) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
	SerialPort.print("VarioScreenObject : ");	
	if (force) 	SerialPort.println("Forceupdate");
	else				SerialPort.println("update");
#endif //SCREEN_DEBUG
  
//  show();
  if( display_needed() || force ) {
    show();
    display_done();
    return true;
  }

  return false;
}

//****************************************************************************************************************************
void VarioScreenObject::reset(void) {
//****************************************************************************************************************************
  setDisplayFlag();
}


//****************************************************************************************************************************
//****************************************************************************************************************************
//                            SCREENDIGIT
//****************************************************************************************************************************
//****************************************************************************************************************************

/* digit */
#define MAX_CHAR_IN_LINE 20
#define FONT_SIZE_WIDTH  22
#define FONT_SIZE_HEIGHT  7

//****************************************************************************************************************************
ScreenDigit::ScreenDigit(uint16_t anchorX, uint16_t anchorY, uint16_t width, uint16_t precision, boolean plusDisplay, boolean zero, int8_t Align, boolean showtitle, int8_t displayTypeID, int8_t large, int nbCarTitle) 
   : VarioScreenObject(0), anchorX(anchorX), anchorY(anchorY), width(width), precision(precision), plusDisplay(plusDisplay), zero(zero), Align(Align), showtitle(showtitle), displayTypeID(displayTypeID), large(large), nbCarTitle(nbCarTitle) { 
//****************************************************************************************************************************
  lastDisplayWidth = 0; 

  display.setFont(&gnuvarioe23pt7b); //Audimat26pt7b);
  if (large == FONTLARGE) 	display.setTextSize(2);
  else 						display.setTextSize(1);

  int16_t box_x = anchorX;
  int16_t box_y = anchorY;
  uint16_t w, h;
  int16_t box_w, box_h; 

#if defined(ESP32)
	ESP_LOGI(TAG, "ScreenDigit constructeur\n");
//  ESP_LOGE(TAG, "Failed to initialize the card (%d). Make sure SD card lines have pull-up resistors in place.", ret);

#endif //ESP32


#ifdef SCREEN_DEBUG	  
  SerialPort.println("");
  SerialPort.println("Constructeur ScreenDigit : ");
  SerialPort.print("Type :  ");
  SerialPort.println(displayTypeID);
  SerialPort.print("X : ");
  SerialPort.print(box_x);
  SerialPort.print("    Y: ");
  SerialPort.println(box_y);
  SerialPort.print("width : ");
  SerialPort.print(width);
  SerialPort.print("    precision :  ");
  SerialPort.println(precision);
  SerialPort.print("Alignement :  ");
  SerialPort.print(Align);
  SerialPort.print("    plus  :  ");
  SerialPort.println(plusDisplay);
	
#endif //SCREEN_DEBUG
  
	display.getTextBounds("0", 0, 100, &box_w, &box_h, &w, &h);

#ifdef SCREEN_DEBUG	  
  SerialPort.print("Max X : ");
  SerialPort.print(box_w);
  SerialPort.print(" -- Max Y : ");
  SerialPort.print(box_h);
  SerialPort.print(" -- width : ");
  SerialPort.print(w);
  SerialPort.print(" -- height :  ");
  SerialPort.println(h);
#endif //SCREEN_DEBUG
	
	int nbcar  = width;
		
	if (plusDisplay) nbcar++;
	
	MaxWidth   = (nbcar * w) + 6;	
	MaxHeight  = h + 3;

	if (precision > 0) {
		display.getTextBounds(".", 0, 100, &box_w, &box_h, &w, &h);
		MaxWidth += w;
	}

  if (large == FONTLARGE) {
		MaxWidth   = MaxWidth *2;	
		MaxHeight  = MaxHeight * 2;
	}

#ifdef SCREEN_DEBUG	  
  SerialPort.print("MaxWidth : ");
  SerialPort.print(MaxWidth);
  SerialPort.print("    MaxHeight : ");
  SerialPort.println(MaxHeight);
#endif


	display.setFont(&FreeSansBold8pt7b);
	display.getTextBounds("W", 0, 100, &box_w, &box_h, &w, &h);
	MaxTitleWidth   = (nbCarTitle * w);	
	MaxTitleHeight  = h;

  if (Align == ALIGNLEFT) {
		titleX = anchorX + 2;
		titleY = anchorY - MaxHeight - 1; 
	}
	else {
		titleX = anchorX - MaxWidth+2;
		if (titleX < 0) titleX = 2;
		titleY = anchorY - MaxHeight - 1; 		
	}

/*
	char TmpChar[MAX_CHAR_IN_LINE];

    int i,j,plus=0;
	
	if (plusDisplay) {
		TmpChar[width+1] = '+';
		plus = 1;
	}

	if (precision > 0) {
		for (i=0; i < width-precision-1;i++) TmpChar[i+plus] = '0';
		
		i= width-precision+plus-1;
		TmpChar[i++] = '.';

		for (j=0; j < precision;j++) TmpChar[i+j] = '0';
		
	} else {		
		for (i=0; i < width;i++) TmpChar[i+plus] = '0';			
	}

  TmpChar[width+plus+1] = '\0';		*
	dtostrf2(999999.999,width,precision,TmpChar,zero);

	
#ifdef SCREEN_DEBUG	  
  SerialPort.println(TmpChar);
#endif //SCREEN_DEBUG
	
	display.getTextBounds(TmpChar, box_x, box_y, &box_w, &box_h, &w, &h);

  if (w < box_x) 	{
		display.getTextBounds(TmpChar, 0, box_y, &box_w, &box_h, &w, &h);
		Zwidth   = w+2;
	} else {
	  Zwidth   = w-box_x+2;
	}

  if (large) Zheight  = 24+6;
  else       Zheight  = 12+3;

    switch (displayTypeID) {
		case DISPLAY_OBJECT_SPEED :
#if defined (MAXW_OBJECT_SPEED)		
		  MaxWidth   = MAXW_OBJECT_SPEED;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;
			
		case DISPLAY_OBJECT_TIME :
		case DISPLAY_OBJECT_DURATION :
#if defined (MAXW_OBJECT_TIME)		
		  MaxWidth   = MAXW_OBJECT_TIME;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;
			
		case DISPLAY_OBJECT_ALTI :
#if defined (MAXW_OBJECT_ALTI)		
		  MaxWidth   = MAXW_OBJECT_ALTI;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;

*		case DISPLAY_OBJECT_HEIGHT :
#if defined (MAXW_OBJECT_ALTI)		
		  MaxWidth   = MAXW_OBJECT_ALTI;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;*
			
		case DISPLAY_OBJECT_VARIO :
#if defined (MAXW_OBJECT_VARIO)		
		  MaxWidth   = MAXW_OBJECT_VARIO;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;
			
		case DISPLAY_OBJECT_TREND :
#if defined (MAXW_OBJECT_TREND)		
		  MaxWidth   = MAXW_OBJECT_TREND;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;

		case DISPLAY_OBJECT_RATIO :
#if defined (MAXW_OBJECT_RATIO)		
		  MaxWidth   = MAXW_OBJECT_RATIO;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;

		case DISPLAY_OBJECT_LAT :
#if defined (MAXW_OBJECT_LAT)		
		  MaxWidth   = MAXW_OBJECT_LAT;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;

		case DISPLAY_OBJECT_LONG :
#if defined (MAXW_OBJECT_LONG)		
		  MaxWidth   = MAXW_OBJECT_LONG;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;

		case DISPLAY_OBJECT_BEARING :
#if defined (MAXW_OBJECT_BEARING)		
		  MaxWidth   = MAXW_OBJECT_BEARING;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;
			
		default :
		  MaxWidth   = Zwidth;
			MaxHeight  = Zheight;
			break;
		}  */
}

//****************************************************************************************************************************
void ScreenDigit::setValue(double Value) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
	SerialPort.println("ScreenDigit : setValue");	
	SerialPort.print("Value = ");	
	SerialPort.println(Value);	
#endif //SCREEN_DEBUG

	int tmpInt;
	tmpInt = width - precision;
	if (precision > 0) tmpInt--;
	if (plusDisplay)   tmpInt--;
	double valueMax = 1;
	for (int i=0;i<tmpInt;i++) valueMax *= 10;
	double tmpPrecision = 1;
	for (int i=0;i<precision;i++) tmpPrecision /= 10;
	valueMax = valueMax - tmpPrecision;
	
	DUMP(valueMax);
	
	if (value > valueMax) value = valueMax;

  if (Value != oldvalue) {
    /* build digit and check changes */
    oldvalue=value;
    value=Value;
		
		DUMP(value);
//    reset();
	}
  reset();
}
 
//****************************************************************************************************************************
int ScreenDigit::digitsBe4Decimal(double number) {
//****************************************************************************************************************************
  int cnt = 1;  // Always has one digit

  // Count -ve sign as one digit
  if(number < 0.0) {
    cnt++;
    number = -number;
  }

  // Count the number of digits beyond the 1st, basically, the exponent.
  while(number >= 10.0) {
    number /= 10;
    cnt++;
  }
  return cnt;
}

//****************************************************************************************************************************
void ScreenDigit::setPositionTitle(uint16_t X, uint16_t Y) {
//****************************************************************************************************************************
	titleX = X;
	titleY = Y;
}

//****************************************************************************************************************************
char * ScreenDigit::dtostrf2(double number, signed char width, unsigned char prec, char *s, boolean zero) {
//****************************************************************************************************************************	
  char *out;
	unsigned long long integer;
	double fraction, rounding;
	int digit, before, i;
	int delta;

	if (isnan(number)) {
			strcpy(s, "nan");
			return s;
	}
	if (isinf(number)) {
			strcpy(s, "inf");
			return s;
	}

	// rounding up to the precision
	rounding = 0.5;
	for (i = 0; i < prec; ++i)
			rounding /= 10.0;

	if (number < 0.0)
			number -= rounding;
	else
			number += rounding;

	out = s;
	before = digitsBe4Decimal(number);
	
	// troncate before
	int tmpwidth = before;
	if (prec > 0) tmpwidth++;
	if (plusDisplay) tmpwidth++;
	if (tmpwidth > abs(width)) {
		if (width>0) {
		  before = width;
		  if (prec > 0) before--;
		  if (plusDisplay) before--;
		}
	}
	

	// check if padding is required
	if (width < 0) {
			delta = (-width) - (before + prec + 1);
			for (i = 0; i < delta; ++i) *out++ = ' ';
			width = 0;
	}

#ifdef SCREEN_DEBUG
	SerialPort.print("number  : ");
	SerialPort.print(number);
	SerialPort.print(" --  width  : ");
	SerialPort.print(width);
	SerialPort.print("  --  zero :");
	SerialPort.println(zero);
#endif //SCREEN_DEBUG
	
	// check if padding is required
	if ((width > 0) && (zero > 0)) {
		delta = width - (before + prec);
		if (prec > 0) delta -= 1; 
		if ((number < 0.0) ||  (plusDisplay)) delta -= 1; 
#ifdef SCREEN_DEBUG
		SerialPort.print("delta  : ");
		SerialPort.println(delta);
#endif //SCREEN_DEBUG
		for (i = 0; i < delta; ++i) {if (zero) {*out++ = '0';}}
	}

	// Handle negative numbers
	if (number < 0.0) {
		*out = '-';
		number = -number;
	}
	else if (plusDisplay) {
		*out = '+';
		++out;		
	}

	// seperate integral and fractional parts
	integer = (unsigned long long) number;
	fraction = (double) (number - integer);

	// generate chars for each digit of the integral part
	i = before;
	while (integer >= 10) {
			digit = integer % 10;
			out[(i--) - 1] = '0' + digit;
			integer /= 10;
	}

	out[i - 1] = '0' + integer;
	out += before;

	if (prec) {
			// generate chars for each digit of the fractional part
			*out++ = '.';
			for (i = 0; i < prec; ++i) {
					fraction *= 10.0;
					digit = ((unsigned long long) fraction) % 10;
					*out++ = (char) ('0' + digit);
			}
	}


	*out = 0;
	return s;
}


// ****************************************************************************************************************************
void ScreenDigit::show() {
// ****************************************************************************************************************************

  /***************/
  /* build digit */
  /***************/

#ifdef SCREEN_DEBUG
	SerialPort.println("Show : ScreenDigit");	
#endif //SCREEN_DEBUG

  //normalise value
  char digitCharacters[MAX_CHAR_IN_LINE];
	char tmpChar[MAX_CHAR_IN_LINE];

	display.setFont(&gnuvarioe23pt7b); //Audimat26pt7b);
  if (large == FONTLARGE) 	display.setTextSize(2);
	else 					display.setTextSize(1);


//  int16_t box_x = anchorX;
  int16_t box_y = anchorY;
  uint16_t w, h, w1, h1;
  int16_t box_w, box_w1; 
  int16_t box_h, box_h1; 
//	int16_t titleX, titleY;
//	int tmpWidth;

//	dtostrf2(999999.999,width,precision,tmpChar,zero);
	if (precision > 0) {
		dtostrf2(value,width,precision,digitCharacters,zero);
		strcpy(tmpChar,digitCharacters);
	}
	else {
		int num = value;
		itoa(num, digitCharacters, 10);
		
		strcpy(tmpChar, "");
		for (int i=0; i < strlen(digitCharacters); i++) strcat(tmpChar,"0");
		
		if (zero) {
		  while (strlen(digitCharacters) < width) {
				char tmpchar[10];
				strcpy(tmpchar, digitCharacters);
				strcpy(digitCharacters, "0");
				strcat(digitCharacters, tmpchar);
				
				strcat(tmpChar,"0");
			}
		}
	}


#ifdef SCREEN_DEBUG
  SerialPort.print("digit value : ");
  SerialPort.println(value);

  SerialPort.print("digit oldvalue : ");
  SerialPort.println(oldvalue);
  
  if (Align == ALIGNLEFT) 	SerialPort.println("leftAlign");
  else 										 	SerialPort.println("rightAlign");
	  
  SerialPort.print(digitCharacters);
  SerialPort.print("-- X : ");
  SerialPort.print(anchorX);
  SerialPort.print("  -- Y: ");
  SerialPort.print(anchorY);
  SerialPort.print("  -- width : ");
  SerialPort.print(width);
  SerialPort.print("  -- precision :  ");
  SerialPort.println(precision);
	SerialPort.print("MaxWidth : ");
	SerialPort.println(MaxWidth);
	SerialPort.print("MaxHeight : ");
	SerialPort.println(MaxHeight);
#endif //SCREEN_DEBUG
  
  display.getTextBounds(tmpChar, 0, box_y, &box_w, &box_h, &w, &h);
//  display.getTextBounds(digitCharacters, 0, box_y, &box_w1, &box_h1, &w1, &h1);
    
#ifdef SCREEN_DEBUG
  SerialPort.print("tmpChar : ");
  SerialPort.println(tmpChar);
  SerialPort.print("W : ");
  SerialPort.println(w);
  SerialPort.print("H : ");
  SerialPort.println(h);
#endif //SCREEN_DEBUG

	
  if (Align == ALIGNLEFT) {
	
#ifdef SCREEN_DEBUG
			SerialPort.println("left align");
#endif //SCREEN_DEBUG
	
//		if ((anchorX+w+2) > display.width()) w = display.width()-anchorX+2;

//		display.fillRect(anchorX, anchorY-Zheight-3, w+5, Zheight+4, GxEPD_WHITE);

//		display.drawRect(anchorX, anchorY-Zheight-3, w+5, Zheight+4, GxEPD_BLACK);

		display.fillRect(anchorX-1, anchorY-MaxHeight-3, MaxWidth+5, MaxHeight+6, GxEPD_WHITE);
	  
    display.setCursor(anchorX, anchorY-1);
    display.print(digitCharacters);
				
	} else {

#ifdef SCREEN_DEBUG
			SerialPort.println("right align");
#endif //SCREEN_DEBUG
			
//		if ((anchorX-w+2) < 0) w = anchorX - 2;

#ifdef SCREEN_DEBUG
		SerialPort.print("anchorX : ");
		SerialPort.println(anchorX);
//		SerialPort.print("W : ");
//		SerialPort.println(w);
#endif //SCREEN_DEBUG
	
//		display.fillRect(anchorX-w-2, anchorY-Zheight-3, w+6, Zheight+4, GxEPD_WHITE);
	
		display.fillRect(anchorX-MaxWidth-1, anchorY-MaxHeight-3, MaxWidth+7, MaxHeight+6, GxEPD_WHITE);

//		display.drawRect(anchorX-MaxWidth-1, anchorY-MaxHeight-3, MaxWidth+3, MaxHeight+6, GxEPD_BLACK);

    display.setCursor(anchorX-w, anchorY-1);  //MaxWidth, anchorY-1);
    display.print(digitCharacters);
	}
 
 /*  Affiche titre */
 
  if (showtitle) {
		
		display.fillRect(titleX-1, titleY-MaxTitleHeight-1, MaxTitleWidth+2, MaxTitleHeight+2, GxEPD_WHITE);
		display.setFont(&FreeSansBold8pt7b);
		display.setTextColor(ColorText);
		display.setTextSize(1);
		display.setCursor(titleX, titleY); 
		
    switch (displayTypeID) {
		case DISPLAY_OBJECT_SPEED :
//			display.drawInvertedBitmap(titleX+2, titleY-16, vitessetext, 47, 12, GxEPD_BLACK);		
//			display.setCursor(titleX, titleY); //titleX+2, titleY);
			display.print(varioLanguage.getText(TITRE_SPEED));
//			display.drawInvertedBitmap(0, 141, speedtext, 41, 14, GxEPD_BLACK);
			break;
			
/*		case DISPLAY_OBJECT_TIME :
//			display.drawInvertedBitmap(85, 141, timetext, 29, 11, GxEPD_BLACK);
			display.drawInvertedBitmap(titleX, titleY-14, heuretext, 38, 12, GxEPD_BLACK);
			break;
		case DISPLAY_OBJECT_DURATION :
			display.drawInvertedBitmap(titleX, titleY-14, tdvtext, 38, 12, GxEPD_BLACK);
			break;*/
			
		case DISPLAY_OBJECT_ALTI :
//		  TRACE();
//			display.drawInvertedBitmap(0, 31, altitext, 18, 11, GxEPD_BLACK);
//			display.drawInvertedBitmap(titleX-95, titleY-14, altitext, 18, 11, GxEPD_BLACK);
// 		display.fillRect(titleX, titleY-12, 45, 14, GxEPD_WHITE);
//			display.drawInvertedBitmap(titleX+3, titleY-12, altitext, 18, 12, GxEPD_BLACK);
//			display.setCursor(titleX+3, titleY-3); //titleX+2, titleY);
			display.print(varioLanguage.getText(TITRE_ALTI));
			break;
			
		case DISPLAY_OBJECT_VARIO :
//			display.drawInvertedBitmap(0, 85, variotext, 31, 11, GxEPD_BLACK);
//			display.drawInvertedBitmap(titleX, titleY-14, variotext, 31, 11, GxEPD_BLACK);
//			display.setCursor(titleX-7, titleY-3); //titleX+2, titleY);
			display.print(varioLanguage.getText(TITRE_VARIO));
			break;
			
		case DISPLAY_OBJECT_RATIO :
//			display.drawInvertedBitmap(132, 85, grtext, 21, 11, GxEPD_BLACK); //finesse/glade ratio
//  		display.fillRect(titleX, titleY-14, 51, 12, GxEPD_WHITE);
//			display.drawInvertedBitmap(titleX, titleY-14, finessetext, 51, 12, GxEPD_BLACK); //finesse/glade ratio
//			display.setCursor(titleX, titleY-3); //titleX+2, titleY);
			display.print(varioLanguage.getText(TITRE_FINESSE));

			break;
			
		case DISPLAY_OBJECT_TREND :
//			display.drawInvertedBitmap(132, 85, grtext, 21, 11, GxEPD_BLACK); //
//   		display.fillRect(titleX, titleY-14, 51, 12, GxEPD_WHITE);
//			display.drawInvertedBitmap(titleX, titleY-14, tdchutetext, 50, 12, GxEPD_BLACK); //taux de chute/sink rate
//			display.setCursor(titleX, titleY-3); //titleX+2, titleY);
			display.print(varioLanguage.getText(TITRE_TCHUTE));

			break;
			
    case DISPLAY_OBJECT_HEIGHT :
//		  TRACE();
//			display.drawInvertedBitmap(titleX, titleY-8, heighttext, 32, 14, GxEPD_BLACK);
//   		display.fillRect(titleX-30, titleY-12, 45, 14, GxEPD_WHITE);
//			display.drawInvertedBitmap(titleX-29, titleY-13, altisoltext, 41, 12, GxEPD_BLACK);
//			display.setCursor(titleX-30, titleY-3); //titleX+2, titleY);
			display.print(varioLanguage.getText(TITRE_AGL));
			break;
			
			case DISPLAY_OBJECT_BEARING :
//  		display.fillRect(titleX+10, titleY-15, 30, 16, GxEPD_WHITE);
//			display.drawInvertedBitmap(titleX+10, titleY-15, captext, 26, 15, GxEPD_BLACK);
//			display.setCursor(titleX, titleY-3); //titleX+2, titleY);
			display.print(varioLanguage.getText(TITRE_CAP));

			break;

		default :
			break;
		}
	}

}






/*
// ****************************************************************************************************************************
void ScreenDigit::show() {
// ****************************************************************************************************************************

  // * **************/
  /* build digit */
  /* **************

#ifdef SCREEN_DEBUG
	SerialPort.println("Show : ScreenDigit");	
#endif //SCREEN_DEBUG

  //normalise value
//  char tmpdigitCharacters[MAX_CHAR_IN_LINE];
  char digitCharacters[MAX_CHAR_IN_LINE];
	char tmpChar[MAX_CHAR_IN_LINE];
   
//  display.setFont(&FreeSansBold12pt7b);
  display.setFont(&FreeSansBold12pt7b);
  display.setTextSize(2);

  int16_t box_x = anchorX;
  int16_t box_y = anchorY;
  uint16_t w, h, w1, h1;
  int16_t box_w, box_w1; 
  int16_t box_h, box_h1; 
	int tmpWidth;

	dtostrf2(999999.999,width,precision,tmpChar,zero);

 // dtostrf(value,width,precision,tmpdigitCharacters);
 dtostrf2(value,width,precision,digitCharacters,zero);
//  dtostrf2(oldvalue,width,precision,tmpdigitCharacters,zero);
//  dtostrf2(value,4,1,digitCharacters,false,false);
  // *if (plusDisplay) {
	  if (value >=0) {
		sprintf(digitCharacters, "+%s", tmpdigitCharacters);  
	  }
	  else {
		sprintf(digitCharacters, "-%s", tmpdigitCharacters);  		  
      }
  }
  
  if (strlen(digitCharacters) > 
  if (rightAlign) {
	  while (strlen(digitCharacters
  }*/
 
 /* int i=0;
  while( i < strlen(digitCharacters)) { tmpdigitCharacters[i++] = '0'; }
  tmpdigitCharacters[i] = 0; 

  SerialPort.print(" tmpdigit : ");
  SerialPort.println(tmpdigitCharacters);*

#ifdef SCREEN_DEBUG
  SerialPort.print("digit value : ");
  SerialPort.println(value);

  SerialPort.print("digit oldvalue : ");
  SerialPort.println(oldvalue);
  
  if (leftAlign) SerialPort.println("leftAlign");
  else 			 SerialPort.println("rightAlign");
	  
  SerialPort.print(digitCharacters);
  SerialPort.print("-- X : ");
  SerialPort.print(box_x);
  SerialPort.print("-- Y: ");
  SerialPort.print(box_y);
  SerialPort.print("-- width : ");
  SerialPort.print(width);
  SerialPort.print("-- precision :  ");
  SerialPort.println(precision);
	SerialPort.print("Zwidth : ");
	SerialPort.println(Zwidth);
	SerialPort.print("Zheight : ");
	SerialPort.println(Zheight);
#endif //SCREEN_DEBUG
  
//  display.getTextBounds(digitCharacters, box_x, box_y, &box_w, &box_h, &w, &h);
//////////  display.getTextBounds(digitCharacters, 0, box_y, &box_w, &box_h, &w, &h);
/////  display.getTextWidth(digitCharacters, &w, &h);
//  display.getTextBounds(tmpdigitCharacters, box_x, box_y, &box_w1, &box_h1, &w1, &h1);

  display.getTextBounds(tmpChar, 0, box_y, &box_w, &box_h, &w, &h);
  
//  if (h1 > h) {h = h1;}
//  if (w1 > w) {w = w1;}
  
#ifdef SCREEN_DEBUG
// *  SerialPort.print("X : ");
  SerialPort.println(box_x);
  SerialPort.print("Y : ");
  SerialPort.println(box_y);
  SerialPort.print("X new : ");
  SerialPort.println(box_w);
  SerialPort.print("Y new : ");
  SerialPort.println(box_h);*
  SerialPort.print("W : ");
  SerialPort.println(w);
  SerialPort.print("H : ");
  SerialPort.println(h);
	SerialPort.print("Zwidth : ");
	SerialPort.println(Zwidth);
	SerialPort.print("Zheight : ");
	SerialPort.println(Zheight);
#endif //SCREEN_DEBUG
  
// *  if (leftAlign) {
	if ((box_x+w1+6) > 200)  
      display.fillRect(box_x, box_h-3, 200-box_x, h1+3, GxEPD_WHITE);
    else
      display.fillRect(box_x, box_h-3, w1+6, h1+3, GxEPD_WHITE);
    display.setCursor(box_x, box_y);
    display.print(digitCharacters);
  }
  else {
		if ((box_x-(w1+6)) > 0) { 
#ifdef SCREEN_DEBUG
			SerialPort.print("right align > 0");
#endif //SCREEN_DEBUG
 
      display.fillRect(box_x-w1, box_h-3, 200-box_x, h1+3, GxEPD_WHITE);
		}
		else {
      display.fillRect(box_x-w1, box_h-3, w1+6, h1+3, GxEPD_WHITE);
		}

//  display.drawRect(box_x-w, box_h+1, w+1, h, GxEPD_BLACK);
    display.setCursor(box_x-w-1, box_y);
    display.print(digitCharacters);
//  display.updateWindow(box_w, box_h, w+box_w, h, true);	  
  }*
	
  if (leftAlign) {
	
#ifdef SCREEN_DEBUG
			SerialPort.println("left align");
#endif //SCREEN_DEBUG
	
#ifdef SCREEN_DEBUG
		SerialPort.print("oldX : ");
		SerialPort.println(oldx);
		SerialPort.print("oldY : ");
		SerialPort.println(oldy);
		SerialPort.print("oldW : ");
		SerialPort.println(oldw);
		SerialPort.print("oldH : ");
		SerialPort.println(oldh);
		SerialPort.print("Zwidth : ");
		SerialPort.println(Zwidth);
		SerialPort.print("Zheight : ");
		SerialPort.println(Zheight);
#endif //SCREEN_DEBUG
	
		if ((oldw != 0) && (oldh != 0)) {
////			display.fillRect(oldx, oldy, oldw+2, oldh, GxEPD_WHITE);
//			display.fillRect(anchorX-2, anchorY-Zwidth-2, Zwidth+2, Zheight+2, GxEPD_WHITE);
//    	display.drawRect(oldx, oldy, oldw+2, oldh, GxEPD_BLACK);
		}	
	
 //   display.drawRect(box_x, box_y-h-2, w+6, h+6, GxEPD_BLACK);

		if ((anchorX+Zwidth+2) > display.width()) tmpWidth = display.width()-anchorX+2;
		else																			tmpWidth = Zwidth+4;

#ifdef SCREEN_DEBUG
		SerialPort.print("tmpWidth : ");
		SerialPort.println(tmpWidth);
#endif //SCREEN_DEBUG

		display.fillRect(anchorX-2, anchorY-Zheight-3, tmpWidth, Zheight+4, GxEPD_WHITE);
	  
    display.setCursor(box_x, box_y-1);
    display.print(digitCharacters);
		
		oldx = box_x-1;
		oldy = box_y-h-1;
		oldw = w+2;
		oldh = h+2;	
		
	} else {

#ifdef SCREEN_DEBUG
			SerialPort.println("right align");
#endif //SCREEN_DEBUG
	
#ifdef SCREEN_DEBUG
		SerialPort.print("oldX : ");
		SerialPort.println(oldx);
		SerialPort.print("oldY : ");
		SerialPort.println(oldy);
		SerialPort.print("oldW : ");
		SerialPort.println(oldw);
		SerialPort.print("oldH : ");
		SerialPort.println(oldh);
		SerialPort.print("Zwidth : ");
		SerialPort.println(Zwidth);
		SerialPort.print("Zheight : ");
		SerialPort.println(Zheight);
#endif //SCREEN_DEBUG
	
		if ((oldw != 0) && (oldh != 0)) {
////			display.fillRect(oldx, oldy, oldw+4, oldh, GxEPD_WHITE);
//				display.fillRect(anchorX-Zwidth-2, anchorY-Zheight-2, Zwidth+2, Zheight+2, GxEPD_WHITE);
//			display.drawRect(oldx, oldy, oldw+4, oldh, GxEPD_BLACK);
		}	
			
		//display.drawRect(box_x-w-6, box_y-h-2, w+6, h+6, GxEPD_BLACK);

		if ((anchorX+4) > display.width()) tmpWidth = Zwidth + (display.width()-anchorX);
		else															 tmpWidth = Zwidth+5;
		
		if ((anchorX-tmpWidth+6) < 0) tmpWidth = anchorX + 6;

#ifdef SCREEN_DEBUG
		SerialPort.print("anchorX : ");
		SerialPort.println(anchorX);
		SerialPort.print("tmpWidth : ");
		SerialPort.println(tmpWidth);
#endif //SCREEN_DEBUG
					
		display.fillRect(anchorX-tmpWidth+6, anchorY-Zheight-3, tmpWidth-1, Zheight+4, GxEPD_WHITE);
		
    display.setCursor(box_x-w-1, box_y-1);
    display.print(digitCharacters);
		
		oldx = box_x-w-2;
		oldy = box_y-h-1;
		oldw = w+2;
		oldh = h+2;	
	}
 

}
*/



//****************************************************************************************************************************
//****************************************************************************************************************************
//				ScreenText
//****************************************************************************************************************************
//****************************************************************************************************************************



//****************************************************************************************************************************
ScreenText::ScreenText(uint16_t anchorX, uint16_t anchorY, uint16_t width, int8_t large, int8_t Align, boolean showtitle, int8_t displayTypeID, int nbCarTitle) 
   : VarioScreenObject(0), anchorX(anchorX), anchorY(anchorY), width(width), large(large), Align(Align), showtitle(showtitle), displayTypeID(displayTypeID) , nbCarTitle(nbCarTitle) { 
//****************************************************************************************************************************
  lastDisplayWidth = 0; 

  display.setFont(&gnuvarioe18pt7b); //Audimat24pt7b);
	if (large== FONTLARGE) display.setTextSize(2);
	else 			 display.setTextSize(1);	
	
  int16_t box_x = anchorX;
  int16_t box_y = anchorY;
  uint16_t w, h;
  int16_t box_w, box_h; 
	int tmpWidth;

#if defined(ESP32)
	ESP_LOGI(TAG, "ScreenText constructeur");
//  ESP_LOGE(TAG, "Failed to initialize the card (%d). Make sure SD card lines have pull-up resistors in place.", ret);
#endif //ESP32


#ifdef SCREEN_DEBUG	  
  SerialPort.print("Constructeur ScreenText : ");
  SerialPort.print("  -- X : ");
  SerialPort.print(anchorX);
  SerialPort.print("  -- Y: ");
  SerialPort.print(anchorY);
  SerialPort.print("  -- width : ");
  SerialPort.print(width);
#endif //SCREEN_DEBUG
  
	display.getTextBounds("W", 0, 100, &box_w, &box_h, &w, &h);

#ifdef SCREEN_DEBUG	  
  SerialPort.print("Max X : ");
  SerialPort.print(box_w);
  SerialPort.print(" -- Max Y : ");
  SerialPort.print(box_h);
  SerialPort.print(" -- width : ");
  SerialPort.print(w);
  SerialPort.print(" -- height :  ");
  SerialPort.println(h);
#endif //SCREEN_DEBUG
	
	int nbcar  = width;
			
	MaxWidth   = (nbcar * w) + 6;	
	MaxHeight  = h + 3;

  if (large == FONTLARGE) {
		MaxWidth   = MaxWidth *2;	
		MaxHeight  = MaxHeight * 2;
	}

#ifdef SCREEN_DEBUG	  
  SerialPort.print("MaxWidth : ");
  SerialPort.print(MaxWidth);
  SerialPort.print("    MaxHeight : ");
  SerialPort.println(MaxHeight);
#endif
	

/*	
//Calcul le nombre de caractère maximum affichable

	int tmpwidth = 0 ;
	
	if (Align == ALIGNLEFT) {
		tmpwidth = display.width() - anchorX;
	}
	else if (Align == ALIGNRIGHT) {
  	tmpwidth = anchorX - display.width();
	}
	else {
	}

	if (tmpwidth < 0) tmpwidth = 0;
	
	float tmpcar;
	int   carwidth;
	if (large) carwidth = 14;
	else       carwidth = 7;
		
	tmpcar = tmpwidth / carwidth;

  if (width > int(tmpcar))  {
		Zwidth = int(tmpcar) * carwidth;
		width    = int(tmpcar);
	}	else {
		Zwidth = width * carwidth;
	}
	
	if (large) Zheight  = 24+6;
	else       Zheight  = 18+3;
		
    switch (displayTypeID) {
		case DISPLAY_OBJECT_BEARING_TEXT :
#if defined (MAXW_OBJECT_BEARING_TEXT)		
		  MaxWidth   = MAXW_OBJECT_BEARING_TEXT;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;
			
		case DISPLAY_OBJECT_LAT_DIR :
#if defined (MAXW_OBJECT_LAT_DIR)		
		  MaxWidth   = MAXW_OBJECT_LAT_DIR;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;
			
		case DISPLAY_OBJECT_LONG_DIR :
#if defined (MAXW_OBJECT_LONG_DIR)		
		  MaxWidth   = MAXW_OBJECT_LONG_DIR;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;
			
		case DISPLAY_OBJECT_LAT :
#if defined (MAXW_OBJECT_LAT)		
		  MaxWidth   = MAXW_OBJECT_LAT;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;

		case DISPLAY_OBJECT_LONG :
#if defined (MAXW_OBJECT_LONG)		
		  MaxWidth   = MAXW_OBJECT_LONG;
#else
		  MaxWidth   = Zwidth;
#endif	
			MaxHeight  = Zheight;
			break;
			
		default :
		  MaxWidth   = Zwidth;
			MaxHeight  = Zheight;
			break;
		}
*/				
/*		
	display.getTextBounds("0", 0, 100, &box_w, &box_h, &w, &h);

#ifdef SCREEN_DEBUG	  
  SerialPort.print("Max X : ");
  SerialPort.print(box_w);
  SerialPort.print(" -- Max Y : ");
  SerialPort.print(box_h);
  SerialPort.print(" -- width : ");
  SerialPort.print(w);
  SerialPort.print(" -- height :  ");
  SerialPort.println(h);
#endif //SCREEN_DEBUG
	
	int nbcar  = width;
		
	if (plusDisplay) nbcar++;
	
	MaxWidth   = (nbcar * w) + 6;	
	MaxHeight  = h + 3;

	if (precision > 0) {
		display.getTextBounds(".", 0, 100, &box_w, &box_h, &w, &h);
		MaxWidth += w;
	}

  if (large) {
		MaxWidth   = MaxWidth *2;	
		MaxHeight  = MaxHeight * 2;
	}

#ifdef SCREEN_DEBUG	  
  SerialPort.print("MaxWidth : ");
  SerialPort.print(MaxWidth);
  SerialPort.print("    MaxHeight : ");
  SerialPort.println(MaxHeight);
#endif
*/

	display.setFont(&FreeSansBold8pt7b);  //&NotoSans6pt7b);
	display.getTextBounds("W", 0, 100, &box_w, &box_h, &w, &h);
	MaxTitleWidth   = (nbCarTitle * w);	
	MaxTitleHeight  = h;	
	
	if (Align == ALIGNLEFT) {
		titleX = anchorX + 2;
		titleY = anchorY - MaxHeight - 1; 
	}
	else {
		titleX = anchorX - MaxWidth+2;
		if (titleX < 0) titleX = 2;
		titleY = anchorY - MaxHeight - 1; 		
	}

}

//****************************************************************************************************************************
void ScreenText::setValue(String Value) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
	SerialPort.println("ScreenText : setValue");	
	SerialPort.print("Value = ");	
	SerialPort.println(Value);	
#endif //SCREEN_DEBUG

  if (Value != oldvalue) {
    /* build digit and check changes */
    oldvalue=value;
    value=Value;
//    reset();
	}
  reset();
}
 
 //****************************************************************************************************************************
void ScreenText::setPositionTitle(uint16_t X, uint16_t Y) {
//****************************************************************************************************************************
	titleX = X;
	titleY = Y;
}

//****************************************************************************************************************************
void ScreenText::show() {
//****************************************************************************************************************************

  /***************/
  /* build digit */
  /***************/

#ifdef SCREEN_DEBUG
	SerialPort.println("Show : ScreenText");	
#endif //SCREEN_DEBUG
	
  //normalise value
//  char digitCharacters[MAX_CHAR_IN_LINE];
//	char tmpChar[MAX_CHAR_IN_LINE];

	display.setFont(&gnuvarioe18pt7b); //jersey18pt7b);
   
/*	if (large) 
	{
		display.setTextSize(1);
	}
	else 			 
	{
		display.setFont(&gnuvarioe18pt7b); //jersey18pt7b);
		display.setTextSize(1);	
	}*/
  if (large == FONTLARGE) display.setTextSize(2);
  else 					  display.setTextSize(1);

//  int16_t box_x = anchorX;
//  int16_t box_y = anchorY;
//  uint16_t w, h, w1, h1;
//  int16_t box_w, box_w1; 
//  int16_t box_h, box_h1; 
//	int16_t titleX, titleY;
//	int tmpWidth;

/*	dtostrf2(999999.999,width,precision,tmpChar,zero);
  dtostrf2(value,width,precision,digitCharacters,zero);

#ifdef SCREEN_DEBUG
  SerialPort.print("digit value : ");
  SerialPort.println(value);

  SerialPort.print("digit oldvalue : ");
  SerialPort.println(oldvalue);
  
  if (Align == ALIGNLEFT) 	SerialPort.println("leftAlign");
  else 										 	SerialPort.println("rightAlign");
	  
  SerialPort.print(digitCharacters);
  SerialPort.print("-- X : ");
  SerialPort.print(box_x);
  SerialPort.print("-- Y: ");
  SerialPort.print(box_y);
  SerialPort.print("-- width : ");
  SerialPort.print(width);
  SerialPort.print("-- precision :  ");
  SerialPort.println(precision);
	SerialPort.print("Zwidth : ");
	SerialPort.println(Zwidth);
	SerialPort.print("Zheight : ");
	SerialPort.println(Zheight);
#endif //SCREEN_DEBUG
  
  display.getTextBounds(tmpChar, 0, box_y, &box_w, &box_h, &w, &h);
  display.getTextBounds(digitCharacters, 0, box_y, &box_w1, &box_h1, &w1, &h1);
    
#ifdef SCREEN_DEBUG
  SerialPort.print("W : ");
  SerialPort.println(w);
  SerialPort.print("H : ");
  SerialPort.println(h);
#endif //SCREEN_DEBUG

	
  if (Align == ALIGNLEFT) {
	
#ifdef SCREEN_DEBUG
			SerialPort.println("left align");
#endif //SCREEN_DEBUG
	
		if ((anchorX+w+2) > display.width()) w = display.width()-anchorX+2;

		display.fillRect(anchorX, anchorY-Zheight-3, w+5, Zheight+4, GxEPD_WHITE);

//		display.drawRect(anchorX, anchorY-Zheight-3, w+5, Zheight+4, GxEPD_BLACK);
	  
    display.setCursor(anchorX, anchorY-1);
		titleX = anchorX + 4;
		titleY = anchorY - MaxHeight - 5; 
    display.print(digitCharacters);
				
	} else {

#ifdef SCREEN_DEBUG
			SerialPort.println("right align");
#endif //SCREEN_DEBUG
			
		if ((anchorX-w+2) < 0) w = anchorX - 2;

#ifdef SCREEN_DEBUG
		SerialPort.print("anchorX : ");
		SerialPort.println(anchorX);
		SerialPort.print("W : ");
		SerialPort.println(w);
#endif //SCREEN_DEBUG
	
//		display.fillRect(anchorX-w-2, anchorY-Zheight-3, w+6, Zheight+4, GxEPD_WHITE);
	
		display.fillRect(anchorX-MaxWidth-1, anchorY-MaxHeight-3, MaxWidth+5, MaxHeight+6, GxEPD_WHITE);

//		display.drawRect(anchorX-MaxWidth-1, anchorY-MaxHeight-3, MaxWidth+3, MaxHeight+6, GxEPD_BLACK);

    display.setCursor(anchorX-w1-2, anchorY-1);
		titleX = anchorX - MaxWidth;
		titleY = anchorY - MaxHeight - 5; 		
    display.print(digitCharacters);
	}*/
 
  if (Align == ALIGNLEFT) {
		display.fillRect(anchorX-1, anchorY-MaxHeight-6, MaxWidth+4, MaxHeight+10, GxEPD_WHITE);
		//display.drawRect(anchorX-1, anchorY-MaxHeight-6, MaxWidth+4, MaxHeight+10, GxEPD_BLACK);
		display.setCursor(anchorX, anchorY);
//		titleX = anchorX + 4;
//		titleY = anchorY - MaxHeight - 5; 
    display.print(value.substring(0,width));
	}
	else if (Align == ALIGNRIGHT) {
	}
	else {
	}
		
 /*  Affiche titre */
 
 /*display.setFont(&FreeSansBold8pt7b);
		display.setTextColor(ColorText);
		display.setTextSize(1);*/
 
  if (showtitle) {
		
		display.fillRect(titleX-8, titleY-MaxTitleHeight-1, MaxTitleWidth+2, MaxTitleHeight+2, GxEPD_WHITE);
		display.setFont(&FreeSansBold8pt7b);
		display.setTextColor(ColorText);
		display.setTextSize(1);
		display.setCursor(titleX, titleY); 
		
		
    switch (displayTypeID) {
		case DISPLAY_OBJECT_LAT :
//			display.setCursor(titleX-8, titleY-7); //titleX+2, titleY);
			display.println(varioLanguage.getText(TITRE_LAT));
//			display.drawInvertedBitmap(titleX-10, titleY-16, lattext, 20, 11, GxEPD_BLACK);
			break;
		case DISPLAY_OBJECT_LONG :
//			display.setCursor(titleX-8, titleY-7); //titleX+2, titleY);
			display.println(varioLanguage.getText(TITRE_LONG));
//			display.drawInvertedBitmap(titleX-10, titleY-16, longtext, 31, 14, GxEPD_BLACK);
			break;
		case DISPLAY_OBJECT_BEARING_TEXT :
//			display.setCursor(titleX-8, titleY-7); //titleX+2, titleY);
			display.println(varioLanguage.getText(TITRE_COMPAS));
//			display.drawInvertedBitmap(titleX-10, titleY-22, compastext, 55, 15, GxEPD_BLACK);
			break;

		default :
			break;
		}
	}

}


//****************************************************************************************************************************
//****************************************************************************************************************************
//				MUnit
//****************************************************************************************************************************
//****************************************************************************************************************************


//****************************************************************************************************************************
void MUnit::show() {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : MUnit");
#endif //SCREEN_DEBUG

  display.setFont(&FreeSansBold9pt7b);
  display.setTextSize(2);
  display.setCursor(posX+5, posY-4);
  display.print('m');
}

//****************************************************************************************************************************
void MUnit::toDisplay() {
//****************************************************************************************************************************
   reset();
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//							MSUnit
//****************************************************************************************************************************
//****************************************************************************************************************************

/*{B000000111100001111000000,
B000001111110011111100000,
B000011100001110000110000,
B000011000001100000110000,
B000011000001100000110000,
B000011000001100000110000,
B000011000001100000110000,
B000011000001100000110000,
B000011000001100000110000,
B000000000000000000000000,
B011111111111111111111110,
B011111111111111111111110,
B000000000000000000000000,
B000000000111111110000000,
B000000001111111110000000,
B000000001100000000000000,
B000000001100000000000000,
B000000001111111100000000,
B000000000111111110000000,
B000000000000000110000000,
B000000000000000110000000,
B000000001111111110000000,
B000000001111111000000000,
B000000000000000000000000}*/

  // 24 x 24 
const unsigned char msicons[] = { 
 /*   0x03, 0xC3, 0xC0, 
	0x07, 0xE7, 0xE0, 
	0x0E, 0x1C,	0x30, 	
	0x0C, 0x18, 0x30, 	
	0x0C, 0x18, 0x30, 
	0x0C, 0x18, 0x30,
	0x0C, 0x18, 0x30,	
	0x0C, 0x18, 0x30,	
	0x0C, 0x18, 0x30,	
	0x00, 0x00, 0x00, 
	0x3F, 0xFF,	0xFC, 
	0x3F, 0xFF, 0xFC, 
	0x00, 0x00, 0x00, 
	0x00, 0x7F, 0x80, 	
	0x00, 0xFF, 0x80,	
	0x00, 0xC0, 0x00,
	0x00, 0xC0, 0x00,
	0x00, 0xFF, 0x00, 	
	0x00, 0x7F,	0x80,
	0x00, 0x01, 0x80, 
	0x00, 0x01, 0x80, 
	0x00, 0xFF, 0x80, 
	0x00, 0xFF, 0x80, 
	0x00, 0x00, 0x00*/
	0x00, 0x00, 0x00,
	0x07, 0xFF, 0xE0,
	0x0F, 0xFF, 0xF0,
	0x0C, 0x18, 0x30,
	0x0C, 0x18, 0x30,
	0x0C, 0x18, 0x30,
	0x0C, 0x18, 0x30,
	0x0C, 0x18, 0x30,
	0x0C, 0x18, 0x30,
	0x00, 0x00, 0x00,
	0x3F, 0xFF,	0xFC, 
	0x3F, 0xFF, 0xFC, 
	0x00, 0x00, 0x00, 
	0x00, 0x7F, 0x80, 	
	0x00, 0xFF, 0x80,	
	0x00, 0xC0, 0x00,
	0x00, 0xC0, 0x00,
	0x00, 0xFF, 0x00, 	
	0x00, 0x7F,	0x80,
	0x00, 0x01, 0x80, 
	0x00, 0x01, 0x80, 
	0x00, 0xFF, 0x80, 
	0x00, 0xFF, 0x80, 
	0x00, 0x00, 0x00
};

/*{B000000000000000000000000,
B000001111111111111100000,
B000011111111111111110000,
B000110000001100000011000,
B000110000001100000011000,
B000110000001100000011000,
B000110000001100000011000,
B000110000001100000011000,
B000110000001100000011000,
B000110000001100000011000,
B000000000000000000000000,
B000000000000000000000000,
B111111111111111111111111,
B111111111111111111111111,
B000000000000000000000000,
B000000000111111000000000,
B000000001100000000000000,
B000000001000000000000000,
B000000001100000000000000,
B000000000111110000000000,
B000000000000011000000000,
B000000000000001000000000,
B000000000000011000000000,
B000000001111110000000000}*/

//****************************************************************************************************************************
void MSUnit::show() {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : MSUnit");
#endif //SCREEN_DEBUG

// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
//	display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
//	display.drawRect(posX, posY, 24, 24, GxEPD_BLACK);
  display.drawBitmap(posX, posY, msicons, 24, 24, GxEPD_BLACK);   //GxEPD_BLACK);
//display.drawInvertedBitmap	
//  display.drawBitmap(100, 10, gridicons_sync, 24, 24, GxEPD_BLACK);
}

//****************************************************************************************************************************
void MSUnit::toDisplay() {
//****************************************************************************************************************************
    reset();
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//				KMHUnit
//****************************************************************************************************************************
//****************************************************************************************************************************

/*{B000000000000000000000000,
B001100001001111111111000,
B001100011011111111111100,
B001100110011000110001100,
B001111100011000110001100,
B001111000011000110001100,
B001101100011000110001100,
B001100110011000110001100,
B001100011011000110001100,
B000000000000000000000000,
B001111111111111111111100,
B001111111111111111111100,
B000000000000000000000000,
B000000001100000000000000,
B000000001100000000000000,
B000000001100000000000000,
B000000001100000000000000,
B000000001111111100000000,
B000000001111111110000000,
B000000001100000110000000,
B000000001100000110000000,
B000000001100000110000000,
B000000001100000110000000,
B000000001100000110000000}*/

  // 24 x 24 
const unsigned char kmhicons[] = { 
    0x00, 0x00, 0x00, 0x30, 0x9F, 0xF8, 0x31, 0xBF,	
	0xFC, 0x33, 0x31, 0x8C, 0x3E, 0x31, 0x8C, 0x3C,
	0x31, 0x8C, 0x36, 0x31, 0x8C, 0x33, 0x31, 0x8C,
	0x31, 0xB1, 0x8C, 0x00, 0x00, 0x00, 0x3F, 0xFF,
	0xFC, 0x3F, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00,
	0xC0, 0x00, 0x00, 0xC0, 0x00, 0x00, 0xC0, 0x00,
	0x00, 0xC0, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0xC1, 0x80, 0x00, 0xC1, 0x80, 0x00, 
	0xC1, 0x80, 0x00, 0xC1, 0x80, 0x00, 0xC1, 0x80
};

//****************************************************************************************************************************
void KMHUnit::show() {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : KmHUnit");
#endif //SCREEN_DEBUG

 display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
 //display.drawRect(posX, posY, 24, 24, GxEPD_BLACK);

// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
 display.drawBitmap(posX, posY, kmhicons, 24, 24, GxEPD_BLACK);   //GxEPD_BLACK);
//  display.drawBitmap(100, 10, gridicons_sync, 24, 24, GxEPD_BLACK);
}

//****************************************************************************************************************************
void KMHUnit::toDisplay() {
//****************************************************************************************************************************
    reset();
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//				TUnit
//****************************************************************************************************************************
//****************************************************************************************************************************


//****************************************************************************************************************************
void TUnit::show() {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : TUnit");
#endif //SCREEN_DEBUG

  display.setFont(&FreeSansBold9pt7b);
  display.setTextSize(2);
  display.setCursor(posX, posY);
  display.print('C');
//  display.drawInvertedBitmap(posX, posY-20, temp, 21, 17, GxEPD_BLACK);
//  display.drawInvertedBitmap(posX-80, posY-40, thermo, 48, 48, GxEPD_BLACK);	
}

//****************************************************************************************************************************
void TUnit::toDisplay() {
//****************************************************************************************************************************
   reset();
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//						BATLEVEL
//****************************************************************************************************************************
//****************************************************************************************************************************

/*  // 48 x 48 
const unsigned char baticons[] = { 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf1, 0xff, 
0xff, 0xff, 0xfe, 0x3f, 0xe7, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xe6, 0xef, 0xff, 0xff, 0xff, 0x9f, 
0xe6, 0x6f, 0xff, 0xff, 0xff, 0x9f, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0x9f, 0xe6, 0x6f, 0xff, 0xff, 
0xff, 0x9f, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0x83, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0x83, 0xe6, 0x6f, 
0xff, 0xff, 0xff, 0xf3, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0xf3, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0xf3, 
0xe6, 0x6f, 0xff, 0xff, 0xff, 0xf3, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0xf3, 0xe6, 0x6f, 0xff, 0xff, 
0xff, 0xf3, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0x83, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0x83, 0xe6, 0x6f, 
0xff, 0xff, 0xff, 0x9f, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0x9f, 0xe6, 0x6f, 0xff, 0xff, 0xff, 0x9f, 
0xe6, 0xef, 0xff, 0xff, 0xff, 0x9f, 0xe7, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xf1, 0xff, 0xff, 0xff, 
0xfe, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};*/


const unsigned char bat0icons[] = { 
 // 'basic1-191_battery_empty-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x0f, 
0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xc7, 0xcf, 0xff, 0xff, 0xc3, 
0xcf, 0xff, 0xff, 0xc3, 0xcf, 0xff, 0xff, 0xc3, 0xcf, 0xff, 0xff, 0xc3, 0xcf, 0xff, 0xff, 0xc3, 
0xcf, 0xff, 0xff, 0xc7, 0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xc0, 0x00, 0x00, 0x0f, 
0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char bat1icons[] = { 
 // 'basic1-192_battery_1-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x0f, 
0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xcc, 0x7f, 0xff, 0xc7, 0xcc, 0x7f, 0xff, 0xc3, 
0xcc, 0x7f, 0xff, 0xc3, 0xcc, 0x7f, 0xff, 0xc3, 0xcc, 0x7f, 0xff, 0xc3, 0xcc, 0x7f, 0xff, 0xc3, 
0xcc, 0x7f, 0xff, 0xc7, 0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xc0, 0x00, 0x00, 0x0f, 
0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char bat2icons[] = { 
 // 'basic1-193_battery_2-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x0f, 
0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xcc, 0x63, 0xff, 0xc7, 0xcc, 0x63, 0xff, 0xc3, 
0xcc, 0x63, 0xff, 0xc3, 0xcc, 0x63, 0xff, 0xc3, 0xcc, 0x63, 0xff, 0xc3, 0xcc, 0x63, 0xff, 0xc3, 
0xcc, 0x63, 0xff, 0xc7, 0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xc0, 0x00, 0x00, 0x0f, 
0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char bat3icons[] = { 
 // 'basic1-194_battery_3-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x0f, 
0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xcc, 0x63, 0x1f, 0xc7, 0xcc, 0x63, 0x1f, 0xc3, 
0xcc, 0x63, 0x1f, 0xc3, 0xcc, 0x63, 0x1f, 0xc3, 0xcc, 0x63, 0x1f, 0xc3, 0xcc, 0x63, 0x1f, 0xc3, 
0xcc, 0x63, 0x1f, 0xc7, 0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xc0, 0x00, 0x00, 0x0f, 
0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char bat4icons[] = { 
 // 'basic1-195_battery_full-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x0f, 
0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xcc, 0x63, 0x18, 0xc7, 0xcc, 0x63, 0x18, 0xc3, 
0xcc, 0x63, 0x18, 0xc3, 0xcc, 0x63, 0x18, 0xc3, 0xcc, 0x63, 0x18, 0xc3, 0xcc, 0x63, 0x18, 0xc3, 
0xcc, 0x63, 0x18, 0xc7, 0xcf, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xcf, 0xc0, 0x00, 0x00, 0x0f, 
0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char batchargeicons[] = { 
 // 'basic1-196_battery_charging-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xfc, 0x7f, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 
0xff, 0xe0, 0x3f, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xc0, 0xe0, 0x06, 0x1f, 0xc0, 0xc0, 0x0c, 0x0f, 
0xcf, 0xc0, 0x1f, 0xcf, 0xcf, 0xf8, 0x3f, 0xcf, 0xcf, 0xf0, 0x3f, 0xcf, 0xcf, 0xf0, 0x7f, 0xc3, 
0xcf, 0xe0, 0xff, 0xc3, 0xcf, 0xe0, 0xff, 0xc3, 0xcf, 0xe0, 0xff, 0xc3, 0xcf, 0xc0, 0x3f, 0xc3, 
0xcf, 0xc0, 0x7f, 0xc7, 0xcf, 0xf8, 0x7f, 0xcf, 0xcf, 0xf8, 0xff, 0xcf, 0xc0, 0x19, 0xc0, 0x0f, 
0xc0, 0x19, 0x80, 0x0f, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

#define SIMPLE_VOLTAGE_VIEW

/* !!! always reset !!! */
//****************************************************************************************************************************
void BATLevel::setVoltage(int voltage) {
//****************************************************************************************************************************
  
/*#define VARIOSCREEN_BAT_A_COEFF 0.0059
#define VARIOSCREEN_BAT_B_COEFF 3.5534

2S %=(T-7,1068)/0,0118        

Tableau

0%    3v
5%    3.3v
10%   3.6v
20%   3.7v
30%   3.75v
40%   3.79v
50%   3.83v
60%   3.87v
70%   3.92v
80%   3.97v
90%   4.10v
100%  4.20v

*/
	
/*
Tension	ADC
4,2	2320
4,1	2260
4	2200
3,9	2160
3,8	2100
3,7	2040
3,6	1980
3,5	1920
3,4	1850
3,3	1790
3,2	1710
3,1	1660
3	1590
2,9	1560

Avec l'equation y=ax+b 
a=0.0016438
b=0.386384
*/
	
#if defined(VOLTAGE_DIVISOR_DEBUG)
  SerialPort.print("Voltage : ");
  SerialPort.println(voltage);
#endif //SCREEN_DEBUG

	DUMP(voltage);
	DUMPLOG(LOG_TYPE_DEBUG, VOLTAGE_DEBUG_LOG,voltage);
  Voltage = voltage;
	
#if not defined(SIMPLE_VOLTAGE_VIEW)
  uVoltage = (VOLTAGE_DIVISOR_VALUE * VOLTAGE_DIVISOR_REF_VOLTAGE * float(voltage))  / VOLTAGE_RESOLUTION;
	
#if defined(VOLTAGE_DIVISOR_DEBUG)
  SerialPort.print("uVoltage : ");
  SerialPort.println(uVoltage);
#endif //SCREEN_DEBUG
  
	float tmp = (uVoltage - VARIOSCREEN_BAT_B_COEFF) / VARIOSCREEN_BAT_A_COEFF;
  pVoltage = (uint8_t)(tmp);
//  uVoltage <<= VARIOSCREEN_BAT_MULTIPLIER;

#if defined(VOLTAGE_DIVISOR_DEBUG)
  SerialPort.print("pVoltage : ");
  SerialPort.println(pVoltage);
#endif //SCREEN_DEBUG

#if defined(VOLTAGE_DIVISOR_DEBUG)
  float percentage = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;
  if (voltage > 4.19) percentage = 100;
  else if (voltage <= 3.50) percentage = 0;
  SerialPort.println(String(percentage)+"%");
#endif //VOLTAGE_DIVISOR_DEBUG
#endif //SIMPLE_VOLTAGE_VIEW
  
  reset();
}

//****************************************************************************************************************************
void BATLevel::show(void) {
//****************************************************************************************************************************
/*

AnalogRead=X


if    2160<X<2320 = affichage de 4 barres

if    2320<X<2100 = 3 barres

if    2100<X<2000 =2 barre

if    2000<X<1750 = 1 barre (qui clignote)

if    X< 1700 = deep sleep (comme ça si la sécu batterie ne fonctionne pas ou si le vario ne plante pas avant, on préserve la batterie.
*/	


  /* battery base */

  /* battery level */
 /* uint16_t baseVoltage = base + inc;
  uint8_t pixelCount = 0;*/
#ifdef VOLTAGE_DIVISOR_DEBUG
  SerialPort.println("Show : BatLevel");
#endif //SCREEN_DEBUG

#if defined (SIMPLE_VOLTAGE_VIEW)

	DUMP(Voltage);
	DUMPLOG(LOG_TYPE_DEBUG, VOLTAGE_DEBUG_LOG,Voltage);
	
  display.fillRect(posX, posY, 32, 32, GxEPD_WHITE);
	
  if (Voltage >= 2160)
    display.drawInvertedBitmap(posX, posY, bat4icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else if ((Voltage < 2160) && (Voltage >= 2100))
    display.drawInvertedBitmap(posX, posY, bat3icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else if ((Voltage < 2100) && (Voltage >= 2000))
    display.drawInvertedBitmap(posX, posY, bat2icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else if ((Voltage < 2000) && (Voltage >= 1750))
    display.drawInvertedBitmap(posX, posY, bat1icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else {
    display.drawInvertedBitmap(posX, posY, bat0icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
		//Deep Sleep
	}

#else //SIMPLE_VOLTAGE_VIEW
#ifdef VOLTAGE_DIVISOR_DEBUG
  SerialPort.print("uVoltage : ");
  SerialPort.println(uVoltage);
#endif //SCREEN_DEBUG

  display.fillRect(posX, posY, 32, 32, GxEPD_WHITE);
//	display.drawRect(posX, posY, 32, 32, GxEPD_BLACK);
  
  if (pVoltage >= 75)
    display.drawInvertedBitmap(posX, posY, bat4icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else if (pVoltage >= 50)
    display.drawInvertedBitmap(posX, posY, bat3icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else if (pVoltage >= 25)
    display.drawInvertedBitmap(posX, posY, bat2icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else if (pVoltage >= 10)
    display.drawInvertedBitmap(posX, posY, bat1icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else
    display.drawInvertedBitmap(posX, posY, bat0icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
	  
 /* while( pixelCount < VARIOSCREEN_BAT_PIXEL_COUNT ) {
    if( baseVoltage < uVoltage ) {
      display.display( 0xff );
    } else {
      display.display( 0x81 );
    }

    baseVoltage += inc;
    pixelCount++;
  }*/

  /* battery end */
#endif //SIMPLE_VOLTAGE_VIEW

}

/*-----------------------------------------*/
/*              BATTERY                    */
/*-----------------------------------------*/
/*
#define BACKCOLOR 0x18E3
#define BARCOLOR 0x0620
#define SCALECOLOR 0xFFFF

//Analog Measurement Declarations
const int analogIn = A0;
int RawValue = 0;
int LastPercent = 0;
 

void setup() {
  tft.begin();
  tft.fillScreen(BACKCOLOR);
  drawScale(); 
}

void loop(){  
    int newPercent;
    RawValue = analogRead(analogIn);
    newPercent = int((RawValue/1024.0)* 100.0);
    
    if (newPercent != LastPercent){
      drawBar(newPercent);     
    }
     
}

void drawScale(){  
  tft.drawFastVLine(55, 20,100, SCALECOLOR ); // Vertical Scale Line  
  tft.drawFastHLine(47, 20, 8, SCALECOLOR); // Major Division
  tft.drawFastHLine(50, 44, 5, SCALECOLOR); // Minor Division
  tft.drawFastHLine(47, 69, 8, SCALECOLOR); // Major Division
  tft.drawFastHLine(50, 94, 5, SCALECOLOR); // Minor Division
  tft.drawFastHLine(47, 119, 8, SCALECOLOR);  // Major Division
}


void drawBar (int nPer){

  if(nPer < LastPercent){
    tft.fillRect(61, 20 + (100-LastPercent), 30, LastPercent - nPer,  BACKCOLOR);     
  }
  else{
    tft.fillRect(61, 20 + (100-nPer), 30, nPer - LastPercent,  BARCOLOR);
  }    
  LastPercent = nPer;  
  
}
*/

//****************************************************************************************************************************
//****************************************************************************************************************************
//						VOLLEVEL
//****************************************************************************************************************************
//****************************************************************************************************************************

 // 'basic1-094_volume-32'

const unsigned char volume0icons[] = { 
 // 'basic1-096_volume_mute-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 
0xff, 0xe0, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 
0xe0, 0x00, 0xf9, 0xcf, 0xe0, 0x00, 0xf0, 0x87, 0xe0, 0x00, 0xf8, 0x0f, 0xe0, 0x00, 0xfc, 0x1f, 
0xe0, 0x00, 0xfc, 0x1f, 0xe0, 0x00, 0xf8, 0x0f, 0xe0, 0x00, 0xf0, 0x87, 0xe0, 0x00, 0xf9, 0xcf, 
0xe0, 0x00, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 
0xff, 0xf8, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char volume1icons[] = { 
 // 'basic1-093_volume_low-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 
0xff, 0xe0, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 
0xe0, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xe7, 0xff, 0xe0, 0x00, 0xf7, 0xff, 0xe0, 0x00, 0xf7, 0xff, 
0xe0, 0x00, 0xf7, 0xff, 0xe0, 0x00, 0xe7, 0xff, 0xe0, 0x00, 0xe7, 0xff, 0xe0, 0x00, 0xff, 0xff, 
0xe0, 0x00, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 
0xff, 0xf8, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char volume2icons[] = { 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 
0xff, 0xe0, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xe0, 0x00, 0xfe, 0xff, 
0xe0, 0x00, 0xfe, 0x7f, 0xe0, 0x00, 0xe6, 0x7f, 0xe0, 0x00, 0xf7, 0x7f, 0xe0, 0x00, 0xf7, 0x7f, 
0xe0, 0x00, 0xf7, 0x7f, 0xe0, 0x00, 0xe7, 0x7f, 0xe0, 0x00, 0xe6, 0x7f, 0xe0, 0x00, 0xfe, 0x7f, 
0xe0, 0x00, 0xfe, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 
0xff, 0xf8, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char volume3icons[] = { 
 // 'basic1-095_volume_loud-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xf8, 0xff, 0x9f, 
0xff, 0xe0, 0xff, 0xdf, 0xff, 0xc0, 0xff, 0xcf, 0xff, 0x80, 0xff, 0xef, 0xe0, 0x00, 0xfe, 0xe7, 
0xe0, 0x00, 0xfe, 0x67, 0xe0, 0x00, 0xe6, 0x77, 0xe0, 0x00, 0xf7, 0x77, 0xe0, 0x00, 0xf7, 0x77, 
0xe0, 0x00, 0xf7, 0x77, 0xe0, 0x00, 0xe7, 0x77, 0xe0, 0x00, 0xe6, 0x77, 0xe0, 0x00, 0xfe, 0x67, 
0xe0, 0x00, 0xfe, 0xe7, 0xff, 0x80, 0xff, 0xef, 0xff, 0xc0, 0xff, 0xcf, 0xff, 0xe0, 0xff, 0x9f, 
0xff, 0xf8, 0xff, 0x9f, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//****************************************************************************************************************************
void VOLLevel::setVolume(int Volume) {
//****************************************************************************************************************************

  volume=Volume;
 
  reset();
}

//****************************************************************************************************************************
void VOLLevel::mute(bool newMuteState) {
//****************************************************************************************************************************
  _mute = newMuteState;
	
	reset();
}

//****************************************************************************************************************************
void VOLLevel::show(void) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
  SerialPort.println("Show : VolLevel");
#endif //SCREEN_DEBUG

#ifdef SCREEN_DEBUG
  SerialPort.print("Volume : ");
  SerialPort.println(volume);
#endif //SCREEN_DEBUG
 
  display.fillRect(posX, posY, 32, 32, GxEPD_WHITE);
      
//	display.drawRect(posX, posY, 32, 32, GxEPD_BLACK);
  if (_mute || (volume == 0))  display.drawInvertedBitmap(posX, posY, volume0icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else if (volume < 5) display.drawInvertedBitmap(posX, posY, volume1icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else if (volume < 9) display.drawInvertedBitmap(posX, posY, volume2icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  else  display.drawInvertedBitmap(posX, posY, volume3icons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
}
   
//****************************************************************************************************************************
//****************************************************************************************************************************
//						SATLEVEL
//****************************************************************************************************************************
//****************************************************************************************************************************
	 
const unsigned char saticons[] = { 
 // 'signal-32'
0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xf0, 
0xff, 0xff, 0xff, 0xe0, 
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xe0, 
0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xf8, 0x60, 
0xff, 0xff, 0xf8, 0x60, 0xff, 0xff, 0xf8, 0x60, 0xff, 0xff, 0xf8, 0x60, 0xff, 0xff, 0xf8, 0x60, 
0xff, 0xff, 0xf8, 0x60, 0xff, 0xff, 0xf8, 0x60, 0xff, 0xfc, 0x38, 0x60, 0xff, 0xfc, 0x38, 0x60, 
0xff, 0xfc, 0x38, 0x60, 0xff, 0xfc, 0x38, 0x60, 0xff, 0xfc, 0x38, 0x60, 0xfe, 0x1c, 0x38, 0x60, 
0xfe, 0x1c, 0x38, 0x60, 0x0e, 0x1c, 0x38, 0x60, 0x06, 0x1c, 0x38, 0x60, 0x06, 0x1c, 0x38, 0x60, 
0x06, 0x1c, 0x38, 0x60, 0x0e, 0x1c, 0x38, 0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
  
const unsigned char satfixicons[] = { 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 
0xff, 0xe0, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xe0, 0x00, 0xfe, 0xff, 
0xe0, 0x00, 0xfe, 0x7f, 0xe0, 0x00, 0xe6, 0x7f, 0xe0, 0x00, 0xf7, 0x7f, 0xe0, 0x00, 0xf7, 0x7f, 
0xe0, 0x00, 0xf7, 0x7f, 0xe0, 0x00, 0xe7, 0x7f, 0xe0, 0x00, 0xe6, 0x7f, 0xe0, 0x00, 0xfe, 0x7f, 
0xe0, 0x00, 0xfe, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xff, 
0xff, 0xf8, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/* !!! always reset !!! */
//****************************************************************************************************************************
void SATLevel::setSatelliteCount(uint8_t count) {
//****************************************************************************************************************************

  satelliteCount = count;
  reset();
}
  
//****************************************************************************************************************************
void SATLevel::show(void) {
//****************************************************************************************************************************

  display.drawInvertedBitmap(posX, posY, saticons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);

#ifdef SCREEN_DEBUG
  SerialPort.print("Nb satelite : ");
  SerialPort.println(satelliteCount);
#endif //SCREEN_DEBUG

  uint8_t satelliteBar = 0;
  if (satelliteCount == 1) satelliteBar = 1;
	else if ((satelliteCount > 1) && (satelliteCount <= 3)) satelliteBar = 2;
	else if ((satelliteCount > 3) && (satelliteCount <= 6)) satelliteBar = 3;
	else if ((satelliteCount > 6) && (satelliteCount <= 9)) satelliteBar = 4;
	else if  (satelliteCount > 9) satelliteBar = 5;
  
#ifdef SCREEN_DEBUG
  SerialPort.print("satelite bar : ");
  SerialPort.println(satelliteBar);
#endif //SCREEN_DEBUG
	
  display.fillRect(posX+floor((float)satelliteBar*6.5), posY, 32-floor((float)satelliteBar*6.5), 32, GxEPD_WHITE);
//	display.drawRect(posX+floor((float)satelliteBar*6.5), posY, 32-floor((float)satelliteBar*6.5), 32, GxEPD_BLACK);
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//			RECORDIndicator
//****************************************************************************************************************************
//****************************************************************************************************************************

const unsigned char recicons[] = { 
 // 'Phone_6-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xfc, 0x0f, 0xe0, 0x0f, 0xf0, 0x07, 0xc3, 0x87, 0xe1, 0xc3, 
0x8f, 0xe3, 0xc7, 0xf1, 0x9f, 0xe3, 0xc7, 0xf9, 0x1f, 0xf3, 0xcf, 0xf8, 0x1f, 0xf3, 0xcf, 0xf8, 
0x1f, 0xf3, 0xcf, 0xf8, 0x1f, 0xf3, 0xcf, 0xf8, 0x9f, 0xe3, 0xc7, 0xf9, 0x8f, 0xe3, 0xc7, 0xf1, 
0xc3, 0x87, 0xe1, 0xc3, 0xe0, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char pauseicons[] = { 
 // 'Clock_4-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xf0, 0x0f, 0x0f, 
0xff, 0x80, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x7f, 0xfc, 0x03, 0xc0, 0x3f, 0xf8, 0x1f, 0xf8, 0x1f, 
0xf0, 0x7f, 0xfe, 0x1f, 0xf0, 0xff, 0xff, 0x0f, 0xe1, 0xfe, 0xff, 0x87, 0xe1, 0xfe, 0xff, 0x87, 
0xc3, 0xfe, 0xff, 0xc3, 0xc3, 0xfe, 0xff, 0xc3, 0xc7, 0xfe, 0xff, 0xe3, 0xc7, 0xfe, 0xff, 0xe3, 
0xc7, 0xfc, 0x7f, 0xe3, 0xc7, 0xfc, 0x3f, 0xe3, 0xc7, 0xfc, 0x7f, 0xe3, 0xc7, 0xfc, 0xff, 0xe3, 
0xc7, 0xff, 0xff, 0xe3, 0xc3, 0xff, 0xff, 0xc3, 0xc3, 0xff, 0xff, 0xc7, 0xe1, 0xff, 0xff, 0x87, 
0xe0, 0xff, 0xff, 0x07, 0xf0, 0x7f, 0xfe, 0x0f, 0xf8, 0x3f, 0xfc, 0x1f, 0xfc, 0x0f, 0xf0, 0x3f, 
0xfe, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xf8, 0x1f, 0xff
};

const unsigned char fix1icons[] = { 
 // 'Arrow_46-32'
0xdf, 0xff, 0xff, 0xfb, 0x8f, 0xff, 0xff, 0xf1, 0x07, 0xcf, 0xf3, 0xe0, 0x83, 0x8f, 0xf1, 0xc1, 
0xc1, 0x0f, 0xf0, 0x03, 0xe0, 0x0f, 0xf0, 0x07, 0xf0, 0x0f, 0xf0, 0x0f, 0xf8, 0x0f, 0xf0, 0x1f, 
0xf0, 0x0f, 0xf0, 0x0f, 0xe0, 0x0f, 0xf0, 0x07, 0xc0, 0x0f, 0xf0, 0x03, 0xc0, 0x1f, 0xf8, 0x03, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xc0, 0x1f, 0xf8, 0x03, 0xc0, 0x0f, 0xf0, 0x03, 0xe0, 0x0f, 0xf0, 0x07, 0xf0, 0x0f, 0xf0, 0x0f, 
0xf8, 0x0f, 0xf0, 0x1f, 0xf0, 0x0f, 0xf0, 0x0f, 0xe0, 0x0f, 0xf0, 0x07, 0xc1, 0x0f, 0xf0, 0x83, 
0x83, 0x8f, 0xf1, 0xc1, 0x07, 0xcf, 0xf3, 0xe0, 0x8f, 0xff, 0xff, 0xf1, 0xdf, 0xff, 0xff, 0xfb
};

const unsigned char fix2icons[] = { 
 // 'Fit-To-Screen-32'
0x80, 0x3f, 0xfc, 0x01, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 
0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 
0x1c, 0x1f, 0xf8, 0x38, 0x3e, 0x0f, 0xf0, 0x7c, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0x9f, 0xf9, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 
0xff, 0x87, 0xf9, 0xff, 0xff, 0x07, 0xf0, 0xff, 0x3e, 0x0f, 0xf0, 0x7c, 0x1c, 0x1f, 0xf8, 0x38, 
0x00, 0x3f, 0xfc, 0x10, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 
0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x80, 0x3f, 0xfc, 0x01
};

const unsigned char norecordicons[] = { 
// 'Error-32', 32x32px
0xff, 0xf8, 0x1f, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x3f,
0xf8, 0x0f, 0xf0, 0x1f, 0xf0, 0x3f, 0xfc, 0x0f, 0xe0, 0xff, 0xfe, 0x07, 0xe1, 0xff, 0xfc, 0x07,
0xc3, 0xff, 0xf8, 0x03, 0xc3, 0xff, 0xf0, 0x43, 0x87, 0xff, 0xe0, 0xe1, 0x87, 0xff, 0xc1, 0xe1,
0x8f, 0xff, 0x83, 0xf1, 0x0f, 0xff, 0x07, 0xf0, 0x0f, 0xfe, 0x0f, 0xf0, 0x0f, 0xfc, 0x1f, 0xf0,
0x0f, 0xf8, 0x3f, 0xf0, 0x0f, 0xf0, 0x7f, 0xf0, 0x0f, 0xe0, 0xff, 0xf0, 0x8f, 0xc1, 0xff, 0xf1,
0x87, 0x83, 0xff, 0xe1, 0x87, 0x07, 0xff, 0xe1, 0xc2, 0x0f, 0xff, 0xc3, 0xc0, 0x1f, 0xff, 0x83,
0xe0, 0x3f, 0xff, 0x87, 0xe0, 0x7f, 0xfe, 0x07, 0xf0, 0x3f, 0xfc, 0x0f, 0xf8, 0x0f, 0xf0, 0x1f,
0xfc, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xf8, 0x1f, 0xff
};

//****************************************************************************************************************************
void RECORDIndicator::stateRECORD(void) {
//****************************************************************************************************************************

  unsigned long FreqDuration = millis() - lastFreqUpdate;
  if( FreqDuration > 1000 ) {
    lastFreqUpdate = millis();
	  displayRecord ^= 1;
    reset();
  }
}

//****************************************************************************************************************************
void RECORDIndicator::setActifSCAN(void) {
//****************************************************************************************************************************

  recordState =  STATE_SCAN;
}  
  
//****************************************************************************************************************************
void RECORDIndicator::setActifRECORD(void) {
//****************************************************************************************************************************

  recordState = STATE_RECORD;
}

//****************************************************************************************************************************
void RECORDIndicator::setActifGPSFIX(void) {
//****************************************************************************************************************************

  recordState = STATE_GPSFIX;
}
  
//****************************************************************************************************************************
void RECORDIndicator::setNoRECORD(void) {
//****************************************************************************************************************************

  recordState = STATE_NORECORD;
}
	
//****************************************************************************************************************************
void RECORDIndicator::show(void) {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : RecordIndicator");
#endif //SCREEN_DEBUG

  display.fillRect(posX, posY, 32, 32, GxEPD_WHITE);

  if( recordState==STATE_RECORD) {
#ifdef SCREEN_DEBUG
		SerialPort.println("Show : STATE_RECORD");
#endif //SCREEN_DEBUG
		
	  if( displayRecord)
        display.drawInvertedBitmap(posX, posY,recicons,  32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
	  else
	    display.fillRect(posX, posY, 32, 32, GxEPD_WHITE);
  } else if  (recordState==STATE_GPSFIX) {
      display.drawInvertedBitmap(posX, posY,pauseicons,  32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  } else if  (recordState==STATE_NORECORD) {
      display.drawInvertedBitmap(posX, posY,norecordicons,  32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  } else if (displayRecord) {
      display.drawInvertedBitmap(posX, posY,fix1icons,  32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  } else  {
      display.drawInvertedBitmap(posX, posY,fix2icons,  32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
  }
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//				TRENDLevel
//****************************************************************************************************************************
//****************************************************************************************************************************

const unsigned char trendupicons[] = { 
 // 'Arrowup-14-24'
0xfc, 0xfc, 0xf8, 0x7c, 0xf0, 0x3c, 0xe0, 0x1c, 0xc0, 0x0c, 0x84, 0x84, 0x0c, 0xc0, 0x1c, 0xe0, 
0x3c, 0xf0, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 
0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc
};

const unsigned char trenddownicons[] = { 
// 'Arrowdown-14-24'
0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 
0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0x3c, 0xf0, 
0x1c, 0xe0, 0x0c, 0xc0, 0x84, 0x84, 0xc0, 0x0c, 0xe0, 0x1c, 0xf0, 0x3c, 0xf8, 0x7c, 0xfc, 0xfc
};

/* !!! always reset !!! */
//****************************************************************************************************************************
void TRENDLevel::stateTREND(int8_t state) {
//****************************************************************************************************************************

  trendState = state;
  reset();
}
  
//****************************************************************************************************************************
void TRENDLevel::show(void) {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : TrendLevel");
#endif //SCREEN_DEBUG

  display.fillRect(posX, posY, 14, 24, GxEPD_WHITE);
// 	display.drawRect(posX, posY, 24, 24, GxEPD_BLACK);

 /* check level and display arrow or blank */
	if( trendState == 1 )  {
	  display.drawInvertedBitmap(posX, posY,trendupicons,  14, 24, GxEPD_BLACK);   //GxEPD_BLACK);
	} else if( trendState == -1 ) {  
	  display.drawInvertedBitmap(posX, posY,trenddownicons,  14, 24, GxEPD_BLACK);   //GxEPD_BLACK);
	} else {
	  display.fillRect(posX, posY, 14, 24, GxEPD_WHITE);
	}
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//				SCREENTIME
//****************************************************************************************************************************
//****************************************************************************************************************************

  // 8 x 24 
const unsigned char doticons[] = { 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

  // 16 x 24 
const unsigned char hicons[] = { 
0x1C, 0x00, 0x1C, 0x00, 0x1C, 0x00, 0x1C, 0x00, 
0x1C, 0x00, 0x1F, 0xF0, 0x1F, 0xF8, 0x1F, 0xFC, 
0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
0x1C, 0x1C, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* time */
//****************************************************************************************************************************
void ScreenTime::setTime(uint32_t newTime) {
//****************************************************************************************************************************

//  newTime = newTime / 10;
  for( uint8_t i = 0; i<3; i++) {
    time[i] = (int8_t)(newTime % 100);
    newTime /= 100;
  }
	
#ifdef SCREEN_DEBUG
  SerialPort.print("Time : ");
  SerialPort.print(time[2]);
  SerialPort.print(":");
  SerialPort.print(time[1]);
  SerialPort.print(":");
  SerialPort.println(time[0]);
#endif //SCREEN_DEBUG

	
/*    uint32_t date = nmeaParser.date;
    for(uint8_t i=0; i<3; i++) {
      uint8_t num = ((uint8_t)(date%100));
      dateCharP[0] = (num/10) + '0';
      dateCharP[1] = (num%10) + '0';
      dateCharP += 2;
      date /= 100;
    }	
*/	
	
  reset();	
}

//****************************************************************************************************************************
void ScreenTime::setTime(int8_t* newTime) {
//****************************************************************************************************************************

  for(uint8_t i = 0; i<3; i++) {
    time[i] = newTime[i];
  }
  reset();	
}

//****************************************************************************************************************************
void ScreenTime::correctTimeZone(int8_t UTCDrift) {
//****************************************************************************************************************************
  
  time[2] += UTCDrift;
  if( time[2] < 0 ) {
    time[2] += 24;
  }
  if( time[2] >= 24 ) {
    time[2] -= 24;
  }
}

//****************************************************************************************************************************
int8_t* ScreenTime::getTime(void) {
//****************************************************************************************************************************

  return time;
}

 //****************************************************************************************************************************
void ScreenTime::setPositionTitle(uint16_t X, uint16_t Y) {
//****************************************************************************************************************************
	titleX = X;
	titleY = Y;
	titlePosition = true;
}

/* !!! never reset, only on page change !!! */
//****************************************************************************************************************************
void ScreenTime::show(void) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
  SerialPort.println("Show : ScreenTime");
#endif //SCREEN_DEBUG

//  display.fillRect(posX-70, posY-32, 65, 34, GxEPD_WHITE);
  display.fillRect(posX-70, posY-32, 16, 34, GxEPD_WHITE);
// 	display.drawRect(posX-70, posY-32, 16, 34, GxEPD_BLACK);


  if (dot_or_h == false) {
#ifdef SCREEN_DEBUG
		SerialPort.println("dot_or_h  : H");
#endif //SCREEN_DEBUG

    display.drawBitmap(posX-70, posY-24,hicons,  16, 24, GxEPD_BLACK);   //GxEPD_BLACK);
	}
  else {	
#ifdef SCREEN_DEBUG
		SerialPort.println("dot_or_h  : DOT");
#endif //SCREEN_DEBUG
  
    display.drawBitmap(posX-70, posY-26, doticons, 16, 24, GxEPD_BLACK);   //GxEPD_BLACK);
	}

#ifdef SCREEN_DEBUG
  SerialPort.print("time : ");
  SerialPort.print(time[2]);
  SerialPort.print(" : ");
  SerialPort.println(time[1]);
#endif //SCREEN_DEBUG

  hour.setValue(time[2]);
  hour.show();
 //  display h ou dot

  minute.setValue(time[1]);
  minute.show();

  display.fillRect(posX-125, posY-17-36, 125, 19, GxEPD_WHITE);
	
	display.setFont(&FreeSansBold8pt7b);
	display.setTextColor(ColorText);
	display.setTextSize(1);
			
  if (dot_or_h == false) {	
//		case DISPLAY_OBJECT_TIME :
//			display.drawInvertedBitmap(posX-125, posY-14-36, heuretext, 38, 12, GxEPD_BLACK);

			if (titlePosition) 	display.setCursor(titleX, titleY); //titleX+2, titleY);
			else								display.setCursor(posX-120, posY-36-3); //titleX+2, titleY);
			display.print(varioLanguage.getText(TITRE_TIME));
	}
	else  {
//		case DISPLAY_OBJECT_DURATION :
//			display.drawInvertedBitmap(posX-125, posY-17-36, tdvtext, 88, 17, GxEPD_BLACK);
			if (titlePosition) 	display.setCursor(titleX, titleY); //titleX+2, titleY);
			else								display.setCursor(posX-120, posY-36-3); //titleX+2, titleY);
			display.print(varioLanguage.getText(TITRE_TDV));
	}	
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//				SCREENELAPSEDTIME
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void ScreenElapsedTime::setBaseTime(int8_t* time) {
//****************************************************************************************************************************

  for(uint8_t i = 0; i<3; i++) {
    baseTime[i] = time[i];
  }
  reset();	
}

//****************************************************************************************************************************
void ScreenElapsedTime::setCurrentTime(int8_t* currentTime) {
//****************************************************************************************************************************

  /* compute elapsed time */
  int8_t rem = 0;
  int8_t v;
  for(uint8_t i = 0; i<3; i++) {
    v = (currentTime[i] - baseTime[i]) - rem;
    if( v < 0 ) {
      v += 60;
      rem = 1;
    } else {
      rem = 0;
    }
    time[i] = v;
  }
  reset();	
}

  // 24 x 24 
const unsigned char fixgpsicons[] = { 
 // 'location-marker-24'
0xff, 0x81, 0xff, 0xff, 0x00, 0xff, 0xfe, 0x00, 0x7f, 0xfc, 0x00, 0x3f, 0xfc, 0x3c, 0x3f, 0xf8, 
0x7e, 0x1f, 0xf8, 0x7e, 0x1f, 0xf8, 0x7e, 0x1f, 0xf8, 0x7e, 0x1f, 0xf8, 0x3c, 0x1f, 0xf8, 0x00, 
0x1f, 0xfc, 0x00, 0x3f, 0xfc, 0x00, 0x3f, 0xfc, 0x00, 0x3f, 0xfe, 0x00, 0x7f, 0xfe, 0x00, 0x7f, 
0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x81, 0xff, 0xff, 0x81, 0xff, 0xff, 0xc3, 0xff, 0xff, 
0xc3, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff
};

//****************************************************************************************************************************
//****************************************************************************************************************************
//					FIXGPSINFO
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void FIXGPSInfo::setFixGps(void) {
//****************************************************************************************************************************
  FixGPS = 	true;
	reset();
}
  
//****************************************************************************************************************************
void FIXGPSInfo::unsetFixGps(void) {
//****************************************************************************************************************************
  FixGPS = false;
	reset();
}

//****************************************************************************************************************************
boolean FIXGPSInfo::getFixGps(void) {
//****************************************************************************************************************************
  return FixGPS;
}

//****************************************************************************************************************************
void FIXGPSInfo::show() {
//****************************************************************************************************************************
// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
 display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
// display.drawRect(posX, posY, 24, 24, GxEPD_BLACK);

 if (FixGPS == true) display.drawInvertedBitmap(posX, posY, fixgpsicons, 24, 24, GxEPD_BLACK);   //GxEPD_BLACK);
// else                display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
  
//  display.drawBitmap(100, 10, gridicons_sync, 24, 24, GxEPD_BLACK);
}


  // 32 x 32 
const unsigned char usbicons[] = { 
 // 'basic1-197_usb-32'
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xef, 0xff, 
0xff, 0xf7, 0xef, 0xff, 0xff, 0xf7, 0xef, 0xff, 0xff, 0xf5, 0xaf, 0xff, 0xff, 0xf7, 0xef, 0xff, 
0xff, 0xf7, 0xef, 0xff, 0xff, 0xf7, 0xef, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xcf, 0xf3, 0xff, 
0xff, 0xcf, 0xf3, 0xff, 0xff, 0xcf, 0xf3, 0xff, 0xff, 0xcf, 0xf3, 0xff, 0xff, 0xcf, 0xf3, 0xff, 
0xff, 0xcf, 0xf3, 0xff, 0xff, 0xcf, 0xf3, 0xff, 0xff, 0xcf, 0xf3, 0xff, 0xff, 0xcf, 0xf3, 0xff, 
0xff, 0xcf, 0xf3, 0xff, 0xff, 0xcf, 0xe7, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xfc, 0x3f, 0xff, 
0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 
0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//****************************************************************************************************************************
//****************************************************************************************************************************
//						INFOLEVEL
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void INFOLevel::set(uint8_t value) {
//****************************************************************************************************************************
  InfoValue = 	value;
	reset();
}

//****************************************************************************************************************************
void INFOLevel::show() {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : InfoLevel");
#endif //SCREEN_DEBUG

 display.fillRect(posX, posY, 32, 32, GxEPD_WHITE);
// display.drawRect(posX, posY, 32, 32, GxEPD_BLACK);

// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
 if (InfoValue == INFO_USB) display.drawInvertedBitmap(posX, posY, usbicons, 32, 32, GxEPD_BLACK);   //GxEPD_BLACK);
// else                       display.fillRect(posX, posY, 32, 32, GxEPD_WHITE);
}


//****************************************************************************************************************************
//****************************************************************************************************************************
//					BTINFO
//****************************************************************************************************************************
//****************************************************************************************************************************

  // 24 x 24 
const unsigned char bticons[] = { 
 // 'Bluetooth-24'
0xff, 0xc3, 0xff, 0xff, 0x00, 0xff, 0xfe, 0x00, 0x7f, 0xfc, 0x10, 0x3f, 0xfc, 0x18, 0x3f, 0xfc, 
0x1c, 0x3f, 0xfc, 0x1e, 0x3f, 0xfc, 0x1b, 0x3f, 0xfc, 0x9b, 0x3f, 0xfc, 0xde, 0x3f, 0xfc, 0x7c, 
0x3f, 0xfc, 0x38, 0x3f, 0xfc, 0x38, 0x3f, 0xfc, 0x7c, 0x3f, 0xfc, 0xde, 0x3f, 0xfc, 0x9b, 0x3f, 
0xfc, 0x1b, 0x3f, 0xfc, 0x1e, 0x3f, 0xfc, 0x1c, 0x3f, 0xfc, 0x18, 0x3f, 0xfc, 0x00, 0x3f, 0xfe, 
0x00, 0x7f, 0xff, 0x00, 0xff, 0xff, 0xc3, 0xff
};

//****************************************************************************************************************************
void BTInfo::setBT(void) {
//****************************************************************************************************************************
  bt = 	true;
	reset();
}
  
//****************************************************************************************************************************
void BTInfo::unsetBT(void) {
//****************************************************************************************************************************
  bt = false;
  reset();	
}

//****************************************************************************************************************************
boolean BTInfo::getBT(void) {
//****************************************************************************************************************************
  return bt;
}

//****************************************************************************************************************************
void BTInfo::show() {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : BTLevel");
#endif //SCREEN_DEBUG

 display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
// display.drawRect(posX, posY, 24, 24, GxEPD_BLACK);

// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
 if (bt == true) display.drawInvertedBitmap(posX, posY, bticons, 24, 24, GxEPD_BLACK);   //GxEPD_BLACK);
// else                display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
 //  display.drawBitmap(100, 10, gridicons_sync, 24, 24, GxEPD_BLACK);
}


//****************************************************************************************************************************
//****************************************************************************************************************************
//					BGLine
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void BGLine::show() {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : BGLine");
#endif //SCREEN_DEBUG

// display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);

  display.drawLine(posX1, posY1, posX2, posY2, GxEPD_BLACK);

}

//****************************************************************************************************************************
void BGLine::toDisplay() {
//****************************************************************************************************************************
    reset();
}

//****************************************************************************************************************************
//****************************************************************************************************************************
//					BGCircle
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void BGCircle::show() {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : BGCircle");
#endif //SCREEN_DEBUG

// display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);

  display.drawCircle(posX, posY, R, GxEPD_BLACK);
}

//****************************************************************************************************************************
void BGCircle::toDisplay() {
//****************************************************************************************************************************
    reset();
}

//****************************************************************************************************************************
void WIND::show() {
//****************************************************************************************************************************
#ifdef SCREEN_DEBUG
  SerialPort.println("Show : MSUnit");
#endif //SCREEN_DEBUG

// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
	display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
//	display.drawRect(posX, posY, 24, 24, GxEPD_BLACK);
  display.drawInvertedBitmap(200, 33, wind, 90, 90, GxEPD_BLACK);
//display.drawInvertedBitmap	
//  display.drawBitmap(100, 10, gridicons_sync, 24, 24, GxEPD_BLACK);
}

//****************************************************************************************************************************
void WIND::toDisplay() {
// ****************************************************************************************************************************
    reset();
}

/*
const unsigned char separationlineicon[] = {
	// 'favicon(13), 64x1px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ****************************************************************************************************************************
void SeparationLine::show() {
	#ifdef SCREEN_DEBUG
  SerialPort.println("Show : KmHUnit");
#endif //SCREEN_DEBUG
display.fillRect(posX, posY, 64, 1, GxEPD_WHITE);
display.drawInvertedBitmap(posX, posY, separationlineicon, 64, 1, GxEPD_BLACK);
};
// ****************************************************************************************************************************
void SeparationLine::toDisplay() {
// ****************************************************************************************************************************
   reset();
}
*/


/**************************************************************************/
/*!
    @brief    Helper to determine size of a character with current font/size.
       Broke this out as it's used by both the PROGMEM- and RAM-resident getTextBounds() functions.
    @param    c       The ascii character in question
    @param    width   width
    @param    height  height
*/
/**************************************************************************
template<typename GxEPD2_Type, const uint16_t page_height> void GxEPD2_BW_U<GxEPD2_Type, page_height>::charWidth(char c, uint16_t *width, uint16_t *height) {

    if(Adafruit_GFX::gfxFont) {

        if(c == '\n') { // Newline?
        } else if(c != '\r') { // Not a carriage return; is normal char
            uint8_t first = pgm_read_byte(&Adafruit_GFX::gfxFont->first),
                    last  = pgm_read_byte(&Adafruit_GFX::gfxFont->last);
            if((c >= first) && (c <= last)) { // Char present in this font?
                GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(
                  &Adafruit_GFX::gfxFont->glyph))[c - first]);
                uint8_t gw = pgm_read_byte(&glyph->width),
                        gh = pgm_read_byte(&glyph->height),
                        xa = pgm_read_byte(&glyph->xAdvance);
                int8_t  xo = pgm_read_byte(&glyph->xOffset),
                        yo = pgm_read_byte(&glyph->yOffset);
												
												
										SerialPort.print("width :  ");
										SerialPort.println(gw);
										SerialPort.print("height :  ");
										SerialPort.println(gh);							
												
                int16_t ts = (int16_t)Adafruit_GFX::textsize;
                *width = (gw * ts) - 1,
                *height = (gh * ts) - 1;
            }
        }

    } else { // Default font

        if(c == '\n') {                     // Newline?
            // min/max x/y unchaged -- that waits for next 'normal' character
        } else if(c != '\r') {  // Normal char; ignore carriage returns
				    int16_t ts = (int16_t)Adafruit_GFX::textsize; 
            *width  =  uint16_t (ts * 6 - 1); // Lower-right pixel of char
            *height =  uint16_t (ts * 8 - 1);
        }
    }
}

/ **************************************************************************/
/*
    @brief    Helper to determine size of a string with current font/size. Pass string and a cursor position, returns UL corner and W,H.
    @param    str     The ascii string to measure
    @param    w      width
    @param    h      height
*/
/**************************************************************************
template<typename GxEPD2_Type, const uint16_t page_height> void GxEPD2_BW_U<GxEPD2_Type, page_height>::getTextWidth(const char *str, uint16_t *w, uint16_t *h) {
	
    uint8_t c; // Current character

    *w  = *h = 0;

    uint16_t width = 0, tmpwidth = 0, height = 0, tmpheight = 0;

    while((c = *str++)) {		
			
        charWidth(c, &tmpwidth, &tmpheight);
				
				width   += tmpwidth;
				height  += tmpheight;
				
				SerialPort.println("CHARWIDTH");
				SerialPort.print("width :  ");
				SerialPort.println(tmpwidth);
				SerialPort.print("height :  ");
				SerialPort.println(tmpheight);
				SerialPort.print("total width :  ");
				SerialPort.println(width);
				SerialPort.print("totoal height :  ");
				SerialPort.println(height);		
		}


  *w  = width;
  *h  = height;
}

// **************************************************************************/
/*
    @brief    Helper to determine size of a string with current font/size. Pass string and a cursor position, returns UL corner and W,H.
    @param    str    The ascii string to measure (as an arduino String() class)
    @param    x      The current cursor X
    @param    y      The current cursor Y
    @param    w      The boundary width, set by function
    @param    h      The boundary height, set by function
*/
/**************************************************************************
template<typename GxEPD2_Type, const uint16_t page_height> void GxEPD2_BW_U<GxEPD2_Type, page_height>::getTextWidth(const String &str, uint16_t *w, uint16_t *h) {
    if (str.length() != 0) {
        getTextWidth(const_cast<char*>(str.c_str()), w, h);
    }
}


// * *************************************************************************/
/*
    @brief    Helper to determine size of a PROGMEM string with current font/size. Pass string and a cursor position, returns UL corner and W,H.
    @param    str     The flash-memory ascii string to measure
    @param    w      The boundary width, set by function
    @param    h      The boundary height, set by function
*/
/**************************************************************************
template<typename GxEPD2_Type, const uint16_t page_height> void GxEPD2_BW_U<GxEPD2_Type, page_height>::getTextWidth(const __FlashStringHelper *str, uint16_t *w, uint16_t *h) {
    uint8_t *s = (uint8_t *)str, c;

    *w  = *h = 0;

    int16_t minx = Adafruit_GFX::_width, miny = Adafruit_GFX::_height, maxx = -1, maxy = -1;

    while((c = pgm_read_byte(s++)))
        Adafruit_GFX::charBounds(c, 0, 0, &minx, &miny, &maxx, &maxy);

    if(maxx >= minx) {
        *w  = maxx - minx + 1;
    }
    if(maxy >= miny) {
        *h  = maxy - miny + 1;
    }
}
*/

#endif //VARIOSCREEN_SIZE
