#if defined(ARDUINO_ARCH_SAMD)
#include <SDU.h>    //FIRMWARE Update
#endif //ARDUINO_ARCH_SAMD

#include <Arduino.h>
#include <VarioSettings.h>
#include <sdcardHAL.h>

#include <DebugConfig.h>

/*SdFat SD;
File file;*/

VarioSettings GnuSettings;

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


  SerialPort.println("Initializing SD card...");
#endif //PRO_DEBBUG

  if (GnuSettings.initSettings()) {
#ifdef PROG_DEBUG
   SerialPort.println("initialization done.");
#endif //PROG_DEBUG

   GnuSettings.readSDSettings();
 
#ifdef PROG_DEBUG
   //Debuuging Printing
 SerialPort.print("Pilot Name = ");
 SerialPort.println(GnuSettings.VARIOMETER_PILOT_NAME);
#endif //PROG_DEBUG
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
}

void loop() {
  // put your main code here, to run repeatedly:

}
