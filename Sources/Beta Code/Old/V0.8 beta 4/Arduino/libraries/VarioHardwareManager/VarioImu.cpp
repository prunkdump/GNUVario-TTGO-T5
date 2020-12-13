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

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#include <VarioSettings.h>
#include "VarioImu.h"

VarioImu::VarioImu()
{
#ifdef TWOWIRESCHEDULER
  imu = new VarioImuTwoWire;
#else
  imu = new VarioImuStd;
#endif
}

//*******************************
void VarioImu::init()
//*******************************
{
  this->imu->init();
}

//*******************************
double VarioImu::firstAlti()
{
  //*******************************

  /******************/
  /* get first data */
  /******************/

#ifdef MS5611_DEBUG
  SerialPort.println("Attente premiere mesure alti");
#endif //MS5611_DEBUG

  while (!this->imu->havePressure())
  {
  }

#ifdef MS5611_DEBUG
  SerialPort.println("première mesure");
#endif //MS5611_DEBUG

  this->imu->updateAlti();
  double firstAlti = this->imu->getAlti();

  if (isnan(firstAlti))
  {
    for (int i = 0; i < 4; i++)
    {
      delay(1000);

      this->imu->updateAlti();
      firstAlti = this->imu->getAlti();

      if (!isnan(firstAlti))
        break;
    }
  }

  if (isnan(firstAlti))
  {
    SerialPort.println("Fail firstAlti : ");
    SerialPort.println("reinit");
    ESP_LOGE(TAG, "Erreur Première mesure d'altitude");
    ESP.restart();
  }

#ifdef MS5611_DEBUG
  SerialPort.print("firstAlti : ");
  SerialPort.println(firstAlti);
#endif //MS5611_DEBUG

  return firstAlti;
}

//*******************************
bool VarioImu::updateData()
{
  //*******************************
  return (this->imu->updateData());
}

//*******************************
double VarioImu::getAlti()
{
  //*******************************
  return this->imu->getAlti();
}

//*******************************
double VarioImu::getTemp()
{
  //*******************************
  return this->imu->getTemp();
}

//*******************************
double VarioImu::getAccel()
{
  //*******************************
  return this->imu->getAccel();
}
