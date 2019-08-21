#include <Arduino.h>
#include <VarioSettings.h>
#include <HardwareConfig.h>
#include <IntTW.h>
#include <ms5611.h>
#include <LightInvensense.h>
#include <TwoWireScheduler.h>

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

unsigned long lastDisplayTimestamp;
unsigned long count = 0;
double alti;

void setup() {

  SerialPort.begin(115200);

  /*****************************/
  /* wait for devices power on */
  /*****************************/
  delay(VARIOMETER_POWER_ON_DELAY);

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

  /* init time */
  lastDisplayTimestamp = millis();   
}

void loop() {

 /* if( vertaccel.readRawAccel(accel, quat) ){
    count++;
  }*/

  if (twScheduler.havePressure( )) {
    
#ifdef PROG_DEBUG
//    SerialPort.println("havePressure && haveAccel");
#endif //PROG_DEBUG

    alti = twScheduler.getAlti();
  }

 
  if( millis() - lastDisplayTimestamp > 1000 ) {

    lastDisplayTimestamp = millis();
    
#ifdef PROG_DEBUG
    SerialPort.print("Alti : ");
    SerialPort.println(alti);
#endif //PROG_DEBUG

  }

}
