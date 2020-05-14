/* AgManager -- 
 *
 * Copyright 2020 JeromeV
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
 *                           AgManager                                           *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    07/03/20                                                            *
 *    1.0.1  09/03/20   Ajout getGroundLevel                                     *
 *    1.0.2  12/03/20   Ajout getAgl                                             *
 *                                                                               *
 *********************************************************************************
 */

#ifndef AGL_MANAGER_H
#define AGL_MANAGER_H

#include "HGTReader.h"

class AglManager
{
public:
    AglManager();
    void setAlti(double dAlti);
    void setAltiGps(double dAlti);
    void setLatitude(double dLatitude);
    void setLongitude(double dLongitude);
    double getHeight() { return currentHeight; }
		boolean IsOk(void); 
		void init(void);
    int getGroundLevel();
		int getAgl();

private:
    void computeHeight();
    static float degMinToDeg(float value);

    HGTReader *hgtReader;
    double currentAlti = -1;
		double currentAltiGps = -1;
    double currentLong = -1;
    double currentLat = -1;
    double currentHeight;
		int groundLevel = -1;
		boolean Directory_AGL_Exists = false;
};

extern   AglManager aglManager;

#endif //AGL_MANAGER_H
