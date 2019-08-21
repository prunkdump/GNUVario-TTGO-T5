#include <Arduino.h>
#include <VarioSettings.h>
#include <IntTW.h>
#include <ms5611.h>
#include <vertaccel.h>
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
#endif



/***************/
/* IMU objects */
/***************/
unsigned long lastDisplayTimestamp;
unsigned long count = 0;



double temperature, pressure;
double alti;

void setup() {

  Serial.begin(115200);

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
  //intTW.begin();
  //twScheduler.vertaccel.init();
  
  
  delay(20);

  /* init time */
  lastDisplayTimestamp = millis();

   
}
  
double accel;

void loop() {

  
   
  //twScheduler.mainInterrupt();
  //xTaskNotify(twScheduler.schedulerTaskHandler, 0, eNoAction);
  //delayMicroseconds(1168);

  
  if( twScheduler.havePressure() ) {
    alti = twScheduler.getAlti();
    count++;    
  }
  
  
  
  
  /*
  if( twScheduler.haveAccel() ) {
    accel = twScheduler.getAccel(NULL);
    count++;
  }
  */
  
  
  
  if( millis() - lastDisplayTimestamp > 1000 ) {

    
    lastDisplayTimestamp = millis();
    Serial.print(count);
    Serial.print(", ");
    Serial.println(alti);
    //Serial.println(uxTaskPriorityGet(NULL));
    count = 0;
    //intTW.lastTwError = I2C_ERROR_OK;
    
  }

  



}
