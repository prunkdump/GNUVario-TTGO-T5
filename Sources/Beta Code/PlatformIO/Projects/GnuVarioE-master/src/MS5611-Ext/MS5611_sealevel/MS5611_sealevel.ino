/*
  MS5611 Barometric Pressure & Temperature Sensor. Sea Level
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
  Serial.begin(9600);

  // Initialize MS5611
  Serial.println("Initialize MS5611");

  // Ultra high resolution: MS5611_ULTRA_HIGH_RES
  // (default) High resolution: MS5611_HIGH_RES
  // Standard: MS5611_STANDARD
  // Low power: MS5611_LOW_POWER
  // Ultra low power: MS5611_ULTRA_LOW_POWER
  while(!ms5611.begin(MS5611_ULTRA_HIGH_RES))
  {
    Serial.println("Could not find a valid MS5611, check wiring!");
    delay(500);
  }

  // Check settings
  checkSettings();
}

void checkSettings()
{
  Serial.print("Oversampling: ");
  Serial.println(ms5611.getOversampling());
}

void loop()
{
  // Set you real altitude
  // My location: Poland, Bytom, 8 floor
  double myRealAltitude = 335;

  // Read true temperature & Pressure
  double realTemperature = ms5611.readTemperature();
  double realPressure = ms5611.readPressure();

  // Calculate sealevel pressure
  double seaLevelPressure = ms5611.getSeaLevel(realPressure, myRealAltitude);

  Serial.println("--");

  Serial.print("realTemp = ");
  Serial.print(realTemperature);
  Serial.print(" *C");

  Serial.print(", realPressure = ");
  Serial.print(realPressure/100);
  Serial.print(" hPa");

  Serial.print(" seaLevelPressure = ");
  Serial.print(seaLevelPressure/100);
  Serial.println(" hPa");

  delay(1000);
}
