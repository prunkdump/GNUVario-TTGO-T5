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

#include "MS5xxx.h"

MS5xxx::MS5xxx(TwoWire *aWire) : i2caddr(I2C_MS5607) {
	_Wire=aWire;
}

void MS5xxx::setI2Caddr(char aAddr) {
	i2caddr=aAddr;
}

byte MS5xxx::send_cmd(byte aCMD)
{
  _Wire->beginTransmission(i2caddr);
  _Wire->write(aCMD);
  uint8_t ret=_Wire->endTransmission(true);
  return ret;
}

uint8_t MS5xxx::connect() {
	
#ifdef _ESP32_HAL_I2C_H_
// for esp32
//Wire.begin(19, 18); //sda, scl
	_Wire->begin(SDA_PIN, SCL_PIN); //sda, scl
#else
	_Wire->begin();
#endif
	
//	_Wire->begin(21,22);
	_Wire->beginTransmission(i2caddr);
	uint8_t ret=_Wire->endTransmission(true);
	return ret;
}

void MS5xxx::ReadProm() {
	send_cmd(MS5xxx_CMD_RESET);
	delay(3);
	  
	for(uint8_t i=0;i<8;i++) 
	{
	    C[i]=0x0000;
	    send_cmd(MS5xxx_CMD_PROM_RD+2*i);
	    _Wire->requestFrom(i2caddr, 2);

	    unsigned int c = _Wire->read();
	    C[i] = (c << 8);
	    c = _Wire->read();
	    C[i] += c;
	    _Wire->endTransmission(true);
	}
	
}

unsigned int MS5xxx::Calc_CRC4(unsigned char poly)
{
    int cnt;                   		// simple counter
    unsigned int n_rem;                 // CRC remainder
    unsigned int crc_read;              // original value of the CRC
    unsigned int l_pol = poly;
    unsigned char n_bit;

    l_pol = ( l_pol << 8 ) & 0xf000;	// shift bits and apply mask
    n_rem = 0x0000;

    crc_read = C[ 7 ];                  // save read RCR
    C[ 7 ] = ( 0xFF00 & ( C[ 7 ] ) );   // CRC byte is replaced by 0
    for ( cnt = 0; cnt < 16; cnt++ )    // operation is performed on bytes
    {// choose LSB or MSB
        if ( cnt % 2 == 1 ) n_rem ^= ( unsigned short ) ( ( C[ cnt >> 1 ] ) & 0x00FF );
        else n_rem ^= ( unsigned short ) ( ( C[ cnt >> 1 ] >> 8) & 0x00FF );

        for ( n_bit = 8; n_bit > 0; n_bit-- )
        {
            if ( n_rem & ( 0x8000 ) )
            {
            	n_rem = ( n_rem << 1 ) ^ l_pol;
            }
            else
            {
                n_rem = ( n_rem << 1 );
            }
        }
    }
    C[ 7 ] = crc_read;
    n_rem = (0x000F & (n_rem >> 12)); // final 4-bit remainder is CRC code
    return n_rem;
}

unsigned int MS5xxx::Read_CRC4()
{

    unsigned int crc_read = ( 0x000F & ( C[ 7 ] ) );
    return ( crc_read );
}

unsigned int MS5xxx::Read_C( unsigned int index)
{
    unsigned int retval = 0;
    if ( ( index >= 0) && ( index <= 7 ) )
        retval = C[ index ];
    return retval;
}

unsigned long MS5xxx::read_adc(unsigned char aCMD)
{
  unsigned long value=0;
  unsigned long c=0;
  
  send_cmd(MS5xxx_CMD_ADC_CONV+aCMD); // start DAQ and conversion of ADC data
  switch (aCMD & 0x0f)
  {
    case MS5xxx_CMD_ADC_256 : delayMicroseconds(900);
    break;
    case MS5xxx_CMD_ADC_512 : delay(3);
    break;
    case MS5xxx_CMD_ADC_1024: delay(4);
    break;
    case MS5xxx_CMD_ADC_2048: delay(6);
    break;
    case MS5xxx_CMD_ADC_4096: delay(10);
    break;
  }
  send_cmd(MS5xxx_CMD_ADC_READ); // read out values
  _Wire->requestFrom(i2caddr, 3);
  c = _Wire->read();
  value = (c<<16);
  c = _Wire->read();
  value += (c<<8);
  c = _Wire->read();
  value += c;
  _Wire->endTransmission(true);
 
  return value;
}

void MS5xxx::Readout() {
	unsigned long D1=0, D2=0;
	
	double dT;
	double OFF;
	double SENS;

	D2=read_adc(MS5xxx_CMD_ADC_D2+MS5xxx_CMD_ADC_4096);
	D1=read_adc(MS5xxx_CMD_ADC_D1+MS5xxx_CMD_ADC_4096);

	// calculate 1st order pressure and temperature (MS5607 1st order algorithm)
	dT=D2-C[5]*pow(2,8);
	OFF=C[2]*pow(2,17)+dT*C[4]/pow(2,6);
	SENS=C[1]*pow(2,16)+dT*C[3]/pow(2,7);
	TEMP=(2000+(dT*C[6])/pow(2,23));
	P=(((D1*SENS)/pow(2,21)-OFF)/pow(2,15));
	 
	// perform higher order corrections
	double T2=0., OFF2=0., SENS2=0.;
	if(TEMP<2000) {
	  T2=dT*dT/pow(2,31);
	  OFF2=61*(TEMP-2000)*(TEMP-2000)/pow(2,4);
	  SENS2=2*(TEMP-2000)*(TEMP-2000);
	  if(TEMP<-1500) {
	    OFF2+=15*(TEMP+1500)*(TEMP+1500);
	    SENS2+=8*(TEMP+1500)*(TEMP+1500);
	  }
	}
	  
	TEMP-=T2;
	OFF-=OFF2;
	SENS-=SENS2;
	P=(((D1*SENS)/pow(2,21)-OFF)/pow(2,15));	
}

double MS5xxx::GetTemp() {
	return TEMP;
}

double MS5xxx::GetPres() {
	return P;
}

unsigned char MS5xxx::CRCcodeTest(){
	unsigned int nprom[] = {0x3132,0x3334,0x3536,0x3738,0x3940,0x4142,0x4344,0x4500}; //expected output is 0xB
	for(uint8_t i=0;i<8;i++) {
		C[i] = nprom[i];
	}
	unsigned char crc = Calc_CRC4(); //expected output is 0xB
	ReadProm();
	return crc;
}

