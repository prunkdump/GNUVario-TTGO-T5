#include "VarioButton.h"
#include <DebugConfig.h>

#include <sdcardHAL.h>

#include <HardwareConfig.h>

void setup() {
  // put your setup code here, to run once:
  VarioButton.begin();

  SerialPort.begin(115200);
  
  SerialPort.print("Initializing SD card...");
  /* initialize SD library with SPI pins */
 if (!SDHAL.begin()) {            //T1:13,15,2,14  T2: 23,5,19,18
    SerialPort.println("initialization failed!");
    return;
  }
 SerialPort.println("initialization done.");
    
}

void loop() {

  ButtonScheduleur.update();

}
