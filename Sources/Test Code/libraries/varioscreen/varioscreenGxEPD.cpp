/* varioscreenGxEPD -- 
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * This file is part of toneHAL.
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

/*********************************************************************************/
/*                                                                               */
/*                           VarioScreenGxEPD                                    */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    03/06/19                                                            */
/*                                                                               */
/*********************************************************************************/

#include <varioscreenGxEPD.h>
#include <Arduino.h>
//#include <avr\dtostrf.h>
#include <stdlib.h>
#include <DebugConfig.h>

#include <HardwareConfig.h>

/* http://javl.github.io/image2cpp/ */

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "GxEPD2_boards.h"
#include "GxEPD2_boards_added.h"
//#include "GxEPD2_more_boards_added.h" // private

#include <imglib/gridicons_sync.h>
#include <gnuvario.h>

#include <VarioSettings.h>

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

volatile unsigned long oldtime;
volatile uint8_t stateDisplay;
volatile uint8_t led1stat = 0; 


#define VARIOSCREEN_DOT_WIDTH 6
#define VARIOSCREEN_DIGIT_WIDTH 11

#define ColorScreen    GxEPD_WHITE
#define ColorText      GxEPD_BLACK

#define VARIOSCREEN_TENSION_ANCHOR_X 140
#define VARIOSCREEN_TENSION_ANCHOR_Y 170

#define VARIOSCREEN_AUTONOMIE_ANCHOR_X 90
#define VARIOSCREEN_AUTONOMIE_ANCHOR_Y 235

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

VarioScreen screen;

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
//								VARIOSCREEN
//****************************************************************************************************************************
//****************************************************************************************************************************


//****************************************************************************************************************************
VarioScreen::~VarioScreen() {
//****************************************************************************************************************************
	free(tensionDigit);
	free(displayList);
	free(schedulerScreen);
}

#define ITEMS_IN_ARRAY(array)   (sizeof(array) / sizeof(*array))

//****************************************************************************************************************************
void VarioScreen::begin(void)
//****************************************************************************************************************************
{
#ifdef SCREEN_DEBUG
	SerialPort.println("init");	
#endif //SCREEN_DEBUG

  display.init(115200);
	
//  setRotation(2);

#ifdef SCREEN_DEBUG
	SerialPort.println("fillScreen");	
#endif //SCREEN_DEBUG

  display.fillScreen(GxEPD_WHITE);
	
#ifdef SCREEN_DEBUG
	SerialPort.println("setTextColor");	
#endif //SCREEN_DEBUG
	
  display.setTextColor(GxEPD_BLACK);
	
#ifdef SCREEN_DEBUG
	SerialPort.println("update");	
#endif //SCREEN_DEBUG
	
	/* création des champs d'affichage */
	
	
//	ScreenDigit tensionDigit(TENSION_DISPLAY_POSX /*VARIOSCREEN_TENSION_ANCHOR_X*/, VARIOSCREEN_TENSION_ANCHOR_Y, 5, 2, false, false, ALIGNRIGHT);

	tensionDigit = new ScreenDigit(VARIOSCREEN_TENSION_ANCHOR_X, VARIOSCREEN_TENSION_ANCHOR_Y, 5, 2, false, false, ALIGNRIGHT);	
	tempratureDigit = new ScreenDigit(VARIOSCREEN_TENSION_ANCHOR_X, VARIOSCREEN_TENSION_ANCHOR_Y, 5, 2, false, false, ALIGNRIGHT);
//	displayList  = new ScreenSchedulerObject[3];
	MaxObjectList = 0;
	
#ifdef SCREEN_DEBUG
	SerialPort.println("schedulerScreen : createObjectDisplay");	
#endif //SCREEN_DEBUG
	
	CreateObjectDisplay(DISPLAY_OBJECT_TENSION, tensionDigit, 0, 0, true); 
//	CreateObjectDisplay(DISPLAY_OBJECT_TEMPRATURE, tempratureDigit, 0, 2, true); 
	
#ifdef SCREEN_DEBUG
	SerialPort.println("schedulerScreen : create");	
		
	SerialPort.print("begin - objectCount : ");	
	SerialPort.println(ITEMS_IN_ARRAY(displayList));	
//	SerialPort.println(sizeof(displayList);	
#endif //SCREEN_DEBUG
	
	schedulerScreen = new ScreenScheduler(displayList, ITEMS_IN_ARRAY(displayList) -1, 0, 0);   //ITEMS_IN_ARRAY(displayList), 0, 0);

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

//****************************************************************************************************************************
void genericTask( void * parameter ){
//****************************************************************************************************************************
  stateDisplay = STATE_BUSY;
#ifdef SCREEN_DEBUG
    SerialPort.print("Created task: Executing on core ");
    SerialPort.println(xPortGetCoreID());
#endif //SCREEN_DEBUG

	display.display(true); // partial update
  stateDisplay = STATE_OK;
  vTaskDelete(NULL);
}

TaskHandle_t taskDisplay;

//****************************************************************************************************************************
void VarioScreen::updateScreen (void)
//****************************************************************************************************************************
{
#ifdef SCREEN_DEBUG
	SerialPort.println("screen update");	
#endif //SCREEN_DEBUG
	
  if (stateDisplay != STATE_OK) return;
	
	display.setFullWindow();
#ifdef SCREEN_DEBUG
	SerialPort.println("screen update : setFullWindows");	
#endif //SCREEN_DEBUG
	
/*	xTaskCreate(
							genericTask,       // Task function. 
							"genericTask",     // String with name of task. 
							10000,             // Stack size in words. 
							NULL,              // Parameter passed as input of the task 
							2,                 // Priority of the task. 
							NULL);             // Task handle. */	
	
	xTaskCreatePinnedToCore(
							genericTask,       // Task function. 
							"TaskDisplay",     // String with name of task. 
							10000,             // Stack size in words. 
							NULL,              // Parameter passed as input of the task 
							2,                 // Priority of the task.
							&taskDisplay,			 // Task handle
							1);             	 // pin task to core 1*/	
							
//	display.display(true); // partial update
#ifdef SCREEN_DEBUG
	SerialPort.println("screen update : create task");	
#endif //SCREEN_DEBUG

//	display.updateWindow(0, 0, display.width(), display.height(), false);
}

//****************************************************************************************************************************
void VarioScreen::updateData(int8_t ObjectDisplayTypeID, double data) {
//****************************************************************************************************************************
	
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
			displayList[i].object->setValue(data);
			
#ifdef SCREEN_DEBUG
		  SerialPort.print("i = ");	
		  SerialPort.println(i);	
		  SerialPort.print("data = ");	
		  SerialPort.println(data);	
#endif //SCREEN_DEBUG
		
			
		}
	}
}


//****************************************************************************************************************************
void VarioScreen::ScreenViewInit(uint8_t Version, uint8_t Sub_Version)
//****************************************************************************************************************************
{
  char tmpbuffer[100];
	
  display.setFullWindow();
  display.firstPage();
  do
  {
 	  display.fillScreen(ColorScreen);

		display.drawBitmap(0, 10, logo_gnuvario, 102, 74, ColorText); //94

		display.setFont(&FreeSansBold12pt7b);
		display.setTextColor(ColorText);
		display.setTextSize(1);

		display.setCursor(100, 30);
		display.println("Version");
		display.setCursor(105, 50);
		display.println(" Beta 1");
		sprintf(tmpbuffer,"%02d.%02d", Version, Sub_Version);
		display.setCursor(125, 70);
		display.println(tmpbuffer);
		sprintf(tmpbuffer,"%s", __DATE__);
		display.setCursor(25, 110);
		display.println(tmpbuffer);

		display.setFont(&FreeSansBold12pt7b);
		display.setTextSize(2);
		display.setCursor(160, VARIOSCREEN_TENSION_ANCHOR_Y);
		display.println("C");
  }
  while (display.nextPage());
	
	delay(2000);
//  display..update();
/*		display..updateWindow(0, 0, display.width(), display.height(), false);
  while (display..GetState() != STATE_OK) {
    display..updateWindow(0, 0, display.width(), display.height(), false);
  }*/	
}


/*void VarioScreen::clearScreen(void) 
{
  eraseDisplay();	
}*/

/*void VarioScreen::beginClear(void) {
  clearingStep = 0;
}

bool VarioScreen::clearStep(void) {

  /* check if clear is needed *
  if( clearingStep == LCDHEIGHT ) {
    return false;
  }

  /* clear one line *

  /* next *
  clearingStep++;
  return true;
}*/

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
bool VarioScreenObject::update(void) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
	SerialPort.println("VarioScreenObject : update");	
#endif //SCREEN_DEBUG
  
  show();
 /* if( display_needed() ) {
    show();
    display_done();
    return true;
  }

  return false;*/
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
#define MAX_CHAR_IN_LINE 7

//****************************************************************************************************************************
void ScreenDigit::setValue(double Value) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
	SerialPort.println("ScreenDigit : setValue");	
	SerialPort.print("Value = ");	
	SerialPort.println(Value);	
#endif //SCREEN_DEBUG

  /* build digit and check changes */
  oldvalue=value;
  value=Value;
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
char * ScreenDigit::dtostrf2(double number, signed char width, unsigned char prec, char *s, boolean zero) {
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


//****************************************************************************************************************************
void ScreenDigit::show() {
//****************************************************************************************************************************

  /***************/
  /* build digit */
  /***************/

#ifdef SCREEN_DEBUG
	SerialPort.println("ScreenDigit : show");	
#endif //SCREEN_DEBUG

  //normalise value
//  char tmpdigitCharacters[MAX_CHAR_IN_LINE];
  char digitCharacters[MAX_CHAR_IN_LINE];
   
//  display.setFont(&FreeSansBold12pt7b);
  display.setFont(&FreeSansBold12pt7b);
  display.setTextSize(2);

  int16_t box_x = anchorX;
  int16_t box_y = anchorY;
  uint16_t w, h, w1, h1;
  int16_t box_w, box_w1; 
  int16_t box_h, box_h1; 

 // dtostrf(value,width,precision,tmpdigitCharacters);
  dtostrf2(value,width,precision,digitCharacters,zero);
//  dtostrf2(oldvalue,width,precision,tmpdigitCharacters,zero);
//  dtostrf2(value,4,1,digitCharacters,false,false);
  /*if (plusDisplay) {
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
  SerialPort.println(tmpdigitCharacters);*/

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
#endif //SCREEN_DEBUG
  
//  display.getTextBounds(digitCharacters, box_x, box_y, &box_w, &box_h, &w, &h);
  display.getTextBounds(digitCharacters, 0, box_y, &box_w, &box_h, &w, &h);
/////  display.getTextWidth(digitCharacters, &w, &h);
//  display.getTextBounds(tmpdigitCharacters, box_x, box_y, &box_w1, &box_h1, &w1, &h1);
  
//  if (h1 > h) {h = h1;}
//  if (w1 > w) {w = w1;}
  
#ifdef SCREEN_DEBUG
/*  SerialPort.print("X : ");
  SerialPort.println(box_x);
  SerialPort.print("Y : ");
  SerialPort.println(box_y);
  SerialPort.print("X new : ");
  SerialPort.println(box_w);
  SerialPort.print("Y new : ");
  SerialPort.println(box_h);*/
  SerialPort.print("W : ");
  SerialPort.println(w);
  SerialPort.print("H : ");
  SerialPort.println(h);
#endif //SCREEN_DEBUG
  
/*  if (leftAlign) {
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
  }*/
	
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
#endif //SCREEN_DEBUG
	
		if ((oldw != 0) && (oldh != 0)) {
			display.fillRect(oldx, oldy, oldw, oldh, GxEPD_WHITE);
		}	
	
  display.drawRect(box_x, box_y-h-2, w+6, h+6, GxEPD_BLACK);
	  
    display.setCursor(box_x+1, box_y-1);
    display.print(digitCharacters);
		
		oldx = box_x;
		oldy = box_y-h-2;
		oldw = w+6;
		oldh = h+6;	
		
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
#endif //SCREEN_DEBUG
	
		if ((oldw != 0) && (oldh != 0)) {
			display.fillRect(oldx, oldy, oldw, oldh, GxEPD_WHITE);
		}	
			
		display.drawRect(box_x-w-6, box_y-h-2, w+6, h+6, GxEPD_BLACK);
		
    display.setCursor(box_x-w-4, box_y-1);
    display.print(digitCharacters);
		
		oldx = box_x-w-6;
		oldy = box_y-h-2;
		oldw = w+6;
		oldh = h+6;	
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
  display.setFont(&FreeSansBold9pt7b);
  display.setTextSize(2);
  display.setCursor(posX, posY);
  display.print('m');
}


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
//****************************************************************************************************************************
//							MSUnit
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void MSUnit::show() {
//****************************************************************************************************************************
// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
 display.drawBitmap(posX, posY, msicons, 24, 24, GxEPD_BLACK,false);   //GxEPD_BLACK);
//  display.drawBitmap(100, 10, gridicons_sync, 24, 24, GxEPD_BLACK);
}

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
//****************************************************************************************************************************
//				KMHUnit
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void KMHUnit::show() {
//****************************************************************************************************************************
// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
 display.drawBitmap(posX, posY, kmhicons, 24, 24, GxEPD_BLACK,false);   //GxEPD_BLACK);
//  display.drawBitmap(100, 10, gridicons_sync, 24, 24, GxEPD_BLACK);
}

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

//****************************************************************************************************************************
//****************************************************************************************************************************
//						BATLEVEL
//****************************************************************************************************************************
//****************************************************************************************************************************

/* !!! always reset !!! */
//****************************************************************************************************************************
void BATLevel::setVoltage(int voltage) {
//****************************************************************************************************************************

#ifdef SCREEN_DEBUG
  SerialPort.print("Voltage : ");
  SerialPort.println(voltage);
#endif //SCREEN_DEBUG
  
  /* shift voltage to 16 bit */
  double tmp;
  tmp = (double)voltage / 43;
#ifdef SCREEN_DEBUG
  SerialPort.print("Voltage : ");
  SerialPort.println(tmp);
#endif //SCREEN_DEBUG
  
  uVoltage = (uint16_t)(tmp*100);
//  uVoltage <<= VARIOSCREEN_BAT_MULTIPLIER;
  
  reset();
}

//****************************************************************************************************************************
void BATLevel::show(void) {
//****************************************************************************************************************************

  /* battery base */

  /* battery level */
 /* uint16_t baseVoltage = base + inc;
  uint8_t pixelCount = 0;*/

#ifdef SCREEN_DEBUG
  SerialPort.print("uVoltage : ");
  SerialPort.println(uVoltage);
#endif //SCREEN_DEBUG
  
  if (uVoltage >= 75)
    display.drawBitmap(posX, posY, bat4icons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  else if (uVoltage >= 50)
    display.drawBitmap(posX, posY, bat3icons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  else if (uVoltage >= 25)
    display.drawBitmap(posX, posY, bat2icons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  else if (uVoltage >= 10)
    display.drawBitmap(posX, posY, bat1icons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  else
    display.drawBitmap(posX, posY, bat0icons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
	  
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
//****************************************************************************************************************************
//						VOLLEVEL
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void VOLLevel::setVolume(int Volume) {
//****************************************************************************************************************************

  volume=Volume;
 
  reset();
}

//****************************************************************************************************************************
void VOLLevel::show(void) {
//****************************************************************************************************************************

  if (volume == 0)  display.drawBitmap(posX, posY, volume0icons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  else if (volume < 5) display.drawBitmap(posX, posY, volume1icons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  else if (volume < 9) display.drawBitmap(posX, posY, volume2icons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  else  display.drawBitmap(posX, posY, volume3icons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
}
    
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

//****************************************************************************************************************************
//****************************************************************************************************************************
//						SATLEVEL
//****************************************************************************************************************************
//****************************************************************************************************************************


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

  display.drawBitmap(posX, posY, saticons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);

  uint8_t satelliteBar = satelliteCount;
  if (satelliteBar > 15) satelliteBar = 15;
  satelliteBar /= 3;
  
  display.fillRect(posX+(satelliteBar*6), posY, 32-(satelliteBar*6), 32, GxEPD_WHITE);
}


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

//****************************************************************************************************************************
//****************************************************************************************************************************
//			RECORDIndicator
//****************************************************************************************************************************
//****************************************************************************************************************************

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
void RECORDIndicator::show(void) {
//****************************************************************************************************************************

  if( recordState==STATE_RECORD) {
	  if( displayRecord)
        display.drawBitmap(posX, posY,recicons,  32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
	  else
	    display.fillRect(posX, posY, 32, 32, GxEPD_WHITE);
  } else if  (recordState==STATE_GPSFIX) {
      display.drawBitmap(posX, posY,pauseicons,  32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  } else if (displayRecord) {
      display.drawBitmap(posX, posY,fix1icons,  32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  } else  {
      display.drawBitmap(posX, posY,fix2icons,  32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
  }
}

const unsigned char trendupicons[] = { 
 // 'Arrow-19-24'
0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xc3, 0xff, 0xff, 0x81, 0xff, 0xff, 0x00, 0xff, 0xfe, 
0x00, 0x7f, 0xfc, 0x24, 0x3f, 0xf8, 0x66, 0x1f, 0xf8, 0xe7, 0x1f, 0xf9, 0xe7, 0x9f, 0xff, 0xe7, 
0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 
0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 
0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff
};

const unsigned char trenddownicons[] = { 
 // 'Arrow-18-24'
0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 
0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 
0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xf9, 0xe7, 0x9f, 0xf8, 0xe7, 0x1f, 
0xf8, 0x66, 0x1f, 0xfc, 0x24, 0x3f, 0xfe, 0x00, 0x7f, 0xff, 0x00, 0xff, 0xff, 0x81, 0xff, 0xff, 
0xc3, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff
};

//****************************************************************************************************************************
//****************************************************************************************************************************
//				TRENDLevel
//****************************************************************************************************************************
//****************************************************************************************************************************


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


   /* check level and display arrow or blank */
    if( trendState == 1 )  {
	  display.drawBitmap(posX, posY,trendupicons,  24, 24, GxEPD_WHITE,false);   //GxEPD_BLACK);
    } else if( trendState == -1 ) {  
	  display.drawBitmap(posX, posY,trenddownicons,  24, 24, GxEPD_WHITE,false);   //GxEPD_BLACK);
    } else {
	  display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
    }
}

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

//****************************************************************************************************************************
//****************************************************************************************************************************
//				SCREENTIME
//****************************************************************************************************************************
//****************************************************************************************************************************

/* time */
//****************************************************************************************************************************
void ScreenTime::setTime(uint32_t newTime) {
//****************************************************************************************************************************

  for( uint8_t i = 0; i<3; i++) {
    time[i] = (int8_t)(newTime % 100);
    newTime /= 100;
  }
}

//****************************************************************************************************************************
void ScreenTime::setTime(int8_t* newTime) {
//****************************************************************************************************************************

  for(uint8_t i = 0; i<3; i++) {
    time[i] = newTime[i];
  }
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

/* !!! never reset, only on page change !!! */
//****************************************************************************************************************************
void ScreenTime::show(void) {
//****************************************************************************************************************************

  display.fillRect(posX-63, posY-32, 65, 34, GxEPD_WHITE);


  if (dot_or_h == false)
    display.drawBitmap(posX-65, posY-24,hicons,  16, 24, GxEPD_BLACK,false);   //GxEPD_BLACK);
  else	  
    display.drawBitmap(posX-67, posY-26, doticons, 16, 24, GxEPD_BLACK,false);   //GxEPD_BLACK);

  hour.setValue(time[2]);
  hour.show();
 //  display h ou dot

  minute.setValue(time[1]);
  minute.show();
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
}
  
//****************************************************************************************************************************
void FIXGPSInfo::unsetFixGps(void) {
//****************************************************************************************************************************
  FixGPS = false;
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
 if (FixGPS == true) display.drawBitmap(posX, posY, fixgpsicons, 24, 24, GxEPD_WHITE,false);   //GxEPD_BLACK);
 else                display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
  
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
}

//****************************************************************************************************************************
void INFOLevel::show() {
//****************************************************************************************************************************
// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
 if (InfoValue == INFO_USB) display.drawBitmap(posX, posY, usbicons, 32, 32, GxEPD_WHITE,false);   //GxEPD_BLACK);
 else                display.fillRect(posX, posY, 32, 32, GxEPD_WHITE);
}



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
//****************************************************************************************************************************
//					BTINFO
//****************************************************************************************************************************
//****************************************************************************************************************************

//****************************************************************************************************************************
void BTInfo::setBT(void) {
//****************************************************************************************************************************
  bt = 	true;
}
  
//****************************************************************************************************************************
void BTInfo::unsetBT(void) {
//****************************************************************************************************************************
  bt = false;
}

//****************************************************************************************************************************
boolean BTInfo::getBT(void) {
//****************************************************************************************************************************
  return bt;
}

//****************************************************************************************************************************
void BTInfo::show() {
//****************************************************************************************************************************
// display.drawBitmap(msicons, posX, posY, 48, 48, GxEPD_WHITE,false);   //GxEPD_BLACK);
 if (bt == true) display.drawBitmap(posX, posY, bticons, 24, 24, GxEPD_WHITE,false);   //GxEPD_BLACK);
 else                display.fillRect(posX, posY, 24, 24, GxEPD_WHITE);
 //  display.drawBitmap(100, 10, gridicons_sync, 24, 24, GxEPD_BLACK);
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
		led1stat = 1 - led1stat;

#ifdef SCREEN_DEBUG
		digitalWrite(pinLED, led1stat);   // turn the LED on or off
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
	pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, LOW); 
#endif //SCREEN_DEBUG
	
	led1stat = 0;  
	oldtime  = 0;

#ifdef TIMER_DISPLAY
	
	timerScreenScheduler = timerBegin(1, 80, true);
	timerAttachInterrupt(timerScreenScheduler, &onTimerScreenScheduler, true);
	timerAlarmWrite(timerScreenScheduler, 10000, true);  //100Hz
	timerAlarmEnable(timerScreenScheduler);
#endif //TIMER_DISPLAY
}
		 
//****************************************************************************************************************************
void ScreenScheduler::displayStep(void) {
//****************************************************************************************************************************

#ifndef TIMER_DISPLAY

	if (millis() - oldtime >= GnuSettings.VARIOMETER_MULTIDISPLAY_DURATION)
	{
		oldtime  = millis();
//		led1stat = 1 - led1stat;
		
#ifdef SCREEN_DEBUG
		digitalWrite(pinLED, led1stat);   // turn the LED on or off
#endif //SCREEN_DEBUG
	}

#endif //TIMER_DISPLAY

  /* next try to find something to display */
  /* for the current page                  */
#ifdef SCREEN_DEBUG
  SerialPort.print("displaystep - objectCount  : ");
  SerialPort.println(objectCount);
#endif //SCREEN_DEBUG

  display.setFullWindow();
 	
	uint8_t n = 0;
	while( n <= objectCount ) {
/*   if( displayList[pos].page == currentPage && displayList[pos].actif == true && displayList[pos].object->update() ) {
	return;
}*/
#ifdef SCREEN_DEBUG
  SerialPort.print("displaylist / Page: ");
//  SerialPort.println(displayList[pos].page);
  SerialPort.println(displayList[n].page);
  SerialPort.print("displaylist / Pageactive: ");
  SerialPort.println(currentPage);
	
#endif //SCREEN_DEBUG

/*	if( displayList[pos].page == currentPage && displayList[pos].actif == true) {
		ScreenDigit* tmppointeur = (ScreenDigit*)displayList[pos].object;
		tmppointeur->update(); 
	}*/

  currentMultiDisplay = 1 + led1stat;
	
#ifdef SCREEN_DEBUG
  SerialPort.print("Current Multidisplay : ");
  SerialPort.println(currentMultiDisplay);
#endif //SCREEN_DEBUG
	
	if( displayList[n].page == currentPage && displayList[n].actif == true && (displayList[n].multiDisplayID == 0 || displayList[n].multiDisplayID == currentMultiDisplay))	displayList[n].object->update();

/* next */
		pos++;
		if( pos == objectCount) {
			pos = 0;
		}
		n++;
	}

#ifdef SCREEN_DEBUG
  SerialPort.println("displayStep : Display");
#endif //SCREEN_DEBUG

/*  display.setCursor(10, 150);
  display.print("HelloWorld");*/

//    display.displayWindow(box_x, box_y, box_w, box_h);

//  display.display(true); // partial update
	
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

//****************************************************************************************************************************
void ScreenScheduler::setPage(int8_t page, boolean force)  {
//****************************************************************************************************************************

  /* check if page change is needed */
  if (force == false) {
    if ( page == currentPage ) {
      return;
    }

    /* set the new page */
    currentPage = page;
  }

  /* screen need to by cleared */
//  display.clearScreen();
 // display.eraseDisplay();
    display.fillRect(0, 0, display.width(), display.height(), GxEPD_WHITE);

  /* all the page object need to be redisplayed */
  /* but no problem to reset all the objects */
  for(uint8_t i = 0; i<objectCount; i++) {
    displayList[i].object->reset();
  }
}

//****************************************************************************************************************************
void ScreenScheduler::nextPage(void) {
//****************************************************************************************************************************
  
  uint8_t newPage = currentPage + 1;
  if( newPage > endPage ) {
    newPage = 0;
  }

  setPage(newPage);
}
 
//****************************************************************************************************************************
void ScreenScheduler::previousPage(void) {
//****************************************************************************************************************************
  
  uint8_t newPage = currentPage - 1;
  if( newPage < 0 ) {
    newPage = endPage;
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

/**************************************************************************/
/*!
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

/**************************************************************************/
/*!
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


/**************************************************************************/
/*!
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