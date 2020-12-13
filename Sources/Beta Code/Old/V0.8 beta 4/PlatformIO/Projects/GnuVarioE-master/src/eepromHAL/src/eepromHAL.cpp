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

#include "eepromHAL.h"
#include <Arduino.h>

#if defined (EEPROMHAL_EXTENDED)	

int EepromHal::readInt(int address){
	int value = 0x0000;
	value = value | (read(address) << 8);
	value = value | read(address+1);
  return value;
}

int EepromHal::readInt(int address, int* value){
  *value = readInt(address);
	return address+2;
}

int EepromHal::writeInt(int address, int value){
  write(address, (value >> 8) & 0xFF );
  write(address+1, value & 0xFF);
	return address+2;
}
 
float EepromHal::readFloat(int address){
   union u_tag {
     byte b[4];
     float fval;
   } u;   
   u.b[0] = read(address);
   u.b[1] = read(address+1);
   u.b[2] = read(address+2);
   u.b[3] = read(address+3);
   return u.fval;
}
 
int EepromHal::writeFloat(int address, float value){
   union u_tag {
     byte b[4];
     float fval;
   } u;
   u.fval=value;
 
   write(address  , u.b[0]);
   write(address+1, u.b[1]);
   write(address+2, u.b[2]);
   write(address+3, u.b[3]);
	 return address + 4;
}

void EepromHal::readString(int offset, int bytes, char *buf){
  char c = 0;
  for (int i = offset; i < (offset + bytes); i++) {
    c = read(i);
    buf[i - offset] = c;
    if (c == 0) break;
  }
}

int EepromHal::writeString(int offset, int bytes, char *buf){
  char c = 0;
  //int len = (strlen(buf) < bytes) ? strlen(buf) : bytes;
  for (int i = 0; i < bytes; i++) {
    c = buf[i];
    write(offset + i, c); 
  }
  return offset+bytes+1;
}


template <class T> int EepromHal::writeAnything(int ee, const T& value)
{
   const byte* p = (const byte*)(const void*)&value;
   int i;
   for (i = 0; i < sizeof(value); i++)
       write(ee++, *p++);
   return ee;  //i
}

template <class T> int EepromHal::readAnything(int ee, T& value)
{
   byte* p = (byte*)(void*)&value;
   int i;
   for (i = 0; i < sizeof(value); i++)
       *p++ = read(ee++);
   return ee;  //i;
}

/*double testInt[12] = { -12.5, -10.00, -5.7, 0, 2.45, 2.90, 3.10, 4 , 5.6, 7.9, 5.5, 4};
byte noElem = 12;
unsigned int baseAddr = 0;
unsigned int n = 0;

void setup() {
 Serial.begin(9600);

 // write data to eeprom 
 for (int i=0; i <= noElem-1; i++){
   n = EEPROM_writeAnything( (i*4)+baseAddr, testInt[i]);
 }

 // read data back
 for (int i=0; i <= noElem-1; i++){
   double val;
   int addr = (i*4)+baseAddr;
   n = EEPROM_readAnything( addr, val);
   Serial.println(val);
 }
}

void loop() {
}
*/

// Write any data structure or variable to EEPROM
int EepromHal::EEPROMAnythingWrite(int pos, char *character, int length)
{
#ifdef EEPROM_DEBUG	
	SerialPort.print("length : ");
	SerialPort.println(length);
  SerialPort.println("EEPROMAnythingWrite : ");	
	for (int i = 0; i < length; i++) {
		SerialPort.printf("%d",int(character[i]));	
  }
  SerialPort.println("");
#endif //EEPROM_DEBUG
	
  for (int i = 0; i < length; i++)
  {
	//	char tmpChar = *character;

	  if (read(pos + i) != *character)  {
      write(pos + i, *character);
	  }
    character++;
  }

  commit();
  return pos + length;
}
 
// Read any data structure or variable from EEPROM
int EepromHal::EEPROMAnythingRead(int pos, char *character, int length)
{
#ifdef EEPROM_DEBUG	
  SerialPort.print("EEPROMAnythingRead : ");	
#endif //EEPROM_DEBUG
	
  for (int i = 0; i < length; i++)
  {
		char tmpchar = read(pos + i);
    *character = tmpchar;
#ifdef EEPROM_DEBUG	
		SerialPort.print(tmpchar);	
#endif //EEPROM_DEBUG
		
    character++;
  }
	
#ifdef EEPROM_DEBUG	
		SerialPort.println("");	
#endif //EEPROM_DEBUG
	
  return pos + length;
}

#endif // EEPROMHAL_EXTENDED



