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
 *    1.0    22/03/20                                                            *
 *    1.1    27/07/20   Ajout gestion tension                                    *
 *    1.0.1  18/10/20   Renomage de la librairie                                 *
 *                      Ajout ScreenCharge()                                     *
 *    1.0.2  22/10/20   Modif calcul pourcentage                                 *
 *                                                                               *
 *********************************************************************************
 */

#include <HardwareConfig.h>
#include <Arduino.h>

#include "VarioPower.h"

#include <VarioButton.h>
#include "VarioHardwareManager.h"
#include <varioscreenGxEPD.h>

#ifdef POWER_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include <VarioLog.h>

//VarioAlim::VarioAlim(){}

void VarioPower::init()
{
/*****************************/
/*  Init Alimentation        */
/*****************************/
#if defined(HAVE_POWER_ALIM)
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, POWER_PIN_STATE); // turn on POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)
#endif

		varioTension.init();
}

/*****************************/
long VarioPower::getVoltage()
/*****************************/
{
	return(varioTension.getVoltage());
}

/*****************************/
float VarioPower::getTension()
/*****************************/
{
	return (varioTension.getTension());
}

/*****************************/
void VarioPower::setRefVoltage(uint16_t refVoltage)
/*****************************/
{
  varioTension.setRefVoltage(refVoltage);
}

/*****************************/
uint16_t VarioPower::getRefVoltage()
/*****************************/
{
	return(varioTension.getRefVoltage());
}

#define TENSION_100 4.2
#define TENSION_90  4.1
#define TENSION_80  3.97
#define TENSION_70  3.92
#define TENSION_60  3.87
#define TENSION_50  3.83
#define TENSION_40  3.79
#define TENSION_30  3.75
#define TENSION_20  3.70
#define TENSION_10  3.60
#define TENSION_5   3.30
#define TENSION_0   3.0

int VarioPower::getCapacite()
{
  return (varioTension.getCapacite());
}	

/*****************************/
void VarioPower::enableAlim()
/*****************************/
{
  digitalWrite(POWER_PIN, POWER_PIN_STATE); // turn on POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)
}

/*****************************/
void VarioPower::disableAlim()
/*****************************/
{
	digitalWrite(POWER_PIN, !POWER_PIN_STATE); // turn off POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)
}
		
		
/*****************************/
void VarioPower::ScreenCharge() 
/*****************************/
{	
  // reset variables 
#ifdef POWER_DEBUG
  SerialPort.println("Batterie en Charge");
#endif //IMU_DEBUG
  
#ifdef PRO_DEBUG
  SerialPort.println("Loop");
#endif //IMU_DEBUG

	screen.ScreenViewBattery(true);
	unsigned long TmplastDisplayTimestamp = millis();

  while (1) {
//*******************************
//  Compute button             *
//*******************************

#ifdef BUTTON_DEBUG
		SerialPort.println("Lecture boutons");
#endif //BUTTON_DEBUG
		ButtonScheduleur.update();

		delay(1);

	  if ((millis() - TmplastDisplayTimestamp) > 5000) {
			screen.ScreenViewBattery(false);
			TmplastDisplayTimestamp = millis();
		}
	}	
}
	
// extern VarioAlim varioAlim;