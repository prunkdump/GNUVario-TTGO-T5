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

#define volumeDefault 10

void setup() {
  // put your setup code here, to run once:
  SerialPort.begin(115200);
//  while (!SerialPort) { ;}
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

/*#if defined(ARDUINO_ARCH_SAMD)

  //activation de l'ampli D
  digitalWrite(6, LOW);
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);

#endif //ARDUINO_ARCH_SAMD*/
    
  toneHAL.init(SPEAKER_PIN);

#if defined(PROG_DEBUG)
  SerialPort.print("Tone volume = ");
  SerialPort.println(volumeDefault);

  SerialPort.println("Tone");
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
  
  toneHAL.tone(2000,10);
#if defined(PROG_DEBUG)
  SerialPort.println("Tone volume : 10");
#endif

/*  delay(2000);
  toneHAL.noTone();
  delay(1000);*/

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
}
