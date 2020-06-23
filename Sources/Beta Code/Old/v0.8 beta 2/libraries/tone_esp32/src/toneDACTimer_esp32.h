#ifndef toneDACTIMER_ESP32_h
#define toneDACTIMER_ESP32_h

/****************************************************/
/*                                                  */
/*          Libraries ToneDAC Timer ESP32           */
/*                                                  */
/*  version    Date     Description                 */
/*    1.0    03/03/19                               */
/*    1.1    10/01/20   Maj                         */
/*                      Ajout defined(TONEDACTIMER) */
/*                                                  */
/****************************************************/

#if defined(ESP32) && defined(TONEDACTIMER)

#include <Arduino.h>

#define TONEDAC_VOLUME  //set to have volume control
//#define TONEDAC_LENGTH  //set to have length control
//#define TONEDAC_EXTENDED

#define WAVEFORM_SINUS			1
#define WAVEFORM_TRIANGLE		2
#define WAVEFORM_SQUARE			3
#define WAVEFORM_WAV 				4
	
#define SAMPLERATE  				10000000
#define SAMPLECOUNT					256
	
#if defined(TONEDAC_EXTENDED)
#include <SPI.h>
#include "SdFat.h"
#endif //HAVE_SDCARD

void IRAM_ATTR onTimer();

extern "C" {
#include "esp32-hal-dac.h"
}

class ToneDacEsp32 {

	public:
		uint32_t 	fileSize = 0;
 		uint32_t 	sampleRate = 1000000; //sample rate of the sine wave
 
    void init(void);
		void init(uint32_t pin);
	  void begin(uint32_t srate);
		void end();
    void tone(unsigned long frequency = 0
  #ifdef TONEDAC_VOLUME
                , uint8_t volume = 100
  #endif
  #ifdef TONEDAC_LENGTH
		, unsigned long length = 0, uint8_t background = false
  #endif
		);

    void noTone();
        /* mute/unmute setting */
    void toneMute(bool newMuteState);
		void setWaveForm(uint8_t form);
		void Audio_Amp_Enable(void);

#if defined(TONEDAC_EXTENDED)
		
    void play(const char *fname) ;
    bool isPlaying();
    uint32_t duration();
    uint32_t remaining();
#endif //TONEDAC_EXTENDED

		uint8_t getVolume();
		
	private:

		bool 			toneDACMuted = false;
		uint32_t 	multiplicateur;
		uint32_t 	_frequency;
				
		void genForm(int sCount);
		void genSin(int sCount);
		void genSquare(int sCount);
		void genTriangle(int sCount);

		void tcReset();
		void tcDisable();
		void tcConfigure(int sampleRate);
		void tcStartCounter();
		bool tcIsSyncing();
		void AUDIO_AMP_DISABLE(void);
		void AUDIO_AMP_ENABLE(void);			
				
#ifdef TONEDAC_LENGTH
		unsigned long _tDAC_time; // Used to track end note with timer when playing note in the background.
#endif

		uint8_t _tDAC_volume = 100; //new duty values for three phased Low Power mode

};

#if defined(TONEDAC_EXTENDED)
//extern Class_ToneDac toneDAC;
extern SdFat SD;
#endif //TONEDAC_EXTENDED

#endif //ESP32
#endif
	
	


