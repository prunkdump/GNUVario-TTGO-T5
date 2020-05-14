/* VarioHardwareManager -- 
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
 *                                                                               *
 *********************************************************************************
 */
 
#include <Arduino.h>

#include <HardwareConfig.h>
#include <DebugConfig.h>

#include "VarioHardwareManager.h"

#include <VarioLog.h>

#ifdef HARDWARE_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>


#include "VarioData.h"

#include <VarioButton.h>
#include <Utility.h>



VarioHardwareManager varioHardwareManager;

//**********************************
VarioHardwareManager::VarioHardwareManager()
//**********************************
{
//	varioAlim 		= new VarioAlim();
//	varioSpeaker 	= new VarioSpeaker();
//	varioImu 			= new VarioImu();
//	varioGps 			= new VarioGps();
//	varioBle 			= new VarioBle();
		
#ifndef HAVE_GPS
	lastVarioSentenceTimestamp = 0;
#endif // !HAVE_GPS		
		
}

void VarioHardwareManager::init()
{

}

//**********************************
void VarioHardwareManager::initAlim()
//**********************************
{
	varioAlim.init();
}

//**********************************
void VarioHardwareManager::initSpeaker()
//**********************************
{
  varioSpeaker.init();
}

//**********************************
void VarioHardwareManager::initSound()
//**********************************
{
	varioSpeaker.initSound();
}

//**********************************
void VarioHardwareManager::initImu()
//**********************************
{
	varioImu.init();
}

//**********************************
void VarioHardwareManager::initButton()
//**********************************
{
#ifdef HAVE_BUTTON
#ifdef BUTTON_DEBUG
  SerialPort.println("initialization bouton");
  SerialPort.flush();
#endif //BUTTON_DEBUG

#if defined(ESP32)
  ESP_LOGI("BUTTON", "initialization button");
#endif //EPS32

  VarioButton.begin();
  ButtonScheduleur.Set_StatePage(STATE_PAGE_INIT);
#endif

}

//**********************************
void VarioHardwareManager::initGps()
//**********************************
{
#if defined(HAVE_GPS)
  varioGps.init();
#endif
}

//**********************************
bool VarioHardwareManager::initBt()
//**********************************
{
#ifdef HAVE_BLUETOOTH
  return(varioBle.init());
#endif
}

//**********************************
double VarioHardwareManager::getAlti()
//**********************************
{
  return varioImu.getAlti();
}

//**********************************
double VarioHardwareManager::getTemp()
//**********************************
{
  return varioImu.getTemp();
}

//**********************************
double VarioHardwareManager::getAccel()
//**********************************
{
	return varioImu.getAccel();
}

//***********************************
double VarioHardwareManager::firstAlti(void)
//***********************************
{
  return(varioImu.firstAlti());
}

//***********************************
bool VarioHardwareManager::updateData(void)
//***********************************
{
	return(varioImu.updateData());
}

//***********************************
void VarioHardwareManager::testInactivity(double velocity)
//***********************************
{
    if (abs(velocity) > GnuSettings.SLEEP_THRESHOLD_CPS)
    {
      // reset sleep timeout watchdog if there is significant vertical motion
      sleepTimeoutSecs = millis();
    }
    else if ((GnuSettings.SLEEP_THRESHOLD_CPS != 0) && ((millis() - sleepTimeoutSecs) >= (GnuSettings.SLEEP_TIMEOUT_MINUTES * 60 * 1000)))
    {
#ifdef HARDWARE_DEBUG
      SerialPort.println("Timed out with no significant climb/sink, put MPU9250 and ESP8266 to sleep to minimize current draw");
      SerialPort.flush();
#endif
      indicatePowerDown();
      //     TRACELOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG);
      MESSLOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG, "Deep sleep - inactivite");
      deep_sleep("Power off");
    }
}

//***********************************
bool VarioHardwareManager::updateBle(double velocity, double alti, double altiCalibrated)
//***********************************
{
#ifdef HAVE_BLUETOOTH
	return(varioBle.update(velocity, alti, altiCalibrated));
#endif
}

//***********************************
bool VarioHardwareManager::updateGps(Kalmanvert kalmanvert)
//***********************************
{
#if defined(HAVE_GPS)
	if (varioGps.update(varioData.kalmanvert, &varioBle.lastSentence)) 
	{
	
#ifdef HAVE_BLUETOOTH
	//* if this is the last GPS sentence *
	//* we can send our sentences *
	if (varioBle.lastSentence)
	{
		varioBle.lastSentence = false;
#ifdef VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE
    varioBle.bluetoothNMEA.begin(kalmanvert.getCalibratedPosition(), kalmanvert.getVelocity());
#else
    varioBle.bluetoothNMEA.begin(kalmanvert.getPosition(), kalmanvert.getVelocity());
#endif
    serialNmea.lock(); //will be writed at next loop
  }
#endif //HAVE_BLUETOOTH
  }
#endif //HAVE_GPS
}