/* VarioImuStd -- 
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
 *                          VarioImuStd                                         *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    22/03/20                                                            *
 *    1.0.1  25/03/20   Ajout haveMeasure(void)																	 *
 *                                                                               *
 *********************************************************************************
 */

#include <HardwareConfig.h>

#ifndef TWOWIRESCHEDULER

#include <MS5611-Ext.h>
#include <Wire.h>
#include <vertaccel2.h>
#include <MPU9250-DMP_SF_EXT.h>

MS5611 ms5611;
double referencePressure;

MPU9250_DMP imu;
Vertaccel vertaccel;

class VarioImuStd
{

public:
    VarioImuStd();
    void 		init();
		bool 		havePressure(void);
		bool   	updateData();
		void    updateAlti();
    double 	getAlti();
    double 	getTemp();
    double 	getAccel();
		
private:
    double 	Alti;
    double 	Temp;
    double 	Accel;
};

#endif