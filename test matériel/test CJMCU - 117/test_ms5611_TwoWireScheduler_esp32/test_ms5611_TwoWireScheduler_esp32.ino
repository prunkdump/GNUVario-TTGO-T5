#include <Arduino.h>
#include <HardwareConfig.h>
#include <VarioSettings.h>
#include "VarioImuTwoWire.h"
#include <esp_heap_caps.h>

/***************/

unsigned long lastDisplayTimestamp;
unsigned long count = 0;
double alti;

#define POWER_PIN 12
#define POWER_PIN_STATE HIGH 

void setup() {

  SerialPort.begin(115200);

  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, POWER_PIN_STATE);     // turn on POWER

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
/*    Serial.print("Memoire Totale : ");
    Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT));
    Serial.print("DRam           : ");
    Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL| MALLOC_CAP_8BIT));
    Serial.print("SpRam          : ");
    Serial.println(heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));*/
#endif //PROG_DEBUG

  }

}
