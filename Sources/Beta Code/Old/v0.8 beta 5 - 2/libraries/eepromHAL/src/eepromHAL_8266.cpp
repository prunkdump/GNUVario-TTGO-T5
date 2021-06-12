/*********************************************************************************/
/*                                                                               */
/*                           Libraries eepromHal_8266                            */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    06/02/19                                                            */
/*    1.1    09/07/19   Ajout init(size)                                         */
/*                                                                               */
/*********************************************************************************/

#include "eepromHAL.h"
#include "eepromHAL_8266.h"
#include <Arduino.h>

#if defined(ESP8266)
#include <EEPROM.h>

#include <DebugConfig.h>

/*#if not defined(DEBUG_H)
//Monitor Port 
#define SerialPort Serial

#define EEPROM_DEBUG

#endif*/

EepromHAL EEPROMHAL;

void EepromHal_8266::init(void) {}

void EepromHal_8266::init(int size) {}

uint8_t EepromHal_8266::read(int address)
{
#if defined(ESP8266)
  return EEPROM.read(address);
#endif //ESP8266
}

void EepromHal_8266::update(int address, uint8_t value)
{
#if defined(ESP8266)
  EEPROM.update(address, value);
#endif //ESP8266
}

void EepromHal_8266::write(int address, uint8_t value)
{
#if defined(ESP8266)
  EEPROM.write(address, value);
#endif //ESP8266
}

bool EepromHal_8266::isValid()
{
  return true;
}

void EepromHal_8266::commit()
{
}

#endif //ESP8266