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
Vertaccel vertaccel;


unsigned long lastDisplayTimestamp;
unsigned long count = 0;
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
  //twScheduler.init();
  vertaccel.init();

  /* init time */
  lastDisplayTimestamp = millis();

   
}

int16_t accel[3];
int32_t quat[4];



void loop() {

  if( vertaccel.readRawAccel(accel, quat) ){
    count++;
  }

  


 
  if( millis() - lastDisplayTimestamp > 1000 ) {

    lastDisplayTimestamp = millis();
    //Serial.println(intTW.lastTwError);
    Serial.println(accel[2]);

  }

}
