// ---------------------------------------------------------------------------
//
// SOUND Library I2S for ESP32
//
// ----------------------------------------------------------------------------

/***********************************************************************************/
/*                                                                                 */
/*                           Libraries tone_esp                                    */
/*                                                                                 */
/*  version    Date          Description                                           */
/*    1.0.0    06/09/20				                                                     */
/*                                                                                 */
/***********************************************************************************/

/* toneI2S_esp32 -- tone HAL
 *
 * Copyright 2020 Jean-philippe GOI
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
 
#ifndef ToneI2S_ESP32_h
#define ToneI2S_ESP32_h

#if defined(ESP32)

#include "Arduino.h"
#include "HardwareConfig.h"
#include "DebugConfig.h"

//#include "utility\HardwareConfig.h"

#if defined(TONEI2S)

#include "driver/i2s.h"

#define TONE_VOLUME  //set to have volume control
//#define TONE_LENGTH  //set to have length control

#ifndef AUDIO_I2S_CONFIG

#define   DAC_INTERNE     // Definit si le DAC interne est utilisé

#define I2S_NUM         (0)
#define I2S_BCK_IO      (26)  //(GPIO_NUM_13)   //BCLK
#define I2S_WS_IO       (25)  //(GPIO_NUM_15)   //LRC
#define I2S_DO_IO       (14) //(GPIO_NUM_21)   //DIN
#define I2S_DI_IO       (-1)

#endif //AUDIO_I2S_CONFIG

//#define   DAC_16B           // Definit si le DAC externe est utiliser en 16bit ou en 8bits

#ifdef DAC_INTERNE    
#undef DAC_16B
#endif

#define   SAMPLE_RATE    (16000)
extern uint16_t  WAVE_FREQ_HZ;
extern uint16_t  VOLUME_I2S;

#ifdef  DAC_16B
extern volatile uint16_t   COEF_AMP; 
#else
extern volatile uint8_t    COEF_AMP;  
#endif
extern uint16_t  CYCLE;  // msec
extern uint8_t   DUTY;  //%

#define PI              (3.14159265)
#define PI2             (6.283185)

#define I2S_DMA_BUFFER  (256)

#ifndef DEFAULT_VOLUME
#define DEFAULT_VOLUME 			5
#endif

//#define TIMERI2S

//Configuration I2S

extern i2s_config_t i2s_config;
extern i2s_pin_config_t pin_config;


#ifdef SOUND_DEBUG
extern int interruptCounter;
extern int totalInterruptCounter;
#endif
 
extern hw_timer_t * timerI2S;
extern portMUX_TYPE timerMux;

extern int16_t TabSinus[360];

bool consumeData(void);
bool consumeFullData(void);
void IRAM_ATTR onTimer();
void beepI2S(uint16_t frequence);

extern int maxPhase;  // = MaxPhase;   // Should make 800 Hz, gives 1600 Hz and all of the harmonics as well

extern uint32_t currentSample, compteur, maxSample;

extern bool actif;
extern int phase;
extern float ecartPhase;

/*
extern "C" {
#include "esp32-hal-dac.h"
}


#define TONE_VOLUME  //set to have volume control
  //#define TONE_LENGTH  //set to have length control*/

class ToneI2SEsp32 {

	public:
		ToneI2SEsp32(void);

		// must be run before using toneAC 
		void init(void);

		void init(uint32_t pin);
		
		void tone(unsigned long frequency);

  #ifdef TONE_VOLUME		
		void tone(unsigned long frequency, uint8_t volume
  #endif
  #ifdef TONE_LENGTH
		, unsigned long length = 0, uint8_t background = false
  #endif
		);

    void noTone();
	
		// mute/unmute setting 
    void toneMute(bool newMuteState);
	
		void setVolume(uint8_t newVolume = DEFAULT_VOLUME);
    uint8_t getVolume();
		void update();

  private:
		
		bool 		 toneMuted = false;		
    uint8_t  _volume;
    uint16_t _frequency;
		
/*
    void noTone();
	
		// mute/unmute setting 
    void toneMute(bool newMuteState);
	
		void setVolume(uint8_t newVolume = DEFAULT_VOLUME);
    uint8_t getVolume();
	
	
  private:
		
		bool 				toneMuted = false;		
		uint8_t   	Speaker_Pin;
    uint8_t   	Tone_Pin_Channel;
		
#ifdef TONE_LENGTH
		unsigned long _tAC_time; // Used to track end note with timer when playing note in the background.
#endif

#ifdef TONE_VOLUME
//uint8_t _tAC_volume[] = { 200, 100, 67, 50, 40, 33, 29, 22, 11, 2 }; // Duty for linear volume control.
//		uint8_t _tAC_volume[10] = { 150, 72, 51, 38, 32, 23, 20, 19, 10, 2 }; //new duty values for three phased Low Power mode
uint8_t _tAC_volume[10] = { 4, 22, 44, 58, 66, 80, 100, 134, 200, 250 }; // Duty for linear volume control.
#endif

    uint8_t _volume;
    uint16_t _beep_duration, _frequency;
    uint16_t _beep_freq;
    void setBeep(uint16_t frequency, uint16_t duration);

		uint16_t  CYCLE        =  500;  // msec
		uint8_t   DUTY         =  100;  //%*/
		
		void AUDIO_AMP_DISABLE(void);
		void AUDIO_AMP_ENABLE(void);				
};

#endif //TONEI2S

#else
#error “This library only supports boards with ESP32 processor.”
#endif

#endif

