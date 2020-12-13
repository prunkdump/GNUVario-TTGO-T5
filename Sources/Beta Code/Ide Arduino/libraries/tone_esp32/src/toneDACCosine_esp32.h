#ifndef toneDACCosine_ESP32_h
#define toneDACCosine_ESP32_h

/*****************************************************************************/
/*                                                                           */
/*                        Libraries ToneDAC ESP32                            */
/*                                                                           */
/*  version    Date        Description                                       */
/*    1.0      03/03/19                                                      */
/*    1.1      07/04/19    Reecriture librairie                              */
/*    1.1.1    10/06/19    Ajout gestion ampli class D externe               */
/*    1.1.2    20/08/19    Reecriture AUDIO_AMP_ENABLE et AUDIO_AMP_DISABLE  */
/*    1.1.3    10/01/20		 ajout defined(TONEDAC)														 */
/*    1.1.4    12/01/20    ajout trace																			 */
/*    1.1.5    26/07/20    Renomage TONEDAC en TONEDACCOSINE                 */
/*                                                                           */
/*****************************************************************************/

#if defined(ESP32) && defined(TONEDACCOSINE)

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

class ToneDacEsp32 {

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
		void Audio_Amp_Enable(void);

#if defined(TONEDAC_EXTENDED)
		
#endif //TONEDAC_EXTENDED

		uint8_t getVolume();
		
	private:

		dac_channel_t channelDAC;
		bool 			toneDACMuted = false;
		uint32_t 	multiplicateur;
		uint32_t 	_frequency;
				
		void dac_cosine_enable(dac_channel_t channel);
		void dac_frequency_set(int clk_8m_div, int frequency_step);
		void dac_scale_set(dac_channel_t channel, int scale);
		void dac_offset_set(dac_channel_t channel, int offset);
		void dac_invert_set(dac_channel_t channel, int invert);
		void AUDIO_AMP_DISABLE(void);
		void AUDIO_AMP_ENABLE(void);			
			
#ifdef TONEDAC_LENGTH
		unsigned long _tDAC_time; // Used to track end note with timer when playing note in the background.
#endif

		uint8_t _tDAC_volume = 100; 

};

#endif //ESP32
#endif
	
	


