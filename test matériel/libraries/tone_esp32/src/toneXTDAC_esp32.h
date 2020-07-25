#ifndef toneXTDAC_ESP32_h
#define toneXTDAC_ESP32_h

/*****************************************************************************/
/*                                                                           */
/*                        Libraries ToneDAC ESP32                            */
/*                                                                           */
/*  version    Date        Description                                       */
/*    1.0      13/06/20                                                      */
/*                                                                           */
/*****************************************************************************/

#include <HardwareConfig.h>

#if defined(ESP32) && defined(TONEXTDAC)

#include <Arduino.h>

#define TONEDAC_VOLUME  //set to have volume control
//#define TONEDAC_LENGTH  //set to have length control
//#define TONEDAC_EXTENDED
	
#if defined(TONEDAC_EXTENDED)
#include <SPI.h>
#include "SdFat.h"
#endif //HAVE_SDCARD

#include "driver/dac.h"

extern "C" {
#include "esp32-hal-dac.h"
}

#include "XT_DAC_Audio.h"

class ToneXTDacEsp32 {

	public: 
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
		void update();
		void Audio_Amp_Enable(void);

#if defined(TONEDAC_EXTENDED)
		
#endif //TONEDAC_EXTENDED

		uint8_t getVolume();
		
	private:

		dac_channel_t channelDAC;
		bool 			toneDACMuted = false;
		uint32_t 	multiplicateur;
		uint32_t 	_frequency;
		
		void AUDIO_AMP_DISABLE(void);
		void AUDIO_AMP_ENABLE(void);			
			
#ifdef TONEDAC_LENGTH
		unsigned long _tDAC_time; // Used to track end note with timer when playing note in the background.
#endif

		uint8_t _tDAC_volume = 100; 

};

#endif //ESP32
#endif
	
	


