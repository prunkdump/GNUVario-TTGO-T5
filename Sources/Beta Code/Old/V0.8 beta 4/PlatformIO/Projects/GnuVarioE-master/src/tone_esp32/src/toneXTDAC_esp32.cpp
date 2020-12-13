/*****************************************************************************/
/*                                                                           */
/*                        Libraries ToneXTDAC ESP32                          */
/*                                                                           */
/*  version    Date        Description                                       */
/*    1.0      13/06/20                                                      */
/*                                                                           */
/*****************************************************************************/

#include <Arduino.h>
#include "DebugConfig.h"
#include "HardwareConfig.h"

#if defined(ESP32) && defined(TONEXTDAC)

#include "toneXTDAC_esp32.h"

#ifdef SOUND_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include "XT_DAC_Audio2.h"

// BEEP PIN
#if not defined(SPEAKER_PIN)
#define SPEAKER_PIN 25		//or 26
#endif

#if not defined(_DEBUG_H_)
#define TONEDAC_DEBUG					//debug Tone
//#define SerialPort Serial
#endif

#ifndef PIN_AUDIO_AMP_ENA
#define PIN_AUDIO_AMP_ENA 			34
#endif

#ifndef HAVE_AUDIO_AMPLI
//#define HAVE_AUDIO_AMPLI
#endif //HAVE_AUDIO_AMPLI

//#define AUDIO_AMP_ENABLE()   {GPIO.out1_w1ts.val = ((uint32_t)1 << (PIN_AUDIO_AMP_ENA - 32));}
//#define AUDIO_AMP_DISABLE()  {GPIO.out1_w1tc.val = ((uint32_t)1 << (PIN_AUDIO_AMP_ENA - 32));}

static int clk_8m_div = 7;  // RTC 8M clock divider (division is by clk_8m_div+1, i.e. 0 means 8MHz frequency)
static int frequency_step = 8;  // Frequency step for CW generator
static int scale = 0;           // full scale
static int offset;              // leave it default / 0 = no any offset
static int invert = 2;          // invert MSB to get sine waveform

		// BEEP PIN
uint8_t Speaker_Pin;	 

int16_t PROGMEM TwinkleTwinkle[] = {1000,SCORE_END};

XT_DAC_Audio_Class DacAudio(25,2);                                            // Create the main player class object. Use GPIO 25 (DAC pin) and timer 0
//XT_Instrument_Class Bip(INSTRUMENT_ORGAN);
XT_Beeper_Class Bip(TwinkleTwinkle,TEMPO_PRESTISSIMO,INSTRUMENT_ORGAN); // TEMPO_ALLEGRO,INSTRUMENT_ORGAN);     // Music score object, handles tunes. Pass Music Data,Tempo, Instrument 


#if defined(TONEDAC_EXTENDED)
#endif //TONEDAC_EXTENDED

/***********************************/
void ToneXTDacEsp32::init(void) {
/***********************************/
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneXTDacEsp32");
	TRACE();
#endif //SOUND_DEBUG

//	ledcSetup(TONE_PIN_CHANNEL, 0, 13);
//  ledcAttachPin(SPEAKER_PIN, TONE_PIN_CHANNEL);

  init(SPEAKER_PIN);
  
}

/***********************************/
void ToneXTDacEsp32::init(uint32_t pin) {
/***********************************/
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneXTDacEsp32");
	TRACE();
#endif //SOUND_DEBUG

  if ((pin != 25) or (pin != 26)) pin = 25;
  Speaker_Pin = pin;
	
	if (Speaker_Pin == 25) channelDAC = DAC_CHANNEL_1;
	else									 channelDAC = DAC_CHANNEL_2;
	
#ifdef HAVE_AUDIO_AMPLI
//init Pin commande ampli

	if (PIN_AUDIO_AMP_ENA != -1) {
		pinMode(PIN_AUDIO_AMP_ENA,OUTPUT);
		digitalWrite(PIN_AUDIO_AMP_ENA,LOW);
	}
//	AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI

  DacAudio.DacVolume=100;
}

/***********************************/
void ToneXTDacEsp32::begin(uint32_t srate) {
/***********************************/
/*#if defined(TONEDAC_EXTENDED)
	audioFileReady = false;
#endif //TONEDAC_EXTENDED

  sampleRate = srate;
	waveForm = WAVEFORM_WAV;
  sIndex = 0;   //Set to zero to start from beginning of waveform
	
	// *Modules configuration *
//  digitalWrite(Speaker_Pin, 0);

	tcConfigure(srate);*/
}

/***********************************/
void ToneXTDacEsp32::tone(unsigned long frequency
/***********************************/
#ifdef TONEDAC_VOLUME
            , uint8_t volume
#endif
#ifdef TONEDAC_LENGTH
            , unsigned long length, uint8_t background
#endif
	    ) {

  /* check if no tone */ 
  if (toneDACMuted || frequency == 0
#ifdef TONEDAC_VOLUME     
      || volume == 0
#endif
      ) { noTone(); return; } 

#ifdef TONEDAC_DEBUG	
	SerialPort.print("volume ToneXTDacEsp32 = ");
	SerialPort.println(volume);
#endif TONEDAC_DEBUG
			
  /* check volume */
#ifdef TONEDAC_VOLUME
  if (volume > 100) volume = 100;
	if (_tDAC_volume != volume) 
	  {
	    _tDAC_volume = volume;
			uint8_t scaleVolume;
		}
#endif

//	DacAudio.DacVolume=_tDAC_volume;

  if (frequency > 10000) frequency = 10000;
	_frequency = frequency;
	
/*	AUDIO_AMP_DISABLE();
  dac_cosine_enable(channelDAC);
  dac_output_enable(channelDAC);
	scale =  2; //3 - opt.misc.speakerVolume;
	offset = 0;
	invert = 2;
	dac_scale_set(channelDAC, 1); //3 - scale);
  dac_offset_set(channelDAC, offset);
  dac_invert_set(channelDAC, invert);*/

//   Bip.SetFrequency(frequency);
  TwinkleTwinkle[0] = frequency;
#ifdef HAVE_AUDIO_AMPLI
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_ENABLE();
#endif //HAVE_AUDIO_AMPLI

  DacAudio.Play(&Bip);     

	#ifdef TONEDAC_LENGTH
  if (length > 0 && background) {  // Background tone playing, returns control to your sketch.

    _tDAC_time = millis() + length; // Set when the note should end.
    //TODO !!!                     // Activate the timer interrupt.
  }

  if (length > 0 && !background) { delay(length); noToneDAC(); } // Just a simple delay, doesn't return control untill finished.
#endif
}

/***********************************/
void ToneXTDacEsp32::noTone() {
/***********************************/
	DacAudio.StopAllSounds();
	dac_output_disable(channelDAC);
#ifdef HAVE_AUDIO_AMPLI	
  if (PIN_AUDIO_AMP_ENA != -1) AUDIO_AMP_DISABLE();
#endif //HAVE_AUDIO_AMPLI
}

#ifdef TONEDAC_LENGTH
// TODO !!! { // Timer interrupt vector.
//  if (millis() >= _tAC_time) noToneAC(); // Check to see if it's time for the note to end.
//}
#endif

/***********************************/
void ToneXTDacEsp32::toneMute(bool newMuteState) {
/***********************************/
  /* stop tone if needed */
  if( newMuteState ) {
    noTone();
  }

  /* save */
  toneDACMuted = newMuteState;
}


#if defined(TONEDAC_EXTENDED)


#endif //TONEDAC_EXTENDED

/***********************************/
uint8_t ToneXTDacEsp32::getVolume()
/***********************************/
{
  return _tDAC_volume;
}

/***********************************/
void ToneXTDacEsp32::AUDIO_AMP_DISABLE(void) {
/***********************************/
	digitalWrite(PIN_AUDIO_AMP_ENA,LOW);
}

/***********************************/
void ToneXTDacEsp32::AUDIO_AMP_ENABLE(void) {
/***********************************/
	digitalWrite(PIN_AUDIO_AMP_ENA,HIGH);
}

/***********************************/
void ToneXTDacEsp32::update(void) {
/***********************************/
  DacAudio.FillBuffer();          // This needs only be in your main loop once, suggest here at the top.
}

#endif //EPS32
