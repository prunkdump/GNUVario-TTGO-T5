#include <Arduino.h>

#if defined(ARDUINO_ARCH_SAMD)
#include <SDU.h>    //FIRMWARE Update
#endif //ARDUINO_ARCH_SAMD

#include "eepromHAL.h"

#include <DebugConfig.h>
#include <HardwareConfig.h>

/*#if not defined(DEBUG_H)
//Monitor Port 
#if defined(ESP8266)
#define SerialPort Serial
#elif defined(ESP32)
#define SerialPort Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialPort SerialUSB
#elif defined(_BOARD_GENERIC_STM32F103C_H_)

#elif defined(ESP8266)
#define SerialPort Serial
#else
#define SerialPort Serial
#endif

#define EEPROM_DEBUG

#endif //DEBUG_H*/

/* The volume of the beeps, max = 10 */
#define VARIOMETER_BEEP_VOLUME 5

/* eeprom sound setting adresses */
#define SOUND_EEPROM_TAG 9806
#define SOUND_EEPROM_ADDR 0x00     
//0x30

//EepromHal_ESP32 
//EepromHAL EepromHAL_DATA;

void setup() {
  
  SerialPort.begin(115200);
  delay(5000);
  SerialPort.println("start...");

  EEPROMHAL.init(512);

   /* check tag */
  uint16_t eepromTag, eepromTagTmp;
  uint8_t tmpValue=0;
  
  if (!EEPROMHAL.isValid()) {
    SerialPort.println("EEPROM is empty, writing some example data:");
  } else {

   
    eepromTag = EEPROMHAL.read(SOUND_EEPROM_ADDR);

    SerialPort.print("EEprom Tag : ");
    SerialPort.println(eepromTag);
  
    eepromTag <<= 8;

    eepromTagTmp = EEPROMHAL.read(SOUND_EEPROM_ADDR + 0x01);
    
    SerialPort.println(eepromTagTmp);

    eepromTag += eepromTagTmp;

    SerialPort.println(eepromTag);

    uint8_t TmpValue;
    if( eepromTag != SOUND_EEPROM_TAG ) { 
      SerialPort.println("EEPROM Error EpromTag");
    } else {
      /* read calibration settings */
      tmpValue =  EEPROMHAL.read(SOUND_EEPROM_ADDR + 0x02);
      SerialPort.print("Read sound value : ");
      SerialPort.println(tmpValue);
    }
  }

  SerialPort.print("Write sound volume : ");
  SerialPort.println(VARIOMETER_BEEP_VOLUME);

  /* write tag */

  SerialPort.print("Write EEprom Tag : ");
   
  eepromTag = SOUND_EEPROM_TAG;

  SerialPort.println(eepromTag);

  eepromTagTmp = eepromTag;
  SerialPort.println((eepromTagTmp>>8) & 0xff);
  
  EEPROMHAL.write(SOUND_EEPROM_ADDR, (eepromTag>>8) & 0xff);

  SerialPort.println(eepromTag);
 
  EEPROMHAL.write(SOUND_EEPROM_ADDR + 0x01, eepromTag & 0xff);

  SerialPort.println(eepromTagTmp & 0xff);

  EEPROMHAL.write(SOUND_EEPROM_ADDR + 0x02 , VARIOMETER_BEEP_VOLUME);
  
  EEPROMHAL.commit();  
}

void loop() {
  // put your main code here, to run repeatedly:

}
