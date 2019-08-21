#include <Arduino.h>
#include <VarioSettings.h>
#include <SerialNmea.h>

#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>

/*******************/
/* General objects */
/*******************/
#define VARIOMETER_STATE_INITIAL 0
#define VARIOMETER_STATE_DATE_RECORDED 1
#define VARIOMETER_STATE_CALIBRATED 2
#define VARIOMETER_STATE_FLIGHT_STARTED 3

#ifdef HAVE_GPS
uint8_t variometerState = VARIOMETER_STATE_INITIAL;
#else
uint8_t variometerState = VARIOMETER_STATE_CALIBRATED;
#endif //HAVE_GPS

unsigned long lastDisplayTimestamp;

void setup() {

  SerialPort.begin(115200);
  serialNmea.begin(9600, true);

  /*****************************/
  /* wait for devices power on */
  /*****************************/
  delay(VARIOMETER_POWER_ON_DELAY);

  lastDisplayTimestamp = millis();
 
    
}
  

void loop() {

  if( millis() - lastDisplayTimestamp > 1000 ) {

    
    lastDisplayTimestamp = millis();
    
      
  }

    
}

  
   
 

  
