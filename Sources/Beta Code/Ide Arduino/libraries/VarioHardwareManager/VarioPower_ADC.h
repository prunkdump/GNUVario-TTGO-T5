/* VarioPower_ADC -- 
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
 *                               VarioPOWER_ADC                                  *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    14/12/20                                                            *
 *                                                                               *
 *********************************************************************************
 */

#ifndef VARIO_POWER_ADC_H
#define VARIO_POWER_ADC_H

#include <HardwareConfig.h>

#ifndef MAX_17XX
class VarioPowerADC
{
public:
 //   VarioAlim();
    void init();
		
		long getVoltage(void);
		float getTension(void);
		int getCapacite(void);
				
		void setRefVoltage(uint16_t refVoltage);
		uint16_t getRefVoltage(void);
    bool getAlert(void);
				
private:
		uint16_t REF_VOLTAGE = 2280;

};

#define VarioTension VarioPowerADC	

#endif //MAX_17XX
#endif //VARIO_POWER_ADC_H