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
 
#if defined(ESP32)

#include "HardwareConfig.h"
 
#if defined(TONEI2S)
 
#include "toneI2S_esp32.h"

#include "DebugConfig.h"
//#include "utility\Debug.h"

#ifdef SOUND_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include "driver/i2s.h"

#if not defined(_DEBUG_H_)
#define SOUND_DEBUG					//debug Tone
//#define SerialPort Serial
#endif

hw_timer_t * timerI2S = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

uint16_t  WAVE_FREQ_HZ =  3000;
uint16_t  VOLUME_I2S   =  100;  //0 à 100

#ifdef  DAC_16B
volatile uint16_t   COEF_AMP   =  32727; 
#else
volatile uint8_t    COEF_AMP   =  127;  
#endif
uint16_t  CYCLE        =  500;  // msec
uint8_t   DUTY         =  100;  //%

//Configuration I2S

i2s_config_t i2s_config =
{
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX
#ifdef DAC_INTERNE
    | I2S_MODE_DAC_BUILT_IN),
#else
    ),
#endif  
  .sample_rate = SAMPLE_RATE,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
#ifdef DAC_INTERNE
  .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
#elif defined (DAC_16B)
  .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S, 
#else
  .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB), //I2S_COMM_FORMAT_I2S_MSB,
#endif  
  .intr_alloc_flags = 0,   // Default interrupt priority
  .dma_buf_count = 2,
  .dma_buf_len = I2S_DMA_BUFFER * 4, //maxPhase * 4,
  .use_apll = false,
  .tx_desc_auto_clear = true
};

i2s_pin_config_t pin_config = {
  .bck_io_num   = I2S_BCK_IO,  //26, //this is BCK pin
  .ws_io_num    = I2S_WS_IO,   //25,     // this is LRCK pin
  .data_out_num = I2S_DO_IO,  //14, //13, //22, // this is DATA output pin
  .data_in_num  = -1   //Not used
};

int16_t   TabSinus[360];

int maxPhase;  // = MaxPhase;   // Should make 800 Hz, gives 1600 Hz and all of the harmonics as well

uint32_t currentSample, compteur, maxSample, buffcompteur;

bool actif = false;
int phase;
float ecartPhase;

#ifdef SOUND_DEBUG
int interruptCounter = 0;
int totalInterruptCounter;
#endif

//********************************************* 
void IRAM_ATTR onTimer() {
//********************************************* 
  portENTER_CRITICAL_ISR(&timerMux);
	if (interruptCounter > 0) return;
  interruptCounter++;

  consumeData();
    
	interruptCounter--;	
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

//********************************************* 
bool consumeData(void) {
//********************************************* 

	if (!actif) return true;
	
	buffcompteur=0;
  for(int j=0;j<256; j++) {
/*    float rad = (float)phase / maxPhase * (float)PI2;
    uint16_t sample = (sin(rad) + 1) * (int)((float)(COEF_AMP / 10) * VOLUME);   // Full amplitude: 127 en 8bits*/
 //   Serial.println(sample);
 //  uint32_t s32;
 //   s32 = (sample << 16) | (sample & 0xffff);
 
		 buffcompteur++;
//		 if (buffcompteur > 256) return false;

     float    f = ((float)VOLUME_I2S / 100) * TabSinus[(int)(ecartPhase * phase)];
     uint16_t sample = (int)round(f) - 127;


#ifdef  DAC_16B
      uint16_t buffer[2];
      buffer[0] = 0;      // Left channel
      buffer[1] = sample; // right channel
#else 
      uint8_t buffer[4];
      buffer[0] = 0;       // unused 
      buffer[1] = 0;       // left channel
      buffer[2] = 0;       // high byte (unused)
      buffer[3] = sample;  //right chanel
#endif
      compteur++;
  
    if (!i2s_write_bytes((i2s_port_t)I2S_NUM, buffer , sizeof(uint32_t), 0)) {
//      Serial.print("FULL DMA : ");
//      Serial.println(compteur);
      compteur = 0;
      return false;
    }
    else
    {
     phase++;
     if (phase >= maxPhase) {
       phase = 0;
     }

     currentSample++;
     if (currentSample > maxSample) {  //SAMPLE_RATE) {
       currentSample = 0;
       actif = false;
       i2s_zero_dma_buffer((i2s_port_t)I2S_NUM);
       i2s_stop((i2s_port_t)I2S_NUM);
			 return true;
//       Serial.println("Fin du Beep");
     }    
    }  
  }
}

//********************************************* 
bool consumeFullData(void) {
//********************************************* 

	buffcompteur=0;
  while (actif) {
/*    float rad = (float)phase / maxPhase * (float)PI2;
    uint16_t sample = (sin(rad) + 1) * (int)((float)(COEF_AMP / 10) * VOLUME);   // Full amplitude: 127 en 8bits*/
 //   Serial.println(sample);
 //  uint32_t s32;
 //   s32 = (sample << 16) | (sample & 0xffff);
 
		 buffcompteur++;
//		 if (buffcompteur > 256) return false;

     float    f = ((float)VOLUME_I2S / 100) * TabSinus[(int)(ecartPhase * phase)];
     uint16_t sample = (int)round(f) - 127;


#ifdef  DAC_16B
      uint16_t buffer[2];
      buffer[0] = 0;      // Left channel
      buffer[1] = sample; // right channel
#else 
      uint8_t buffer[4];
      buffer[0] = 0;       // unused 
      buffer[1] = 0;       // left channel
      buffer[2] = 0;       // high byte (unused)
      buffer[3] = sample;  //right chanel
#endif
      compteur++;
  
    if (!i2s_write_bytes((i2s_port_t)I2S_NUM, buffer , sizeof(uint32_t), 0)) {
//      Serial.print("FULL DMA : ");
//      Serial.println(compteur);
      compteur = 0;
      return false;
    }
    else
    {
     phase++;
     if (phase >= maxPhase) {
       phase = 0;
     }

     currentSample++;
     if (currentSample > maxSample) {  //SAMPLE_RATE) {
       currentSample = 0;
       actif = false;
       i2s_zero_dma_buffer((i2s_port_t)I2S_NUM);
       i2s_stop((i2s_port_t)I2S_NUM);
//       Serial.println("Fin du Beep");
     }    
    }  
  }
}

//********************************************* 
void beepI2S(uint16_t frequence) {
//********************************************* 

  portENTER_CRITICAL(&timerMux);
  phase    = 0;  //maxPhase * 0.75;
  compteur = 0;
  maxPhase = (int)(SAMPLE_RATE  /frequence); //WAVE_FREQ_HZ);
  ecartPhase = 360 / maxPhase;
  maxSample = (uint32_t)((SAMPLE_RATE / 1000) * CYCLE);
  portEXIT_CRITICAL(&timerMux);

  Serial.print("Max Sample : ");
  Serial.println(maxSample);
  
  i2s_start((i2s_port_t)I2S_NUM);
  portENTER_CRITICAL(&timerMux);
  actif = true;  
  portEXIT_CRITICAL(&timerMux);
  
  consumeFullData();
}

/***********************************/
ToneI2SEsp32::ToneI2SEsp32(void) {
/***********************************/
//    _volume = 8;
}

/***********************************/
void ToneI2SEsp32::init(void)
/***********************************/
{
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneI2SEsp32");
	TRACE();
#endif //SOUND_DEBUG

#if defined(DAC_INTERNE) && defined(HAVE_AUDIO_AMPLI)

//init Pin commande ampli

#ifdef SOUND_DEBUG
    SerialPort.println("AUDIO_AMP_INIT");
#endif //SOUND_DEBUG

  if (PIN_AUDIO_AMP_ENA != -1) {
    pinMode(PIN_AUDIO_AMP_ENA,OUTPUT);
    
#ifdef SOUND_DEBUG
    SerialPort.print("pin : ");
    SerialPort.println(PIN_AUDIO_AMP_ENA);
#endif //SOUND_DEBUG
    
#ifdef AUDIO_AMP_MODE_LOW
    digitalWrite(PIN_AUDIO_AMP_ENA,LOW);
#else
    digitalWrite(PIN_AUDIO_AMP_ENA,HIGH);
#endif  
  }
#endif
   
  for(int i=0; i<360;i++) {

//    # Chaque donnée représente 1/360ème du cycle, la valeur de crête est 127
    float f = 255.0 * sin( 3.14 * i / 180.0 );

//    # Conversion en entier et transposition de la courbe vers des valeurs positives
    int v = int( round(f) );  

//    # Empêche de sortir de l’intervalle valide au cas où
    if (v > 255) v = 255;
    if (v < 0)   v = 0;

    int16_t sample = v;
#ifdef SOUND_DEBUG
    Serial.println(sample); 
#endif
    TabSinus[i] = sample;
  }
	 
// Volume

	_volume 			= DEFAULT_VOLUME;
	VOLUME_I2S    = DEFAULT_VOLUME * 10;
	 
// Driver I2S
	 
  // Initialize I2S with configurations above
  i2s_driver_install((i2s_port_t)I2S_NUM, &i2s_config, 0, NULL);

#ifdef DAC_INTERNE
//  DAC Interne
//  i2s_set_pin((i2s_port_t)I2S_NUM,NULL);
  i2s_set_dac_mode((i2s_dac_mode_t)I2S_DAC_CHANNEL_RIGHT_EN);   // Pin 25
#else
  i2s_set_pin((i2s_port_t)I2S_NUM, &pin_config);
#endif

// Timer consumeData

  timerI2S = timerBegin(2, 80, true);   // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timerI2S, &onTimer, true);
  timerAlarmWrite(timerI2S, 50000, true);  // 250000 * 1 us = 250 ms, autoreload true
#ifdef TIMERI2S
  timerAlarmEnable(timerI2S);
#endif
}

/***********************************/
void ToneI2SEsp32::init(uint32_t pin)
/***********************************/
{
 init();
}

// ***********************************
void ToneI2SEsp32::tone(unsigned long frequency) {
// ***********************************

#ifdef SOUND_DEBUG
	SerialPort.println("ToneI2SESP32 frequency ");
#endif //SOUND_DEBUG

  tone(frequency, _volume);
}

#ifdef TONE_VOLUME
// ***********************************
void ToneI2SEsp32::tone(unsigned long frequency
// ***********************************
            , uint8_t volume
#ifdef TONE_LENGTH
            , unsigned long length, uint8_t background
#endif
	    ) {

#ifdef SOUND_DEBUG
	SerialPort.print("ToneI2SESP32 volume : ");
	SerialPort.println(volume);
#endif //SOUND_DEBUG

  // check if no tone 
  if (toneMuted || frequency == 0 || volume == 0) { 
	  noTone(); 
		return; 
	} 

  // check volume 
  if (volume > 10)  volume = 10;
	_volume = volume;	
  volume = volume * 10;
	if (VOLUME_I2S != volume) VOLUME_I2S = volume;

#ifdef SOUND_DEBUG
	SerialPort.print("ToneI2SESP32 VOLUME_I2S : ");
	SerialPort.println(VOLUME_I2S);
#endif //SOUND_DEBUG

  if (frequency > 10000) frequency = 10000;
	_frequency = frequency;
	
#ifdef HAVE_AUDIO_AMPLI
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI

  portENTER_CRITICAL(&timerMux);
  phase    = 0;  //maxPhase * 0.75;
  compteur = 0;
  maxPhase = (int)(SAMPLE_RATE  /frequency); //WAVE_FREQ_HZ);
  ecartPhase = 360 / maxPhase;
  maxSample = (uint32_t)((SAMPLE_RATE / 1000) * CYCLE);
  portEXIT_CRITICAL(&timerMux);

#ifdef SOUND_DEBUG
  SerialPort.print("Max Sample : ");
  SerialPort.println(maxSample);
#endif
  
  i2s_start((i2s_port_t)I2S_NUM);
  portENTER_CRITICAL(&timerMux);
  actif = true;  
  portEXIT_CRITICAL(&timerMux);
  
  consumeFullData();

  // compute length time 
#ifdef TONE_LENGTH
  if (length > 0 && background) {  // Background tone playing, returns control to your sketch.

    _tAC_time = millis() + length; // Set when the note should end.
    //TODO !!!                     // Activate the timer interrupt.
  }

  if (length > 0 && !background) { delay(length); noTone(); } // Just a simple delay, doesn't return control untill finished.
#endif
}
#endif

// ***********************************
void ToneI2SEsp32::noTone() {
// ***********************************

#ifdef TONE_LENGTH
  //TODO !!!!                     // Remove the timer interrupt.
#endif
	currentSample = 0;
	actif = false;
	i2s_zero_dma_buffer((i2s_port_t)I2S_NUM);
	i2s_stop((i2s_port_t)I2S_NUM);
			 
#ifdef HAVE_AUDIO_AMPLI	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI
}

#ifdef TONE_LENGTH
// TODO !!! { // Timer interrupt vector.
//  if (millis() >= _tAC_time) noToneAC(); // Check to see if it's time for the note to end.
//}
#endif

// ***********************************
void ToneI2SEsp32::toneMute(bool newMuteState) {
// ***********************************

  // stop tone if needed 
  if( newMuteState ) {
    noTone();
  }

  // save 
  toneMuted = newMuteState;
}

/***********************************/
void ToneI2SEsp32::setVolume(uint8_t newVolume) {
/***********************************/
  _volume = newVolume;
}

/***********************************/
uint8_t ToneI2SEsp32::getVolume() {
/***********************************/
  if (toneMuted) return 0;
	else            return _volume;
}

/*
// ***********************************
void ToneI2SEsp32::setBeep(uint16_t frequency, uint16_t duration) {
// ***********************************
    _beep_freq = frequency;
    _beep_duration = duration;
}

*/ 

/***********************************/
void ToneI2SEsp32::update() {
/***********************************/
#ifndef TIMERI2S
  consumeFullData();
#endif
}
 
/***********************************/
void ToneI2SEsp32::AUDIO_AMP_DISABLE(void) {
/***********************************/
	digitalWrite(PIN_AUDIO_AMP_ENA,LOW);
}

/***********************************/
void ToneI2SEsp32::AUDIO_AMP_ENABLE(void) {
/***********************************/
	digitalWrite(PIN_AUDIO_AMP_ENA,HIGH);
}

/***********************************
void ToneI2SEsp32::update(void) {
/ ***********************************
}*/

#endif //TONEI2S

#endif //ESP32
