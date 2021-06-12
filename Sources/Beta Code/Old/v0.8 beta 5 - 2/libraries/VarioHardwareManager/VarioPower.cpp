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

/***********************************/
/*  Init mesure tension batterie   */
/***********************************/
#if defined(HAVE_VOLTAGE_DIVISOR)
    pinMode(VOLTAGE_DIVISOR_PIN, INPUT);
    analogReadResolution(12);

#if defined(VOLTAGE_DIVISOR_DEBUG)
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
#endif
#endif
}

/*****************************/
long VarioPower::getVoltage()
/*****************************/
{
	long TmpVoltage = 0;
  for (int i = 0; i < 10; i++)
    TmpVoltage += analogRead(VOLTAGE_DIVISOR_PIN);
  TmpVoltage = TmpVoltage / 10;
	DUMP(TmpVoltage);
	return(TmpVoltage);
}

/*****************************/
float VarioPower::getTension()
/*****************************/
{
		
	// 2281   = 2.1    
	// 2 * 3.3
	// 3.3 / 4096 = 0.0008056640625
	// 0.0018412976764577
	long voltage = getVoltage();
	
//	float uVoltage = (VOLTAGE_DIVISOR_VALUE * VOLTAGE_DIVISOR_REF_VOLTAGE * float(voltage))  / VOLTAGE_RESOLUTION;
	float uVoltage = (4.2 * float(voltage))  / REF_VOLTAGE; //2280;
	return uVoltage;
}

/*****************************/
void VarioPower::setRefVoltage(uint16_t refVoltage)
/*****************************/
{
	
#ifdef POWER_DEBUG
  SerialPort.print("RefVoltage : ");
	SerialPort.println(refVoltage);
#endif //IMU_DEBUG

	REF_VOLTAGE = refVoltage;
	GnuSettings.REF_VOLTAGE = REF_VOLTAGE;
	
	char tmpchar[20] = "/params.jso";
	GnuSettings.saveConfigurationVario(tmpchar);

#ifdef POWER_DEBUG
  SerialPort.print("REF_VOLTAGE : ");
	SerialPort.println(REF_VOLTAGE);
  SerialPort.print("GnuSettings.REF_VOLTAGE : ");
	SerialPort.println(GnuSettings.REF_VOLTAGE);
#endif //IMU_DEBUG

//	beeper.generateTone(800, 500);
}

/*****************************/
uint16_t VarioPower::getRefVoltage()
/*****************************/
{
	return(REF_VOLTAGE);
}

		uint16_t setRefVoltage(void);

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
/*	long voltage = getVoltage();
	float percentage = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;
  if (voltage > 4.19) percentage = 100;
  else if (voltage <= 3.50) percentage = 0;*/
	
	double tension = getTension();
	int pourcentage ;
	if (tension > TENSION_100)
	{
		pourcentage = 100;
	}
	else if (tension < TENSION_100 && tension > TENSION_90)
	{
		pourcentage = 90 + ( ( tension - TENSION_90 ) * ( 100 - 90 ) / ( TENSION_100 - TENSION_90 ) );
	}
	else if (tension < TENSION_90 && tension > TENSION_80)
	{
		pourcentage = 80 + ( ( tension - TENSION_80 ) * ( 90 - 80 ) / ( TENSION_90 - TENSION_80 ) );
	}
	else if (tension < TENSION_80 && tension > TENSION_70)
	{
		pourcentage = 70 + ( ( tension - TENSION_70 ) * ( 80 - 70 ) / ( TENSION_80 - TENSION_70 ) );
	}
	else if (tension < TENSION_70 && tension  >TENSION_60)
	{
		pourcentage = 60 + ( ( tension - TENSION_60 ) * ( 70 - 60 ) / ( TENSION_70 - TENSION_60 ) );	
	}
	else if (tension < TENSION_60 && tension > TENSION_50)
	{
		pourcentage = 50 + ( ( tension - TENSION_50 ) * ( 60 - 50 ) / ( TENSION_60 - TENSION_50 ) );	
	}
	else if (tension < TENSION_50 && tension > TENSION_40)
	{
		pourcentage = 40 + ( ( tension - TENSION_40 ) * ( 50 - 40 ) / ( TENSION_50 - TENSION_40 ) );	
	}
	else if (tension < TENSION_40 && tension > TENSION_30)
	{
		pourcentage = 30 + ( ( tension - TENSION_30 ) * ( 40 - 30 ) / ( TENSION_40 - TENSION_30 ) );	
	}
	else if (tension < TENSION_30 && tension > TENSION_20)
	{
		pourcentage = 20 + ( ( tension - TENSION_20 ) * ( 30 - 20 ) / ( TENSION_30 - TENSION_20 ) );	
	}
	else if (tension < TENSION_20 && tension > TENSION_10)
	{
		pourcentage = 10 + ( ( tension - TENSION_10 ) * ( 20 - 10 ) / ( TENSION_20 - TENSION_10 ) );
	}
	else if (tension <TENSION_10 && tension > TENSION_5)
	{
		pourcentage = 5 + ( ( tension - TENSION_5 ) * ( 10 - 5 ) / ( TENSION_10 - TENSION_5 ) );	
	}
	else if (tension <TENSION_5 && tension >TENSION_0)
	{
		pourcentage = 0;
	}
	
  return pourcentage;
}	

void VarioPower::enableAlim()
{
  digitalWrite(POWER_PIN, POWER_PIN_STATE); // turn on POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)
}

void VarioPower::disableAlim()
{
	digitalWrite(POWER_PIN, !POWER_PIN_STATE); // turn off POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)
}
		
		
void VarioPower::ScreenCharge() 
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