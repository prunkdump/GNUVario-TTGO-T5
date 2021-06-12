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
 *                               VarioPOWER                                      *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    14/12/20                                                            *
 *                                                                               *
 *********************************************************************************
 */

#ifndef VARIO_POWER_MAX17XX_H
#define VARIO_POWER_MAX17XX_H

#include <HardwareConfig.h>

#ifdef MAX_17XX

class VarioPowerMAX
{
public:
 //   VarioAlim();
    void init();
		
		long getVoltage(void);
		float getTension(void);
		int getCapacite(void);
    bool getAlert(void);
				
		void setRefVoltage(uint16_t refVoltage);
		uint16_t getRefVoltage(void);
		
private:
		uint16_t REF_VOLTAGE = 2280;
};

#define VarioTension VarioPowerMAX	

#endif
#endif //VARIO_POWER_MAX17XX_H