/* VarioImu -- 
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
 *                           VarioImu                                            *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    22/03/20                                                            *
 *    1.0.1  25/03/20   ajout haveMeasure()																			 *
 *                                                                               *
 *********************************************************************************
 */

#ifndef VARIO_IMU_H
#define VARIO_IMU_H

#include <HardwareConfig.h>
#include <kalmanvert.h>

#ifdef TWOWIRESCHEDULER
#include <VarioImuTwoWire.h>
#else
#include <VarioImuStd.h>
#endif

class VarioImu
{
private:
#ifdef TWOWIRESCHEDULER
    VarioImuTwoWire *imu;
#else
    VarioImuStd *imu;
#endif
    Kalmanvert kalmanVert;

public:
    VarioImu();
    void init();
		double firstAlti();
		bool   updateData();
    double getAlti();
    double getTemp();
    double getAccel();
};

#endif //VARIO_IMU_H