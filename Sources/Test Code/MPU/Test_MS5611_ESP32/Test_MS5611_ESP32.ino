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

#include <Wire.h>
#include <MS5611.h>

MS5611 sensor(&Wire);

float temp;
int   pressure;

void setup() {
  Serial.begin(115200);
  if(sensor.connect()>0) {
    Serial.println("Error connecting...");
    delay(500);
    setup();
  }
}

void loop() {
  sensor.ReadProm();
  sensor.Readout();
  temp = sensor.GetTemp();
  Serial.print("Temperature [0.01 C]: ");
  Serial.println(sensor.GetTemp());
  Serial.print("Temperature [C]: ");
  Serial.println(temp * 0.01);
  pressure = sensor.GetPres();
  Serial.print("Pressure [Pa]: ");
  Serial.println(pressure);
  Serial.print("Pressure [mBar]: ");
  Serial.println(pressure/100);
  test_crc();
  Serial.println("---");
  delay(500);
}

void test_crc() {
  sensor.ReadProm();
  sensor.Readout(); 
  Serial.print("CRC=0x");
  Serial.print(sensor.Calc_CRC4(), HEX);
    Serial.print(" (should be 0x");
  Serial.print(sensor.Read_CRC4(), HEX);
  Serial.print(")\n");
  Serial.print("Test Code CRC=0x");
  Serial.print(sensor.CRCcodeTest(), HEX);
  Serial.println(" (should be 0xB)");
}
