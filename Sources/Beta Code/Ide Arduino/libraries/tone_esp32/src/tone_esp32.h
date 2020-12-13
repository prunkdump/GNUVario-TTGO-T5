// ---------------------------------------------------------------------------
//
// SOUND Library PWM for ESP32
//
// ----------------------------------------------------------------------------

/***********************************************************************************/
/*                                                                                 */
/*                           Libraries tone_esp                                    */
/*                                                                                 */
/*  version    Date          Description                                           */
/*    1.0.0    17/02/19				                                                     */
/*    1.0.1    19/03/19                                                            */
/*    1.0.2    10/06/19      Ajout gestion ampli class D externe                   */
/*    1.0.3    20/08/19      Reecriture AUDIO_AMP_ENABLE et AUDIO_AMP_DISABLE      */
/*    1.0.4    25/08/19      Ajout AUDIO_AMP_MODE_LOW                              */
/*		1.0.5		 30/08/19			 Deplacement gestion ampli dans tone_HAL							 */
/*                                                                                 */
/***********************************************************************************/

/* tone_esp32 -- tone HAL
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
 
#ifndef tone_ESP32_h
#define tone_ESP32_h

#if defined(ESP32)

#include "HardwareConfig.h"

#if defined(TONE)

#include "Arduino.h"
//#include "utility\HardwareConfig.h"

//#define SPEAKER_PIN 25
//#define SPEAKER_PIN 26
//#define TONE_PIN_CHANNEL 0


extern "C" {
#include "esp32-hal-dac.h"
}


#define TONEAC_VOLUME  //set to have volume control
  //#define TONEAC_LENGTH  //set to have length control

	class ToneEsp32 {

	public:
		ToneEsp32(void);

		/* must be run before using toneAC */
		void init(void);

		void init(uint32_t pin);
		
		void tone(unsigned long frequency = 0
  #ifdef TONEAC_VOLUME
                , uint8_t volume = 10
  #endif
  #ifdef TONEAC_LENGTH
		, unsigned long length = 0, uint8_t background = false
  #endif
		);

    void noTone();
	
/* mute/unmute setting */
    void toneMute(bool newMuteState);
	
  private:
		
		bool 				toneACMuted = false;		
		uint8_t   	Speaker_Pin;
    uint8_t   	Tone_Pin_Channel;
		
#ifdef TONEAC_LENGTH
		unsigned long _tAC_time; // Used to track end note with timer when playing note in the background.
#endif

#ifdef TONEAC_VOLUME
//uint8_t _tAC_volume[] = { 200, 100, 67, 50, 40, 33, 29, 22, 11, 2 }; // Duty for linear volume control.
//		uint8_t _tAC_volume[10] = { 150, 72, 51, 38, 32, 23, 20, 19, 10, 2 }; //new duty values for three phased Low Power mode
uint8_t _tAC_volume[10] = { 4, 22, 44, 58, 66, 80, 100, 134, 200, 250 }; // Duty for linear volume control.
#endif

    uint8_t _volume;
    uint16_t _beep_duration;
    uint16_t _beep_freq;
    void setBeep(uint16_t frequency, uint16_t duration);

};

#endif //TONE

#else
#error “This library only supports boards with ESP32 processor.”
#endif

#endif

