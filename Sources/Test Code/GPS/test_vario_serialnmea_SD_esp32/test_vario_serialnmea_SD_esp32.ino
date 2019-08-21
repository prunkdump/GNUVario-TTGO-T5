#include <Arduino.h>
#include <VarioSettings.h>
#include <HardwareConfig.h>
#include <IntTW.h>
#include <ms5611.h>
#include <vertaccel.h>
#include <LightInvensense.h>
#include <TwoWireScheduler.h>

#include <kalmanvert.h>

#include <toneHAL.h>
#include <beeper.h>

#include <SerialNmea.h>

#include <sdcardHAL.h>

#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <FlightHistory.h>

/*******************/
/* Version         */
/*******************/

#define VERSION      0
#define SUB_VERSION  1
#define DEVNAME      "JPG63"

/*******************/
/*     VERSION     */
/*      ESP32      */
/*                 */
/*    Historique   */
/*******************/
/* v 0.1     beta 1 version
 * 
*******************/

/***************/
/* IMU objects */
/***************/
#ifdef HAVE_BMP280
Bmp280 TWScheduler::bmp280;
#else
Ms5611 TWScheduler::ms5611;
#endif
#ifdef HAVE_ACCELEROMETER
Vertaccel TWScheduler::vertaccel;
#endif //HAVE_ACCELEROMETER

//Vertaccel vertaccel;

/**********************/
/* alti/vario objects */
/**********************/
#define POSITION_MEASURE_STANDARD_DEVIATION 0.1
#ifdef HAVE_ACCELEROMETER 
#define ACCELERATION_MEASURE_STANDARD_DEVIATION 0.3
#else
#define ACCELERATION_MEASURE_STANDARD_DEVIATION 0.6
#endif //HAVE_ACCELEROMETER 

kalmanvert kalmanvert;

#ifdef HAVE_SPEAKER

#define volumeDefault 5

#include <beeper.h>

Beeper beeper(volumeDefault);

#define BEEP_FREQ 800
#endif //HAVE_SPEAKER

/**********************/
/* SDCARD objects     */
/**********************/

VarioSettings GnuSettings;

#ifdef HAVE_SDCARD
File fileIgc;
IGCHeader header;
IGCSentence igc;

#define SDCARD_STATE_INITIAL 0
#define SDCARD_STATE_INITIALIZED 1
#define SDCARD_STATE_READY 2
#define SDCARD_STATE_ERROR -1
int8_t sdcardState = SDCARD_STATE_INITIAL;

#endif //HAVE_SDCARD

unsigned long lastDisplayTimestamp;
//unsigned long count = 0;
//double alti;

void setup() {

  SerialPort.begin(115200);

/************************/
/*    BOOT SEQUENCE     */
/************************/

#ifdef PROG_DEBUG

  SerialPort.begin(115200);
///  while (!SerialPort) { ;}
  char tmpbuffer[100];
  sprintf(tmpbuffer,"ESP32 MPU9250 MS5611 VARIO compiled on %s\0", __DATE__); // at %s", __DATE__, __TIME__);
  SerialPort.println(tmpbuffer);
  sprintf(tmpbuffer,"VERSION %i.%i - %s\0", VERSION,SUB_VERSION,DEVNAME); 
  SerialPort.println(tmpbuffer);
  SerialPort.flush();
#endif //PRO_DEBBUG

  /*****************************/
  /* wait for devices power on */
  /*****************************/
//  delay(VARIOMETER_POWER_ON_DELAY);
  delay (5000);
  
  /******************/
  /* Init Speaker   */
  /******************/
  
#if defined( HAVE_SPEAKER)
  toneHAL.init();
#endif

  /******************/
  /* Init SDCARD    */
  /******************/

#ifdef HAVE_SDCARD

  if (GnuSettings.initSettings()) {
#ifdef PROG_DEBUG
   SerialPort.println("initialization done.");
#endif //PROG_DEBUG

   sdcardState = SDCARD_STATE_INITIALIZED;
   GnuSettings.readSDSettings();

//****************************************
//****************************************
//****************************************
//  createSDCardTrackFile();


/*  fileIgc = SDHAL.open("TVARIO.TXT", FILE_WRITE);
  if (fileIgc) {
    
#ifdef IMU_DEBUG
        //Debuuging Printing
    SerialPort.println("Write File SD");
#endif //IMU_DEBUG

     // writing in the file works just like regular print()/println() function
    fileIgc.println("[TEST]");
    fileIgc.flush();
//    myFile2.close();

      sdcardState = SDCARD_STATE_READY;
#ifdef PROG_DEBUG
      SerialPort.println("fichier test créé");
#endif //PROG_DEBUG
    }*/
    
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

//    indicateFaultSDCARD();
    }
#endif //HAVE_SPEAKER 
  }
#endif //HAVE_SDCARD

  /**************************/
  /* init Two Wires devices */
  /**************************/
  //!!!
  intTW.begin();
  twScheduler.init();
//  vertaccel.init();

  /******************/
  /* get first data */
  /******************/
  
  /* wait for first alti and acceleration */
  while( ! twScheduler.havePressure() ) { }

#ifdef PROG_DEBUG
    SerialPort.println("première mesure");
#endif //PROG_DEBUG

  /* init kalman filter with 0.0 accel*/
  double firstAlti = twScheduler.getAlti();
#ifdef PROG_DEBUG
    SerialPort.print("firstAlti : ");
    SerialPort.println(firstAlti);
#endif //PROG_DEBUG

  kalmanvert.init(firstAlti,
                  0.0,
                  POSITION_MEASURE_STANDARD_DEVIATION,
                  ACCELERATION_MEASURE_STANDARD_DEVIATION,
                  millis());

#ifdef PROG_DEBUG
    SerialPort.println("kalman init");
#endif //PROG_DEBUG

  serialNmea.begin(9600, true);
#ifdef PROG_DEBUG
    SerialPort.println("SerialNmea init");
#endif //PROG_DEBUG

  /* init time */
  lastDisplayTimestamp = millis();   
}

int16_t accel[3];
int32_t quat[4];



void loop() {

 /* if( vertaccel.readRawAccel(accel, quat) ){
    count++;
  }*/

#ifdef HAVE_ACCELEROMETER
  if( twScheduler.havePressure() && twScheduler.haveAccel() ) {
    
#ifdef PROG_DEBUG
//    SerialPort.println("havePressure && haveAccel");
#endif //PROG_DEBUG

    kalmanvert.update( twScheduler.getAlti(),
                       twScheduler.getAccel(NULL),
                       millis() );
#else
  if( twScheduler.havePressure() ) {
    
#ifdef PROG_DEBUG
//    SerialPort.println("havePressure");
#endif //PROG_DEBUG

    kalmanvert.update( twScheduler.getAlti(),
                       0.0,
                       millis() );
#endif //HAVE_ACCELEROMETER
  }

#ifdef HAVE_SPEAKER

    beeper.setVelocity( kalmanvert.getVelocity() );
//*****************
//* update beeper *
//*****************

  beeper.update();
#ifdef PROG_DEBUG
//    SerialPort.println("beeper update");
#endif //PROG_DEBUG

#endif //HAVE_SPEAKER

 
  if( millis() - lastDisplayTimestamp > 1000 ) {

    lastDisplayTimestamp = millis();
    //Serial.println(intTW.lastTwError);
//    SerialPort.println(accel[2]);
    
#ifdef PROG_DEBUG
//    SerialPort.println("loop");

    SerialPort.print("Vario : ");
    SerialPort.println(kalmanvert.getVelocity());
#endif //PROG_DEBUG


  }

}
