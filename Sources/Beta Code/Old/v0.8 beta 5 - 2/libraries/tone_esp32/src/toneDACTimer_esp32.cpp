/****************************************************/
/*                                                  */
/*          Libraries ToneDAC ESP32                 */
/*                                                  */
/*  version    Date     Description                 */
/*    1.0    07/03/19                               */
/*    1.1    10/01/20   Maj                         */
/*                      Ajout defined(TONEDACTIMER) */
/*    1.1.1  12/01/20   Ajout trace debug           */
/*                                                  */
/****************************************************/

#include "HardwareConfig.h"

#if defined(ESP32) && defined (TONEDACTIMER)


#include "toneDACTimer_esp32.h"
#include <Arduino.h>
#include "DebugConfig.h"

#ifdef SOUND_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

//#include <SysCall.h>

// BEEP PIN
#ifndef SPEAKER_PIN 
#define SPEAKER_PIN 25		//or 26
#endif

#ifndef TONEDAC_DEBUG
#define TONEDAC_DEBUG					//debug Tone
#endif

#ifndef SerialPort
#define SerialPort Serial
#endif

#ifndef PIN_AUDIO_AMP_ENA
#define PIN_AUDIO_AMP_ENA 			34
#endif

#ifndef HAVE_AUDIO_AMPLI
//#define HAVE_AUDIO_AMPLI
#endif //HAVE_AUDIO_AMPLI

#ifdef TONEDAC_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>


/*#ifndef VARIO_SETTINGS_H

//Monitor Port 
#if defined(ESP8266)

#elif defined(ESP32)

#elif defined(ARDUINO_ARCH_SAMD)
#define SerialPort SerialUSB
#elif defined(_BOARD_GENERIC_STM32F103C_H_)

#elif defined(ARDUINO_AVR_PRO)
#define SerialPort Serial
#else
#define SerialPort Serial
#endif

#define TONEDAC_DEBUG

#endif*/

uint8_t  	waveForm = WAVEFORM_SINUS;
volatile	int 			sIndex; //Tracks sinewave points in array
volatile  int 			*wavSamples; //array to store sinewave points
volatile  int 			sampleCount = 12; // Number of samples to read in block

volatile  bool      tmpstate = 0;

/*#define AUDIO_AMP_ENABLE()   {GPIO.out1_w1ts.val = ((uint32_t)1 << (pinAudioAmpEna - 32));}
#define AUDIO_AMP_DISABLE()  {GPIO.out1_w1tc.val = ((uint32_t)1 << (pinAudioAmpEna - 32));}*/

		// BEEP PIN
uint8_t Speaker_Pin;	 

bool tone_high = true;

volatile 	uint32_t 	elapsedBytes = 0;

#if defined(TONEDAC_EXTENDED)
File 			audioFile;
volatile 	uint32_t 	HeadIndex;
volatile	bool 			audioFileReady = false;
#endif //TONEDAC_EXTENDED

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
volatile int interruptCounter;
int totalInterruptCounter;
 
/***********************************/
void ToneDacEsp32::setWaveForm(uint8_t form)
/***********************************/
{
#ifdef TONEDAC_DEBUG	
	SerialPort.print("Set form : ");
	SerialPort.println(form);
#endif TONEDAC_DEBUG

	waveForm = form;
	genForm(sampleCount);
#ifdef TONEDAC_DEBUG	
	SerialPort.println(waveForm);
#endif TONEDAC_DEBUG
}

//This function generates a sine wave and stores it in the wavSamples array
//The input argument is the number of points the sine wave is made up of
/***********************************/
void ToneDacEsp32::genForm(int sCount) {
/***********************************/
#ifdef TONEDAC_DEBUG	
	SerialPort.print("Gen Form : ");
	SerialPort.println(waveForm);
#endif TONEDAC_DEBUG
	
	TRACE();
  switch(waveForm) {
		case WAVEFORM_SINUS:
#ifdef TONEDAC_DEBUG	
		  SerialPort.println("Wave form Sinus");
#endif TONEDAC_DEBUG
		  genSin(sCount);
			break;
			
		case WAVEFORM_TRIANGLE:
#ifdef TONEDAC_DEBUG	
		  SerialPort.println("Wave form Triangle");
#endif TONEDAC_DEBUG
		  genTriangle(sCount);
			break;
			
		case WAVEFORM_SQUARE:
#ifdef TONEDAC_DEBUG	
		  SerialPort.println("Wave form Square");
#endif TONEDAC_DEBUG
		  genSquare(sCount);
			break;
			
		default:
#ifdef TONEDAC_DEBUG	
		  SerialPort.println("Wave form dsefault");
#endif TONEDAC_DEBUG
		  genSin(sCount);
    break;
  }
}

/***********************************/
void ToneDacEsp32::genSin(int sCount) {
/***********************************/
  const float pi2 = 6.28; //2 x pi
  float in, amp; 
 
#ifdef TONEDAC_DEBUG	
  SerialPort.println("GenSin");
	SerialPort.print("volume = ");
	SerialPort.println(_tDAC_volume);
	SerialPort.print("sCount = ");
	SerialPort.println(sCount);
#endif TONEDAC_DEBUG

  amp = ((255 * _tDAC_volume) /100) / 2;

#ifdef TONEDAC_DEBUG	
	SerialPort.print("Ampl = ");
	SerialPort.println(amp);
#endif TONEDAC_DEBUG
	
  for(int i=0; i<sCount;i++) { //loop to build sine wave based on sample count
    in = pi2*(1/(float)sCount)*(float)i; //calculate value in radians for sin()
    wavSamples[i] = ((int)(sin(in)*amp + amp)); //Calculate sine wave value and offset based on DAC resolution 511.5 = 1023/2
#ifdef TONEDAC_DEBUG	
/*		SerialPort.print(i);
    SerialPort.print(" - ");
		SerialPort.println(wavSamples[i]);*/
#endif TONEDAC_DEBUG
  }
}

/***********************************/
void ToneDacEsp32::genTriangle(int sCount) {
/***********************************/
  float pas;
  float in, amp; 

#ifdef TONEDAC_DEBUG	
	SerialPort.println("GenTriangle");
#endif TONEDAC_DEBUG

  amp = ((255 * _tDAC_volume) / 100);
 
  pas = amp / (sCount/ 2);
#ifdef TONEDAC_DEBUG	
	SerialPort.print("Pas : ");
	SerialPort.println(pas);
#endif TONEDAC_DEBUG
	in = 0;

  for(int i=0; i<sCount;i++) { //loop to build sine wave based on sample count
#ifdef TONEDAC_DEBUG	
	  SerialPort.print("WaveSample : ");
		SerialPort.println(in);
#endif TONEDAC_DEBUG
    wavSamples[i] = (int)in;
    if (i < (int)(sCount/2))
      in += pas;
    else 
		  in -= pas;
  }
}

/***********************************/
void ToneDacEsp32::genSquare(int sCount) {
/***********************************/
 float in, amp; 
 
#ifdef TONEDAC_DEBUG	
 SerialPort.println("GenSquare");
#endif TONEDAC_DEBUG
 amp = ((255 * _tDAC_volume) / 100);

 for(int i=0; i<sCount;i++) { //loop to build sine wave based on sample count
  if (i < (int)(sCount/2))
    wavSamples[i] = ((int)amp); 
	else 
		wavSamples[i] = 0;
 }
}

//Reset Timer 
/***********************************/
void ToneDacEsp32::tcReset()
/***********************************/
{
	timerRestart(timer);
}

//disable Timer
/***********************************/
void ToneDacEsp32::tcDisable()
/***********************************/
{
  timerAlarmDisable(timer);
}

// Configures the TC to generate output events at the sample frequency.
//Configures the TC in Frequency Generation mode, with an event output once
//each time the audio sample frequency period expires.
/***********************************/
void ToneDacEsp32::tcConfigure(int sampleRate)
/***********************************/
{
  timerAlarmWrite(timer, sampleRate, true);

} 

//This function enables TC5 and waits for it to be ready
/***********************************/
void ToneDacEsp32::tcStartCounter()
/***********************************/
{
  timerAlarmEnable(timer);
}


/***********************************/
void IRAM_ATTR onTimer() 
/***********************************/
{
  portENTER_CRITICAL(&timerMux);
/*  interruptCounter++;
 
	if (waveForm != WAVEFORM_WAV) { 
		dacWrite(Speaker_Pin, wavSamples[sIndex]);
		if (sIndex==sampleCount-1)
			{
				sIndex = 0;	
// *     toneDAC.tcDisable();
     toneDAC.tcReset();
	   toneDAC.tcConfigure(toneDAC.sampleRate); //setup the timer counter based off of the user entered sample rate
//start timer, once timer is done interrupt will occur and DAC value will be updated
     toneDAC.tcStartCounter(); *
			}
		else
			{		
				sIndex++;
			}
	}
	#if defined(TONEDAC_EXTENDED)
	else {
		 if (audioFile.available()) {
        
        
        uint32_t current_SampleIndex = sIndex;
            
        if (current_SampleIndex > HeadIndex) {
          audioFile.read(&wavSamples[HeadIndex], current_SampleIndex - HeadIndex);
          HeadIndex = current_SampleIndex;
        }
        else if (current_SampleIndex < HeadIndex) {
          audioFile.read(&wavSamples[HeadIndex],SAMPLECOUNT-1 - HeadIndex);
          audioFile.read(wavSamples, current_SampleIndex);
          HeadIndex = current_SampleIndex;
        }
        
        
        if (sIndex < SAMPLECOUNT - 1)
        {
          dacWrite(Speaker_Pin, wavSamples[sIndex++]);
          elapsedBytes++;
            // Clear the interrupt
            //TC5->COUNT16.INTFLAG.bit.MC0 = 1;
        }
        else
        {
          sIndex = 0;
            //TC5->COUNT16.INTFLAG.bit.MC0 = 1;
        }
		 }
		 else if (audioFileReady) {
       audioFile.close();
       audioFileReady = false;
       elapsedBytes=0;
        //tc reset
        //TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
        //while (TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
        //while (TC5->COUNT16.CTRLA.bit.SWRST);
    
        //tc disable
 //      TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
 //      while (TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
        
       dacWrite(Speaker_Pin, 128);
     }
	}
#endif //#TONEDAC_EXTENDED*/

/*  if (tone_high) {
    dacWrite(26, 255);
		tone_high = false;
	}
	else {
    dacWrite(26, 0);
		tone_high = true;
	}*/
	
	
/*		dacWrite(26, wavSamples[sIndex]);
		if (sIndex>=sampleCount-1)
			{
				sIndex = 0;	
			}
		else
			{		
				sIndex++;
			}*/
			
/*	tmpstate = !tmpstate;		
	dacWrite(26, tmpstate * 255);

  SysCall::yield();*/
	
//		dacWrite(Speaker_Pin, wavSamples[sIndex]);
		if (sIndex==sampleCount-1)
			{
				sIndex = 0;	
/*     toneDAC.tcDisable();
				toneDAC.tcReset();
				toneDAC.tcConfigure(toneDAC.sampleRate); //setup the timer counter based off of the user entered sample rate
//start timer, once timer is done interrupt will occur and DAC value will be updated
				toneDAC.tcStartCounter(); */
			}
		else
			{		
				sIndex++;
			}
	

  portEXIT_CRITICAL(&timerMux);	
}


/***********************************/
void ToneDacEsp32::init(void) {
/***********************************/
//	ledcSetup(TONE_PIN_CHANNEL, 0, 13);
//  ledcAttachPin(SPEAKER_PIN, TONE_PIN_CHANNEL);

#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneDacTimerEsp32");
	TRACE();
#endif //SOUND_DEBUG

  init(SPEAKER_PIN);
  
}

/***********************************/
void ToneDacEsp32::init(uint32_t pin) {
/***********************************/
#ifdef SOUND_DEBUG
	SerialPort.println("Init ToneDacTimerEsp32");
	TRACE();
#endif //SOUND_DEBUG

  if ((pin != 25) or (pin != 26)) pin = 25;
  Speaker_Pin = pin;

	dacWrite(Speaker_Pin, 0);

  _tDAC_volume = 100;
	waveForm = WAVEFORM_SINUS;
  
  /*Allocate the buffer where the samples are stored*/
//  wavSamples = (int *) malloc(sampleCount * sizeof(int));
//  genForm(sampleCount); //function generates sine wave
  wavSamples = (int *) malloc(SAMPLECOUNT * sizeof(int));
	genForm(SAMPLECOUNT); //function generates sine wave
	
  timer = timerBegin(2, 240, true);
	timerAttachInterrupt(timer, &onTimer, true);
}

/***********************************/
void ToneDacEsp32::begin(uint32_t srate) {
/***********************************/
#if defined(TONEDAC_EXTENDED)
	audioFileReady = false;
#endif //TONEDAC_EXTENDED

  sampleRate = srate;
	waveForm = WAVEFORM_WAV;
  sIndex = 0;   //Set to zero to start from beginning of waveform
	
	/*Modules configuration */
//  digitalWrite(Speaker_Pin, 0);

	tcConfigure(srate);
}

/***********************************/
void ToneDacEsp32::tone(unsigned long frequency
/***********************************/
#ifdef TONEDAC_VOLUME
            , uint8_t volume
#endif
#ifdef TONEDAC_LENGTH
            , unsigned long length, uint8_t background
#endif
	    ) {

  uint32_t tmpSampleCount; 			
  /* check if no tone */ 
  if (toneDACMuted || frequency == 0
#ifdef TONEDAC_VOLUME     
      || volume == 0
#endif
      ) { noTone(); return; } 

#ifdef TONEDAC_DEBUG	
	SerialPort.print("volume toneDacEsp32 = ");
	SerialPort.println(volume);
#endif TONEDAC_DEBUG
			
  /* check volume */
#ifdef TONEDAC_VOLUME
  if (volume > 100) volume = 100;
	if (_tDAC_volume != volume) 
	  {
	    _tDAC_volume = volume;
		}
#endif

  if (frequency > 10000) frequency = 10000;
	_frequency = frequency;
  tmpSampleCount = SAMPLERATE / frequency;
	multiplicateur = 1;
	while (tmpSampleCount > SAMPLECOUNT) {
		tmpSampleCount /= 2;
		multiplicateur *= 2;
	}

#ifdef TONEDAC_DEBUG	
  SerialPort.print("Multiplicateur : ");
	SerialPort.println(multiplicateur);
  SerialPort.print("tmpSampleCount : ");
	SerialPort.println(tmpSampleCount);
#endif TONEDAC_DEBUG
	
  sIndex = 0;   //Set to zero to start from beginning of waveform
	sampleCount = tmpSampleCount;
	sampleRate = (SAMPLERATE / (frequency * sampleCount)); 
//	if (sampleRate > 120000) sampleRate = 120000;
	
#ifdef TONEDAC_DEBUG	
  SerialPort.print("SampleCount : ");
	SerialPort.println(sampleCount);
	
  SerialPort.print("SampleRate : ");
	SerialPort.println(sampleRate);
	
#endif TONEDAC_DEBUG
	
	genForm(sampleCount);	
  tcConfigure(sampleRate); //setup the timer counter based off of the user entered sample rate
/*  //loop until all the sine wave points have been played
  while (sIndex<sampleCount)
  { 
 //start timer, once timer is done interrupt will occur and DAC value will be updated
    tcStartCounter(); 
  }
  //disable and reset timer counter
  tcDisable();
  tcReset();*/
  
	tcStartCounter(); 	
	
  /* compute length time */
#ifdef TONEDAC_LENGTH
  if (length > 0 && background) {  // Background tone playing, returns control to your sketch.

    _tDAC_time = millis() + length; // Set when the note should end.
    //TODO !!!                     // Activate the timer interrupt.
  }

  if (length > 0 && !background) { delay(length); noToneDAC(); } // Just a simple delay, doesn't return control untill finished.
#endif
}

/***********************************/
void ToneDacEsp32::noTone() {
/***********************************/
#ifdef TONEDAC_LENGTH
  //TODO !!!!                     // Remove the timer interrupt.
#endif

//  digitalWrite(LED3,false);
//  ledcWriteTone(TONE_PIN_CHANNEL, 0);
//  digitalWrite(SPEAKER_PIN, 0);
	dacWrite(Speaker_Pin, 0);
  tcDisable();
//  tcReset();
}

#ifdef TONEDAC_LENGTH
// TODO !!! { // Timer interrupt vector.
//  if (millis() >= _tAC_time) noToneAC(); // Check to see if it's time for the note to end.
//}
#endif

/***********************************/
void ToneDacEsp32::toneMute(bool newMuteState) {
/***********************************/
  /* stop tone if needed */
  if( newMuteState ) {
    noTone();
  }

  /* save */
  toneDACMuted = newMuteState;
}

/***********************************/
void ToneDacEsp32::end() {
/***********************************/
	tcDisable();
	tcReset();
	dacWrite(Speaker_Pin, 0);
}

#if defined(TONEDAC_EXTENDED)

/***********************************/
void ToneDacEsp32::play(const char *fname) {
/***********************************/
  if(audioFileReady) audioFile.close();
  
	audioFile = SD.open(fname);
  fileSize = audioFile.size();
    
  if(!audioFile){
    end();
    return;
  }
    
  for(int i =0; i<44; i++) audioFile.read();
    
  audioFile.read(wavSamples, SAMPLECOUNT);
  HeadIndex = 0;
    
    /*once the buffer is filled for the first time the counter can be started*/
  tcStartCounter();
    
  audioFileReady = true;
    
}

/***********************************/
uint32_t ToneDacEsp32::duration()
/***********************************/
{
  if(audioFileReady)  return (fileSize/sampleRate);
  return 0;
}

/***********************************/
uint32_t ToneDacZero::remaining()
/***********************************/
{
  if(audioFileReady) return ((fileSize-elapsedBytes)/sampleRate);
  return 0;
}


/***********************************/
bool ToneDacEsp32::isPlaying()
/***********************************/
{
  return audioFileReady;
}

#endif //TONEDAC_EXTENDED

/***********************************/
uint8_t ToneDacEsp32::getVolume()
/***********************************/
{
  return _tDAC_volume;
}

/***********************************/
void ToneDacEsp32::AUDIO_AMP_DISABLE(void) {
/***********************************/
	digitalWrite(PIN_AUDIO_AMP_ENA,LOW);
}

/***********************************/
void ToneDacEsp32::AUDIO_AMP_ENABLE(void) {
/***********************************/
	digitalWrite(PIN_AUDIO_AMP_ENA,HIGH);
}

#endif //EPS32
