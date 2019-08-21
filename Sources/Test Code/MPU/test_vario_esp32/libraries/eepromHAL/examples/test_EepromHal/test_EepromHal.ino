#include <Arduino.h>

#if defined(ARDUINO_ARCH_SAMD)
#include <SDU.h>    //FIRMWARE Update
#endif //ARDUINO_ARCH_SAMD

#include "eepromHAL.h"
#if defined(ESP8266)
#include "eepromHAL_8266.h"
#elif defined(ESP32)
#include "eepromHAL_ESP32.h"
#elif defined(ARDUINO_ARCH_SAMD)
#include "eepromHAL_M0.h"
#elif defined(_BOARD_GENERIC_STM32F103C_H_)

#elif defined(ARDUINO_AVR_PRO)
#include "eepromHAL_PRO.h"
#else

#endif

#include <VarioSettings.h>

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
//#define SOUND_EEPROM_TAG 9806
//#define SOUND_EEPROM_ADDR 0x30

//EepromHal_ESP32 
EepromHAL EepromHAL_DATA;

void setup() {
  
  SerialPort.begin(115200);
  SerialPort.println("start...");

  EepromHAL_DATA.init();

   /* check tag */
  uint16_t eepromTag;
  uint8_t tmpValue=0;
  
  if (!EepromHAL_DATA.isValid()) {
    SerialPort.println("EEPROM is empty, writing some example data:");
  } else {
    
    eepromTag = EepromHAL_DATA.read(SOUND_EEPROM_ADDR);
    eepromTag <<= 8;
    eepromTag += EepromHAL_DATA.read(SOUND_EEPROM_ADDR + 0x01);
  
    uint8_t TmpValue;
    if( eepromTag != SOUND_EEPROM_TAG ) { 
      SerialPort.println("EEPROM Error EpromTag");
    } else {
      /* read calibration settings */
      tmpValue =  EepromHAL_DATA.read(SOUND_EEPROM_ADDR + 0x02);
      SerialPort.print("Read sound value : ");
      SerialPort.println(tmpValue);
    }
  }

  SerialPort.print("Write sound volume : ");
  SerialPort.println(VARIOMETER_BEEP_VOLUME);

  /* write tag */
  eepromTag = SOUND_EEPROM_TAG;
  EepromHAL_DATA.write(SOUND_EEPROM_ADDR, (eepromTag>>8) & 0xff);
  EepromHAL_DATA.write(SOUND_EEPROM_ADDR + 0x01, eepromTag & 0xff);

  EepromHAL_DATA.write(SOUND_EEPROM_ADDR + 0x02 , VARIOMETER_BEEP_VOLUME);
  
  EepromHAL_DATA.commit();  
}

void loop() {
  // put your main code here, to run repeatedly:

}
