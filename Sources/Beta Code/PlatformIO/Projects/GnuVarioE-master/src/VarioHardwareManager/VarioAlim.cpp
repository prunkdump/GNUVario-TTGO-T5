/* VarioAlim -- 
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
 *                           VarioHardwareManager                                *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    22/03/20                                                            *
 *    1.1    27/07/20   Ajout gestion tension                                    *
 *                                                                               *
 *********************************************************************************
 */

#include "VarioAlim.h"
#include <HardwareConfig.h>
#include <Arduino.h>

//VarioAlim::VarioAlim(){}

void VarioAlim::init()
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

long VarioAlim::getVoltage()
{
	long TmpVoltage = 0;
  for (int i = 0; i < 10; i++)
    TmpVoltage += analogRead(VOLTAGE_DIVISOR_PIN);
  TmpVoltage = TmpVoltage / 10;
	return(TmpVoltage);
}

float VarioAlim::getTension()
{
	long voltage = getVoltage();
	float uVoltage = (VOLTAGE_DIVISOR_VALUE * VOLTAGE_DIVISOR_REF_VOLTAGE * float(voltage))  / VOLTAGE_RESOLUTION;
	return uVoltage;
}

int VarioAlim::getCapacite()
{
	long voltage = getVoltage();
	float percentage = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;
  if (voltage > 4.19) percentage = 100;
  else if (voltage <= 3.50) percentage = 0;
	return percentage;
}

void VarioAlim::enableAlim()
{
  digitalWrite(POWER_PIN, POWER_PIN_STATE); // turn on POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)
}

void VarioAlim::disableAlim()
{
	digitalWrite(POWER_PIN, !POWER_PIN_STATE); // turn off POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)
}
		
// extern VarioAlim varioAlim;