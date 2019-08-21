/*
    MS5xxx.h - Library for accessing MS5xxx sensors via I2C
    Copyright (c) 2012 Roman Schmitz

    This file is part of arduino-ms5xxx.

    arduino-ms5xxx is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    arduino-ms5xxx is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with arduino-ms5xxx.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef MS5xxx_h
#define MS5xxx_h

#include "Arduino.h"
#include <Wire.h>

// typical I2C-Address of chip
#define I2C_MS5607 0x76
#define I2C_MS5611 0x77

// I2C commands of chip
#define MS5xxx_CMD_RESET	0x1E    // perform reset
#define MS5xxx_CMD_ADC_READ 0x00    // initiate read sequence
#define MS5xxx_CMD_ADC_CONV 0x40    // start conversion
#define MS5xxx_CMD_ADC_D1   0x00    // read ADC 1
#define MS5xxx_CMD_ADC_D2   0x10    // read ADC 2
#define MS5xxx_CMD_ADC_256  0x00    // set ADC oversampling ratio to 256
#define MS5xxx_CMD_ADC_512  0x02    // set ADC oversampling ratio to 512
#define MS5xxx_CMD_ADC_1024 0x04    // set ADC oversampling ratio to 1024
#define MS5xxx_CMD_ADC_2048 0x06    // set ADC oversampling ratio to 2048
#define MS5xxx_CMD_ADC_4096 0x08    // set ADC oversampling ratio to 4096
#define MS5xxx_CMD_PROM_RD  0xA0    // initiate readout of PROM registers

#ifdef _ESP32_HAL_I2C_H_
//TTGO-T5
#define SDA_PIN 21 //19
#define SCL_PIN 22 //18
//TTGO-TS
//#define SDA_PIN 19
//#define SCL_PIN 18
#endif

class MS5xxx
{
  protected:
	unsigned int C[8];
	double P;
	double TEMP;
	char i2caddr;
	TwoWire *_Wire;
	
	unsigned char send_cmd(unsigned char aCMD);
	unsigned long read_adc(unsigned char aCMD);
	
  public:
    MS5xxx(TwoWire *aWire);
    void setI2Caddr(char aAddr);
    byte connect();
    
    void ReadProm();
    void Readout();

    unsigned int Calc_CRC4(unsigned char poly=0x30);
    unsigned int Read_CRC4();
    unsigned char CRCcodeTest();

    unsigned int Read_C(unsigned int index);
    
    double GetTemp();
    double GetPres();
};

#endif
