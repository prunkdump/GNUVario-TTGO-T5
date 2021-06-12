/* VarioData -- 
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
 *                           VarioData                                           *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    22/03/20                                                            *
 *    1.0.1  25/03/20   Ajout dataAcquisition(void)                              *
 *        							compteurErrorMPU																				 *
 *    1.0.2  07/04/20   ajout updateBeeper(void)                                 *
 *    1.0.3  18/04/20   Ajour reglage du kalman                                  *
 *                      Ajout intégration vario                                  *
 *    1.0.4  08/05/20   Ajout getCap                                             *
 *    1.0.5  27/06/20   Ajout varioxbeeper                                       *
 *    1.0.6  30/07/20   Ajout gestion valeur maximum des données                 *
 *    1.0.7  27/09/20   Ajout test sur lecture des fichiers de config            *
 *    1.0.8  21/12/20   Mofig updateBeeper                                       *
 *    1.0.9  25/12/20   Modif getCap                                             *
 *    1.0.10 11/04/21   Modif getAlti                                            *
 *    1.0.11 12/04/21   Ajout gestion Mute_VarioBegin                            *
 *                                                                               *
 *********************************************************************************
 */

#include "VarioData.h"
#include <HardwareConfig.h>
#include <DebugConfig.h>

#ifdef DATA_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include "VarioHardwareManager.h"

#include <Utility.h>

#include <sdcardHAL.h>
#include <VarioSettings.h>

#include <VarioLog.h>

#include <SerialNmea.h>

#ifdef HAVE_WIFI
//#include <VarioWifiServer.h>
#endif //HAVE_WIFI

#include <GPSSentence.h>

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
#include <AglManager.h>
#endif //HAVE_SDCARD && HAVE_GPS

#include <varioscreenGxEPD.h>

#include <VarioXBeeper.h>

//******************************************
//   Valeur maximum des données            *
//******************************************

#if (VARIOSCREEN_SIZE == 154)
#define MAX_CLIMRATE 9.9
#else
#define MAX_CLIMRATE 99.9
#endif

#if (VARIOSCREEN_SIZE == 154)
#define MAX_VELOCITY 9.9
#else
#define MAX_VELOCITY 99.9
#endif

#define MAX_ALTI 9999

#if (VARIOSCREEN_SIZE == 154)
#define MAX_SPEED 99
#else
#define MAX_SPEED 99
#endif

#define ALTI_FILTER
#define COEF_ALTI_FILTERED 0.1

/**********************/
/* SDCARD objects     */
/**********************/

int8_t sdcardState = SDCARD_STATE_INITIAL;

VarioSettings GnuSettings;

//****************************************************************************************************************************
VarioData::VarioData()
//****************************************************************************************************************************
{
#ifdef HAVE_GPS
  variometerState = VARIOMETER_STATE_INITIAL;
#else
  variometerState = VARIOMETER_STATE_CALIBRATED;
#endif //HAVE_GPS
}

//****************************************************************************************************************************
void VarioData::init(uint8_t version, uint8_t sub_version, uint8_t beta_code, String devname)
//****************************************************************************************************************************
{

  Version = version;
  Sub_Version = sub_version;
  Beta_Code = beta_code;
  Devname = devname;
#ifdef PROG_DEBUG
  ///  while (!SerialPort) { ;}
  char tmpbuffer[100];
  sprintf(tmpbuffer, "GNUVARIO compiled on %s", __DATE__); // at %s", __DATE__, __TIME__);
  SerialPort.println(tmpbuffer);
  sprintf(tmpbuffer, "VERSION %i.%i", Version, Sub_Version);
  SerialPort.println(tmpbuffer);
  if (Beta_Code > 0)
  {
    SerialPort.print("Beta ");
    SerialPort.println(Beta_Code);
  }
  SerialPort.flush();
#endif //PRO_DEBBUG

#if defined(ESP32)
  if (Beta_Code > 0)
  {
    ESP_LOGI("GNUVARIO", "GnuVario-E version %d.%d Beta %d.", Version, Sub_Version, Beta_Code);
  }
  else
  {
    ESP_LOGI("GNUVARIO", "GnuVario-E version %d.%d.", Version, Sub_Version);
    //  ESP_LOGE(TAG, "Failed to initialize the card (%d). Make sure SD card lines have pull-up resistors in place.", ret);
  }
#endif //ESP32
}

//*******************************************
void VarioData::initKalman(double firstAlti)
//*******************************************
{
  DUMP(firstAlti);
  kalmanvert.init(firstAlti,
                  0.0,
                  POSITION_MEASURE_STANDARD_DEVIATION,
                  GnuSettings.ACCELERATION_MEASURE_STANDARD_DEVIATION,
                  millis());

#ifdef DATA_DEBUG
  SerialPort.println("kalman init");
#endif //KALMAN_DEBUG

  TRACE();
  SDUMP("Test INT MPU");
}

//*******************************************
bool VarioData::initSettings(bool Test_SD)
//*******************************************
{
#ifdef HAVE_SDCARD
#ifdef SDCARD_DEBUG
  SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
#endif

  if (GnuSettings.initSettings(Test_SD))
  {

#ifdef SDCARD_DEBUG
    SerialPort.println("initialization done.");
    SerialPort.flush();
#endif //SDCARD_DEBUG

#if defined(ESP32)
    ESP_LOGI("SDCARD", "initialization done.");
#endif //EPS32

#ifdef HAVE_WIFI
    SD_present = true;
#endif //HAVE_WIFI
    sdcardState = SDCARD_STATE_INITIALIZED;
    /*    char FileName[15] = "SETTINGS.TXT";
    GnuSettings.readSDSettings(FileName);*/

    GnuSettings.setVersion(Version, Sub_Version, Beta_Code);

    SerialPort.println("Chargement des parametres depuis le fichier params.jso");
    char tmpchar[20] = "/params.jso";
    GnuSettings.loadConfigurationVario(tmpchar);

#ifdef SDCARD_DEBUG
    //Debuuging Printing
    SerialPort.print("Pilot Name = ");
    SerialPort.println(GnuSettings.VARIOMETER_PILOT_NAME);
#endif //SDCARD_DEBUG

#ifdef PROG_DEBUG
    //Debuuging Printing
    SerialPort.print("Pilot Name = ");
    SerialPort.println(GnuSettings.VARIOMETER_PILOT_NAME);
#endif //PROG_DEBUG

    char __dataPilotName[GnuSettings.VARIOMETER_PILOT_NAME.length()];
    GnuSettings.VARIOMETER_PILOT_NAME.toCharArray(__dataPilotName, sizeof(__dataPilotName) + 1);

#ifdef PROG_DEBUG
    //Debuuging Printing
    SerialPort.print("__dataPilotName = ");
    SerialPort.print(__dataPilotName);
    SerialPort.print(" - ");
    int tmpint = sizeof(__dataPilotName);
    SerialPort.print(tmpint);
    SerialPort.print(" / ");
    SerialPort.print(GnuSettings.VARIOMETER_PILOT_NAME);
    SerialPort.print(" - ");
    SerialPort.println(GnuSettings.VARIOMETER_PILOT_NAME.length());
#endif //PROG_DEBUG

    char __dataGliderName[GnuSettings.VARIOMETER_GLIDER_NAME.length()];
    GnuSettings.VARIOMETER_GLIDER_NAME.toCharArray(__dataGliderName, sizeof(__dataGliderName) + 1);

#ifdef PROG_DEBUG
    //Debuuging Printing
    SerialPort.print("__dataGliderName = ");
    SerialPort.print(__dataGliderName);
    SerialPort.print(" - ");
    tmpint = sizeof(__dataGliderName);
    SerialPort.print(tmpint);
    SerialPort.print(" / ");
    SerialPort.print(GnuSettings.VARIOMETER_GLIDER_NAME);
    SerialPort.print(" - ");
    SerialPort.println(GnuSettings.VARIOMETER_GLIDER_NAME.length());
#endif //PROG_DEBUG

    header.saveParams(VARIOMETER_MODEL_NAME, __dataPilotName, __dataGliderName);
    varioHardwareManager.varioPower.setRefVoltage(GnuSettings.REF_VOLTAGE);

    boolean ModifValue = false;
    char tmpFileName[15] = "/wifi.cfg";

#ifdef SDCARD_DEBUG
    SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
#endif

    if (SDHAL_SD.exists(tmpFileName))
    {
      GnuSettings.readSDSettings(tmpFileName, &ModifValue);

      SerialPort.println("");
      SerialPort.println("Lecture du fichier wifi.cfg");

      SerialPort.print("Wifi SSID 1 : ");
      SerialPort.println(GnuSettings.VARIOMETER_SSID_1);

      SerialPort.print("Wifi Password 1 : ");
      SerialPort.println(GnuSettings.VARIOMETER_PASSWORD_1);

      SerialPort.print("Wifi SSID 2 : ");
      SerialPort.println(GnuSettings.VARIOMETER_SSID_2);

      SerialPort.print("Wifi Password 2 : ");
      SerialPort.println(GnuSettings.VARIOMETER_PASSWORD_2);

      SerialPort.print("Wifi SSID 3 : ");
      SerialPort.println(GnuSettings.VARIOMETER_SSID_3);

      SerialPort.print("Wifi Password 3 : ");
      SerialPort.println(GnuSettings.VARIOMETER_PASSWORD_3);

      SerialPort.print("Wifi SSID 4 : ");
      SerialPort.println(GnuSettings.VARIOMETER_SSID_4);

      SerialPort.print("Wifi Password 4 : ");
      SerialPort.println(GnuSettings.VARIOMETER_PASSWORD_4);
    }

    //lecture parametre de configuration

#ifdef SDCARD_DEBUG
    SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
#endif

    strcpy(tmpFileName, "/variocal.cfg");

    if (SDHAL_SD.exists(tmpFileName))
    {
      if (!GnuSettings.readSDSettings(tmpFileName, &ModifValue))
        return false;
    }

    //lecture parametre de configuration du son

    strcpy(tmpFileName, "/variosound.cfg");

    if (SDHAL_SD.exists(tmpFileName))
    {
      if (!varioXBeeper.readSDSettings(tmpFileName))
        return false;
      varioXBeeper.majVarioSettings(&ModifValue);
      //			varioXBeeper.getFrequence(-1.5);
    }

#ifdef SDCARD_DEBUG
    SerialPort.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
    SerialPort.println("initialization OK");
#endif

    return true;
  }
  else
  {
#ifdef HAVE_WIFI
    SD_present = false;
#endif //HAVE_WIFI

#ifdef SDCARD_DEBUG
    SerialPort.println("initialization failed!");
#endif //SDCARD_DEBUG

#if defined(ESP32)
    ESP_LOGE("SDCARD", "initialization failed!");
#endif //EPS32

#ifdef HAVE_SPEAKER
    if (GnuSettings.ALARM_SDCARD)
    {
      indicateFaultSDCARD();
    }
#endif //HAVE_SPEAKER
    return false;
  }
#else //HAVE_SDCARD
#ifdef HAVE_WIFI
  SD_present = false;
#endif //HAVE_WIFI

#ifdef SDCARD_DEBUG
  SerialPort.println("initialization failed!");
#endif //SDCARD_DEBUG

#if defined(ESP32)
  ESP_LOGE("SDCARD", "initialization failed!");
#endif //EPS32

#ifdef HAVE_SPEAKER
  if (GnuSettings.ALARM_SDCARD)
  {
    indicateFaultSDCARD();
  }
  return false;
#endif //HAVE_SPEAKER
#endif //HAVE_SDCARD
}

//*******************************************
bool VarioData::initLog(void)
//*******************************************
{
  varioLog.init();
  String tmpStr;
  tmpStr = "FirmWare : " + String(Version) + "." + String(Sub_Version);
  if (Beta_Code > 0)
    tmpStr = tmpStr + "b" + String(Beta_Code);

  INFOLOG(tmpStr);
  TRACELOG(LOG_TYPE_DEBUG, MAIN_DEBUG_LOG);

  return true;
}

//***********************************************
// INIT AGL
//***********************************************
//*******************************************
bool VarioData::initAGL(void)
//*******************************************
{
  return (aglManager.init());
}

//***********************************************
// INIT TIME
//***********************************************
//*******************************************
void VarioData::initTime(void)
//*******************************************
{
  lastDisplayTimestamp = millis();
  lastDisplayTimestamp2 = millis();
  varioHardwareManager.time_deep_sleep = lastDisplayTimestamp;
  varioHardwareManager.sleepTimeoutSecs = lastDisplayTimestamp;
  displayLowUpdateState = true;
  displayUpdateState = true;
}

//**********************************************************
//  ACQUISITION DES DONNEES
//**********************************************************

//*******************************************
void VarioData::update(void)
//*******************************************
{

  if (varioHardwareManager.updateData())
  {
    compteurErrorMPU = millis();

    alti = varioHardwareManager.getAlti();
    if (altiFiltered != 0)
    {
      altiFiltered = altiFiltered + COEF_ALTI_FILTERED * (alti - altiFiltered);
    }
    else
    {
      altiFiltered = alti; // first reading so set filtered to reading
    }

    temperature = varioHardwareManager.getTemp();
    accel = varioHardwareManager.getAccel();

#ifdef DATA_DEBUG
    SerialPort.println("Kalman Update");
#endif //PROG_DEBUG

    unsigned long myTime = millis();
#ifdef ALTI_FILTER

    kalmanvert.update(altiFiltered, accel, myTime);
#else
    kalmanvert.update(alti, accel, myTime);
#endif

    velocity = kalmanvert.getVelocity();
    calibratedAlti = kalmanvert.getCalibratedPosition();

    if (calibratedAlti < 0)
      calibratedAlti = 0;

#ifdef DATA_DEBUG
    SerialPort.println("VarioData Update");
    SerialPort.print("Alti : ");
    SerialPort.println(alti);
    SerialPort.print("Temperature : ");
    SerialPort.println(temperature);
    SerialPort.print("Accel : ");
    SerialPort.println(accel);
    SerialPort.print("Velocity : ");
    SerialPort.println(velocity);
    SerialPort.print("Calibrated alti : ");
    SerialPort.println(calibratedAlti);
#endif //DATA_DEBUG

    //**********************************************************
    //  UPDATE BEEPER
    //**********************************************************

    /*#ifdef HAVE_SPEAKER
    beeper.setVelocity(velocity);
#endif //HAVE_SPEAKER*/

    //**********************************************************
    //  TRAITEMENT DES DONNEES
    //**********************************************************

    /* set history */
    if ((GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE) || (GnuSettings.RATIO_CLIMB_RATE > 1))
      history.setAlti(calibratedAlti, millis());

    if ((GnuSettings.VARIOMETER_INTEGRATED_CLIMB_RATE) || (GnuSettings.RATIO_CLIMB_RATE > 1))
      buzzerHistory.setAlti(calibratedAlti, millis());

#ifdef DATA_DEBUG
    SerialPort.print("Baro Alti : ");
    SerialPort.println(alti);
    SerialPort.print("Temperature : ");
    SerialPort.println(temperature);
    SerialPort.print("accelerateur : ");
    SerialPort.println(accel);
    SerialPort.print("Kalman Alti : ");
    SerialPort.println(calibratedAlti);
    SerialPort.print("Kalman Vario : ");
    SerialPort.println(velocity);
#endif //DATA_DEBUG

    /* set screen */

    //**********************************************************
    //  MAJ STATISTIQUE
    //**********************************************************

    flystat.SetAlti(calibratedAlti);
    flystat.SetVario(velocity);

    if (displayLowUpdateState)
    {
#ifdef AGL_MANAGER_H
      aglManager.setAlti(calibratedAlti);
#endif
    }

    if (history.haveNewClimbRate())
    {
      haveNewClimbRateData = true;
      climbRate = history.getClimbRate(GnuSettings.SETTINGS_CLIMB_PERIOD_COUNT);

      //**********************************************************
      //  DISPLAY FINESSE / TAUX DE CHUTE MOYEN
      //**********************************************************

      trend = history.getClimbRate(GnuSettings.SETTINGS_CLIMB_PERIOD_COUNT);

#ifdef DATA_DEBUG
      SerialPort.print("Trend value : ");
      SerialPort.println(trend);
#endif //DATA_DEBUG

      if (trend == 0)
        stateTrend = 0;
      else if (trend > 0)
        stateTrend = 1;
      else
        stateTrend = -1;

      trend = abs(trend);
      if (trend > 10)
        trend = 9.9;
    }
    else
    {
      haveNewClimbRateData = false;
      climbRate = 0;
      trend = 0;
      stateTrend = 0;
    }

    if (buzzerHistory.haveNewClimbRate())
    {
      haveNewClimbRateDataBuzzer = true;
      climbRateBuzzer = buzzerHistory.getClimbRate(GnuSettings.SETTINGS_VARIO_PERIOD_COUNT);
    }
    else
    {
      haveNewClimbRateDataBuzzer = false;
      climbRateBuzzer = 0;
    }

#ifdef HAVE_SPEAKER
    updateBeeper();
#endif //HAVE_SPEAKER
  }
  else
  {

    /**************************************************************/
    /*   ERREUR BAROMETRE / MPU                                   */
    /**************************************************************/

    if (millis() - compteurErrorMPU > 2000)
    {
      SerialPort.println("ERREUR ERREUR BARO / ACCELEROMETRE");

      //**********************************************************
      //  DISABLE BEEPER
      //**********************************************************

#ifdef HAVE_SPEAKER
      beeper.setVelocity(0);
#endif //HAVE_SPEAKER

      alti = 0;
      temperature = 0;
      accel = 0;
      velocity = 0;
      calibratedAlti = 0;
      haveNewClimbRateData = false;
      climbRate = 0;
      trend = 0;
      stateTrend = 0;

      if (displayLowUpdateState)
      {
#ifdef AGL_MANAGER_H
        aglManager.setAlti(0);
#endif

        double tmpAlti, tmpTemp, tmpAccel;

        MESSLOG(LOG_TYPE_DEBUG, MS5611_DEBUG_LOG, "ERREUR MPU");
        if (twScheduler.havePressure())
        {
          twScheduler.getTempAlti(tmpTemp, tmpAlti);
          DUMPLOG(LOG_TYPE_DEBUG, MS5611_DEBUG_LOG, tmpAlti);
        }
        else
        {
          MESSLOG(LOG_TYPE_DEBUG, MS5611_DEBUG_LOG, "AUCUNE MESURE MS5611");
        }

        if (twScheduler.haveAccel())
        {
          tmpAccel = twScheduler.getAccel(NULL);
          DUMPLOG(LOG_TYPE_DEBUG, MS5611_DEBUG_LOG, tmpAccel);
        }
        else
        {
          MESSLOG(LOG_TYPE_DEBUG, MS5611_DEBUG_LOG, "AUCUNE MESURE MPU");
        }
      }
    }
  }
}

//*******************************************
void VarioData::updateBeeper(void)
//*******************************************
{
  if (GnuSettings.VARIOMETER_INTEGRATED_CLIMB_RATE)
  {
    if (haveNewClimbRateDataBuzzer)
      beeper.setVelocity(climbRateBuzzer);
    else
      beeper.setVelocity(0);
  }
  else
  {
    beeper.setVelocity(velocity);
  }
}

//*******************************************
double VarioData::getVelocity()
//*******************************************
{
  double tmpvalue = velocity;
  if (tmpvalue > MAX_VELOCITY)
    tmpvalue = MAX_VELOCITY;
  if (tmpvalue < -MAX_VELOCITY)
    tmpvalue = -MAX_VELOCITY;

  return tmpvalue;
}

//*******************************************
double VarioData::getCalibratedAlti()
{
  //*******************************************
  //	return calibratedAlti;
  double tmpvalue = calibratedAlti;
  if (tmpvalue > MAX_ALTI)
    tmpvalue = MAX_ALTI;
  if (tmpvalue < -MAX_ALTI)
    tmpvalue = -MAX_ALTI;

  return tmpvalue;
}

//*******************************************
double VarioData::getCurrentHeight()
//*******************************************
{
  double tmpvalue = currentHeight;
  if (tmpvalue > MAX_ALTI)
    tmpvalue = MAX_ALTI;
  if (tmpvalue < -MAX_ALTI)
    tmpvalue = -MAX_ALTI;

  return tmpvalue;
}

//*******************************************
void VarioData::setCurrentHeight(double height)
//*******************************************
{
  currentHeight = height;
}

//*******************************************
void VarioData::setGpsAlti(double gpsalti)
//*******************************************
{
  gpsAlti = gpsalti;
}

//*******************************************
double VarioData::getGpsAlti()
//*******************************************
{
  double tmpvalue = gpsAlti;
  if (tmpvalue > MAX_ALTI)
    tmpvalue = MAX_ALTI;
  if (tmpvalue < -MAX_ALTI)
    tmpvalue = -MAX_ALTI;

  return tmpvalue;
}

//*******************************************
double VarioData::getClimbRate()
{
  //*******************************************

  double tmpvalue = climbRate;
  if (tmpvalue > MAX_CLIMRATE)
    tmpvalue = MAX_CLIMRATE;
  if (tmpvalue < -MAX_CLIMRATE)
    tmpvalue = -MAX_CLIMRATE;

  return tmpvalue;
}

//*******************************************
bool VarioData::haveNewClimbRate()
{
  //*******************************************
  return haveNewClimbRateData;
}

//*******************************************
double VarioData::getTrend()
{
  //*******************************************
  return trend;
}

//*******************************************
int VarioData::getStateTrend()
{
  //*******************************************
  return stateTrend;
}

//*******************************************
uint8_t VarioData::getVariometerState()
{
  //*******************************************
  return variometerState;
}

//*******************************************
bool VarioData::updateBluetooth()
{
  //*******************************************
  return (varioHardwareManager.updateBluetooth(kalmanvert.getVelocity(), kalmanvert.getPosition(), kalmanvert.getCalibratedPosition()));
}

//*******************************************
void VarioData::updateGps()
{
  //*******************************************
  varioHardwareManager.updateGps(kalmanvert);
}

//*******************************************
void VarioData::updateState()
{
//*******************************************
#ifdef HAVE_GPS

  /***************************/
  /* update variometer state */
  /*    (after parsing)      */
  /***************************/
  if (variometerState < VARIOMETER_STATE_FLIGHT_STARTED)
  {

    /* if initial state check if date is recorded  */
    if (variometerState == VARIOMETER_STATE_INITIAL)
    {
      if (nmeaParser.haveDate())
      {

#ifdef GPS_DEBUG
        SerialPort.println("VARIOMETER_STATE_DATE_RECORDED");
#endif //GPS_DEBUG

        variometerState = VARIOMETER_STATE_DATE_RECORDED;
      }
    }

    /* check if we need to calibrate the altimeter */
    else if (variometerState == VARIOMETER_STATE_DATE_RECORDED)
    {

#ifdef GPS_DEBUG
      SerialPort.print("NmeaParser Precision : ");
      SerialPort.println(nmeaParser.precision);
      SerialPort.print("VARIOMETER_GPS_ALTI_CALIBRATION_PRECISION_THRESHOLD : ");
      SerialPort.println(VARIOMETER_GPS_ALTI_CALIBRATION_PRECISION_THRESHOLD);
#endif //GPS_DEBUG

      /* we need a good quality value */
      if (nmeaParser.haveNewAltiValue() && (nmeaParser.precision < VARIOMETER_GPS_ALTI_CALIBRATION_PRECISION_THRESHOLD))
      {

        compteurGpsFix++;
        double tmpGpsAlti = nmeaParser.getAlti();
        aglManager.setAltiGps(tmpGpsAlti);

        //         DUMPLOG(LOG_TYPE_DEBUG,GPS_DEBUG_LOG,tmpGpsAlti);

        //Moyenne alti gps
        /*				if (compteurGpsFix > 5)
					gpsAlti = (gpsAlti + tmpGpsAlti) / 2;
				else
					gpsAlti = tmpGpsAlti;*/

        gpsAlti = tmpGpsAlti;

#ifdef GPS_DEBUG
        SerialPort.print("CompteurGpsFix : ");
        SerialPort.println(compteurGpsFix);
#endif //GPS_DEBUG

        gpsFix = 1;
        /*#ifdef HAVE_SCREEN
        screen.recordIndicator->setActifGPSFIX();
          //  recordIndicator->stateRECORD();
#endif //HAVE_SCREEN*/

#if defined(DATA_DEBUG) || defined(GPS_DEBUG)
        SerialPort.print("Gps Alti : ");
        SerialPort.println(gpsAlti);
#endif //DATA_DEBUG

        if (compteurGpsFix > NB_ACQUISITION_FIX_GPS)
        {
#ifdef GPS_DEBUG
          SerialPort.println("GPS FIX");
#endif //GPS_DEBUG

          /* calibrate */
#ifdef HAVE_SPEAKER
          if (GnuSettings.ALARM_GPSFIX)
          {
#ifdef HAVE_SPEAKER
            if (GnuSettings.MUTE_VARIOBEGIN)
            {
              varioHardwareManager.varioSpeaker.UnMute();
            }
#endif //HAVE_SPEAKER

            //           toneAC(BEEP_FREQ);
            beeper.generateTone(GnuSettings.BEEP_FREQ, 200);
            //            delay(200);
            //            toneAC(0);

#ifdef HAVE_SPEAKER
            if (GnuSettings.MUTE_VARIOBEGIN)
            {
              varioHardwareManager.varioSpeaker.Mute();
            }
#endif //HAVE_SPEAKER
          }
#endif //defined(HAVE_SPEAKER)

          gpsFix = 2;
          /*#ifdef HAVE_SCREEN
            screen.fixgpsinfo->setFixGps();
            screen.recordIndicator->setActifGPSFIX();
            //  recordIndicator->stateRECORD();
#endif //HAVE_SCREEN*/

          kalmanvert.calibratePosition(gpsAlti + GnuSettings.COMPENSATION_GPSALTI);
          /*            if (currentHeight == 0) kalmanvert.calibratePosition(gpsAlti+GnuSettings.COMPENSATION_GPSALTI);
            else                    kalmanvert.calibratePosition(gpsAlti+currentHeight);*/

#ifdef DATA_DEBUG
          SerialPort.print("Gps Alti : ");
          SerialPort.println(gpsAlti);
#endif //DATA_DEBUG

#ifdef GPS_DEBUG
          SerialPort.print("GpsAlti : ");
          SerialPort.println(gpsAlti);
          SerialPort.print("Compensation : ");
          SerialPort.println(GnuSettings.COMPENSATION_GPSALTI);
          SerialPort.println("Kalman CalibratePosition : ");
          SerialPort.println(gpsAlti + GnuSettings.COMPENSATION_GPSALTI);
#endif //GPS_DEBUG

#if defined(HAVE_GPS)
          if (GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)
            history.init(varioData.gpsAlti, millis());
#endif //defined(HAVE_GPS)

          if (GnuSettings.VARIOMETER_INTEGRATED_CLIMB_RATE)
            buzzerHistory.init(varioData.gpsAlti, millis());

          variometerState = VARIOMETER_STATE_CALIBRATED;

#ifdef GPS_DEBUG
          SerialPort.println("GPS Calibrated");
#endif //GPS_DEBUG

#ifdef HAVE_SDCARD
          if (!GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START)
          {

#ifdef SDCARD_DEBUG
            SerialPort.println("createSDCardTrackFile when GPS Fix");
#endif //SDCARD_DEBUG

            createSDCardTrackFile();
          }
#endif //HAVE_SDCARD
        }
      }
    }

    /* else check if the flight have started */
    else if (variometerState == VARIOMETER_STATE_CALIBRATED)
    { //variometerState == VARIOMETER_STATE_CALIBRATED

      /* check flight start condition */

      DUMP(getVelocity());
      DUMP(GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD);
      DUMP(GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD);

      if (millis() > GnuSettings.FLIGHT_START_MIN_TIMESTAMP)
      {
        if (!GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START)
        {
          DUMP(GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START);
          enableflightStartComponents();
        }
        else
        {
          if (((kalmanvert.getVelocity() < GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD) || (kalmanvert.getVelocity() > GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD)) &&
              (!CompteurStartFlyEnable))
          {
            //pré-declenchement du début du vol
            CompteurStartFlyEnable = true;
            TimeStartFly = millis();
            CompteurStartFly = 0;
          }

          if (CompteurStartFlyEnable)
          {
            if (nmeaParser.getSpeed() < GnuSettings.FLIGHT_START_MIN_SPEED)
            {
              // si la vitesse n'est pas atteinte
              CompteurStartFly++;

              if (CompteurStartFly > 10)
                CompteurStartFlyEnable = false;
            }
            else
            {
              CompteurStartFly = 0;
              if ((millis() - TimeStartFly) > 3000)
              {
                //          variometerState = VARIOMETER_STATE_FLIGHT_STARTED;
                TRACE();
                enableflightStartComponents();
              }
            }
          }
        }
      }
    }
#else  // HAVE_GPS
  // * if no GPS, we can't calibrate, and we have juste to check flight start *
  if (variometerState == VARIOMETER_STATE_CALIBRATED)
  { //already calibrated at start
    /*    if( (millis() > GnuSettings.FLIGHT_START_MIN_TIMESTAMP) &&
					(kalmanvert.getVelocity() < GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD || kalmanvert.getVelocity() > GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD) ) {
				variometerState = VARIOMETER_STATE_FLIGHT_STARTED;
				enableflightStartComponents();*/

    if ((millis() > GnuSettings.FLIGHT_START_MIN_TIMESTAMP) &&
        (((GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START) &&
          ((kalmanvert.getVelocity() < GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD) || (kalmanvert.getVelocity() > GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD))) ||
         (!GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START)))
    {
      //        variometerState = VARIOMETER_STATE_FLIGHT_STARTED;
      enableflightStartComponents();
    }
  }
#endif //HAVE_GPS
  }
}

/**************************************************/
#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
void VarioData::createSDCardTrackFile(void)
{
  /**************************************************/
  /* start the sdcard record */

#ifdef SDCARD_DEBUG
  SerialPort.println("createSDCardTrackFile : begin ");
#endif //SDCARD_DEBUG

  if (sdcardState == SDCARD_STATE_INITIALIZED)
  {

#ifdef SDCARD_DEBUG
    SerialPort.println("createSDCardTrackFile : SDCARD_STATE_INITIALIZED ");
#endif //SDCARD_DEBUG

    flystat.Begin();
    uint8_t dateN[3];
    igcSD.CreateIgcFile(dateN, GnuSettings.NO_RECORD);

#ifdef SDCARD_DEBUG
    SerialPort.print("DateNum Gnuvario-E.ino : ");
#endif //SDCARD_DEBUG

    for (uint8_t i = 0; i < 3; i++)
    {
#ifdef SDCARD_DEBUG
      SerialPort.print(dateN[i]);
      SerialPort.print(" - ");
#endif //SDCARD_DEBUG
    }

#ifdef SDCARD_DEBUG
    SerialPort.println("");
#endif //SDCARD_DEBUG

    flystat.SetDate(dateN);
  }
}
#endif //defined(HAVE_SDCARD) && defined(HAVE_GPS)

/*******************************************/
void VarioData::enableflightStartComponents(void)
{
  /*******************************************/

#ifdef PROG_DEBUG
  SerialPort.println("enableflightStartComponents ");
#endif //SDCARD_DEBUG

  variometerState = VARIOMETER_STATE_FLIGHT_STARTED;

#ifdef HAVE_SPEAKER
  if (GnuSettings.MUTE_VARIOBEGIN)
  {
    varioHardwareManager.varioSpeaker.UnMute();
  }
#endif //HAVE_SPEAKER

  if (!GnuSettings.NO_RECORD)
  {

#ifdef HAVE_SPEAKER
    if (GnuSettings.ALARM_FLYBEGIN)
    {
      for (int i = 0; i < 2; i++)
      {
        //     toneAC(BEEP_FREQ);
        //     delay(200);
        //    toneAC(0);
        beeper.generateTone(GnuSettings.BEEP_FREQ, 200);
        delay(200);
      }
    }
#endif //HAVE_SPEAKER
  }

  /* set base time */
#if defined(HAVE_SCREEN) && defined(HAVE_GPS)
#ifdef PROG_DEBUG
  SerialPort.println("screenElapsedTime");
#endif //SDCARD_DEBUG

  if (nmeaParser.haveDate())
  {

    /* set time */
#if defined(GPS_DEBUG) || defined(DATA_DEBUG)
    SerialPort.print("Time : ");
    SerialPort.println(nmeaParser.time);
#endif //GPS_DEBUG

    screen.screenTime->setTime(nmeaParser.time);
    screen.screenTime->correctTimeZone(GnuSettings.VARIOMETER_TIME_ZONE);

    screen.screenElapsedTime->setBaseTime(screen.screenTime->getTime());
#endif //defined(HAVE_SCREEN) && defined(HAVE_GPS)
  }

  /* enable near climbing */
#ifdef HAVE_SPEAKER
  //#ifdef VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM
  if (GnuSettings.VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM)
  {
    beeper.setGlidingAlarmState(true);
  }
  //#endif

  //#ifdef VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP
  if (GnuSettings.VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP)
  {
    beeper.setGlidingBeepState(true);
  }
//#endif
#endif //HAVE_SPEAKER

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
  //&& defined(VARIOMETER_RECORD_WHEN_FLIGHT_START)
  if (GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START && (!GnuSettings.NO_RECORD))
  {

#ifdef SDCARD_DEBUG
    SerialPort.println("createSDCardTrackFile");
#endif //SDCARD_DEBUG

    createSDCardTrackFile();
  }
#endif // defined(HAVE_SDCARD) && defined(VARIOMETER_RECORD_WHEN_FLIGHT_START)

  if (!GnuSettings.NO_RECORD)
  {
#ifdef SDCARD_DEBUG
    SerialPort.println("Record Start");
#endif //SDCARD_DEBUG

    gpsFix = 3;
    screen.recordIndicator->setActifRECORD();
    screen.recordIndicator->stateRECORD();
  }
  else
  {
    gpsFix = 4;
    screen.recordIndicator->setNoRECORD();
    screen.recordIndicator->stateRECORD();
  }
  flystat.Enable();
}

/*******************************************/
bool VarioData::updateSpeed(void)
{
  /*******************************************/

#ifdef HAVE_GPS
  /* when getting speed from gps, display speed and ratio */

  if ((variometerState >= VARIOMETER_STATE_DATE_RECORDED) && (nmeaParser.haveNewSpeedValue()))
  {

    currentSpeed = nmeaParser.getSpeed();
    ratio = history.getGlideRatio(currentSpeed, serialNmea.getReceiveTimestamp(), GnuSettings.SETTINGS_GLIDE_RATIO_PERIOD_COUNT);

#if defined(GPS_DEBUG) || defined(DATA_DEBUG)
    SerialPort.print("GpsSpeed : ");
    SerialPort.println(currentSpeed);
#endif //GPS_DEBUG

    //     DUMPLOG(LOG_TYPE_DEBUG,GPS_DEBUG_LOG,currentSpeed);

    flystat.SetSpeed(currentSpeed);
    timeSpeedMesure = millis();

    // display speed and ratio
    if (currentSpeed > 99)
      currentSpeed = 99;

    if (currentSpeed >= GnuSettings.RATIO_MIN_SPEED && ratio >= 0.0 && ratio < GnuSettings.RATIO_MAX_VALUE && displayLowUpdateState)
    {
      //      screen.ratioDigit->setValue(ratio);
    }
    else
    {
      ratio = 0.0;
      //      screen.ratioDigit->setValue(0.0);
    }
    speedAvailable = true;
    return true;
  }
  else
  {
    speedAvailable = false;
    return false;
  }
#endif //HAVE_GPS
}

//*******************************************
double VarioData::getSpeed()
{
  //*******************************************
  //	return calibratedAlti;
  double tmpvalue = currentSpeed;
  if (tmpvalue > MAX_SPEED)
    tmpvalue = MAX_SPEED;
  if (tmpvalue < -MAX_SPEED)
    tmpvalue = -MAX_SPEED;

  return tmpvalue;
}

/*******************************************/
void VarioData::updateVoltage(void)
{
/*******************************************/
#if defined(HAVE_SCREEN) && defined(HAVE_VOLTAGE_DIVISOR)
  //  int tmpVoltage = analogRead(VOLTAGE_DIVISOR_PIN);
  //  if (maxVoltage < tmpVoltage) {maxVoltage = tmpVoltage;}

  /* update battery level */

  long TmpVoltage = 0;
  for (int i = 0; i < 10; i++)
    TmpVoltage += analogRead(VOLTAGE_DIVISOR_PIN);
  TmpVoltage = TmpVoltage / 10;

  if (compteurBoucle == 4)
  {
    DUMPLOG(LOG_TYPE_DEBUG, VOLTAGE_DEBUG_LOG, TmpVoltage);
    compteurBoucle = 0;
  }
  else
  {
    compteurBoucle++;
  }

  //    if (TmpVoltage > MaxVoltage)
  //      MaxVoltage = TmpVoltage;

  //    if (MaxVoltage < 1750)
  if (TmpVoltage < 1750)
  {
    if (millis() - varioHardwareManager.time_deep_sleep > 10000)
    {
      screen.ScreenViewMessage("Bat Low", 3);
      indicatePowerDown();
      //        TRACELOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG);
      MESSLOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG, "Deep sleep - Batterie low");
      DUMPLOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG, TmpVoltage);
      deep_sleep("Batt Low"); //protection batterie
    }
  }
  else
  {
    varioHardwareManager.time_deep_sleep = millis();
  }

  voltage = TmpVoltage;
  //    screen.batLevel->setVoltage(MaxVoltage);
  //    varioData.MaxVoltage = 0;
  //  batLevel.setVoltage( maxVoltage );
  //  maxVoltage = 0;

#endif //HAVE_VOLTAGE_DIVISOR
}

/*******************************************/
int VarioData::getCap(void)
{
  /*******************************************/

  /*
> > > Pour la cap magnetique, je pense que tu as compris le principe de base :
> > > 1) Tu testes si tu as une valeur d'accélération (haveAccel)
> > > 2) Si oui tu lis l'accélération. (getAccel) en lisant en même temps le
> > > vecteur vertical :
> > > double vertVector[3];
> > > double vertAccel = twScheduler.getaccel(vertVector);
> > >
> > > 3) Ensuite tu testes si tu as une valeur du magnetomètre (haveMag)
> > > 4) Si oui tu recupères le vecteur pointant vers le nord en redonnant
> > > le vecteur vectical :
> > > double northVector[2];
> > > twScheduler.getNorthVector(vertVector,  northVector)
> > >
> > > Tu obtient alors un vecteur (x,y) qui pointe vers le nord sur un plan
> > > horiaontal. Normalement il est à peu près normalisé. Mais tu peux le
> > > normaliiser à nouveau si tu veux par sécurité :
> > > norm = sqrt(x*x+y*y)
> > > x = x/norm
> > > y = y/norm
> > >
> > > Je pense que dans un premier temps il faut juste que tu essayes
> > > d'afficher ce vecteur à l'écran  ;
> > > -> Tu réserves un carré sur l'écran de 2 par 2. (à toi de voir la
> > > taille de l'unité)
> > > -> Tu met un point au centre du carré (c'est ta coordonnée (0,0) )
> > > -> Tu trace une ligne à partir de ce point jusqu'au point donné par le
> > > nortvector(il ne peut pas faire plus de 1 de long)
> > > Normalement tu aura alors une flèche qui pointe vers le nord.

*/

  // vitesse > 5km et dernière mesure de vitesse de moins de 1.5s
  if ((variometerState > VARIOMETER_STATE_CALIBRATED) && (speedAvailable || ((millis() - timeSpeedMesure) < nbMsLastMesureAcceptable)) && (currentSpeed > 5))
  {
    if (nmeaParser.haveBearing())
    {

      bearing = nmeaParser.getBearing();

      gpsAvailable = true;
      timeCapMesure = millis();

#ifdef DATA_DEBUG
      SerialPort.print("Compas GPS : ");
      SerialPort.println(bearing);
#endif //DATA_DEBUG
    }
  }

  // desactive le baro GPS si pas de mesure durant 1,5sec - passe au baro magnetique
  if ((gpsAvailable) && ((millis() - timeCapMesure) < nbMsLastMesureAcceptable))
  {
    return bearing;
  }
  else
  {
    gpsAvailable = false;
  }

  if (twScheduler.haveAccel())
  {
    double vertVector[3];
    twScheduler.getAccel(vertVector);

    // accelerometer and magnetometer data
    float a, ax, ay, az;

    ax = vertVector[0];
    ay = vertVector[1];
    az = vertVector[2];

    // Normalize accelerometer and magnetometer data
    a = sqrtf(ax * ax + ay * ay + az * az);
    ax /= a;
    ay /= a;
    az /= a;
    twScheduler.resetNewAccel();
#ifdef BEARING_DEBUG
    SerialPort.print("ax : ");
    SerialPort.println(ax);
    SerialPort.print("ay : ");
    SerialPort.println(ay);
    SerialPort.print("az : ");
    SerialPort.println(az);
#endif //DATA_DEBUG

    if (twScheduler.haveMag())
    {
      double northVector[2];
      double northVectorNorm[2];
      twScheduler.getNorthVector(vertVector, northVector);

      double norm = sqrt(northVector[0] * northVector[0] + northVector[1] * northVector[1]);
      northVectorNorm[0] = northVector[0] / norm;
      northVectorNorm[1] = northVector[1] / norm;

      int tmpcap = atan2(northVectorNorm[1], northVectorNorm[0]) * 180 / M_PI;
      if (tmpcap < 0)
      {
        tmpcap = tmpcap + 360;
      }
      bearing = tmpcap;
    }
    else
    {
      bearing = -1;
      nbMesureCap = 0;
      // return 0;
    }
  }
  else
  {
    bearing = -1;
    nbMesureCap = 0;
    // return 0;
  }

  return bearing;
}

// Bound angle between 0 and 360
/*******************************************/
float VarioData::constrainAngle360(float dta)
{
  /*******************************************/
  dta = fmod(dta, 2.0 * PI);
  if (dta < 0.0)
    dta += 2.0 * PI;
  return dta;
}