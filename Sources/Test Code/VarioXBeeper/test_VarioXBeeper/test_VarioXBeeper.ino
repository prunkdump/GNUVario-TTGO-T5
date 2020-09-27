#include <Arduino.h>

#define VERSION 0
#define SUB_VERSION 8
#define BETA_CODE 4
#define DEVNAME "JPG63/MICELPA/RATAMUSE"
#define AUTHOR "J" //J=JPG63  P=PUNKDUMP  M=MICHELPA

#include <DebugConfig.h>
#include <VarioLog.h>

#ifdef PROG_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#ifndef ARDUINOTRACE_SERIAL
#define ARDUINOTRACE_SERIAL SerialPort
#endif

#include <ArduinoTrace.h>

#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif //HAVE_SDCARD

#include <VarioHardwareManager.h>

#ifdef HAVE_SPEAKER
#include <toneHAL.h>
#include <beeper.h>
#endif //HAVE_SPEAKER

#include <VarioData.h>
VarioData varioData;

#include <VarioSettings.h>
#include <VarioLanguage.h>

#include <VarioXBeeper.h>

void setup() {
  // put your setup code here, to run once:

  /****************/
  /* init SD Card */
  /****************/

  
#ifdef PROG_DEBUG
  // Open serial communications and wait for port to open:
  SerialPort.begin(115200);
/*  while (!SerialPort) {
    ; // wait for serial port to connect. Needed for native USB port only
  }*/
  delay(5000);

  SerialPort.println("Initializing SD card...");
#endif //PRO_DEBBUG

  varioData.init(VERSION, SUB_VERSION, BETA_CODE, String(DEVNAME));

  varioHardwareManager.init();

  /******************/
  /* Init Speaker   */
  /******************/
  varioHardwareManager.initSpeaker();

  beeper.generateTone(2000, 300);

  if (varioData.initSettings(true)) {
#ifdef PROG_DEBUG
    SerialPort.println("initialization done.");
#endif //PROG_DEBUG

    SerialPort.print("Pilot Name = ");
    SerialPort.println(GnuSettings.VARIOMETER_PILOT_NAME);
  }
  else
  {
#ifdef HAVE_SPEAKER
    if (GnuSettings.ALARM_SDCARD) {
#ifdef PROG_DEBUG
      SerialPort.println("initialization failed!");
#endif //PROG_DEBUG

//      indicateFaultSDCARD();
    }
#endif //HAVE_SPEAKER 
  }  

  //***********************************************
  // INIT Sound
  //      init Beeper avec les valeurs personnelles
  //      init Volume
  //***********************************************

  varioHardwareManager.initSound();

  beeper.generateTone(2000, 300);
  beeper.generateTone(2000, 300);

  beeper.setVelocity(0);
}

int incomingByte = 0; // for incoming serial data
float vario = 0;

void loop() {
  //**********************************************************
  //  TRAITEMENT DU SON
  //**********************************************************

  toneHAL.update();

  //**********************************************************
  //  EMISSION DES BIPS
  //**********************************************************

  /*****************/
  /* update beeper */
  /*****************/
#ifdef HAVE_SPEAKER
  beeper.update();
#ifdef PROG_DEBUG
//    SerialPort.println("beeper update");
#endif //PROG_DEBUG
#endif //HAVE_SPEAKER

  // test saisie
  if (SerialPort.available() > 0) {
    // read the incoming byte:
    SerialPort.println("Read");
    incomingByte = SerialPort.read();

    // say what you got:
    SerialPort.print("I received: ");
    SerialPort.println(incomingByte, DEC);

    if (incomingByte == 45) {
      // -
      vario = vario - 0.1;
      if (vario < -10.0) vario = -10.0;
    }
    else if (incomingByte == 43) {
      // +
      vario = vario + 0.1;
      if (vario > 10.0) vario = 10.0;      
    }

    if ((incomingByte == 43) || (incomingByte == 45)) {
#ifdef HAVE_SPEAKER
      beeper.setVelocity(vario);
#endif //HAVE_SPEAKER
 
      SerialPort.print("Vario: ");
      SerialPort.println(vario);

      float freq = varioXBeeper.getFrequence(vario);
      SerialPort.print("Frequence: ");
      SerialPort.println(freq);

      int cycle = varioXBeeper.getCycle(vario);
      SerialPort.print("Cycle: ");
      SerialPort.println(cycle);    

      int duty = varioXBeeper.getDuty(vario);
      SerialPort.print("Duty: ");
      SerialPort.println(duty);    
    }  
  }
}
