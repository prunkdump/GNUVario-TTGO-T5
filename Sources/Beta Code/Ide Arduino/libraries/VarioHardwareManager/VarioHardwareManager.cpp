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
 *    1.0.1  09/06/20   Ajout GnuSettings.BLUETOOTH_SEND_CALIBRATED_ALTITUDE     *
 *    1.0.2  25/12/20   Modif getCap                                             *
 *    1.0.3  11/04/21   Mofig getAlti                                            *
 *                                                                               *
 *********************************************************************************
 */

#include <Arduino.h>

#include <HardwareConfig.h>
#include <DebugConfig.h>

#include <VarioHardwareManager.h>

#include <VarioLog.h>

#ifndef ARDUINOTRACE_ENABLE
#ifdef HARDWARE_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif
#endif

#ifndef ARDUINOTRACE_SERIAL
#define ARDUINOTRACE_SERIAL SerialPort
#endif

#include <ArduinoTrace.h>

#include <VarioData.h>

#include <VarioButton.h>
#include <Utility.h>

#include <VarioLanguage.h>

#include <VarioSettings.h>

VarioHardwareManager varioHardwareManager;

//**********************************
VarioHardwareManager::VarioHardwareManager()
//**********************************
{
#ifndef HAVE_GPS
  lastVarioSentenceTimestamp = 0;
#endif // !HAVE_GPS
}

//**********************************
void VarioHardwareManager::init()
//**********************************
{
#ifndef TWOWIRESCHEDULER
	// Init BUS I2C  
  delay(100);
  Wire.flush();
  Wire.begin (VARIO_SDA_PIN, VARIO_SCL_PIN);
	Wire.setClock(400000); //Increase I2C data rate to 400kHz
  delay (250);
#endif
}

//**********************************
void VarioHardwareManager::initAlim()
//**********************************
{
  varioPower.init();
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
#if defined(HAVE_BLUETOOTH)
  return varioBT.init();
#endif //HAVE_BLUETOOTH
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
  return (varioImu.firstAlti());
}

//***********************************
bool VarioHardwareManager::updateData(void)
//***********************************
{
  return (varioImu.updateData());
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
    deep_sleep(varioLanguage.getText(TITRE_VEILLE)); //"En veille");
  }
}

//***********************************
bool VarioHardwareManager::updateBluetooth(double velocity, double alti, double altiCalibrated)
//***********************************
{
#if defined(HAVE_BLUETOOTH)
  return (varioBT.update(velocity, alti, altiCalibrated));
#endif //HAVE_BLUETOOTH
}

//***********************************
bool VarioHardwareManager::updateGps(Kalmanvert kalmanvert)
//***********************************
{
#if defined(HAVE_GPS)
  boolean lastSentencetmp;
  if (varioGps.update(varioData.kalmanvert, &lastSentencetmp))
  {

#if defined(HAVE_BLUETOOTH)
    varioBT.lastSentence = lastSentencetmp;
    //* if this is the last GPS sentence *
    //* we can send our sentences *
    if (varioBT.lastSentence)
    {
      varioBT.lastSentence = false;
#ifdef VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE
      if (GnuSettings.VARIOMETER_SENT_LXNAV_SENTENCE == LK8000_SENTENCE)
        varioBT.bluetoothNMEA_Lk.begin(kalmanvert.getCalibratedPosition(), kalmanvert.getVelocity());
      else
        varioBT.bluetoothNMEA_Lx.begin(kalmanvert.getCalibratedPosition(), kalmanvert.getVelocity());
#else
      if (GnuSettings.VARIOMETER_SENT_LXNAV_SENTENCE == LK8000_SENTENCE)
        varioBT.bluetoothNMEA_Lk.begin(kalmanvert.getPosition(), kalmanvert.getVelocity());
      else
        varioBT.bluetoothNMEA_Lx.begin(kalmanvert.getPosition(), kalmanvert.getVelocity());
#endif

      serialNmea.lock(); //will be writed at next loop
    }
#endif //HAVE_BLUETOOTH

    return true;
  }
  else
  {
    return false;
  }
#else  //HAVE_GPS
  return false;
#endif //HAVE_GPS
}