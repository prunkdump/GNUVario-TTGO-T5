/* VarioGps -- 
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
 *    1.0                                                                        *
 *    1.0.1  25/12/20   Modif getCap                                             *
 *                                                                               *
 *********************************************************************************
 */

#ifndef VARIO_GPS_H
#define VARIO_GPS_H

#include <HardwareConfig.h>
#include <DebugConfig.h>

#include <kalmanvert.h>

#ifdef HAVE_GPS

class VarioGps
{
public:
 //   VarioAlim();
    void init();
		bool update(Kalmanvert kalmanvert, bool *lastSentence);
};

#endif //HAVE_GPS
#endif //VARIO_GPS_H