/*********************************************************************************/
/*                                                                               */
/*                        Libraries EEpromHAL_ESP32                              */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    01/04/19                                                            */
/*    1.0.1  02/04/19   Ajout fonction init                                      */
/*    1.0.2  07/05/19   Ajout #include <DebugConfig.h> et EepromHAL EEPROMHAL;   */
/*    1.0.3  09/07/19   Ajout init(int size)                                     */
/*    1.0.4  14/07/19   Correction bug taille memoire                            */
/*                                                                               */
/*********************************************************************************/


#ifndef eepromHAL_ESP32_h
#define eepromHAL_ESP32_h

#include <Arduino.h>
#include "eepromHAL.h"

#if defined(ESP32)

#include <EEPROM.h>

class EepromHal_ESP32 : public EepromHal {

  public:

	  void init(void);

	  void init(int size);
	
	
    /**
     * Read an eeprom cell
     * @param address
     * @return value
     */
    uint8_t read(int address);

    /**
     * Write value to an eeprom cell
     * @param address
     * @param value
     */
    void write(int address, uint8_t value);

    /**
     * Update a eeprom cell
     * @param address
     * @param value
     */
    void update(int address, uint8_t value);

    /**
     * Check whether the eeprom data is valid
     * @return true, if eeprom data is valid (has been written at least once), false if not
     */
    bool isValid();

    /**
     * Write previously made eeprom changes to the underlying flash storage
     * Use this with care: Each and every commit will harm the flash and reduce it's lifetime (like with every flash memory)
     */
    void commit();

#if defined (EEPROMHAL_EXTENDED)	

		uint8_t 	readByte(int address);
		int8_t 		readChar(int address);
		uint8_t 	readUChar(int address);
		int16_t 	readShort(int address);
		uint16_t 	readUShort(int address);
		int32_t 	readIntE(int address);
		uint32_t 	readUInt(int address);
		int32_t 	readLong(int address);
		uint32_t 	readULong(int address);
		int64_t 	readLong64(int address);
		uint64_t 	readULong64(int address);
		float_t 	readFloatE(int address);
		double_t 	readDouble(int address);
		bool 			readBool(int address);
		size_t 		readString(int address, char* value, size_t maxLen);
		String 		readString(int address);
		size_t 		readBytes(int address, void * value, size_t maxLen);
		template <class T> T readAll (int address, T &);

		size_t writeByte(int address, uint8_t value);
		size_t writeChar(int address, int8_t value);
		size_t writeUChar(int address, uint8_t value);
		size_t writeShort(int address, int16_t value);
		size_t writeUShort(int address, uint16_t value);
		size_t writeIntE(int address, int32_t value);
		size_t writeUInt(int address, uint32_t value);
		size_t writeLong(int address, int32_t value);
		size_t writeULong(int address, uint32_t value);
		size_t writeLong64(int address, int64_t value);
		size_t writeULong64(int address, uint64_t value);
		size_t writeFloatE(int address, float_t value);
		size_t writeDouble(int address, double_t value);
		size_t writeBool(int address, bool value);
		size_t writeString(int address, const char* value);
		size_t writeString(int address, String value);
		size_t writeBytes(int address, const void* value, size_t len);
		template <class T> T writeAll (int address, const T &);
#endif

	private:
		
		int Eeprom_Size;
	
};


#define EepromHAL EepromHal_ESP32
extern EepromHAL EEPROMHAL;

#endif //ESP32

#endif
	
	


