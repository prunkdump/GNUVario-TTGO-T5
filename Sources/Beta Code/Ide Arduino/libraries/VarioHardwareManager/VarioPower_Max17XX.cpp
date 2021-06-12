/* VarioPower -- 
 *
 * Copyright 2020 MichelPa / Jpg63
 * 
 * This file is part of GnuVario-E.
 *
 * ToneHAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ToneHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 *********************************************************************************
 *                                                                               *
 *                           VarioPower                                          *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    14/12/20                                                            *
 *                                                                               *
 *********************************************************************************
 */

#include <HardwareConfig.h>
#include <Arduino.h>

#ifdef MAX_17XX

#include "VarioPower_Max17XX.h"

#include "VarioHardwareManager.h"

#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library

//SFE_MAX1704X lipo; // Defaults to the MAX17043
//SFE_MAX1704X lipo(MAX1704X_MAX17043); // Create a MAX17043
//SFE_MAX1704X lipo(MAX1704X_MAX17044); // Create a MAX17044
SFE_MAX1704X lipo(MAX1704X_MAX17048); // Create a MAX17048
//SFE_MAX1704X lipo(MAX1704X_MAX17049); // Create a MAX17049

#ifdef POWER_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include <VarioLog.h>

//VarioAlim::VarioAlim(){}

void VarioPowerMAX::init()
{
//init gauge 

  // Set up the MAX17043 LiPo fuel gauge:
  if (lipo.begin() == false) // Connect to the MAX17043 using the default wire port
  {
    Serial.println(F("MAX17043 not detected. Please check wiring. Freezing."));
    while (1)
      ;
  }

  // Quick start restarts the MAX17043 in hopes of getting a more accurate
  // guess for the SOC.
  lipo.quickStart();

  // We can set an interrupt to alert when the battery SoC gets too low.
  // We can alert at anywhere between 1% - 32%:
  lipo.setThreshold(20); // Set alert threshold to 20%.  
  lipo.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

}

/*****************************/
long VarioPowerMAX::getVoltage()
/*****************************/
{

  double voltage = lipo.getVoltage();

	float uVoltage = (REF_VOLTAGE / 4.2) * voltage ; 
  return round(uVoltage);
}

/*****************************/
float VarioPowerMAX::getTension()
/*****************************/
{
	//LECTURE TENSION
// lipo.getVoltage() returns a voltage value (e.g. 3.93)
	double voltage = lipo.getVoltage();

	return voltage;
}

/*****************************/
void VarioPowerMAX::setRefVoltage(uint16_t refVoltage)
/*****************************/
{
  REF_VOLTAGE = refVoltage;
}

/*****************************/
uint16_t VarioPowerMAX::getRefVoltage()
/*****************************/
{
	return(REF_VOLTAGE);
}

/*****************************/
int VarioPowerMAX::getCapacite()
/*****************************/
{
  // lipo.getSOC() returns the estimated state of charge (e.g. 79%)
  double soc = lipo.getSOC();

  return round(soc);
}	

/*****************************/
int VarioPowerMAX::getAlert()
/*****************************/
{
  // lipo.getAlert() returns a 0 or 1 (0=alert not triggered)
  bool alert = lipo.getAlert();
}
	
#endif
// extern VarioAlim varioAlim;