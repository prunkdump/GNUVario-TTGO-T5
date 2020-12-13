/*********************************************************************************/
/*                                                                               */
/*                           Libraries EepromHAL                                 */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    20/01/19                                                            */
/*    1.1    24/01/19   Réecriture des classes                                   */
/*                      répartition en plusieurs fichiers                        */
/*    1.2    26/01/19   Modifications mineures                                   */
/*    1.3    01/04/19   Ajout ESP32                                              */
/*    1.3.1  02/04/19   Ajout fonction init                                      */
/*    1.3.2  07/05/19   Ajout #include <DebugConfig.h> et EepromHAL EEPROMHAL;   */
/*    1.3.3  09/07/19   Ajout init(int size)                                     */
/*                                                                               */
/*********************************************************************************/

#ifndef eepromHAL_h
#define eepromHAL_h

#include <Arduino.h>

#define EEPROM_SIZE 512

#define EEPROMHAL_EXTENDED

class EepromHal  {

  public:

	  virtual void init(void) = 0;

	  virtual void init(int size) = 0;

    /**
     * Read an eeprom cell
     * @param address
     * @return value
     */
    virtual uint8_t read(int address) = 0;

    /**
     * Write value to an eeprom cell
     * @param address
     * @param value
     */
    virtual void write(int address, uint8_t value) = 0;

    /**
     * Update a eeprom cell
     * @param address
     * @param value
     */
    virtual void update(int address, uint8_t value) = 0;

    /**
     * Check whether the eeprom data is valid
     * @return true, if eeprom data is valid (has been written at least once), false if not
     */
    virtual bool isValid() = 0;

    /**
     * Write previously made eeprom changes to the underlying flash storage
     * Use this with care: Each and every commit will harm the flash and reduce it's lifetime (like with every flash memory)
     */
    virtual void commit() = 0;

#if defined (EEPROMHAL_EXTENDED)	
	int readInt(int address);
	int readInt(int address, int *value);
	int writeInt(int address, int value);
	float readFloat(int address);
	int writeFloat(int address, float value);
	void readString(int offset, int bytes, char *buf);
	int writeString(int offset, int bytes, char *buf);
		
	template <class T> int writeAnything(int ee, const T& value);
	template <class T> int readAnything(int ee, T& value);

  int EEPROMAnythingWrite(int pos, char *character, int length);
  int EEPROMAnythingRead(int pos, char *character, int length);
		
	virtual uint8_t 	readByte(int address)	=0;
	virtual int8_t 		readChar(int address)	=0;
	virtual uint8_t 	readUChar(int address)	=0;
	virtual int16_t 	readShort(int address)	=0;
	virtual uint16_t 	readUShort(int address)	=0;
	virtual int32_t 	readIntE(int address)	=0;
	virtual uint32_t 	readUInt(int address)	=0;
	virtual int32_t 	readLong(int address)	=0;
	virtual uint32_t 	readULong(int address)	=0;
	virtual int64_t 	readLong64(int address)	=0;
	virtual uint64_t 	readULong64(int address)	=0;
	virtual float_t 	readFloatE(int address)	=0;
	virtual double_t 	readDouble(int address)	=0;
	virtual bool 			readBool(int address)	=0;
	virtual size_t 		readString(int address, char* value, size_t maxLen)	=0;
	virtual String 		readString(int address)	=0;
	virtual size_t 		readBytes(int address, void * value, size_t maxLen)	=0;

	virtual size_t writeByte(int address, uint8_t value)	=0;
	virtual size_t writeChar(int address, int8_t value)	=0;
	virtual size_t writeUChar(int address, uint8_t value)	=0;
	virtual size_t writeShort(int address, int16_t value)	=0;
	virtual size_t writeUShort(int address, uint16_t value)	=0;
	virtual size_t writeIntE(int address, int32_t value)	=0;
	virtual size_t writeUInt(int address, uint32_t value)	=0;
	virtual size_t writeLong(int address, int32_t value)	=0;
	virtual size_t writeULong(int address, uint32_t value)	=0;
	virtual size_t writeLong64(int address, int64_t value)	=0;
	virtual size_t writeULong64(int address, uint64_t value)	=0;
	virtual size_t writeFloatE(int address, float_t value)	=0;
	virtual size_t writeDouble(int address, double_t value)	=0;
	virtual size_t writeBool(int address, bool value)	=0;
	virtual size_t writeString(int address, const char* value)	=0;
	virtual size_t writeString(int address, String value)	=0;
	virtual size_t writeBytes(int address, const void* value, size_t len)	=0;
		
#endif	// EEPROMHAL_EXTENDED	
};

#if defined(ESP8266)

#elif defined(ESP32)
#include "eepromHAL_ESP32.h"
#elif defined(ARDUINO_ARCH_SAMD)
#include "eepromHAL_M0.h"
#elif defined(_BOARD_GENERIC_STM32F103C_H_)

#elif defined(ARDUINO_AVR_PRO)
#include "eepromHAL_PRO.h"
#else

#endif

#endif
	