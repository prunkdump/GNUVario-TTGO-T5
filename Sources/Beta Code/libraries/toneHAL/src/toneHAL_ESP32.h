/* toneHAL -- derived class for ESP32
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * This file is part of ToneHAL.
 *
 * GNUVario is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNUVario is distributed in the hope that it will be useful,
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
/*    1.2    13/06/20   Ajout XTDAC                                              */
/*    1.2.1  26/07/20   Renomage lib TONEDAC en TONEDACCOSINE                    */
/*                                                                               */
/*********************************************************************************/
 
#ifndef toneHAL_ESP32_h
#define toneHAL_ESP32_h

#if defined(ESP32)

#include <HardwareConfig.h>

#include <toneHAL.h>

#if defined(TONEDACCOSINE) || defined(TONEDACTIMER)
#include <toneDACCosine_esp32.h>
#include <toneDACTimer_esp32.h>

class ToneHalDAC_Esp32 : public ToneHal {
	public:

		void init(void);
		void init(uint32_t pin);
    void init(uint32_t pin1, uint32_t pin2);
    void tone(unsigned long frequency);
    void tone(unsigned long frequency, uint8_t volume);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length, uint8_t background);

		void noTone();
		void update() {};

#if defined(TONEDAC_EXTENDED)
	  void beginPlayWav(uint32_t srate);
		void endPlayWav();
    void playWav(const char *fname) ;
    bool isPlayingWav();
    uint32_t duration();
    uint32_t remaining();	
#endif //TONEDAC_EXTENDED
		
	private:
		ToneDacEsp32 privateToneDacEsp32;
};	
	
#define ToneHAL ToneHalDAC_Esp32	
	
#elif	defined(TONEXTDAC)
#include <toneXTDAC_esp32.h>

class ToneHalXTDAC_Esp32 : public ToneHal {
	public:

		void init(void);
		void init(uint32_t pin);
    void init(uint32_t pin1, uint32_t pin2);
    void tone(unsigned long frequency);
    void tone(unsigned long frequency, uint8_t volume);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length, uint8_t background);

		void noTone();
		
		void update() {privateToneXTDacEsp32.update();};

#if defined(TONEDAC_EXTENDED)
	  void beginPlayWav(uint32_t srate);
		void endPlayWav();
    void playWav(const char *fname) ;
    bool isPlayingWav();
    uint32_t duration();
    uint32_t remaining();	
#endif //TONEDAC_EXTENDED
		
	private:
		ToneXTDacEsp32 privateToneXTDacEsp32;
};	
	
#define ToneHAL ToneHalXTDAC_Esp32	
	
#elif defined(TONEAC)

#elif defined(TONE)
//#include <tone32.h>
#include <tone_esp32.h>

class ToneHal_Esp32 : public ToneHal {
	public:

		void init(void);
	  void init(uint32_t pin);
    void init(uint32_t pin1, uint32_t pin2);
    void tone(unsigned long frequency);
    void tone(unsigned long frequency, uint8_t volume);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length, uint8_t background);

		void noTone();
		
		void update() {};
		
	protected:
		uint32_t _pin;
		ToneEsp32 privateToneEsp32;
};	

#define ToneHAL ToneHal_Esp32	

#elif defined(TONEI2S)

/*class ToneHalI2S_Esp32 : ToneHal{
	public:

		void init(void);
		void init(uint32_t pin);
    void init(uint32_t pin1, uint32_t pin2);
    void tone(unsigned long frequency);
    void tone(unsigned long frequency, uint8_t volume);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length, uint8_t background);

		void noTone();
		
		void update() {};

		void setWaveForm(uint8_t form);

	  void beginPlayWav(uint32_t srate);
		void endPlayWav();
    void playWav(const char *fname) ;
    bool isPlayingWav();
    uint32_t duration();
    uint32_t remaining();		
};	

#define ToneHAL ToneHalI2S_Esp32	*/

#include <toneI2S_esp32.h>

class ToneHal_Esp32 : public ToneHal {
	public:

		void init(void);
	  void init(uint32_t pin);
    void init(uint32_t pin1, uint32_t pin2);
    void tone(unsigned long frequency);
    void tone(unsigned long frequency, uint8_t volume);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length);
    void tone(unsigned long frequency, uint8_t volume, unsigned long length, uint8_t background);

		void noTone();
		
		void update();
		
	protected:
		uint32_t _pin;
		ToneI2SEsp32 privateToneI2SEsp32;
};	

#define ToneHAL ToneHal_Esp32	

#endif //Interface type

#endif //ESP32

#endif

