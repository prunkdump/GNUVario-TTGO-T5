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
#endif

unsigned long lastDisplayTimestamp;
unsigned long count = 0;
double alti;

void setup() {

  SerialPort.begin(115200);

  /*****************************/
  /* wait for devices power on */
  /*****************************/
  delay(VARIOMETER_POWER_ON_DELAY);

  /******************/
  /* Init Speaker   */
  /******************/
  
#if defined( HAVE_SPEAKER)
  toneHAL.init();
#endif

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
