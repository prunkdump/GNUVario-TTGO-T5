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

#include <TinyGPS++.h>

#include <sdcardHAL.h>

#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <FlightHistory.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

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

/*****************/
/* screen        */
/*****************/

#include <varioscreenGxEPD.h>

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

/**********************/
/* sPEAKER objects    */
/**********************/

#ifdef HAVE_SPEAKER

#define volumeDefault 8

#include <beeper.h>

Beeper beeper(volumeDefault);

#define BEEP_FREQ 800
#endif

/**********************/
/* GPS objects        */
/**********************/

#ifdef HAVE_GPS
char data;
double latitude;
double longitude;
double alt; //altitude
double vitesse;
unsigned long nbre_sat;

uint8_t heures;
uint8_t minutes;
uint8_t secondes;

TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

NmeaParser nmeaParser;

#endif //HAVE_GPS

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
unsigned long count = 0;
double alti;

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
void createSDCardTrackFile(void);
#endif //defined(HAVE_SDCARD) && defined(HAVE_GPS)

/**********************************/
/*********************************/
void setup() {
/*********************************/
/*********************************/  

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
 
  /***************/
  /* init screen */
  /***************/
#ifdef PROG_DEBUG
      SerialPort.println("initialization screen");
      SerialPort.flush();
#endif //PROG_DEBUG

  screen.begin();
  
  /******************/
  /* Init Speaker   */
  /******************/
  
#if defined( HAVE_SPEAKER)
  toneHAL.init();
#endif

  /******************/
  /* Init SDCARD    */
  /******************/

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


  fileIgc = SDHAL.open("TVARIO.TXT", FILE_WRITE);
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
    }
    
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

//   indicateFaultSDCARD();
   }
#endif //HAVE_SPEAKER 
  }

  /**************************/
  /* init Two Wires devices */
  /**************************/
  //!!!
  intTW.begin();
  twScheduler.init();
//  vertaccel.init();


  /******************/
  /* init GPS       */
  /******************/

#ifdef HAVE_GPS
  SerialGPS.begin(9600, SERIAL_8N1, 19, -1, false);  //-1
#endif //HAVE_GPS

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

#if defined(HAVE_BLUETOOTH)

#ifdef PROG_DEBUG
  SerialPort.println("Starting BLE work!");
#endif //PROG_DEBUG

  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
#ifdef PROG_DEBUG
  SerialPort.println("Characteristic defined! Now you can read it in your phone!");
#endif //PROG_DEBUG
#endif //HAVE_BLUETOOTH
/*----------------------------------------*/
/*                                        */
/*             DISPLAY BOOT               */
/*                                        */
/*----------------------------------------*/

#ifdef PROG_DEBUG
  SerialPort.println("Display boot");
#endif //PRO_DEBBUG


  screen.ScreenViewInit(VERSION,SUB_VERSION);

#ifdef PROG_DEBUG
    SerialPort.println("update screen");
#endif //PRO_DEBBUG

  screen.schedulerScreen->enableShow();
  
  /* init time */
  lastDisplayTimestamp = millis();   
}

int16_t accel[3];
int32_t quat[4];
double temprature=0;

/************************/
/************************/
void loop() {
/************************/
/************************/  

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

#ifdef HAVE_GPS
    while (SerialGPS.available()) {
      data = SerialGPS.read();
//      SerialPort.print(data);
      gps.encode(data);
    }
#endif //HAVE_GPS 

  if( millis() - lastDisplayTimestamp > 1000 ) {

    lastDisplayTimestamp = millis();
    //Serial.println(intTW.lastTwError);
//    SerialPort.println(accel[2]);
    
#ifdef PROG_DEBUG
//    SerialPort.println("loop");
#endif //PROG_DEBUG

    SerialPort.println("");
    SerialPort.println("-------- VARIO ------------");
    SerialPort.print("Vario : ");
    SerialPort.println(kalmanvert.getVelocity());
    SerialPort.println("");
    SerialPort.flush();

#ifdef HAVE_GPS
    if (gps.location.isUpdated())
     {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      alt = gps.altitude.meters();
      vitesse = gps.speed.kmph();
      nbre_sat = gps.satellites.value();

      SerialPort.println("-------- FIX GPS ------------");
      SerialPort.print("LATITUDE="); SerialPort.println(latitude);
      SerialPort.print("LONGITUDE="); SerialPort.println(longitude);
      SerialPort.print("ALTITUDE (m) ="); SerialPort.println(alt);
      SerialPort.print("VITESSE (km/h)="); SerialPort.println(vitesse);
      SerialPort.print("NOMBRE SATTELLITES="); SerialPort.println(nbre_sat);
      SerialPort.println("");
      SerialPort.flush();
    }

    if (gps.time.isUpdated())
    {
      heures   = gps.time.hour();
      minutes  = gps.time.minute();
      secondes = gps.time.second();

      SerialPort.println("-------- TIME GPS ------------");
      SerialPort.print(heures);
      SerialPort.print(":"); 
      SerialPort.print(minutes);
      SerialPort.print(":"); 
      SerialPort.println(secondes);
      SerialPort.println("");
      SerialPort.flush();
    }    
#endif //HAVE_GPS


    temprature += 0.1; //(temprature_sens_read() - 32) / 1.8;
    if (temprature > 99.99) temprature = 0; 
 
#ifdef PROG_DEBUG
    SerialPort.print("tenperature : ");
    SerialPort.print(temprature);
    SerialPort.println(" °C");
#endif //PRO_DEBBUG

    screen.tensionDigit->setValue(temprature);
    screen.tempratureDigit->setValue(0);
//   screen.updateData(DISPLAY_OBJECT_TEMPRATURE, temprature);
    screen.schedulerScreen->displayStep();
    screen.updateScreen(); 

#ifdef HAVE_SDCARD
    fileIgc.print(heures);
    fileIgc.print(":"); 
    fileIgc.print(minutes);
    fileIgc.print(":"); 
    fileIgc.print(secondes);
    fileIgc.print(" - ");
    fileIgc.println(kalmanvert.getVelocity());
    fileIgc.flush();
#endif
  }
}

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
void createSDCardTrackFile(void) {
  /* start the sdcard record */
  if( sdcardState == SDCARD_STATE_INITIALIZED ) {

    /* build date : convert from DDMMYY to YYMMDD */
    uint8_t dateChar[8]; //two bytes are used for incrementing number on filename
    uint8_t* dateCharP = dateChar;
    uint32_t date = nmeaParser.date;
    for(uint8_t i=0; i<3; i++) {
      uint8_t num = ((uint8_t)(date%100));
      dateCharP[0] = (num/10) + '0';
      dateCharP[1] = (num%10) + '0';
      dateCharP += 2;
      date /= 100;
    }

    /* create file */    
    fileIgc = SD.open((char*)dateChar, FILE_WRITE);
    if (fileIgc) {
      sdcardState = SDCARD_STATE_READY;
            
      /* write the header */
      int16_t datePos = header.begin();
      if( datePos >= 0 ) {
        while( datePos ) {
        fileIgc.write(header.get());
          datePos--;
        }

        /* write date : DDMMYY */
        uint8_t* dateCharP = &dateChar[4];
        for(int i=0; i<3; i++) {
          fileIgc.write(dateCharP[0]);
          fileIgc.write(dateCharP[1]);
          header.get();
          header.get();
          dateCharP -= 2;
        }
            
        while( header.available() ) {
          fileIgc.write(header.get());
        }
      }
    } else {
      sdcardState = SDCARD_STATE_ERROR; //avoid retry 
    }
  }
}
#endif //defined(HAVE_SDCARD) && defined(HAVE_GPS)
