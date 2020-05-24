/* toneHAL -- derived class for ESP32
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * This file is part of ToneHAL.
 *
 * toneHAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * toneHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
 
/*********************************************************************************/
/*                                                                               */
/*                           Libraries ToneHal                                   */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0                                                                        */
/*    1.1    30/08/19   Ajout gestion commande Ampli                             */
/*    1.1.1  12/01/20   Ajout Trace                                              */
/*                                                                               */
/*********************************************************************************/
 
#include "toneHAL.h"
#include "toneHAL_ESP32.h"

#if defined(ESP32)
#include <Arduino.h>
#include "DebugConfig.h"
//#include "utility\DebugConfig.h"

#include <HardwareConfig.h>

#ifdef SOUND_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#define TONEDAC_VOLUME  //set to have volume control
//#define TONEDAC_LENGTH  //set to have length control

#if defined(TONEDAC)
/****************************************/
/*            D A C						*/
/****************************************/

/***********************************/
void ToneHalDAC_Esp32::init(void) {
/***********************************/
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneDAC");
	TRACE();
#endif //SOUND_DEBUG

  privateToneDacEsp32.init();
#if defined(HAVE_AUDIO_AMPLI)
	if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_INIT();

#ifndef AUDIO_AMPLI_LOWPOWER
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif 
#endif //HAVE_AUDIO_AMPLI
}

/***********************************/
void ToneHalDAC_Esp32::init(uint32_t pin) {
/***********************************/
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneDAC");
	TRACE();
#endif //SOUND_DEBUG

  privateToneDacEsp32.init(pin);
#if defined(HAVE_AUDIO_AMPLI)
	if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_INIT();

#ifndef AUDIO_AMPLI_LOWPOWER
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif 
#endif //HAVE_AUDIO_AMPLI
}

/***********************************/
void ToneHalDAC_Esp32::init(uint32_t pin1, uint32_t pin2) {
/***********************************/
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneDAC");
	TRACE();
#endif //SOUND_DEBUG

  privateToneDacEsp32.init(pin1);
#if defined(HAVE_AUDIO_AMPLI)
	if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_INIT();

#ifndef AUDIO_AMPLI_LOWPOWER
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif 
#endif //HAVE_AUDIO_AMPLI
}

/***********************************/
void ToneHalDAC_Esp32::tone(unsigned long frequency)
/***********************************/           
{
#ifdef SOUND_DEBUG
					SerialPort.print("tone - frequence : ");
					SerialPort.println(frequency);
#endif //SOUND_DEBUG
	
#if defined (TONEHAL_EXTENDED_VOLUME)
	if (_toneMuted) {
		
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
		if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI

		return;
	}

#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI
	
  privateToneDacEsp32.tone(frequency, _volume);
#else
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI

	privateToneDacEsp32.tone(frequency, privateToneDacEsp32.getVolume());
#endif	
}

/***********************************/
void ToneHalDAC_Esp32::tone(unsigned long frequency , uint8_t volume)
/***********************************/           
{
#ifdef SOUND_DEBUG
					SerialPort.print("tone - frequence : ");
					SerialPort.println(frequency);
					SerialPort.print("tone - volume : ");
					SerialPort.println(volume);
#endif //SOUND_DEBUG
	
#if defined (TONEHAL_EXTENDED_VOLUME)
	if (_toneMuted) {
		
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
		if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI
		
		return;
	}
	if (volume > 10) volume = 10;
	_volume = volume;
#endif
	
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI
	
#ifdef TONEDAC_DEBUG	
	SerialPort.print("volume toneHalDac = ");
	SerialPort.println(volume);
#endif TONEDAC_DEBUG
	
#ifdef TONEDAC_DEBUG	
	SerialPort.println("privateToneDacEsp32(freq,vol)");
#endif TONEDAC_DEBUG
  privateToneDacEsp32.tone(frequency, volume*10);
}

/***********************************/
void ToneHalDAC_Esp32::tone(unsigned long frequency , uint8_t volume, unsigned long length)
/***********************************/           
{
#ifdef SOUND_DEBUG
					SerialPort.print("tone - frequence : ");
					SerialPort.println(frequency);
					SerialPort.print("tone - volume : ");
					SerialPort.println(volume);
					SerialPort.print("tone - length : ");
					SerialPort.println(length);
#endif //SOUND_DEBUG
	
#if defined (TONEHAL_EXTENDED_VOLUME)
	if (_toneMuted) {
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
		if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI
		return;
	}
	if (volume > 10) volume = 10;
	_volume = volume;
#endif

#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI

  privateToneDacEsp32.tone(frequency, volume*10);
}

/***********************************/
void ToneHalDAC_Esp32::tone(unsigned long frequency , uint8_t volume, unsigned long length, uint8_t background)
/***********************************/
{
#ifdef SOUND_DEBUG
					SerialPort.print("tone - frequence : ");
					SerialPort.println(frequency);
					SerialPort.print("tone - volume : ");
					SerialPort.println(volume);
					SerialPort.print("tone - length : ");
					SerialPort.println(length);
#endif //SOUND_DEBUG
	
#if defined (TONEHAL_EXTENDED_VOLUME)
	if (_toneMuted) {
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
		if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI

		return;
	}
	if (volume > 10) volume = 10;
	_volume = volume;
#endif

	tone(frequency, volume*10, length);
}

/***********************************/
void ToneHalDAC_Esp32::noTone() {
/***********************************/
#ifdef SOUND_DEBUG
					SerialPort.println("notone");
#endif //SOUND_DEBUG

#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
		if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI

  privateToneDacEsp32.noTone();
}
		
#if defined(TONEDAC_EXTENDED)
/***********************************/
void ToneHalDAC_Esp32::beginPlayWav(uint32_t srate)
/***********************************/           
{
	privateToneDacEsp32.begin(srate);
}

/***********************************/
void ToneHalDAC_Esp32::endPlayWav()
/***********************************/          
{
	privateToneDacEsp32.end();
}

/***********************************/
void ToneHalDAC_Esp32::playWav(const char *fname) {
/***********************************/
  privateToneDacEsp32.play(fname);
}

/***********************************/
bool ToneHalDAC_Esp32::isPlayingWav() {
/***********************************/
  return privateToneDacEsp32.isPlaying();
}

/***********************************/
uint32_t ToneHalDAC_Esp32::duration() {
/***********************************/
  return privateToneDacEsp32.duration();
}

/***********************************/
uint32_t ToneHalDAC_Esp32::remaining() {
/***********************************/
  return privateToneDacEsp32.remaining();
}

#endif //TONEDAC_EXTENDED
#elif defined(TONEAC)
/****************************************/
/*         P  W  M  - Push-Pull      		*/
/****************************************/

#elif defined(TONE)

//****************************************
//*          P W M    - 1 pin           	*
//****************************************

/***********************************/
void ToneHal_Esp32::init(void) {
/***********************************/
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneAC");
	TRACE();
#endif //SOUND_DEBUG

	privateToneEsp32.init();
	
#if defined(HAVE_AUDIO_AMPLI)
	if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_INIT();

#ifndef AUDIO_AMPLI_LOWPOWER
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif 
#endif //HAVE_AUDIO_AMPLI}
}

/***********************************/
void ToneHal_Esp32::init(uint32_t pin) {
/***********************************/
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneAC");
	TRACE();
#endif //SOUND_DEBUG

	privateToneEsp32.init(pin);
	
#if defined(HAVE_AUDIO_AMPLI)
	if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_INIT(); 

#ifndef AUDIO_AMPLI_LOWPOWER
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif
#endif //HAVE_AUDIO_AMPLI	
}

/***********************************/
void ToneHal_Esp32::init(uint32_t pin1, uint32_t pin2) {
/***********************************/
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneAC");
	TRACE();
#endif //SOUND_DEBUG

	privateToneEsp32.init(pin1);
	
#if defined(HAVE_AUDIO_AMPLI)
	if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_INIT();
 
#ifndef AUDIO_AMPLI_LOWPOWER
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif
#endif //HAVE_AUDIO_AMPLI	
}

/***********************************/
void ToneHal_Esp32::tone(unsigned long frequency)
/***********************************/           
{
#ifdef SOUND_DEBUG
					SerialPort.print("tone - frequence : ");
					SerialPort.println(frequency);
#endif //SOUND_DEBUG
	
#if defined (TONEHAL_EXTENDED_VOLUME)
	if (_toneMuted) {
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
		if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI
		return;
	}
	
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI
	
  privateToneEsp32.tone(frequency,_volume);	
#else
	
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI

  privateToneEsp32.tone(frequency, 5);	
#endif
}

/***********************************/
void ToneHal_Esp32::tone(unsigned long frequency , uint8_t volume)
/***********************************/           
{
#ifdef SOUND_DEBUG
					SerialPort.print("tone - frequence : ");
					SerialPort.println(frequency);
					SerialPort.print("tone - volume : ");
					SerialPort.println(volume);
#endif //SOUND_DEBUG

#if defined (TONEHAL_EXTENDED_VOLUME)	
	if (_toneMuted) {
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
		if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI
		return;
	}
	if (volume > 10) volume = 10;
//	_volume = volume;
#endif
#ifdef TONEAC_VOLUME

#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI

	privateToneEsp32.tone(frequency, volume);
#else
	
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI

	privateToneEsp32.tone(frequency);
#endif		
}

/***********************************/
void ToneHal_Esp32::tone(unsigned long frequency , uint8_t volume, unsigned long length)
/***********************************/           
{
#ifdef SOUND_DEBUG
					SerialPort.print("tone - frequence : ");
					SerialPort.println(frequency);
					SerialPort.print("tone - volume : ");
					SerialPort.println(volume);
					SerialPort.print("tone - length : ");
					SerialPort.println(length);
#endif //SOUND_DEBUG

#if defined (TONEHAL_EXTENDED_VOLUME)	
	if (_toneMuted) {
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
		if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI
		return;
	}
	if (volume > 10) volume = 10;
//	_volume = volume;
	if (length > 1024) length = 1024;
#endif
	
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI
	
#ifdef TONEAC_LENGTH
		  privateToneEsp32.tone(frequency, volume, length);
#else
#ifdef TONEAC_VOLUME
	privateToneEsp32.tone(frequency, volume);
#else
	privateToneEsp32.tone(frequency);
#endif	
#endif

}

/***********************************/
void ToneHal_Esp32::tone(unsigned long frequency , uint8_t volume, unsigned long length, uint8_t background)
/***********************************/           
{
#ifdef SOUND_DEBUG
					SerialPort.print("tone - frequence : ");
					SerialPort.println(frequency);
					SerialPort.print("tone - volume : ");
					SerialPort.println(volume);
					SerialPort.print("tone - length : ");
					SerialPort.println(length);
#endif //SOUND_DEBUG

#if defined (TONEHAL_EXTENDED_VOLUME)	
	if (_toneMuted) {
#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
		if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI
		return;
	}
	if (volume > 10) volume = 10;
//	_volume = volume;
	if (length > 1024) length = 1024;
#endif

#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI

#ifdef TONEAC_LENGTH
		  privateToneEsp32.tone(frequency, volume, length, background);
#else
#ifdef TONEAC_VOLUME
	privateToneEsp32.tone(frequency, volume);
#else
	privateToneEsp32.tone(frequency);
#endif	
#endif	
}

/***********************************/
void ToneHal_Esp32::noTone(void)
/***********************************/           
{
#ifdef SOUND_DEBUG
					SerialPort.println("notone");
#endif //SOUND_DEBUG

#if defined(HAVE_AUDIO_AMPLI) && defined(AUDIO_AMPLI_LOWPOWER)	
	if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI
	privateToneEsp32.noTone();
}

/*
#elif defined(TONE)
// ****************************************
// *          P W M    - 1 pin            *
// ****************************************

// ***********************************
void ToneHal_Esp32::init(void) {
// ***********************************
  _pin = 2;
}

// ***********************************
void ToneHal_Esp32::init(uint32_t pin) {
// ***********************************
  _pin = pin;
}

// ***********************************
void ToneHal_Esp32::init(uint32_t pin1, uint32_t pin2) {
// ***********************************
  _pin = pin1;
}

// ***********************************
void ToneHal_Esp32::tone(unsigned long frequency)
// ***********************************           
{
	if (_toneMuted) return;
  privateToneEsp32.tone(_pin,frequency,512);	
}

// ***********************************
void ToneHal_Esp32::tone(unsigned long frequency , uint8_t volume)
// ***********************************           
{
	if (_toneMuted) return;
	if (volume > 10) volume = 10;
	_volume = volume;
  privateToneEsp32.tone(_pin,frequency,512);
}

// ***********************************
void ToneHal_Esp32::tone(unsigned long frequency , uint8_t volume, unsigned long length)
// ***********************************           
{
	if (_toneMuted) return;
	if (volume > 10) volume = 10;
	_volume = volume;
	if (length > 1024) length = 1024;
  privateToneEsp32.tone(_pin,frequency,length);
}

// ***********************************
void ToneHal_Esp32::tone(unsigned long frequency , uint8_t volume, unsigned long length, uint8_t background)
// ***********************************
{
	tone(frequency, volume, length);
}

// ***********************************
void ToneHal_Esp32::noTone(void)
// ***********************************           
{
	privateToneEsp32.noTone(_pin);
}
*/

#elif defined(TONEI2S)
/****************************************/
/*            I  2  S					*/
/****************************************/

#endif //Interface type

#endif //ESP32