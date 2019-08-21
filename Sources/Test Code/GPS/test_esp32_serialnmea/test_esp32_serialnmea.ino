#include <Arduino.h>
#include <VarioSettings.h>
#include <SerialNmea.h>

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

  
   
 

  
