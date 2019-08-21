/*********************************************/
/*        PARAMETRAGE                        */
/*********************************************/
/* Interface Type                            */
/*********************************************/
/*                                           */
/* dans toneHAL.h                            */
/* #define  TONEI2S   I2S                    */
/* #define  TONEDAC   DAC                    */
/* #define  TONEAC    2 pins Push-Pull PWM   */
/* #define  TONE      1 pin PWM              */
/*********************************************/
/*                                           */
/*********************************************/
/*         PIN AUDIO                         */
/*********************************************/
/*                                           */
/* dans HardwareConfigESP32                  */
/* #define pinAudioAmpEna       (34)         */
/* #define pinAudioDAC          (25)         */
/* #define SPEAKER_PIN 25    //or 26         */
/* #define TONE_PIN_CHANNEL 0  // or 1       */
/*********************************************/

#include <arduino.h>

#include <DebugConfig.h>
#include <HardwareConfig.h>

#include <toneHAL.h>

#if defined(TONEDAC_EXTENDED)
#include <SPI.h>
//#include <SD.h>
#include "SdFat.h"
SdFat SD;

File myFile;

const char *filename = "music.wav";

#endif //TONEDAC_EXTENDED

#define volumeDefault 2

//indicate sample rate here (use audacity to convert your wav)
const unsigned int sampleRateWav = 22050;

void setup() {
  // put your setup code here, to run once:
  SerialPort.begin(115200);
//  while (!SerialPort) { ;}
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

#if defined(ARDUINO_ARCH_SAMD)

  //activation de l'ampli D
  digitalWrite(6, LOW);
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);

#endif //ARDUINO_ARCH_SAMD

#if defined(TONEDAC_EXTENDED)

  SerialPort.print("Initializing SD card...");

  if (!SD.begin(SDCARD_CS_PIN)) {
    SerialPort.println("initialization failed!");
//    return;
  }
  else {
    SerialPort.println("initialization done.");

/*    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) {
       
    }*/
  }
#endif //TONEDAC_EXTENDED
    
  toneHAL.init(SPEAKER_PIN);

#if defined(PROG_DEBUG)
  SerialPort.print("Tone Sin volume = ");
  SerialPort.println(volumeDefault);

  SerialPort.println("Tone Sin");
#endif

  for(int i=1; i<=10; i++) {
    toneHAL.tone(1000*i,volumeDefault);
#if defined(PROG_DEBUG)
    SerialPort.print("Tone frequence : ");
    SerialPort.println(1000*i);
#endif
    delay(2000);
  }
  toneHAL.noTone();
  delay(2000);
  
  SerialPort.println("Tone volume");
  for(int i=0; i<=10; i++) {
    toneHAL.tone(1000,i);
#if defined(PROG_DEBUG)
    SerialPort.print("Tone volume : ");
    SerialPort.println(i);
#endif
    delay(1000);
  }
  toneHAL.noTone();
  delay(2000);
  
#if defined(TONEDAC) && defined(ARDUINO_ARCH_SAMD)
  
  SerialPort.println("Tone Square");
  toneHAL.setWaveForm(WAVEFORM_SQUARE);
  for(int i=1; i<=10; i++) {
    toneHAL.tone(1000*i,volumeDefault);
    delay(1000);
  }
  toneHAL.noTone();
  delay(2000);
   
  SerialPort.println("Tone Triangle");
  toneHAL.setWaveForm(WAVEFORM_TRIANGLE);
  for(int i=1; i<=10; i++) {
    toneHAL.tone(1000*i,volumeDefault);
    delay(1000);
  }
  toneHAL.noTone();
  delay(2000); 

//  toneHAL.beginPlayWav(sampleRateWav);
//  toneHAL.playWav(filename);
  SerialPort.println("Playing file.....");
#endif //TONEDAC

  toneHAL.tone(2000,2);
#if defined(PROG_DEBUG)
  SerialPort.println("Tone volume : 2");
#endif
  delay(2000);
  toneHAL.noTone();
  delay(1000);

  toneHAL.tone(2000,5);
#if defined(PROG_DEBUG)
  SerialPort.println("Tone volume : 5");
#endif
  delay(2000);
  toneHAL.noTone();
  delay(1000);

  toneHAL.tone(2000,7);
#if defined(PROG_DEBUG)
  SerialPort.println("Tone volume : 7");
#endif
  delay(2000);
  toneHAL.noTone();
  delay(1000);

  toneHAL.tone(2000,10);
#if defined(PROG_DEBUG)
  SerialPort.println("Tone volume : 10");
#endif
  delay(2000);
  toneHAL.noTone();
  delay(1000);

/*
//  toneHAL.setWaveForm(WAVEFORM_SINUS);
  toneHAL.tone(1000,10);
  delay(2000);
//  toneHAL.noTone();
*/

#if defined(PROG_DEBUG)
  SerialPort.println("loop");
#endif

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(pinLED, HIGH);
  delay(200);
  digitalWrite(pinLED, LOW);
  delay(200);
  if (SerialPort.available()) {
    char c = SerialPort.read();
    if (c == 'p') {
      for(int i=1; i<=10; i++) {
        toneHAL.tone(1000*i,volumeDefault);
        delay(200);
      }
      toneHAL.noTone();    
    }

    if (c == 'q') {
      for(int i=1; i<=10; i++) {
        toneHAL.tone(3000,i);
        delay(200);
      }
      toneHAL.noTone();    
    }    
  }
}
