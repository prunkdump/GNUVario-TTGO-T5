/*******************/
/* Version         */
/*******************/

#define VERSION      0
#define SUB_VERSION  4
#define BETA_CODE    7
#define DEVNAME      "JPG63"
#define AUTHOR "J"    //J=JPG63  P=PUNKDUMP

#include <sdcardHAL.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>
#include <SPI.h>

#include <wifiServer.h>

#include <VarioSettings.h>

VarioSettings GnuSettings;

String webpage = "";

#ifdef ESP8266
  ESP8266WiFiMulti wifiMulti; 
  ESP8266WebServer server(80);
#else
  WiFiMulti wifiMulti;
  ESP32WebServer server(80);
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup(void){
  SerialPort.begin(115200);
  delay(5000);
   
  SerialPort.println(F("Initializing SD card...")); 

  if (GnuSettings.initSettings()) {
#ifdef PROG_DEBUG
    SerialPort.println("initialization done.");
    SerialPort.println(F("Card initialised... file access enabled..."));
#endif //PROG_DEBUG
    SD_present = true; 

    GnuSettings.readSDSettings();
 
#ifdef PROG_DEBUG
   //Debuuging Printing
    SerialPort.print("Pilot Name = ");
    SerialPort.println(GnuSettings.VARIOMETER_PILOT_NAME);
#endif //PROG_DEBUG
  }
  else
  {
#ifdef PROG_DEBUG
    SerialPort.println("initialization failed!");
    SerialPort.println(F("Card failed or not present, no SD Card data logging possible..."));
#endif //PROG_DEBUG
    SD_present = false; 
    
#ifdef HAVE_SPEAKER
    if (GnuSettings.ALARM_SDCARD) {
//      indicateFaultSDCARD();
    }
#endif //HAVE_SPEAKER 
  }  

  if (BETA_CODE > 0)
    sprintf(ServerVersion,"v %i.%i beta %i- %s\0", VERSION,SUB_VERSION,BETA_CODE,DEVNAME); 
  else
    sprintf(ServerVersion,"v %i.%i - %s\0", VERSION,SUB_VERSION,DEVNAME); 
    
  wifiServer.begin();
  wifiServer.connect();

  wifiServer.start();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop(void){
  wifiServer.handleClient(); 
}
