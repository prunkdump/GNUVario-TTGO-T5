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

#include <HardwareConfig.h>
#include <DebugConfig.h>

#include "AglManager.h"

#ifdef AGL_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include <VarioLog.h>

#define AGL_Directory "/AGL"

//****************************************************************************************************************************
AglManager::AglManager()
//****************************************************************************************************************************
{
    String tmpStr = AGL_Directory;
    tmpStr += "/";
    hgtReader = new HGTReader(tmpStr);
}

//****************************************************************************************************************************
void AglManager::init(void)
//****************************************************************************************************************************
{
    char tmpFileName[15] = AGL_Directory;

    if (SDHAL_SD.exists(tmpFileName))
        Directory_AGL_Exists = true;
    else
        Directory_AGL_Exists = false;

#ifdef AGL_DEBUG
    SerialPort.print("INIT AGL : Directory exists : ");
    SerialPort.println(Directory_AGL_Exists);
#endif //AGL_DEBUG
}

//****************************************************************************************************************************
void AglManager::setAlti(double dAlti)
//****************************************************************************************************************************
{
    if (currentAlti != dAlti)
    {
        currentAlti = dAlti;
        computeHeight();
    }
}

//****************************************************************************************************************************
void AglManager::setAltiGps(double dAlti)
//****************************************************************************************************************************
{
	currentAltiGps = dAlti;
}

//****************************************************************************************************************************
void AglManager::setLatitude(double dLatitude)
//****************************************************************************************************************************
{
    if (currentLat != dLatitude)
    {
        currentLat = dLatitude;
        computeHeight();
    }
}

//****************************************************************************************************************************
void AglManager::setLongitude(double dLongitude)
//****************************************************************************************************************************
{
    if (currentLong != dLongitude)
    {
        currentLong = dLongitude;
        computeHeight();
    }
}

//****************************************************************************************************************************
void AglManager::computeHeight()
//****************************************************************************************************************************
{
#ifdef AGL_DEBUG
    SerialPort.print("ComputeHeight ");
    SerialPort.print("Alti : ");
    SerialPort.print(currentAlti);
    SerialPort.print(", Lat : ");
    SerialPort.print(currentLat);
    SerialPort.print(", Long :");
    SerialPort.println(currentLong);
#endif //AGL_DEBUG

    if (IsOk())
    {
        if (currentAlti != -1 && currentLat != -1 && currentLong != -1 && currentLat != 0 && currentLong != 0)
        {
            groundLevel = hgtReader->getGroundLevel(degMinToDeg(currentLat), degMinToDeg(currentLong));
            if (groundLevel != NO_FILE_FOR_POS)
            {
                currentHeight = currentAlti - groundLevel;
            }
            else
            {
                currentHeight = currentAlti;
            }
        }
        else
        {
            currentHeight = currentAlti;
						groundLevel = -1;
        }
    }
    else
    {
        currentHeight = currentAlti;
				groundLevel = 1;
    }
#ifdef AGL_DEBUG
    SerialPort.print("AGL Height ");
    SerialPort.println(currentHeight);
#endif //AGL_DEBUG
}

//****************************************************************************************************************************
int AglManager::getGroundLevel()
//****************************************************************************************************************************
{
    groundLevel = -1;

    if (IsOk())
    {
        if (currentAlti != -1 && currentLat != -1 && currentLong != -1 && currentLat != 0 && currentLong != 0)
        {
            groundLevel = hgtReader->getGroundLevel(degMinToDeg(currentLat), degMinToDeg(currentLong));
        }
    }

    return groundLevel;
}

//****************************************************************************************************************************
int AglManager::getAgl()
//****************************************************************************************************************************
{
    if (groundLevel == -1 || currentAltiGps == -1)  return -1;
		else                                            return groundLevel-currentAltiGps;
}

//****************************************************************************************************************************
boolean AglManager::IsOk(void)
//****************************************************************************************************************************
{

#ifdef AGL_DEBUG
    SerialPort.print("AGL Directory exists : ");
    SerialPort.println(Directory_AGL_Exists);
#endif //AGL_DEBUG

    return Directory_AGL_Exists;
}

/**
 \brief Fonction utilitaire : convertit un angle au format degrés.minutes vers degrés.decimales
 \param value : un angle au format degrés.minutes 
 \return un angle en degrés.décimales
*/
//****************************************************************************************************************************
float AglManager::degMinToDeg(float value)
//****************************************************************************************************************************
{
    float r = value;
    int intValue = value;
    float min = value - intValue;
    float decimal = min / 0.6;
    return intValue + decimal;
}
