/*********************************************************************************/
/*                                                                               */
/*                           Libraries eepromHal_ESP32                           */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    01/04/19                                                            */
/*    1.0.1  02/04/19   Ajout fonction init                                      */
/*    1.0.2  07/05/19   Ajout #include <DebugConfig.h> et EepromHAL EEPROMHAL;   */
/*    1.0.3  09/07/19   Ajout init(int size)                                     */
/*    1.0.4  14/07/19   Correction bug taille memoire                            */
/*                                                                               */
/*********************************************************************************/

//#include "eepromHAL.h"
#include "eepromHAL_ESP32.h"
#include <Arduino.h>

#if defined(ESP32)
#include <EEPROM.h>

#include <DebugConfig.h>

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 512
#endif

EepromHAL EEPROMHAL;

void EepromHal_ESP32::init(void) 
{
	
#if defined (EEPROM_DEBUG) 
    SerialPort.println("initialise EEPROM"); 
		SerialPort.flush();
#endif //EEPROM_DEBUG

	Eeprom_Size = EEPROM_SIZE;
  if (!EEPROM.begin(Eeprom_Size))
  {
#if defined (EEPROM_DEBUG) 
    SerialPort.println("failed to initialise EEPROM"); 
		SerialPort.flush();
		delay(1000000);
#endif //EEPROM_DEBUG
  }	
}

void EepromHal_ESP32::init(int size) 
{
	
#if defined (EEPROM_DEBUG) 
    SerialPort.println("initialise EEPROM"); 
		SerialPort.flush();
#endif //EEPROM_DEBUG

	Eeprom_Size = size;

  if (!EEPROM.begin(Eeprom_Size))
  {
#if defined (EEPROM_DEBUG) 
    SerialPort.println("failed to initialise EEPROM"); 
		SerialPort.flush();
		delay(1000000);
#endif //EEPROM_DEBUG
  }	
}

uint8_t EepromHal_ESP32::read(int address)
{
	uint8_t value;
	value = EEPROM.read(address);
#if defined (EEPROM_DEBUG) 
    SerialPort.print("EEPROM Read : "); 
    SerialPort.println(value); 
	SerialPort.flush();
#endif //EEPROM_DEBUG
	
  return value;
}

void EepromHal_ESP32::update(int address, uint8_t value)
{	
	if (address <= Eeprom_Size) {
		EEPROM.write(address, value);
		EEPROM.commit();
	}
}

void EepromHal_ESP32::write(int address, uint8_t value)
{
#if defined (EEPROM_DEBUG) 
    SerialPort.print("EEPROM write : "); 
    SerialPort.println(value); 
	SerialPort.flush();
#endif //EEPROM_DEBUG
	
  if (address <= Eeprom_Size) EEPROM.write(address, value);
}

bool EepromHal_ESP32::isValid()
{
  return true;
}

void EepromHal_ESP32::commit()
{
	EEPROM.commit();
}

#if defined (EEPROMHAL_EXTENDED)	

uint8_t 	EepromHal_ESP32::readByte(int address) {
	return (EEPROM.readByte(address));
}

int8_t 		EepromHal_ESP32::readChar(int address) {
  return(EEPROM.readChar(address));
}

uint8_t 	EepromHal_ESP32::readUChar(int address) {
  return(EEPROM.readUChar(address));	
}

int16_t 	EepromHal_ESP32::readShort(int address) {
  return(EEPROM.readShort(address));	
}
uint16_t 	EepromHal_ESP32::readUShort(int address) {
  return(EEPROM.readUShort(address));	
}

int32_t 	EepromHal_ESP32::readIntE(int address) {
  return(EEPROM.readInt(address));
}

uint32_t 	EepromHal_ESP32::readUInt(int address) {
  return(EEPROM.readUInt(address));
}

int32_t 	EepromHal_ESP32::readLong(int address) {
  return(EEPROM.readLong(address));
}

uint32_t 	EepromHal_ESP32::readULong(int address) {
  return(EEPROM.readULong(address));
}

int64_t 	EepromHal_ESP32::readLong64(int address) {
  return(EEPROM.readLong64(address));
}

uint64_t 	EepromHal_ESP32::readULong64(int address) {
  return(EEPROM.readULong64(address));
}

float_t 	EepromHal_ESP32::readFloatE(int address) {
  return(EEPROM.readFloat(address));
}

double_t 	EepromHal_ESP32::readDouble(int address) {
  return(EEPROM.readDouble(address));
}

bool 			EepromHal_ESP32::readBool(int address) {
  return(EEPROM.readBool(address));
}

size_t 		EepromHal_ESP32::readString(int address, char* value, size_t maxLen) {
  return(EEPROM.readString(address, value, maxLen));
}

String 		EepromHal_ESP32::readString(int address) {
  return(EEPROM.readString(address));
}

size_t 		EepromHal_ESP32::readBytes(int address, void * value, size_t maxLen) {
  return(EEPROM.readBytes(address, value, maxLen));
}

template <class T> T EepromHal_ESP32::readAll (int address, T &) {
	
}

size_t EepromHal_ESP32::writeByte(int address, uint8_t value) {
  return(EEPROM.writeByte(address, value));
}

size_t EepromHal_ESP32::writeChar(int address, int8_t value) {
  return(EEPROM.writeChar(address, value));
}

size_t EepromHal_ESP32::writeUChar(int address, uint8_t value) {
  return(EEPROM.writeUChar(address, value));
}

size_t EepromHal_ESP32::writeShort(int address, int16_t value) {
  return(EEPROM.writeShort(address, value));
}

size_t EepromHal_ESP32::writeUShort(int address, uint16_t value) {
  return(EEPROM.writeUShort(address, value));
}

size_t EepromHal_ESP32::writeIntE(int address, int32_t value) {
  return(EEPROM.writeInt(address, value));
}

size_t EepromHal_ESP32::writeUInt(int address, uint32_t value) {
  return(EEPROM.writeUInt(address, value));
}

size_t EepromHal_ESP32::writeLong(int address, int32_t value) {
  return(EEPROM.writeLong(address, value));
}

size_t EepromHal_ESP32::writeULong(int address, uint32_t value) {
  return(EEPROM.writeULong(address, value));
}

size_t EepromHal_ESP32::writeLong64(int address, int64_t value) {
  return(EEPROM.writeLong64(address, value));
}

size_t EepromHal_ESP32::writeULong64(int address, uint64_t value) {
  return(EEPROM.writeULong64(address, value));
}

size_t EepromHal_ESP32::writeFloatE(int address, float_t value) {
  return(EEPROM.writeFloat(address, value));
}

size_t EepromHal_ESP32::writeDouble(int address, double_t value) {
  return(EEPROM.writeDouble(address, value));
}

size_t EepromHal_ESP32::writeBool(int address, bool value) {
  return(EEPROM.writeBool(address, value));	
}

size_t EepromHal_ESP32::writeString(int address, const char* value) {
  return(EEPROM.writeString(address, value));
}

size_t EepromHal_ESP32::writeString(int address, String value) {
  return(EEPROM.writeString(address, value));
}

size_t EepromHal_ESP32::writeBytes(int address, const void* value, size_t len) {
  return(EEPROM.writeBytes(address, value, len));
}

template <class T> T EepromHal_ESP32::writeAll (int address, const T &) {
	
}

#endif


#endif //ESP32