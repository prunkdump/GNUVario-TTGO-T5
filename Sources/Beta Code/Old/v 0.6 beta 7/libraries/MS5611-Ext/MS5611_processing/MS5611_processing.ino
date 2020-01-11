/*
  MS5611 Barometric Pressure & Temperature Sensor. Output for MS5611_processing.pde
  Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/czujnik-cisnienia-i-temperatury-ms5611.html
  GIT: https://github.com/jarzebski/Arduino-MS5611
  Web: http://www.jarzebski.pl
  (c) 2014 by Korneliusz Jarzebski
 */

#include <Wire.h>
#include <MS5611.h>

MS5611 ms5611;

void setup() 
{
  Serial.begin(115200);

  // Initialize MS5611 sensor
  // Ultra high resolution: MS5611_ULTRA_HIGH_RES
  // (default) High resolution: MS5611_HIGH_RES
  // Standard: MS5611_STANDARD
  // Low power: MS5611_LOW_POWER
  // Ultra low power: MS5611_ULTRA_LOW_POWER
  while(!ms5611.begin(MS5611_ULTRA_HIGH_RES))
  {
    delay(500);
  }
}

void loop()
{
  // Read true temperature & Pressure (without compensation)
  double realTemperature = ms5611.readTemperature();
  long realPressure = ms5611.readPressure();
  double realAltitude = ms5611.getAltitude(realPressure);

  // Read true temperature & Pressure (with compensation)
  double realTemperature2 = ms5611.readTemperature(true);
  long realPressure2 = ms5611.readPressure(true);
  double realAltitude2 = ms5611.getAltitude(realPressure2);

  // Output
  Serial.print(realTemperature);
  Serial.print(":");
  Serial.print(realTemperature2);
  Serial.print(":");
  Serial.print(realPressure);
  Serial.print(":");
  Serial.print(realPressure2);
  Serial.print(":");
  Serial.print(realAltitude);
  Serial.print(":");
  Serial.print(realAltitude2);
  Serial.println();
}

