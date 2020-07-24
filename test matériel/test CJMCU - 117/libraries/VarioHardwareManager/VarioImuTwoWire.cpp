/* VarioImuTwoWire -- 
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
 *                          VarioImuTwoWire                                      *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    22/03/20                                                            *
 *    1.0.1  25/03/20   Ajout haveMeasure(void)																	 *
 *                                                                               *
 *********************************************************************************
 */
 
#include <Arduino.h> 
#include <HardwareConfig.h>
#include <DebugConfig.h>
#include <VarioLog.h>

#include "VarioImuTwoWire.h"
#include "VarioData.h"

#ifdef TWOWIRESCHEDULER


#ifdef HAVE_BMP280
Bmp280 TWScheduler::bmp280;
#else
Ms5611 TWScheduler::ms5611;
#endif

#ifdef HAVE_ACCELEROMETER
Vertaccel TWScheduler::vertaccel;
#endif //HAVE_ACCELEROMETER


//**********************************
VarioImuTwoWire::VarioImuTwoWire()
//**********************************
{
}

//**********************************
void VarioImuTwoWire::init()
//**********************************
{
    /**************************/
    /* init Two Wires devices */
    /**************************/
    //!!!
#ifdef HAVE_ACCELEROMETER
    intTW.begin();
    twScheduler.init();
    //  vertaccel.init();

#endif //HAVE_ACCELEROMETER
}

//**********************************
bool VarioImuTwoWire::havePressure(void)
//**********************************
{
	return(twScheduler.havePressure());
}


//**********************************
bool VarioImuTwoWire::updateData(void)
//**********************************
{
#ifdef HAVE_ACCELEROMETER
  if (twScheduler.havePressure() && twScheduler.haveAccel())
  {
		CompteurAccel = 0;
		twScheduler.resetNewAccel();
    twScheduler.getTempAlti(Temp, Alti);
    Temp += GnuSettings.COMPENSATION_TEMP; //MPU_COMP_TEMP;
    Accel = twScheduler.getAccel(NULL);
		
#ifdef DATA_DEBUG
    SerialPort.print("VarioImuTwoWire Update");
    SerialPort.print("Alti : ");
    SerialPort.println(Alti);
    SerialPort.print("Temperature : ");
    SerialPort.println(Temp);
    SerialPort.print("Accel : ");
    SerialPort.println(Accel);
#endif //DATA_DEBUG
				
		return true;
	} 
	else if (twScheduler.haveNewAccel())
	{
		CompteurAccel++;
		twScheduler.resetNewAccel();
		if (CompteurAccel > 100) {
			CompteurAccel = 0;
			twScheduler.resetNewAccel();
			MESSLOG(LOG_TYPE_DEBUG,MS5611_DEBUG_LOG,"ERREUR MPU");
			MESSLOG(LOG_TYPE_DEBUG,MS5611_DEBUG_LOG,"AUCUNE MESURE MS5611");       
		}
	}
#else //HAVE_ACCELEROMETER
	
  if (twScheduler.havePressure())
  {

#ifdef MS5611_DEBUG
//    SerialPort.println("havePressure");
#endif //MS5611_DEBUG

    twScheduler.getTempAlti(Temp, Alti);
    Temp += GnuSettings.COMPENSATION_TEMP; //MPU_COMP_TEMP;
		Accel = 0;
		
#ifdef DATA_DEBUG
    SerialPort.print("Alti : ");
    SerialPort.println(Alti);
    SerialPort.print("Temperature : ");
    SerialPort.println(Temp);
    SerialPort.print("Accel : ");
    SerialPort.println(Accel);
#endif //DATA_DEBUG
		
		return true;
	}
#endif

/*  Temp = 0;
	Alti = 0;
	Accel =0;
	
#ifdef DATA_DEBUG
	SerialPort.println("ERREUR ACQUISITION MS5611/MPU");
	SerialPort.print("Alti : ");
	SerialPort.println(Alti);
	SerialPort.print("Temperature : ");
	SerialPort.println(Temp);
	SerialPort.print("Accel : ");
	SerialPort.println(Accel);
#endif //DATA_DEBUG*/
	
	return false;
}

//**********************************
void VarioImuTwoWire::updateAlti()
//**********************************
{
  Alti = twScheduler.getAlti();
}

//**********************************
double VarioImuTwoWire::getAlti()
//**********************************
{
  return Alti; //twScheduler.getAlti();
}

//**********************************
double VarioImuTwoWire::getTemp()
//**********************************
{
  return Temp; //twScheduler.getAlti();
}

//**********************************
double VarioImuTwoWire::getAccel()
//**********************************
{
  return Accel; //twScheduler.getAlti();
}

#endif // TWOWIRESCHEDULER
