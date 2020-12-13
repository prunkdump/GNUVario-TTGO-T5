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
/*    1.0.6    12/01/19      Ajout Trace																					 */
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
 
#if defined(ESP32)

#include "HardwareConfig.h"

#if defined(TONE)
 
#include "tone_esp32.h"

#include "DebugConfig.h"
//#include "utility\Debug.h"

#ifdef SOUND_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

// BEEP PIN
#ifndef SPEAKER_PIN
#define SPEAKER_PIN 25		//or 26
#endif

#if not defined(_DEBUG_H_)
#define TONEDAC_DEBUG					//debug Tone
//#define SerialPort Serial
#endif

//#define AUDIO_AMP_ENABLE()   {GPIO.out1_w1ts.val = ((uint32_t)1 << (PIN_AUDIO_AMP_ENA - 32));}
//#define AUDIO_AMP_DISABLE()  {GPIO.out1_w1tc.val = ((uint32_t)1 << (PIN_AUDIO_AMP_ENA - 32));}

ToneEsp32::ToneEsp32(void) {
//    _volume = 8;
}

/***********************************/
void ToneEsp32::init(void)
/***********************************/
{
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneEsp32");
	TRACE();
#endif //SOUND_DEBUG

/*	Speaker_Pin      = SPEAKER_PIN;
  Tone_Pin_Channel = TONE_PIN_CHANNEL;*/

	init(SPEAKER_PIN);
/*	ledcSetup(Tone_Pin_Channel, 0, 13);
  ledcAttachPin(Speaker_Pin, Tone_Pin_Channel);
  // digitalWrite(SPEAKER_PIN, 0);
  setBeep(1000, 100);*/
}

/***********************************/
void ToneEsp32::init(uint32_t pin)
/***********************************/
{
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneEsp32");
	TRACE();
#endif //SOUND_DEBUG
	
//	if ((pin != 25) and (pin != 26)) pin = 25;
	Speaker_Pin = pin;
	
	Tone_Pin_Channel = TONE_PIN_CHANNEL;

	ledcSetup(Tone_Pin_Channel, 0, 8);
  ledcAttachPin(Speaker_Pin, Tone_Pin_Channel);
	digitalWrite(Speaker_Pin,0);
  // digitalWrite(SPEAKER_PIN, 0);
  setBeep(1000, 100);
}

/***********************************/
void ToneEsp32::tone(unsigned long frequency
/***********************************/
#ifdef TONEAC_VOLUME
            , uint8_t volume
#endif
#ifdef TONEAC_LENGTH
            , unsigned long length, uint8_t background
#endif
	    ) {

#ifdef SOUND_DEBUG
	SerialPort.print("ToneESP32 volume : ");
	SerialPort.println(volume);
#endif //SOUND_DEBUG

  /* check if no tone */ 
  if (toneACMuted || frequency == 0
#ifdef TONEAC_VOLUME     
      || volume == 0
#endif
      ) { noTone(); return; } 

  /* check volume */
#ifdef TONEAC_VOLUME
  if (volume > 10) volume = 10;
#endif

//  _volume = volume;
  /* set duty cycle */
#ifdef TONEAC_VOLUME
  uint8_t duty = _tAC_volume[volume - 1]; // Calculate the duty cycle (volume).
#else
  uint8_t duty = 125 ;
#endif

  ledcWriteTone(Tone_Pin_Channel, frequency);
	ledcWrite(Tone_Pin_Channel, duty);


/*  ledcWriteTone(Tone_Pin_Channel, 2000);
 
  for (int tmpvolume = 0; tmpvolume < 10; tmpvolume++){
 
    duty = _tAC_volume[tmpvolume];
    SerialPort.print(tmpvolume);
    SerialPort.print(" - ");
    SerialPort.println(duty);
 
    ledcWrite(Tone_Pin_Channel, duty);
    delay(2000);
  }

	  ledcWriteTone(Tone_Pin_Channel, 2000);
    ledcWrite(Tone_Pin_Channel, 255);

	
  ledcWriteTone(Tone_Pin_Channel, 2000);
 
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle=dutyCycle+10){
 
    SerialPort.println(dutyCycle);
 
    ledcWrite(Tone_Pin_Channel, dutyCycle);
    delay(1000);
  }
 
  ledcWrite(Tone_Pin_Channel, 125);
 
  for (int freq = 255; freq < 10000; freq = freq + 250){
 
     SerialPort.println(freq);
 
     ledcWriteTone(Tone_Pin_Channel, freq);
     delay(1000);
  }	*/

  /* compute length time */
#ifdef TONEAC_LENGTH
  if (length > 0 && background) {  // Background tone playing, returns control to your sketch.

    _tAC_time = millis() + length; // Set when the note should end.
    //TODO !!!                     // Activate the timer interrupt.
  }

  if (length > 0 && !background) { delay(length); noToneAC(); } // Just a simple delay, doesn't return control untill finished.
#endif
}

/***********************************/
void ToneEsp32::noTone() {
/***********************************/

#ifdef TONEAC_LENGTH
  //TODO !!!!                     // Remove the timer interrupt.
#endif
	ledcWriteTone(Tone_Pin_Channel, 0);
  digitalWrite(Speaker_Pin, 0);
}

#ifdef TONEAC_LENGTH
// TODO !!! { // Timer interrupt vector.
//  if (millis() >= _tAC_time) noToneAC(); // Check to see if it's time for the note to end.
//}
#endif

/***********************************/
void ToneEsp32::toneMute(bool newMuteState) {
/***********************************/

  /* stop tone if needed */
  if( newMuteState ) {
    noTone();
  }

  /* save */
  toneACMuted = newMuteState;
}

/***********************************/
void ToneEsp32::setBeep(uint16_t frequency, uint16_t duration) {
/***********************************/
    _beep_freq = frequency;
    _beep_duration = duration;
}

#endif //TONE
#endif //ESP32
