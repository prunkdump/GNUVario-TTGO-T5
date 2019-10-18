#include <Arduino.h>
#include <DebugConfig.h>

#if defined(ESP32)
static const char* TAG = "Gnuvario";
#include "esp_log.h"
#endif //ESP32

#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif //HAVE_SDCARD
#include <VarioSettings.h>

#ifdef HAVE_SPEAKER
#include <toneHAL.h>
#include <beeper.h>
#endif //HAVE_SPEAKER

//#define TWOWIRESCHEDULER

#ifdef TWOWIRESCHEDULER
#include <IntTW.h>
#include <ms5611TW.h>
#include <vertaccel.h>
#include <LightInvensense.h>
#include <TwoWireScheduler.h>
#else
#include <MS5611.h>
#include <Wire.h>
#include <vertaccel2.h>
#include <SparkFunMPU9250-DMP.h>
#endif

#include <kalmanvert.h>

#ifdef HAVE_GPS
#include <SerialNmea.h>
#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <GPSSentence.h>
#endif //HAVE_GPS

#include <FlightHistory.h>
#include <variostat.h>
#include <VarioButton.h>

#include <Utility.h>

#include <driver/adc.h>

#include <Update.h>
#include <SD_Update.h>

#ifdef HAVE_WIFI
#include <wifiServer.h>
#endif //HAVE_WIFI

#ifdef HAVE_BLUETOOTH
#include "SimpleBLE.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

SimpleBLE ble;
#endif //HAVE_BLUETOOTH

/*******************/
/* Version         */
/*******************/

#define VERSION      0
#define SUB_VERSION  5
#define BETA_CODE    6
#define DEVNAME      "JPG63"
#define AUTHOR "J"    //J=JPG63  P=PUNKDUMP

/******************************************************************************************************/
/*                                              VERSION                                               */
/*                                               ESP32                                                */
/*                                       Optimisé pour TTGO-T5                                        */
/*                                                                                                    */
/*                                              Historique                                            */
/******************************************************************************************************/
/* v 0.1                             beta 1 version                                                   *
*  v 0.2     beta 1      23/06/19    debug VarioScreen                                                *      
*  v 0.3     beta 1      25/06/19    correction mesure tension                                        *
*                                    correction mesure de vitesse                                     *
*  v 0.3     beta 2      26/06/19    correction save IGC                                              *                                 
*  v 0.4     beta 1      03/07/19    ajout la coupure du son                                          *
*  v 0.4     beta 2      06/07/19    ajout statistique                                                *
*  v 0.4     beta 3      18/07/19    Correction gestion Eeprom                                        *
*                                    Correction tache affichage                                       * 
*                                    Ajout delete task display                                        *
*                                    Correction affichage statistique                                 *
* v 0.4      beta 4      22/07/19    Correction effacement screendigit                                *                             
*                                    Correction affichage duree du vol                                *
*                                    Correction affichage statistique                                 *
*                                    Modification FlightHistory                                       *
*                                    Ajout parametre flighthistory dans SETTINGS.TXT                  *
*                                    Enregistrement des statistique toutes les 60sec - reduction des  *
*                                    cycle d'ecriture dans la mémoire flash de l'ESP32                *
* v 0.4     beta 5    24/07/19       Ajout TrendDigit                                                 *
*                                    Modification TrendLevel                                          *
*                                    Modification screendigit                                         *
*                                    Correction démarrage du vol                                      *
*                                    indicateur de monte/descente                                     *
* v 0.4     beta 6    25/07/19       Ajout NO_RECORD                                                  *                                                                      
*                                    Correction TWOWIRESCHEDULER                                      *
* v 0.4     beta 7    05/08/19       Désactivation enregistrement stat pour eviter le bug du plantage * 
*                                    Ajout paramettre wifi                                            *
*                                    Modification SETTINGS.TXT v1.2                                   *
* v 0.4     beta 8    06/08/19       Ajout icone Norecord                                             *
*                                    Correction bug statistique                                       *
*                                    Raffraichissement de l'ensemle de l'écran toutes les 30sec       *
* v 0.5     beta 1    10/08/19       Ajout Gestionnaire d'erreur ESP32                                *
*                                    Mise à jour via SDCARD /update.bin                               *
*                                    Ajout Serveur Web update/download                                *
*                                    Ajout HAVE_WIFI                                                  *
*                                    Ajout BT                                                         *
* v 0.5     beta 2    20/08/19       Ajout dans hardwareConfig32 de la  version du PCB et de la TTGO  *
*                                    Ajout lecture de la temperature                                  *
*                                    MAJ OTA Web serveur                                              *
*                                    Ajout changement de page                                         *
*                                    Ajout 2ème ecran                                                 *
* v 0.5     beta 3  25/08/19         Ajout LOW/HIGH level cmd ampli                                   *
*                                    Ajout ecran reglage volume du son                                *
*                                    Correction Bug bouton                                            *
* v 0.5     beta 4  28/08/19         Ajout écran stat                                                 *                                   
*                                    Modification librairie ToneHAL                                   *
*                                    Correction bug d'affichage mineurs                               *
* v 0.5     beta 5  31/08/19         Correction bg reglage volume                                     *
*                                    Ajout commande ampli dans loop pour test                         *
* v 0.5     beta 6  04/09/19         Changement librairies MS5611 et MPU9250                          *                                   
*                                    Modifié la calibration de l'altitude par le GPS                  *
*                                    Ajout d'un coeficiant de compensation de temperature             *
*                                    Modification la séquence de démarrage de l'enregistrement        *
*                                                                                                     *
*******************************************************************************************************
*                                                                                                     *
*                                   Developpement a venir                                             *
*                                                                                                     *                                                             
* V0.4                                                                                                *    
* bug affichage finesse                                                                               *                                            
*                                                                                                     *
* V0.5                                                                                                *
* Recupération vol via USB                                                                            *                                                                                        
* Calibration MPU                                                                                     *                                 
* Mise à jour ESP32 via USB                                                                           * 
* revoir volume du son ToneESP32                                                                      *
* verifier effacement du m (altitude)                                                                 *
* bug d'affichage des fleches                                                                         *
* voir réactivité des données GPS                                                                     *
*                                                                                                     *
* VX.X                                                                                                *
* Refaire gestion du son                                                                              *
* Paramètrage des écrans                                                                              *
* Gérer le son via le DAC                                                                             *
* Afficher la boussole                                                                                *
* Sens et vitesse du vent                                                                             *
* Carnet de vol (10 derniers vols)                                                                    *
*     10 zones d'eeprom - reduit le nombre d'écriture et économise la mémoire flash                   *
*******************************************************************************************************/

/************************************************************************
 *                   Fonctionalitées                                    *
 *                                                                      *
 * Version 0.3                                                          *                                                                     
 * - Parametre utilisateur dans fichier SETTINGS.TXT                    *
 * - Coupure du son via le bouton central (en vol)                      *
 *                                                                      *
 * Version 0.4                                                          *
 * - Statistiques de Vol                                                *
 * - taux de chute et finesse                                           *
 * - indicateur de monte/descente                                       *
 * - Possibilité de ne pas enregistrer les vols                         *
 * Version 0.5                                                          *   
 *   Taille du code :                                                   *
 *          29% Sans BT et Wifi                                         *
 *          69% avec Wifi                                               *
 *          85% avec BT                                                 *
 * - Mise à jour via la carte SD - /update.bin                          *
 * - Récupération des vol via Wifi                                      *
 * - Mise à jour via Wifi                                               *
 * - Upload des fichiers de configuration via Wifi                      *
 * - Ajout Bluetooth                                                    *
 * - Multi-écran (ajout 2ème page / gestion des bouton droit et gauche) *
 * - Affichage de la température                                        *
 * - Page de configuration du volume sonore                             *
 * - Page de statistique accéssible via les boutons                     *
 *                                                                      *
 ************************************************************************
 
/************************************************************************
*                          Recommandation                               *    
 *                                                                      *
 * Dans DebugConfig.h commanter #define ENABLE_DEBUG pour eviter les    *                                                                      
 *                    ralentissement et dans le cas ou le BlueTooth est *
 *                    activé                                            *
 * Wous ne pouvez pas activer en même temps le Wifi et le BlueTooth, si *
 *                    vous activez le BT vous ne pourrez plus utiliser  *
 *                    les services Web du Gnuvario                      *                                                             
 * A chaque nouvelle mise à jour, verifiez la version du fichier        *
 *                   SETTINGS.TXT il peut avoir évolué                  *
 ************************************************************************/

/************************************************************************
*                        Compilation                                    *
*                                                                       *
* Le code du Gnuvario-E est prévu pour fonctionner sur une board TTGO-T5*
* à base d'ESP32                                                        *
*                                                                       *
* https://dl.espressif.com/dl/package_esp32_index.json                  *
*                                                                       *
* Type de carte : ESP32 dev Module                                      *
* CPU frequency : 80Mhz (Wifi                                           *
*************************************************************************/

/************************************************************************
*               Fichiers de configuration                               *
*                                                                       *
* SETTINGS.TXT             parametres utilisateur                       *
* HardwareConfig.h         parametres matériels communs                 *
* HardwareConfigESP32.h    parametre spécifique à l'ESP32               *
* DebugConfig.h            parametre de debuggage                       *
* VarioSettings.h          parametres par defaut (utilisé si il n'y a   *
*                          pas de SDcard ou de fichier SETTINGS.TXT     *
*************************************************************************/

/**************************************************************************************************************
 *              Liens utiles                                                                                  *
 *                                                                                                            *
 * https://learn.sparkfun.com/tutorials/9dof-razor-imu-m0-hookup-guide/all#libraries-and-example-firmware     *        
 *                                                                                                            *
 **************************************************************************************************************/

/*******************/
/* General objects */
/*******************/
#define VARIOMETER_STATE_INITIAL 0
#define VARIOMETER_STATE_DATE_RECORDED 1
#define VARIOMETER_STATE_CALIBRATED 2
#define VARIOMETER_STATE_FLIGHT_STARTED 3

#ifdef HAVE_GPS
uint8_t variometerState = VARIOMETER_STATE_INITIAL;
#else
uint8_t variometerState = VARIOMETER_STATE_CALIBRATED;
#endif //HAVE_GPS

/*****************/
/* screen        */
/*****************/

#include <varioscreenGxEPD.h>

/***************/
/* IMU objects */
/***************/
#ifdef TWOWIRESCHEDULER
#ifdef HAVE_BMP280
Bmp280 TWScheduler::bmp280;
#else
Ms5611 TWScheduler::ms5611;
#endif
#ifdef HAVE_ACCELEROMETER
Vertaccel TWScheduler::vertaccel;
#endif //HAVE_ACCELEROMETER
#else

MS5611 ms5611;
double referencePressure;

MPU9250_DMP imu;
Vertaccel vertaccel;

#endif

//Vertaccel vertaccel;

/**********************/
/* alti/vario objects */
/**********************/
#define POSITION_MEASURE_STANDARD_DEVIATION 0.1
#ifdef HAVE_ACCELEROMETER 
#define ACCELERATION_MEASURE_STANDARD_DEVIATION 0.3
#else
#define ACCELERATION_MEASURE_STANDARD_DEVIATION 0.6
#endif //HAVE_ACCELEROMETER 

kalmanvert kalmanvert;

/**********************/
/* SDCARD objects     */
/**********************/

VarioSettings GnuSettings;

/************************************/
/* glide ratio / average climb rate */
/************************************/

/* two minutes history */
#ifdef HAVE_GPS
constexpr double historyGPSPeriodCountF = (double)(GPS_PERIOD) / 500.0;
constexpr int8_t historyGPSPeriodCount = (int8_t)(0.5 + historyGPSPeriodCountF);
SpeedFlightHistory<500, 120, historyGPSPeriodCount> history;
#else 
FlightHistory<500, 120> history;
#endif

/***************/
/* gps objects */
/***************/
#ifdef HAVE_GPS

//NmeaParser nmeaParser;

#ifdef HAVE_BLUETOOTH
boolean lastSentence = false;
#endif //HAVE_BLUETOOTH

#endif //HAVE_GPS

/*********************/
/* bluetooth objects */
/*********************/
#ifdef HAVE_BLUETOOTH
#if defined(VARIOMETER_SENT_LXNAV_SENTENCE)
LxnavSentence bluetoothNMEA;
#elif defined(VARIOMETER_SENT_LK8000_SENTENCE)
LK8Sentence bluetoothNMEA;
#else
#error No bluetooth sentence type specified !
#endif

#ifndef HAVE_GPS
unsigned long lastVarioSentenceTimestamp = 0;
#endif // !HAVE_GPS
#endif //HAVE_BLUETOOTH

unsigned long lastDisplayTimestamp;
boolean displayLowUpdateState=true;

VarioStat flystat;

/*************************************************
 Serveur Web
 *************************************************/
#ifdef HAVE_WIFI 
String webpage = "";

#ifdef ESP8266
  ESP8266WiFiMulti wifiMulti; 
  ESP8266WebServer server(80);
#else
  WiFiMulti wifiMulti;
  ESP32WebServer server(80);
#endif
#endif //HAVE_WIFI

/*************************************************
Internal TEMPERATURE Sensor
/*************************************************

/* 
 *  https://circuits4you.com
 *  ESP32 Internal Temperature Sensor Example
 */

 #ifdef __cplusplus
  extern "C" {
 #endif

  uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

int tmpint = 0;
int compteurGpsFix = 0;

//****************************
//****************************
void setup() {
//****************************
//****************************  

/*****************************/
/*  Init Alimentation        */
/*****************************/
#if defined(HAVE_POWER_ALIM) 
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, POWER_PIN_STATE);   // turn on POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)
#endif  

/***********************************/
/*  Init mesure tension batterie   */
/***********************************/
#if defined(HAVE_VOLTAGE_DIVISOR) 
    pinMode(VOLTAGE_DIVISOR_PIN, INPUT);
    analogReadResolution(12);

#if defined(VOLTAGE_DIVISOR_DEBUG)
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11);
#endif
#endif  

  SerialPort.begin(115200);

  /*****************************/
  /* wait for devices power on */
  /*****************************/
#ifdef PROG_DEBUG
  delay (5000);
#else  
  delay(VARIOMETER_POWER_ON_DELAY);
#endif  

 /******************************/
 /* Eteint la led de la ttgo   */
 /******************************/

/*  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, HIGH); */

/************************/
/*    BOOT SEQUENCE     */
/************************/

#ifdef PROG_DEBUG

///  while (!SerialPort) { ;}
  char tmpbuffer[100];
  sprintf(tmpbuffer,"GNUVARIO compiled on %s\0", __DATE__); // at %s", __DATE__, __TIME__);
  SerialPort.println(tmpbuffer);
  sprintf(tmpbuffer,"VERSION %i.%i - %s\0", VERSION,SUB_VERSION,DEVNAME); 
  SerialPort.println(tmpbuffer);
  if (BETA_CODE > 0) {
    SerialPort.print("Beta ");
    SerialPort.println(BETA_CODE);
  }
  SerialPort.flush();
#endif //PRO_DEBBUG

#if defined(ESP32)
  if (BETA_CODE > 0) {
    ESP_LOGI(TAG, "GnuVario-E version %d.%d Beta %d.", VERSION,SUB_VERSION,BETA_CODE);
  } else {
    ESP_LOGI(TAG, "GnuVario-E version %d.%d.", VERSION,SUB_VERSION);
//  ESP_LOGE(TAG, "Failed to initialize the card (%d). Make sure SD card lines have pull-up resistors in place.", ret);
  }
#endif //ESP32

  
  /******************/
  /* Init Speaker   */
  /******************/
  
#if defined( HAVE_SPEAKER)
  toneHAL.init();
  beeper.setVolume(10);
  beeper.generateTone(2000,300); 
#endif

  /******************/
  /* Init SDCARD    */
  /******************/

#ifdef HAVE_SDCARD

  if (GnuSettings.initSettings()) {
#ifdef SDCARD_DEBUG
    SerialPort.println("initialization done.");
    SerialPort.flush();
#endif //PROG_DEBUG

#if defined(ESP32)
    ESP_LOGI("SDCARD", "initialization done.");
#endif //EPS32

#ifdef HAVE_WIFI
    SD_present = true; 
#endif //HAVE_WIFI
    sdcardState = SDCARD_STATE_INITIALIZED;
    GnuSettings.readSDSettings();
    
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
    GnuSettings.VARIOMETER_PILOT_NAME.toCharArray(__dataPilotName, sizeof(__dataPilotName)+1);

#ifdef PROG_DEBUG
   //Debuuging Printing
    SerialPort.print("__dataPilotName = ");
    SerialPort.print(__dataPilotName);
    SerialPort.print(" - ");
    SerialPort.print(sizeof(__dataPilotName));
    SerialPort.print(" / ");
    SerialPort.print(GnuSettings.VARIOMETER_PILOT_NAME);
    SerialPort.print(" - ");
    SerialPort.println(GnuSettings.VARIOMETER_PILOT_NAME.length());
#endif //PROG_DEBUG

    char __dataGliderName[GnuSettings.VARIOMETER_GLIDER_NAME.length()];
    GnuSettings.VARIOMETER_GLIDER_NAME.toCharArray(__dataGliderName, sizeof(__dataGliderName)+1);

#ifdef PROG_DEBUG
   //Debuuging Printing
    SerialPort.print("__dataGliderName = ");
    SerialPort.print(__dataGliderName);
    SerialPort.print(" - ");
    SerialPort.print(sizeof(__dataGliderName));
    SerialPort.print(" / ");
    SerialPort.print(GnuSettings.VARIOMETER_GLIDER_NAME);
    SerialPort.print(" - ");
    SerialPort.println(GnuSettings.VARIOMETER_GLIDER_NAME.length());
#endif //PROG_DEBUG

    header.saveParams(VARIOMETER_MODEL_NAME, __dataPilotName, __dataGliderName);
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
    if (GnuSettings.ALARM_SDCARD) {
      indicateFaultSDCARD();
    }
#endif //HAVE_SPEAKER 
  }
#endif //HAVE_SDCARD

/*  uint8_t tmp[4];
  tmp[0]=1;
  tmp[1]=1;
  tmp[2]=19;
  tmp[3]=00;

  flystat.SetDate(tmp);
  flystat.ForceWrite();*/

#ifdef HAVE_SPEAKER
  beeper.setVolume(GnuSettings.VARIOMETER_BEEP_VOLUME);
  toneHAL.setVolume(GnuSettings.VARIOMETER_BEEP_VOLUME);
#endif //HAVE_SPEAKER

#ifdef HAVE_SDCARD
  updateFromSDCARD();
#endif //HAVE_SDCARD
  
  /***************/
  /* init screen */
  /***************/
#ifdef HAVE_SCREEN
#ifdef SCREEN_DEBUG
  SerialPort.println("initialization screen");
  SerialPort.flush();
#endif //SCREEN_DEBUG

#if defined(ESP32)
      ESP_LOGI("SCREEN", "initialization screen");
#endif //EPS32

  screen.init();
  screen.createScreenObjects();
  screen.begin();
#endif

  /***************/
  /* init button */
  /***************/

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

#ifdef HAVE_SCREEN
/*----------------------------------------*/
/*                                        */
/*             DISPLAY BOOT               */
/*                                        */
/*----------------------------------------*/

#ifdef SCREEN_DEBUG
  SerialPort.println("Display boot");
#endif //SCREEN_DEBUG

#if defined(ESP32)
      ESP_LOGI(TAG, "Display Boot");
#endif //EPS32

  screen.ScreenViewInit(VERSION,SUB_VERSION, AUTHOR,BETA_CODE);

  
#endif //HAVE_SCREEN
  
#ifdef TWOWIRESCHEDULER
  /**************************/
  /* init Two Wires devices */
  /**************************/
  //!!!
#ifdef HAVE_ACCELEROMETER
  intTW.begin();
  twScheduler.init();
//  vertaccel.init();

#endif //HAVE_ACCELEROMETER
#else //TWOWIRESCHEDULER
#ifdef MS5611_DEBUG
  SerialPort.println("Initialize MS5611 Sensor");
#endif //MS5611_DEBUG

  while(!ms5611.begin())
  {
    SerialPort.println("Could not find a valid MS5611 sensor, check wiring!");
#if defined(ESP32)
    ESP_LOGE(TAG, "Erreur capteur MS5611 introuvable");
#endif //EPS32
    delay(500);
  }

  // Get reference pressure for relative altitude
  referencePressure = ms5611.readPressure();

  SerialPort.print("Oversampling: ");
  SerialPort.println(ms5611.getOversampling());

  vertaccel.init();

#ifdef HAVE_ACCELEROMETER
#ifdef ACCEL_DEBUG
  SerialPort.println("configuring device.");
#endif //ACCEL_DEBUG

#ifdef ACCEL_DEBUG
    SerialPort.println("configured 9Axis I2C MPU9250");
#endif //ACCEL_DEBUG

  // Call imu.begin() to verify communication and initialize
  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
      SerialPort.println("Unable to communicate with MPU-9250");
      SerialPort.println("device error");
      ESP_LOGE(TAG, "Erreur capteur MPU9250 introuvable");
      while(1);
    }
  }
  
/*  imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | // Enable 6-axis quat
               DMP_FEATURE_GYRO_CAL, // Use gyro calibration
              10); // Set DMP FIFO rate to 10 Hz
  // DMP_FEATURE_LP_QUAT can also be used. It uses the 
  // accelerometer in low-power mode to estimate quat's.
  // DMP_FEATURE_LP_QUAT and 6X_LP_QUAT are mutually exclusive*/

    imu.dmpBegin(DMP_FEATURE_SEND_RAW_ACCEL | // Send accelerometer data
                 DMP_FEATURE_GYRO_CAL       | // Calibrate the gyro data
                 DMP_FEATURE_SEND_CAL_GYRO  | // Send calibrated gyro data
                 DMP_FEATURE_6X_LP_QUAT     , // Calculate quat's with accel/gyro
                 10);                         // Set update rate to 10Hz.
  
#endif //HAVE_ACCELEROMETER

#endif //TWOWIRESCHEDULER


#ifdef HAVE_SCREEN
// Affichage Statistique
  flystat.Display();
  screen.ScreenViewStat();
#endif //HAVE_SCREEN

#ifdef HAVE_ACCELEROMETER
  /******************/
  /* get first data */
  /******************/

#ifdef MS5611_DEBUG
    SerialPort.println("Attente premiere mesure alti");
#endif //MS5611_DEBUG

#ifdef TWOWIRESCHEDULER
  /* wait for first alti and acceleration */
  while( ! twScheduler.havePressure() ) { }
#else //TWOWIRESCHEDULER
#endif //TWOWIRESCHEDULER

#ifdef MS5611_DEBUG
    SerialPort.println("première mesure");
#endif //MS5611_DEBUG

#ifdef TWOWIRESCHEDULER
  /* init kalman filter with 0.0 accel*/
  double firstAlti = twScheduler.getAlti();
#else //TWOWIRESCHEDULER
  double firstAlti = ms5611.readPressure();
#endif //TWOWIRESCHEDULER

  if (isnan(firstAlti)) {
    SerialPort.println("Fail firstAlti : ");
    SerialPort.println("reinit");
    ESP_LOGE(TAG, "Erreur Première mesure d'altitude");
    ESP.restart();         
  }
  
#ifdef MS5611_DEBUG
    SerialPort.print("firstAlti : ");
    SerialPort.println(firstAlti);
#endif //MS5611_DEBUG

  kalmanvert.init(firstAlti,
                  0.0,
                  POSITION_MEASURE_STANDARD_DEVIATION,
                  ACCELERATION_MEASURE_STANDARD_DEVIATION,
                  millis());

#ifdef KALMAN_DEBUG
  SerialPort.println("kalman init");
#endif //KALMAN_DEBUG
#endif //HAVE_ACCELEROMETER

  compteurGpsFix = 0;

#if defined(HAVE_GPS) 
  if (GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE) history.init(firstAlti, millis());
#endif //defined(HAVE_GPS) 

#ifdef HAVE_GPS
  serialNmea.begin(9600, true);
#ifdef GPS_DEBUG
  SerialPort.println("SerialNmea init");
#endif //GPS_DEBUG
#endif //HAVE_GPS

#ifdef HAVE_SCREEN
 
  screen.ScreenViewPage(0,true);
  screen.updateScreen ();
  
  screen.volLevel->setVolume(toneHAL.getVolume());

#ifdef SCREEN_DEBUG
  SerialPort.println("update screen");
#endif //SCREEN_DEBUG

  screen.clearScreen();
  screen.schedulerScreen->enableShow();
#endif //HAVE_SCREEN

#ifdef HAVE_BLUETOOTH
#ifdef BT_DEBUG
  SerialPort.setDebugOutput(true);
//    pinMode(0, INPUT_PULLUP);
  SerialPort.print("ESP32 SDK: ");
  SerialPort.println(ESP.getSdkVersion());
#endif //BT_DEBUG
  ble.begin("GnuVario-E");
#endif //HAVE_BLUETOOTH

  ButtonScheduleur.Set_StatePage(STATE_PAGE_VARIO);
  /* init time */
  lastDisplayTimestamp = millis(); 
  displayLowUpdateState = true;  
}

double temprature=0;

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
void createSDCardTrackFile(void);
#endif //defined(HAVE_SDCARD) && defined(HAVE_GPS)
void enableflightStartComponents(void);

void compute(int16_t *imuAccel, int32_t *imuQuat, double* vertVector, double& vertAccel);

//*****************************
//*****************************
void loop() {
//****************************  
//****************************

 /* if( vertaccel.readRawAccel(accel, quat) ){
    count++;
  }*/

/*  LOW UPDATE DISPLAY */
   if( millis() - lastDisplayTimestamp > 500 ) {

     lastDisplayTimestamp = millis();
     displayLowUpdateState = true;
 /*    if (tmpint == 0) tmpint = 1000;
     else             tmpint = 0;*/
   }


/*******************************/
/*  Compute button             */
/*******************************/

  ButtonScheduleur.update();

  /*****************************/
  /* compute vertical velocity */
  /*****************************/

#ifdef HAVE_ACCELEROMETER
#ifdef TWOWIRESCHEDULER
  if( twScheduler.havePressure() && twScheduler.haveAccel() ) {

    double tmpAlti, tmpTemp, tmpAccel;
    twScheduler.getTempAlti(tmpTemp, tmpAlti);
    tmpAccel = twScheduler.getAccel(NULL);
#else //TWOWIRESCHEDULER

  if ( imu.fifoAvailable() ) {

    double tmpAlti, tmpTemp, tmpAccel;
    int16_t rawAccel[3];
    int32_t quat[4];

    long realPressure = ms5611.readPressure();
    tmpAlti = ms5611.getAltitude(realPressure);
    tmpTemp = ms5611.readTemperature();
    tmpTemp += MPU_COMP_TEMP;

    // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
    if ( imu.dmpUpdateFifo() == INV_SUCCESS)
    {
      // computeEulerAngles can be used -- after updating the
      // quaternion values -- to estimate roll, pitch, and yaw
//      imu.computeEulerAngles();

      quat[0] = imu.qw;
      quat[1] = imu.qx;
      quat[2] = imu.qy;
      quat[3] = imu.qz;
   
      rawAccel[0] = imu.ax;
      rawAccel[1] = imu.ay;
      rawAccel[2] = imu.az;

      double tmpVertVector[3];
      vertaccel.compute(rawAccel, quat, tmpVertVector, tmpAccel);

//      tmpAccel = 0;
    }

#endif //TWOWIRESCHEDULER
    
#ifdef DATA_DEBUG
    SerialPort.print("Alti : ");
    SerialPort.println(tmpAlti);
    SerialPort.print("Temperature : ");
    SerialPort.println(tmpTemp);
    SerialPort.print("Accel : ");
    SerialPort.println(tmpAccel);
#endif //DATA_DEBUG

    kalmanvert.update( tmpAlti,
                       tmpAccel,
                       millis() );
#else
#ifdef TWOWIRESCHEDULER
  if( twScheduler.havePressure() ) {
    
#ifdef MS5611_DEBUG
//    SerialPort.println("havePressure");
#endif //MS5611_DEBUG

    double tmpAlti, tmpTemp;
    twScheduler.getTempAlti(tmpTemp, tmpAlti);
#else //TWOWIRESCHEDULER
    double tmpAlti, tmpTemp, tmpAccel;

    long realPressure = ms5611.readPressure();
    tmpAlti = ms5611.getAltitude(realPressure);
    tmpTemp = ms5611.readTemperature();
    tmpTemp += MPU_COMP_TEMP;

#endif //TWOWIRESCHEDULER

#ifdef DATA_DEBUG
    SerialPort.print("Alti : ");
    SerialPort.println(tmpAlti);
    SerialPort.print("Temperature : ");
    SerialPort.println(tmpTemp);
#endif //DATA_DEBUG

    kalmanvert.update( tmpAlti,
                       0.0,
                       millis() );
#endif //HAVE_ACCELEROMETER
  
    if (displayLowUpdateState) {
      screen.tempDigit->setValue(tmpTemp);
      screen.tunit->toDisplay();
    }

//*****************
//* update beeper *
//*****************

#ifdef HAVE_SPEAKER
		beeper.setVelocity( kalmanvert.getVelocity() );
#endif //HAVE_SPEAKER

   /* set history */
#if defined(HAVE_GPS) 
    if ((GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)	|| (GnuSettings.RATIO_CLIMB_RATE > 1)) history.setAlti(kalmanvert.getCalibratedPosition(), millis());
#endif

		double currentalti  = kalmanvert.getCalibratedPosition();
		double currentvario = kalmanvert.getVelocity();

#ifdef DATA_DEBUG
    SerialPort.print("Kalman Alti : ");
    SerialPort.println(currentalti);
    SerialPort.print("Kalman Vario : ");
    SerialPort.println(currentvario);
#endif //DATA_DEBUG

    /* set screen */

    flystat.SetAlti(currentalti);
    flystat.SetVario(currentvario);

#ifdef HAVE_SCREEN
#ifdef PROG_DEBUG
 //   SerialPort.print("altitude : ");
 //   SerialPort.println((uint16_t)currentalti);
#endif //PROG_DEBUG

    screen.altiDigit->setValue((uint16_t)currentalti+tmpint);
    if (GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE) {    
      if( history.haveNewClimbRate() ) {
        screen.varioDigit->setValue(history.getClimbRate(GnuSettings.SETTINGS_CLIMB_PERIOD_COUNT));
      }
    } else {
      screen.varioDigit->setValue(currentvario);
    }

    if( history.haveNewClimbRate() ) {
      double TmpTrend;
      TmpTrend = history.getClimbRate(GnuSettings.SETTINGS_CLIMB_PERIOD_COUNT);
#ifdef PROG_DEBUG
      SerialPort.print("Trend value : ");
      SerialPort.println(TmpTrend);
#endif //PROG_DEBUG
      
      if (displayLowUpdateState) {
        if (GnuSettings.RATIO_CLIMB_RATE > 1) {
          if (abs(TmpTrend) < 10) screen.trendDigit->setValue(abs(TmpTrend)); 
          else                    screen.trendDigit->setValue(9.9);
        }

#ifdef PROG_DEBUG
        SerialPort.println("display trendLevel");
#endif //PROG_DEBUG

        if (TmpTrend == 0)     screen.trendLevel->stateTREND(0);
        else if (TmpTrend > 0) screen.trendLevel->stateTREND(1);
        else                   screen.trendLevel->stateTREND(-1);
      }
    }  
#else
    if (GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE) {
      if( history.haveNewClimbRate() ) {
        screen.varioDigit->setValue(history.getClimbRate(GnuSettings.SETTINGS_CLIMB_PERIOD_COUNT));
      }
    else {
      screen.varioDigit->setValue(currentvario);
    }
#endif //HAVE_SCREEN
     
  }

  /*****************/
  /* update beeper */
  /*****************/
#ifdef HAVE_SPEAKER
  beeper.update();
#ifdef PROG_DEBUG
//    SerialPort.println("beeper update");
#endif //PROG_DEBUG
#endif //HAVE_SPEAKER

 
  /********************/
  /* update bluetooth */
  /********************/
#ifdef HAVE_BLUETOOTH
#ifdef HAVE_GPS
  /* in priority send vario nmea sentence */
  if( bluetoothNMEA.available() ) {
    while( bluetoothNMEA.available() ) {
       serialNmea.write( bluetoothNMEA.get() );
    }
    serialNmea.release();
  }
#else //!HAVE_GPS
  /* check the last vario nmea sentence */
  if( millis() - lastVarioSentenceTimestamp > VARIOMETER_SENTENCE_DELAY ) {
    lastVarioSentenceTimestamp = millis();
#ifdef VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE
    bluetoothNMEA.begin(kalmanvert.getCalibratedPosition(), kalmanvert.getVelocity());
#else
    bluetoothNMEA.begin(kalmanvert.getPosition(), kalmanvert.getVelocity());
#endif
    while( bluetoothNMEA.available() ) {
       serialNmea.write( bluetoothNMEA.get() );
    }
  }
#endif //!HAVE_GPS
#endif //HAVE_BLUETOOTH

   /**************/
  /* update GPS */
  /**************/
#ifdef HAVE_GPS
#ifdef HAVE_BLUETOOTH
  /* else try to parse GPS nmea */
  else {
#endif //HAVE_BLUETOOTH
    
    /* try to lock sentences */
    if( serialNmea.lockRMC() ) {
      
#ifdef GPS_DEBUG
    SerialPort.println("mneaParser : beginRMC");
#endif //GPS_DEBUG

      nmeaParser.beginRMC();      
    } else if( serialNmea.lockGGA() ) {
      
#ifdef GPS_DEBUG
    SerialPort.println("mneaParser : beginGGA");
#endif //GPS_DEBUG

      nmeaParser.beginGGA();
#ifdef HAVE_BLUETOOTH
      lastSentence = true;
#endif //HAVE_BLUETOOTH
#ifdef HAVE_SDCARD      
      /* start to write IGC B frames */
      if (!GnuSettings.NO_RECORD) igcSD.writePosition(kalmanvert);
#endif //HAVE_SDCARD
    }
  
    /* parse if needed */
    if( nmeaParser.isParsing() ) {

#ifdef GPS_DEBUG
      SerialPort.println("mneaParser : isParsing");
#endif //GPS_DEBUG

#ifdef SDCARD_DEBUG
      SerialPort.print("writeGGA : ");
#endif //SDCARD_DEBUG
      
      while( nmeaParser.isParsing() ) {
        uint8_t c = serialNmea.read();
        
        /* parse sentence */        
        nmeaParser.feed( c );

#ifdef HAVE_SDCARD          
        /* if GGA, convert to IGC and write to sdcard */
        if( sdcardState == SDCARD_STATE_READY && nmeaParser.isParsingGGA() ) {
          igc.feed(c);
/*          while( igc.available() ) {
            fileIgc.write( igc.get() );
          }*/
          if (!GnuSettings.NO_RECORD) igcSD.writeGGA();          
        }
#endif //HAVE_SDCARD
      }
      
      serialNmea.release();
      fileIgc.flush();
#ifdef SDCARD_DEBUG
      SerialPort.println("");
#endif //SDCARD_DEBUG
    
#ifdef HAVE_BLUETOOTH   
      /* if this is the last GPS sentence */
      /* we can send our sentences */
      if( lastSentence ) {
          lastSentence = false;
#ifdef VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE
          bluetoothNMEA.begin(kalmanvert.getCalibratedPosition(), kalmanvert.getVelocity());
#else
          bluetoothNMEA.begin(kalmanvert.getPosition(), kalmanvert.getVelocity());
#endif
          serialNmea.lock(); //will be writed at next loop
      }
#endif //HAVE_BLUETOOTH
    }
    
    /***************************/
    /* update variometer state */
    /*    (after parsing)      */
    /***************************/
    if( variometerState < VARIOMETER_STATE_FLIGHT_STARTED ) {

      /* if initial state check if date is recorded  */
      if( variometerState == VARIOMETER_STATE_INITIAL ) {
        if( nmeaParser.haveDate() ) {
          
#ifdef GPS_DEBUG
          SerialPort.println("VARIOMETER_STATE_DATE_RECORDED");
#endif //GPS_DEBUG

          variometerState = VARIOMETER_STATE_DATE_RECORDED;
        }
      }
      
      /* check if we need to calibrate the altimeter */
      else if( variometerState == VARIOMETER_STATE_DATE_RECORDED ) {

#ifdef GPS_DEBUG
          SerialPort.print("NmeaParser Precision : ");
          SerialPort.println(nmeaParser.precision);
          SerialPort.print("VARIOMETER_GPS_ALTI_CALIBRATION_PRECISION_THRESHOLD : ");
          SerialPort.println(VARIOMETER_GPS_ALTI_CALIBRATION_PRECISION_THRESHOLD);        
#endif //GPS_DEBUG

        /* we need a good quality value */
        if( nmeaParser.haveNewAltiValue() && nmeaParser.precision < VARIOMETER_GPS_ALTI_CALIBRATION_PRECISION_THRESHOLD ) {

          compteurGpsFix++;
          double gpsAlti = nmeaParser.getAlti();

#ifdef GPS_DEBUG
          SerialPort.print("CompteurGpsFix : ");
          SerialPort.println(compteurGpsFix);
#endif //GPS_DEBUG
          
#ifdef HAVE_SCREEN
          screen.recordIndicator->setActifGPSFIX();
        //  recordIndicator->stateRECORD();
#endif //HAVE_SCREEN

#if defined(DATA_DEBUG) || defined(GPS_DEBUG)
          SerialPort.print("Gps Alti : ");
          SerialPort.println(gpsAlti);
#endif //DATA_DEBUG

          if (compteurGpsFix > 10) {
#ifdef GPS_DEBUG
            SerialPort.println("GPS FIX");
#endif //GPS_DEBUG
          
          /* calibrate */
 #ifdef HAVE_SPEAKER 
            if (GnuSettings.ALARM_GPSFIX) {
 //           toneAC(BEEP_FREQ);
              beeper.generateTone(GnuSettings.BEEP_FREQ, 200);
//            delay(200);
//            toneAC(0);
            }
 #endif //defined(HAVE_SPEAKER) 

#ifdef HAVE_SCREEN
            screen.fixgpsinfo->setFixGps();
            screen.recordIndicator->setActifGPSFIX();
        //  recordIndicator->stateRECORD();
#endif //HAVE_SCREEN
            kalmanvert.calibratePosition(gpsAlti);

#ifdef DATA_DEBUG
            SerialPort.print("Gps Alti : ");
            SerialPort.println(gpsAlti);
#endif //DATA_DEBUG

#ifdef GPS_DEBUG
            SerialPort.print("GpsAlti : ");
            SerialPort.println(gpsAlti);
            SerialPort.println("Kalman CalibratePosition");        
#endif //GPS_DEBUG
          
#if defined(HAVE_GPS) 
            if (GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)  history.init(gpsAlti, millis());
#endif //defined(HAVE_GPS) 

            variometerState = VARIOMETER_STATE_CALIBRATED;

#ifdef GPS_DEBUG
            SerialPort.println("GPS Calibrated");        
#endif //GPS_DEBUG
          
#ifdef HAVE_SDCARD 
            if (!GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START) {

#ifdef SDCARD_DEBUG
              SerialPort.println("createSDCardTrackFile");        
#endif //SDCARD_DEBUG

             createSDCardTrackFile();
            }
#endif //HAVE_SDCARD
          }
        }
      }
      
      /* else check if the flight have started */
      else {  //variometerState == VARIOMETER_STATE_CALIBRATED
        
        /* check flight start condition */
        if( (millis() > GnuSettings.FLIGHT_START_MIN_TIMESTAMP) &&
            ((GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START) &&   
             ((kalmanvert.getVelocity() < GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD) || (kalmanvert.getVelocity() > GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD)) 
#ifdef HAVE_GPS

             && (nmeaParser.getSpeed() > GnuSettings.FLIGHT_START_MIN_SPEED)
#endif //HAVE_GPS

            ) || (!GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START)
     
  //        && (kalmanvert.getVelocity() < FLIGHT_START_VARIO_LOW_THRESHOLD || kalmanvert.getVelocity() > FLIGHT_START_VARIO_HIGH_THRESHOLD) &&
          ) {
//          variometerState = VARIOMETER_STATE_FLIGHT_STARTED;
          enableflightStartComponents();
        }
      }
    }
#ifdef HAVE_BLUETOOTH
  }
#endif //HAVE_BLUETOOTH
#endif //HAVE_GPS

  /* if no GPS, we can't calibrate, and we have juste to check flight start */
#ifndef HAVE_GPS
  if( variometerState == VARIOMETER_STATE_CALIBRATED ) { //already calibrated at start 
/*    if( (millis() > GnuSettings.FLIGHT_START_MIN_TIMESTAMP) &&
        (kalmanvert.getVelocity() < GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD || kalmanvert.getVelocity() > GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD) ) {
      variometerState = VARIOMETER_STATE_FLIGHT_STARTED;
      enableflightStartComponents();*/
      if( (millis() > GnuSettings.FLIGHT_START_MIN_TIMESTAMP) &&
          (((GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START) &&   
           (kalmanvert.getVelocity() < GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD || kalmanvert.getVelocity() > GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD)) || 
           (!GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START))) {
//        variometerState = VARIOMETER_STATE_FLIGHT_STARTED;
        enableflightStartComponents();      
    }
  }
#endif // !HAVE_GPS

#if defined(HAVE_SCREEN) && defined(HAVE_VOLTAGE_DIVISOR) 
//  int tmpVoltage = analogRead(VOLTAGE_DIVISOR_PIN);
//  if (maxVoltage < tmpVoltage) {maxVoltage = tmpVoltage;}

      /* update battery level */
#if defined(VOLTAGE_DIVISOR_DEBUG)
    int val = adc1_get_raw(ADC1_CHANNEL_7);

    SerialPort.print("Tension : ");
    SerialPort.println(val);
#endif //VOLTAGE_DIVISOR_DEBUG
    
  if (displayLowUpdateState) screen.batLevel->setVoltage( analogRead(VOLTAGE_DIVISOR_PIN) );
//  batLevel.setVoltage( maxVoltage );
//  maxVoltage = 0;

#endif //HAVE_VOLTAGE_DIVISOR

  /**********************************/
  /* update low freq screen objects */
  /**********************************/
#ifdef HAVE_SCREEN

/************************************/
/* Update Time, duration            */
/* Voltage, SatLevel                */
/************************************/

#ifdef HAVE_GPS

  if (displayLowUpdateState) {
    if (nmeaParser.haveDate()) {
      
      /* set time */
#if defined(GPS_DEBUG) || defined(DATA_DEBUG)
      SerialPort.print("Time : ");
      SerialPort.println(nmeaParser.time);
#endif //GPS_DEBUG

      screen.screenTime->setTime( nmeaParser.time );
      screen.screenTime->correctTimeZone( GnuSettings.VARIOMETER_TIME_ZONE );
      screen.screenElapsedTime->setCurrentTime( screen.screenTime->getTime() );
      flystat.SetTime(screen.screenTime->getTime());
      flystat.SetDuration(screen.screenElapsedTime->getTime());      
    }
    
      /* update satelite count */
    screen.satLevel->setSatelliteCount( nmeaParser.satelliteCount );
#ifdef GPS_DEBUG
    SerialPort.print("Sat : ");
    SerialPort.println(nmeaParser.satelliteCount);
#endif //GPS_DEBUG
  }    
#endif //HAVE_GPS  

  /*****************/
  /* update screen */
  /*****************/
#ifdef HAVE_GPS
  /* when getting speed from gps, display speed and ratio */

  if ((variometerState >= VARIOMETER_STATE_DATE_RECORDED ) && ( nmeaParser.haveNewSpeedValue() )) {

    double currentSpeed = nmeaParser.getSpeed();
    double ratio = history.getGlideRatio(currentSpeed, serialNmea.getReceiveTimestamp(), GnuSettings.SETTINGS_GLIDE_RATIO_PERIOD_COUNT);

#if defined(GPS_DEBUG) || defined(DATA_DEBUG)
          SerialPort.print("GpsSpeed : ");
          SerialPort.println(currentSpeed);
#endif //GPS_DEBUG

     flystat.SetSpeed(currentSpeed);

    // display speed and ratio    
    if (currentSpeed > 99)      screen.speedDigit->setValue( 99 );
    else                        screen.speedDigit->setValue( currentSpeed );

    if ( currentSpeed >= GnuSettings.RATIO_MIN_SPEED && ratio >= 0.0 && ratio < GnuSettings.RATIO_MAX_VALUE  && displayLowUpdateState) {
      screen.ratioDigit->setValue(ratio);
    } else {
      screen.ratioDigit->setValue(0.0);
    }
  }
#endif //HAVE_GPS
/*  if( millis() - lastDisplayTimestamp > 1000 ) {

    lastDisplayTimestamp = millis();
    //Serial.println(intTW.lastTwError);
//    SerialPort.println(accel[2]);
    
#ifdef PROG_DEBUG
//    SerialPort.println("loop");

    SerialPort.print("Vario : ");
    SerialPort.println(kalmanvert.getVelocity());
#endif //PROG_DEBUG

#ifdef HAVE_SCREEN
    temprature += 0.1; //(temprature_sens_read() - 32) / 1.8;
    if (temprature > 99.99) temprature = 0; 
 
#ifdef PROG_DEBUG
    SerialPort.print("tenperature : ");
    SerialPort.print(temprature);
    SerialPort.println(" °C");
#endif //PRO_DEBBUG

    screen.tensionDigit->setValue(temprature);
    screen.tempratureDigit->setValue(0);
//   screen.updateData(DISPLAY_OBJECT_TEMPRATURE, temprature);
    screen.schedulerScreen->displayStep();
    screen.updateScreen(); 
#endif //HAVE_SCREEN

#ifdef HAVE_GPS    
    SerialPort.print("Time : ");
    SerialPort.println(nmeaParser.time);

    SerialPort.print("Sat : ");
    SerialPort.println(nmeaParser.satelliteCount);
    
    fileIgc.print(nmeaParser.time);
    fileIgc.print(" - ");
    fileIgc.println(kalmanvert.getVelocity());
#endif //HAVE_GPS*/

   if (displayLowUpdateState) {
      screen.recordIndicator->stateRECORD();
#ifdef PROG_DEBUG
      SerialPort.println("Record Indicator : staterecord ");
      SerialPort.print("VarioState : ");
      SerialPort.println(variometerState);
#endif //PROG_DEBUG
      
   }


   if (displayLowUpdateState) {
    
#ifdef PROG_DEBUG
      SerialPort.print("Temperature: ");
      // Convert raw temperature in F to Celsius degrees
      SerialPort.print((temprature_sens_read() - 32) / 1.8);
      SerialPort.println(" C");
#endif //PROG_DEBUG
  }
   
   displayLowUpdateState = false;

   screen.schedulerScreen->displayStep();
   screen.updateScreen(); 
#endif //HAVE_SCREEN

   flystat.Handle(); 
 // }
/*******************************/
/*      TEST TEST TEST         */

#ifdef HAVE_AUDIO_AMPLI
   toneHAL.enableAmpli();
#endif
  
/*******************************/
/*******************************/ 
}


/**************************************************/
#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
void createSDCardTrackFile(void) {
/**************************************************/  
  /* start the sdcard record */

#ifdef SDCARD_DEBUG
      SerialPort.println("createSDCardTrackFile : begin ");
#endif //SDCARD_DEBUG
 
  if( sdcardState == SDCARD_STATE_INITIALIZED ) {

#ifdef SDCARD_DEBUG
    SerialPort.println("createSDCardTrackFile : SDCARD_STATE_INITIALIZED ");
#endif //SDCARD_DEBUG

    flystat.Begin();
    uint8_t dateN[3];
    igcSD.CreateIgcFile(dateN,GnuSettings.NO_RECORD);

#ifdef SDCARD_DEBUG
    SerialPort.print("DateNum Gnuvario-E.ino : ");
#endif //SDCARD_DEBUG

    for(uint8_t i=0; i<3; i++) {
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
void enableflightStartComponents(void) {
/*******************************************/  

#ifdef PROG_DEBUG
  SerialPort.println("enableflightStartComponents ");
#endif //SDCARD_DEBUG

  variometerState = VARIOMETER_STATE_FLIGHT_STARTED;

  if (!GnuSettings.NO_RECORD) {
    
#ifdef HAVE_SPEAKER
    if (GnuSettings.ALARM_FLYBEGIN) {
      for( int i = 0; i<2; i++) {
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

  if (nmeaParser.haveDate()) {
      
      /* set time */
#if defined(GPS_DEBUG) || defined(DATA_DEBUG)
    SerialPort.print("Time : ");
    SerialPort.println(nmeaParser.time);
#endif //GPS_DEBUG

    screen.screenTime->setTime( nmeaParser.time );
    screen.screenTime->correctTimeZone( GnuSettings.VARIOMETER_TIME_ZONE );

    screen.screenElapsedTime->setBaseTime( screen.screenTime->getTime() );
#endif //defined(HAVE_SCREEN) && defined(HAVE_GPS)
  }

  /* enable near climbing */
#ifdef HAVE_SPEAKER
//#ifdef VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM
if (GnuSettings.VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM) {
  beeper.setGlidingAlarmState(true);
}
//#endif

//#ifdef VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP
if (GnuSettings.VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP) {
  beeper.setGlidingBeepState(true);
}
//#endif
#endif //HAVE_SPEAKER

#if defined(HAVE_SDCARD) && defined(HAVE_GPS) 
//&& defined(VARIOMETER_RECORD_WHEN_FLIGHT_START)
  if (GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START && (!GnuSettings.NO_RECORD)) {
  
#ifdef SDCARD_DEBUG
    SerialPort.println("createSDCardTrackFile");        
#endif //SDCARD_DEBUG

    createSDCardTrackFile();
  }
#endif // defined(HAVE_SDCARD) && defined(VARIOMETER_RECORD_WHEN_FLIGHT_START)

  if (!GnuSettings.NO_RECORD) {
#ifdef SDCARD_DEBUG
    SerialPort.println("Record Start");        
#endif //SDCARD_DEBUG

    screen.recordIndicator->setActifRECORD();
    screen.recordIndicator->stateRECORD();
  }
  else {
    screen.recordIndicator->setNoRECORD();
    screen.recordIndicator->stateRECORD();
  }
  flystat.Enable(); 
}

//$GNGGA,064607.000,4546.2282,N,00311.6590,E,1,05,2.6,412.0,M,0.0,M,,*77
//$GNRMC,064607.000,A,4546.2282,N,00311.6590,E,0.76,0.00,230619,,,A*7D
