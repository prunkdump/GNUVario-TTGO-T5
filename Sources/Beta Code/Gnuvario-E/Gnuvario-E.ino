#include <Arduino.h>
#include <DebugConfig.h>

#ifdef PROG_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>
//#include "myassert.h"

#if defined(ESP32)
//static const char* TAG = "GnuvarioE";
#include "esp_log.h"
#endif //ESP32

#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif //HAVE_SDCARD

#include <VarioSettings.h>

#include <VarioLog.h>

#ifdef HAVE_SPEAKER
#include <toneHAL.h>
#include <beeper.h>
#endif //HAVE_SPEAKER

#ifdef TWOWIRESCHEDULER
#include <IntTW.h>
#include <ms5611TW.h>
#include <vertaccel.h>
#include <LightInvensense.h>
#include <TwoWireScheduler.h>
#else
#include <MS5611-Ext.h>
#include <Wire.h>
#include <vertaccel2.h>
//#include <SparkFunMPU9250-DMP.h>
#include <MPU9250-DMP_SF_EXT.h>
#endif

#include <kalmanvert.h>

#ifdef HAVE_GPS
#include <SerialNmea.h>
#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <GPSSentence.h>
#else
#define SDCARD_STATE_INITIAL 0
#define SDCARD_STATE_INITIALIZED 1
#define SDCARD_STATE_READY 2
#define SDCARD_STATE_ERROR -1
#endif //HAVE_GPS

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
#include <AglManager.h>
#endif //HAVE_SDCARD && HAVE_GPS

#include <FlightHistory.h>
#include <variostat.h>
#include <VarioButton.h>

#include <Utility.h>

#include <driver/adc.h>

#include <Update.h>
#include <SD_Update.h>

#ifdef HAVE_WIFI
#include <VarioWifiServer.h>
#include <esp32fota2.h>
#endif //HAVE_WIFI

#ifdef HAVE_BLUETOOTH
#include "SimpleBLE.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

SimpleBLE ble;
#endif //HAVE_BLUETOOTH

#include <SysCall.h>

/*******************/
/* Version         */
/*******************/

#define VERSION      0
#define SUB_VERSION  7
#define BETA_CODE    8
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
* v 0.5     beta 7  10/09/19         Ajout de SDA_Pin et SCL_Pin                                      *                                                                 
*                                    Modification des librairies du MPU9250 / ajout fonctions de      *                                                                             
*                                    Calibration                                                      *
*                                    Modification de la sequence de démarrage -                       *
*                                    allongement du temps de l'écran de stat à 6 sec                  *
*                                    init MS5611 avant ecran stat, ajout acquisition durant ecran     *
*                                    stat et init kalman après                                        *
*                                    Ajout d'un paramettre de nombre d'acquisition du GPS avant       * 
*                                    la mise à jour de Altitude Barametrique                          *
*                                    Modification librairie EEPROM                                    *
*                                    Ajout de la lecture de l'alti baro durant l'attente de l'écran   *
*                                    de stat                                                          *
* v 0.5   beta 8  22/09/19           Modification de l'affichage de Vbat                              *
*                                    Ajout d'un filtrage de la mesure de Vbat                         *
*                                    Ajout deep_sleep en cas de batterie trop faible                  *
* v 0.5   beta 9  23/09/19           Correction affichage ecran Stat                                  *                                    
*                                    Ajout possibilité d'avoir plusieurs tailles d'écran              *
*                                    Deep-sleep sur bouton central (3 sec)                            *
* v 0.6   beta 1  26/09/19           Ajout mise en sommeil                                            *                                   
*                                    Flash via USB en utilisant flash_download_tools                  *
*                                    Ajout ecran calibration                                          *
*                                    Modification SETTING.TXT (Version 1.3)                           *
*                                    Correction HAVE_SDCARD                                           *
*                                    Changement de la librairie webserver                             *
* v 0.6   beta 2  06/10/19           Ajout varioscreen pour l'écran 2.9''                             *
*                                    Ajout serveur Web sur sdcard                                     *
*                                    Correction du bug d'affichage des fleches                        *
*                                    Ajout du nom des champs de données sur l'écran                   *
*                                    Couper le SSID à 12 caractères                                   *                     
*                                    Modification du fichier SETTINGS.TXT v1.4                        *                                                                                                    
*                                    Ajout paramettre d'activation du BT dans le fichier SETTINGS.TXT *
*                                    Modification de varioscreen pour gérer les lignes et les cercles *
*                                    ainsi que les titres des champs de donnée                        *
*                                    Modification de l'arborescence de la carte SD                    *           
*                                    Enregistrement des fichiers .IGC (traces) dans le dossier 'vols' *
*                                    Mise à jour Ecran 2,9''                                          *
*                                    Modifier l'affichage des titres pour que la position             *
*                                    soit liée au digit                                               *
* v 0.6   beta 3  20/10/19           Correction bug enregistrement sur la SD                          *
*                                    recodage de l'affichage des satellites                           *       
*                                    Ajout desactivation bip au demarrage                             * 
*                                    Ajout écran 2.13''                                               *
*                                    Ajout compensation de température et correction altitude GPS     *
*                                    dans le fichier SETTINGS.TXT                                     *
* v 0.6   beta 4  27/10/19           Correction gestion zone horaire                                  *
*                                    Changement de librairie MS5611 et MPU                            *
*                                    Ajout compensation alti gps                                      *
*                                    Ajout d'un bip lors du reglage du volume                         *
*                                    Amélioration gestion des boutons et des écrans au boot           *
*                                    Correction gestion du volume                                     *
*                                    Correction Calibration enableAmp                                 *
*                                    Correction du bug d'affichage de l'écran de stat                 *
*                                    Correction du bug de reboot / blocage si #define DEBUG_PRO est   *      
*                                    commenté                                                         *
* v 0.6  beta 5 03/11/19             Déplacement validation deep-sleep sur bouton gauche              *
*                                    Corrections mineures sur varioscreen 2.9''                       *
*                                    Corrections mineures sur varioscreen 1.54''                      *  
*                                    Amélioration de la gestion de la première mesure d'altitude      *
*                                    Correction du bug de l'écran qui fige                            *
*                                    Moyenne alti Gps sur 15 mesures avant calibration baro           *
* v 0.6  beta 6 11/11/19             plusieurs fichiers de configurations / params.jso                *
*                                    variocal.cfg, wifi.cfg                                           *
*                                    Ajout gestion des versions params.jso automatique / création     *
*                                    automatique des champs recup données                             *
*                                    Correction bug volume                                            *
*                                    AJOUT - dans params.jso - MULTIDISPLAY_DURATION et               *
*                                    plusieurs voiles                                                 *
* v0.6  beta 7  17/11/19             Mise à jour ecran 2.9''                                          *
*                                    Mise à jour fichier params.jso                                   *
*                                    Mise à jour page web                                             *
*                                    Correction Bug serveurWeb                                        *
*                                    Correction Bug deep sleep                                        *
*                                    Ajout TRACE                                                      *
*                                    Ajout Mode veille paramètrable                                   *
*                                    Ajout Passage en veille en cas d'inactivité                      *
* v0.7  beta 1  28/11/19             Changement librairie Sdcard - SdFat                              *
*                                    Changement librairie Webserveur par VarioWebServeur              * 
*                                    Renomage WifiServeur en VarioWifiServer                          *
* v0.7  beta 2  15/12/19             Recodage updateSD - correction bug                               *                                    
*                                    Correction MPU                                                   *
*                                    Ajout variiolog                                                  *
*                                    AJOUT - gestion du MPU par Interruption                          *
*                                    Correction BUG reboot à l'init du MPU  - nan lors du first alti  *
*                                    Ajout logger sur SDcard                                          *
* v0.7  beta 3  22/12/19             Mise à jour librarie MPU                                         *
*                                    Correction Update webserver                                      *
*                                    Ajout Log sur SDcard                                             *
* v0.7  beta 4  29/12/19             Modif MPU en mode INT - résolution bug                           *
*                                    Modif screen 2.90'' et 2.13''                                    *
*                                    Correction bug ecran figé                                        *
*                                    Finesse bug résolu                                               *
*                                    Update webszerver OK                                             *
*                                    Gestion des fichiers de log                                      *
*                                    Correction norcissement de l'écran - display.poweroff            *
* v0.7  beta 5  09/01/20             Correction bug d'affichage vario / alti -                        *
*                                    raffraichissement > 10 ms                                        * 
*                                    Ajout direction via GPS - degré et texte                         *
* v0.7  beta 6  11/01/20             Ajout DISPLAY_LOW_UPDATE et DISPLAY_UPDATE                       *
*                                    Correction de bug d'affichage mineur                             *
* v0.7  beta 7  17/01/20             Correction problème affichage Alti quand on sort du deep sleep   *                                                               
*                                    Desactivation effacement de l'écran toute les 30 sec             * 
*                                    ligne 1427 varioscreenGxEPD_154.cpp et                           *
*                                    varioscreenGxEPD_290.cpp (ligne 1534)                            *
*                                    Ajout DISPLAY_STAT_DURATION - passage en v1.1 du fichier         * 
*                                    param.json - Configuration du delai d'affichage de l'écran de    *
*                                    statistique                                                      *
*                                    Correction Probleme consommation - SDcard - deep slepp           *
*                                    Modification affichage deep_sleep                                *
*                                    Correction upload wifi                                           *
*                                    Nouvel écran vol dans la page web                                *
*                                    Ajout de la configuration du delai de la page stat de démarrage  *
*                                    Ajout librairie Esp32Fota                                        *
*                                    Ajout décodage lat / long GPS                                    *
*                                    Ajout écran d'info lors de l'upgrade via la sdcard               *
* v0.7  beta 8  31/01/20             Correction mise à jour via internet                              *
*                                    Passage à la librairie varioWebserver                            *
*                                    Ajout de la barre de status sur l'écran 2                        *
*                                    Ajout Affichage du cap, longitude et latitude sur l'écran 2      *
*                                    Amélioration de la page de vol (serveur Web)                     *
*                                    Ajout écran info lors de l'update via internet                   *
*                                    Nettoyage des librairie et compatibilité avec vscode             *
*                                    Ajout Update via site Internet                                   *
*                                    Correction bug, seul de monté/descente sur la SDcard non pris en *
*                                    compte                                                           *
*               10/02/20             Correction ecran 2                                               *
*               12/02/20             Correction affichage lat / long sur écran 2                      *
*                                    Correction time out Gxepd à l'affichage de l'écran de stat       *
*               15/02/20             Correction BUG seuils de monte / descente non pris en compte     *
*                                    ajout beeper.init                                                *
*                                    Correction Bug rotation du fichier de log                        *
*                                    Mise à jour via internet - site Web + firmware                   *
*               19/02/20             Ajout gestion écran 2.90'' en mode portrait                      *
*               21/02/20             Correction Bug affichage batterie                                *
*                                    Correction rotation fichier log                                  *
*               25/02/20             Correction precision Lat/long                                    *                     
*                                    Ajout ScreenBackground                                           *
*                                    Correction Bug   - Affichage long - lat                          *                     
*                                    reccupération long / lat sur 6 ou 7 digits après la virgule      *
*               28/02/20             Correction calcul lat/long                                       *
*               29/02/20             Modification Mise à jour auto internet                           *
*                                    Nouveau site web embarqué                                        *
*                                    Mise à jour calibration                                          *
*               01/03/20             Correction affichage lat/long                                    *
*               04/03/20             Ajout AGL                                                        *
*               05/03/20             Correction ecran qui clignote                                    *
*                                    AJOUT - Champs enable AGL                                        *
*               07/03/20             Correction gestion AGL                                           *
*                                    Correction screenGxEpd2 - xSemaphore                             *
*               08/03/20             Mise à jour GxEpd2                                               *
*               09/03/20             Correction bug ViewSound                                         *
*                                    correction bug effacement GR / TR                                *
*                                    correction Bug affichage titre GR/TR                             *
*                                    Calibration manuel du baromètre via l'AGL                        *
*                                    Déclenchement manuel de l'enregistrement du vol                  *
*               14/03/20             Calibration de compensation du GPS via l'AGL                     *
*******************************************************************************************************
*                                                                                                     *
*                                   Developpement a venir                                             *
*                                                                                                     *
* v0.6                                                                                                *   
* MODIF - Refaire gestion Eeprom avec preference                                                      *
* BUG   - temperature                                                                                 *
* BUG   - DISPLAY_OBJECT_LINE object ligne ne fonctionne pas                                          *
*                                                                                                     *
* v0.7                                                                                                *
* BUG   - upload wifi - ne se termine pas  - bug espressif le buffer n'est pas vidé à la fin          *
* BUG   - update manuelle - doit être lancée 2 fois                                                   *
* BUG   - download à verifier                                                                         *
* BUG   - stat affichage temps de vol                                                                 *
* VERIF - Seuil déclenchement début du vol                                                            *
* VERIF - Sensibilité du vario                                                                        *
* BUG   - blocage du MPU                                                                              *
* AJOUT - effacement ecran 1 fois / min                                                               *
*                                                                                                     *        
* v0.8                                                                                                *       
* MODIF - Réecrire loop                                                                               *
* AJOUT - Récupération du cap depuis le capteur baromètrique                                          *
* AJOUT - Espaces aeriens                                                                             *
*                                                                                                     *
* VX.X                                                                                                *
* Paramètrage des écrans                                                                              *
* Gérer le son via le DAC                                                                             *
* revoir volume du son ToneESP32                                                                      *
* Refaire gestion du son (parametrage via xctracer)                                                   *
* Boussole graphique                                                                                  *                                                                       
* Création dynamique des objets screen                                                                *
* Sens et vitesse du vent                                                                             *
* Carnet de vol (10 derniers vols)                                                                    *
*     10 zones d'eeprom - reduit le nombre d'écriture et économise la mémoire flash                   *
* verifier fonctionnement BT                                                                          *
* Recupération vol via USB                                                                            *                                                                                        
* Espaces aeriens                                                                                     *
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
 * - Mise en veille automatique en cas de batterie trop faible          *
 *                                                                      *
 * Version 0.6                                                          *
 * - Page de calibration                                                * 
 * - Mise en veille prolongée                                           *
 * - Ajout gestion des écran 2.9'' et 2.13''                            *
 * - Ajout du serveur Web sur SDCARD                                    *
 * - Activation du Bluetooth en parametre dans le fichier SETTINGS.TXT  *
 * - Ajout de titre au dessus des champs de données                     *
 * - Ajout de nouveau objects screen - ligne - rose des vents           *
 * - Modification de l'organisation des fichiers sur la carte SD        *
 * - Ajout compensation de température dans fichier SETTINGS.TXT        *
 * - Ajout correction d'altitude GPS dans SETTINGS.TXT                  *
 * - Modification calibration alti GPS - 5 mesures puis moyenne de      *
 * - l'altitude sur 10 mesures avant de calibrer le barometre           * 
 * - 3 fichiers de paramètrage params.jso, wifi.cfg, variocal.cfg       *  
 * - Gestion automatique de la mise à jour du fichier params.jso en cas *
 *   d'ajout ou de suppréssion de champs                                *
 * - Mise en veille en cas d'inactivité, paramètrable - 0 infini        *
 *                                                                      *
 * Version 0.7                                                          *                                                                     
 * - Ajout de fichiers de log                                           *
 * - Ajout configuration du delai d'affichage de l'écran de stat au     *
 *   démmarage                                                          *
 * - Ajout Update via internet                                          *
 * - Ajout affichage du cap, longitude, latitude                        *
 * - Calibration des accèlerometres                                     *
 * - AGL                                                                *
 * - Calibration manuel du baro via l'AGL                               *
 * - Declenchement de vol manuellement                                  *
 * - Compensation du GPS via l'AGL                                      *
 *                                                                      *
 ************************************************************************/
 
/************************************************************************
*                          Recommandation                               *    
 *                                                                      *
 * Dans DebugConfig.h commanter #define ENABLE_DEBUG pour desactiver    *                                                                      
 *                    les message de debug et éviter certains           *
 *                    ralentissements                                   *
 ************************************************************************/

/************************************************************************
*                        Compilation                                    *
*                                                                       *
* Le code du Gnuvario-E est prévu pour fonctionner sur une board TTGO-T5*
* à base d'ESP32                                                        *
*                                                                       *
* https://dl.espressif.com/dl/package_esp32_index.json                  *
*                                                                       *
* version beta                                                          *
* https://dl.espressif.com/dl/package_esp32_dev_index.json              *
*                                                                       *
* Installer la carte Esp32 by Espressif system                          * 
*                                                                       *
* Type de carte    : ESP32 dev Module                                   *
* CPU frequency    : 240Mhz                                             *
* Partition scheme : Minimal SPIFFS (1.9Mb APP with OTA / 190kb spiffs) *
*************************************************************************/

/************************************************************************
*               Fichiers de configuration                               *
*                                                                       *
* params.jso               parametres utilisateur                       *
* wifi.cfg                 paramètres de configuration du wifi          *
* log.cfg                  paramètres de configuration du fichier de log*
* variocal.cfg             paramètres de calibration du MPU             *
* record00cal.igc          fichier contenant les mesures utiles au      *
*                          calcul de la calibration                     *
* HardwareConfig.h         parametres matériels communs                 *
* HardwareConfigESP32.h    parametre spécifique à l'ESP32               *
* DebugConfig.h            parametre de debuggage                       *
* VarioSettings.h          parametres par defaut (utilisé si il n'y a   *
*                          pas de SDcard ou de fichier params.jso       *
*************************************************************************/

/***********************************************************************
*               arborescence de la SDCARD                              *
*                                                                      *
*               /                                                      *
*               LOGS/                   - repertoire de log            *                                                       
*                      GNUVARIO.LOG     - Fichier de log               *
*               RECORD00.CAL            - fichier de calibration       *
*               PARAMS.JSO              - fichier de configuration     *
*               LOG.CFG                 - Paramètres de debug          *
*               VARIOCAL.CFG            - Paramètres de calibration    *
*               WIFI.CFG                - Paramètres Wifi              *
*               VOLS/                   - repertoire des traces        *
*                       19101200.IGC                                   *
*                       19101201.IGC                                   *
*               WWW/                    - repertoire du site Web       *
*                       INDEX.HTM                                      *
*                       INDEXB.JS                                      *
*                       INDEXB~1.MAP                                   *
*                                                                      *
************************************************************************/

/**************************************************************************************************************
 *              Liens utiles                                                                                  *
 *                                                                                                            *
 * https://learn.sparkfun.com/tutorials/9dof-razor-imu-m0-hookup-guide/all#libraries-and-example-firmware     *        
 *                                                                                                            *
 **************************************************************************************************************/

 /*************************************************************************************************************
  *                                                                                                           *
  *                                            UTILISATION DES TOUCHES                                        *                                                                                       
  *   Ecran         Touche      Fonction                                                                      *
  *                                                                                                           *
  *   Init          Gauche      passage en mode Wifi                                                          *                                                                                                              
  *   Init          Droite      Calibration                                                                   *
  *                                                                                                           *
  *   Vario         Centre      Coupe et remet le son (Mute)                                                  *
  *   Vario         Centre 3s   Mode veille                                                                   *
  *   Vario         Gauche      écran précédent                                                               *
  *   Vario         Droite      écran suivant                                                                 *
  *   Vario         Gauche 2s   Calibrarion manuel du baro via l'AGL                                          *
  *   Vario         Gauche 0    Déclenchement de l'enregistrement du vol si en attente                        *
  *                                                                                                           *
  *   Wifi          Gauche      Sort du mode Wifi                                                             *
  *                                                                                                           *
  *   Sound         Gauche      baisse le volume                                                              *
  *   Sound         Droit       Monte le volume                                                               *
  *   Sound         Centre      Entre dans la configuration / Valide la configuration                         *
  *                                                                                                           *
  *   Sleep         Gauche      Valide la mise en veille                                                      * 
  *                                                                                                           *
  *   Calibration   Centre      Démarre la calibration                                                        *                                                                                                        
  *   Calibration   Gauche      Sort du mode calibration (reboot)                                             *
  *                                                                                                           *
  *************************************************************************************************************
  *                                                                                                           *
  * Mise à jour via la carte SD, nom du fichier : update.bin                                                  *                                                                                                           
  *                                                                                                           *
  *************************************************************************************************************
  *                                                                                                           *
  * Debbug                                                                                                    *
  *                                                                                                           *
  * TRACE();                  Affiche sur le moniteur serie le numero de ligne, le nom du fichier             *
  * DUMP(someValue);          Affcihe sur le moniteur serie la variable ainsi que le fichier et la ligne      *
  * SDUMP(someText);          Affiche sur le moniteur serie le texte ainsi que le fichier et la ligne         *
  *                                                                                                           *
  * TRACELOG(type, module)           Enregistre dans le fichier de log le fichier et la ligne                 *
  * DUMPLOG(type, module, variable)  Enregistre dans le fichier de log la variable, le fichier et la ligne    * 
  * MESSLOG(type, module, Text)      Enregistre dans le fichier de log un message avac la fichier et la ligne *
  * INFOLOG(Text)                    Enregistre dans le fichier de log un texte                               *                                                                                                     
  *************************************************************************************************************/  

//#define TEST_SD

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

Kalmanvert kalmanvert;

/**********************/
/* SDCARD objects     */
/**********************/

int8_t sdcardState = SDCARD_STATE_INITIAL;

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

#ifdef AGL_MANAGER_H
  AglManager aglManager;
#endif

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

unsigned long lastDisplayTimestamp, time_deep_sleep, sleepTimeoutSecs, lastDisplayTimestamp2;
boolean displayLowUpdateState=true;
boolean displayUpdateState=true;

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
#ifdef ESP32WEBSERVEUR 
  VarioESP32WebServer server(80);
#elif defined(ESPASYNCWEBSERVER)
  AsyncWebServer server(80); 
#elif defined(ETHERNETWEBSERVER)
  EthernetServer server(80); 
#elif defined(ESPRESSIFWEBSERVEUR)
  WebServer server(80);    
#else //ESP32WEBSERVEUR
  VarioWebServer server(80);
#endif //ESP32WEBSERVEUR
#endif

esp32FOTA2 esp32FOTA("Gnuvario" + String(VARIOSCREEN_SIZE), VERSION, SUB_VERSION , BETA_CODE);   //esp32-fota-http", 0,6,0);

#endif //HAVE_WIFI



/* *************************************************
Internal TEMPERATURE Sensor
   *************************************************/

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
double gpsAlti = 0;

long MaxVoltage   = 0;

long compteurInt = 0;

int compteurBoucle = 0;

void IRAM_ATTR isr() {
  compteurInt++;  
}

//****************************
//****************************
void setup() {
//****************************
//****************************  

  SerialPort.begin(115200);

  // Wait for USB Serial
  while (!SerialPort) {
    SysCall::yield();
  }

// *******************************************************  
// *   
// ******************************************************* 
#ifdef TEST_SD
  delay(5000);
  TestSDCARD(true);
#endif
 
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
  sprintf(tmpbuffer,"GNUVARIO compiled on %s", __DATE__); // at %s", __DATE__, __TIME__);
  SerialPort.println(tmpbuffer);
  sprintf(tmpbuffer,"VERSION %i.%i - %s", VERSION,SUB_VERSION,DEVNAME); 
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
  //beeper.generateTone(2000,300); 
#endif

  /******************/
  /* Init SDCARD    */
  /******************/

#ifdef HAVE_SDCARD

#ifdef TEST_SD
  if (GnuSettings.initSettings(false)) {
#else 
  if (GnuSettings.initSettings(true)) {
#endif
  
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

    GnuSettings.setVersion(VERSION, SUB_VERSION, BETA_CODE);

    SerialPort.println("Chargement des parametres depuis le fichier params.jso");
    char tmpchar[20] = "params.jso";
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
    GnuSettings.VARIOMETER_PILOT_NAME.toCharArray(__dataPilotName, sizeof(__dataPilotName)+1);

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
    GnuSettings.VARIOMETER_GLIDER_NAME.toCharArray(__dataGliderName, sizeof(__dataGliderName)+1);

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

    boolean ModifValue = false;
    char tmpFileName[15] = "wifi.cfg";
   
    if (SDHAL_SD.exists(tmpFileName)) {
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
    
    strcpy(tmpFileName,"variocal.cfg");
   
    if (SDHAL_SD.exists(tmpFileName)) {
      GnuSettings.readSDSettings(tmpFileName, &ModifValue);
    }
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
    if (GnuSettings.ALARM_SDCARD) {
      indicateFaultSDCARD();
    }
#endif //HAVE_SPEAKER 
#endif //HAVE_SDCARD

/*  uint8_t tmp[4];
  tmp[0]=1;
  tmp[1]=1;
  tmp[2]=19;
  tmp[3]=00;

  flystat.SetDate(tmp);
  flystat.ForceWrite();*/

#ifdef HAVE_SPEAKER
  if (GnuSettings.ALARM_VARIOBEGIN) beeper.generateTone(2000,300); 
/*  beeper.setVolume(GnuSettings.VARIOMETER_BEEP_VOLUME);
  toneHAL.setVolume(GnuSettings.VARIOMETER_BEEP_VOLUME);*/
#endif //HAVE_SPEAKER

/*********************/
/*    init logger    */
/*********************/

  varioLog.init();
  String tmpStr;
  tmpStr =   "FirmWare : " + String(VERSION) + "." + String(SUB_VERSION); 
  if (BETA_CODE > 0) tmpStr = tmpStr + "b" + String(BETA_CODE);

  INFOLOG(tmpStr);
  TRACELOG(LOG_TYPE_DEBUG, MAIN_DEBUG_LOG);

//***********************************************
// INIT AGL
//***********************************************

  aglManager.init();
  
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

/********************/
/** Update Firmware */
/********************/
 
#ifdef HAVE_SDCARD
  updateFromSDCARD();
#endif //HAVE_SDCARD


/*****************************/
/** Update Site web embarqué */
/*****************************/

#ifdef HAVE_SDCARD
  esp32FOTA.UpdateWwwDirectory();
#endif //HAVE_SDCARD

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

//***********************************************
// INIT Sound
//      init Beeper avec les valeurs personnelles
//      init Volume
//***********************************************

#ifdef HAVE_SPEAKER
  beeper.init(GnuSettings.VARIOMETER_SINKING_THRESHOLD, GnuSettings.VARIOMETER_CLIMBING_THRESHOLD, GnuSettings.VARIOMETER_NEAR_CLIMBING_SENSITIVITY);

  GnuSettings.VARIOMETER_BEEP_VOLUME = GnuSettings.soundSettingRead();
  beeper.setVolume(GnuSettings.VARIOMETER_BEEP_VOLUME);
  toneHAL.setVolume(GnuSettings.VARIOMETER_BEEP_VOLUME);

#ifdef SOUND_DEBUG
  SerialPort.print("Volume SOUND : ");
  SerialPort.println(GnuSettings.VARIOMETER_BEEP_VOLUME);
#endif //SOUND_DEBUG

#endif //HAVE_SPEAKER

  
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

#if defined(VARIO_SDA_PIN) && defined(VARIO_SCL_PIN)
  while(!ms5611.begin(VARIO_SDA_PIN, VARIO_SCL_PIN))
#else
  while(!ms5611.begin())
#endif
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
#if defined(VARIO_SDA_PIN) && defined(VARIO_SCL_PIN)
  if (imu.begin(VARIO_SDA_PIN, VARIO_SCL_PIN) != INV_SUCCESS)
#else
  if (imu.begin() != INV_SUCCESS)
#endif

  {
    while (1)
    {
      SerialPort.println("Unable to communicate with MPU-9250");
      SerialPort.println("device error");
      ESP_LOGE(TAG, "Erreur capteur MPU9250 introuvable");
      while(1);
    }
  }

  // Use setSensors to turn on or off MPU-9250 sensors.
  // Any of the following defines can be combined:
  // INV_XYZ_GYRO, INV_XYZ_ACCEL, INV_XYZ_COMPASS,
  // INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
  // Enable all sensors:
  imu.setSensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);

/*  // Use setGyroFSR() and setAccelFSR() to configure the
  // gyroscope and accelerometer full scale ranges.
  // Gyro options are +/- 250, 500, 1000, or 2000 dps
  imu.setGyroFSR(2000); // Set gyro to 2000 dps
  // Accel options are +/- 2, 4, 8, or 16 g
  imu.setAccelFSR(8); // Set accel to +/-2g
  // Note: the MPU-9250's magnetometer FSR is set at 
  // +/- 4912 uT (micro-tesla's)

  // setLPF() can be used to set the digital low-pass filter
  // of the accelerometer and gyroscope.
  // Can be any of the following: 188, 98, 42, 20, 10, 5
  // (values are in Hz).
  imu.setLPF(5); // Set LPF corner frequency to 5Hz*/

  // The sample rate of the accel/gyro can be set using
  // setSampleRate. Acceptable values range from 4Hz to 1kHz
  imu.setSampleRate(100); // Set sample rate to 10Hz

  // Likewise, the compass (magnetometer) sample rate can be
  // set using the setCompassSampleRate() function.
  // This value can range between: 1-100Hz
  imu.setCompassSampleRate(100); // Set mag rate to 10Hz
  
/*  imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | // Enable 6-axis quat
               DMP_FEATURE_GYRO_CAL, // Use gyro calibration
              10); // Set DMP FIFO rate to 10 Hz
  // DMP_FEATURE_LP_QUAT can also be used. It uses the 
  // accelerometer in low-power mode to estimate quat's.
  // DMP_FEATURE_LP_QUAT and 6X_LP_QUAT are mutually exclusive*/

    imu.dmpBegin(DMP_FEATURE_SEND_RAW_ACCEL | // Send accelerometer data
//                 DMP_FEATURE_SEND_RAW_GYRO  | // Send raw gyroscope values to FIFO
                 DMP_FEATURE_GYRO_CAL       | // Calibrate the gyro data
                 DMP_FEATURE_SEND_CAL_GYRO  | // Send calibrated gyro data
                 DMP_FEATURE_6X_LP_QUAT     , // Calculate quat's with accel/gyro
                 100);                         // Set update rate to 10Hz.
  
#endif //HAVE_ACCELEROMETER

#endif //TWOWIRESCHEDULER

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
    for (int i=0;i<4;i++) {
      delay(1000);

#ifdef TWOWIRESCHEDULER
  /* init kalman filter with 0.0 accel*/
      firstAlti = twScheduler.getAlti();
#else //TWOWIRESCHEDULER
      firstAlti = ms5611.readPressure();
#endif //TWOWIRESCHEDULER

      if (!isnan(firstAlti)) break;   
    }
  }
  
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

  //Calibration
  if (ButtonScheduleur.Get_StatePage() == STATE_PAGE_CALIBRATION) screen.ScreenViewMessage("Calibration",5);

#ifdef HAVE_SCREEN
// Affichage Statistique
  flystat.Display();
  screen.ScreenViewStat();


  unsigned long TmplastDisplayTimestamp = millis();
  int compteur = 0;
  while (compteur < GnuSettings.DISPLAY_STAT_DURATION) {
          
    if( millis() - TmplastDisplayTimestamp > 1000 ) {

      TmplastDisplayTimestamp = millis();
      compteur++;

//    Messure d'altitude
#if not defined (TWOWIRESCHEDULER)
      firstAlti = ms5611.readPressure();
#endif //TWOWIRESCHEDULER
    
    }
  }
  
#endif //HAVE_SCREEN

  kalmanvert.init(firstAlti,
                  0.0,
                  POSITION_MEASURE_STANDARD_DEVIATION,
                  ACCELERATION_MEASURE_STANDARD_DEVIATION,
                  millis());

#ifdef KALMAN_DEBUG
  SerialPort.println("kalman init");
#endif //KALMAN_DEBUG
#endif //HAVE_ACCELEROMETER


    TRACE();
    SDUMP("Test INT MPU");




/*  
 *   
       
 ******************************************************      
 *      TEST INT MPU
 *****************************************************      
       
  pinMode(VARIO_MPUINT_PIN, INPUT_PULLUP);   //INPUT);

  attachInterrupt(VARIO_MPUINT_PIN, isr, FALLING);
   
  while (1) {
    SerialPort.print("valeur : ");
    SerialPort.println(compteurInt);
    delay(1);        // delay in between reads for stability
  }

*/





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

#ifdef SOUND_DEBUG
  SerialPort.print("ToneHal Volume Sound : ");
  SerialPort.println(toneHAL.getVolume()); //GnuSettings.VARIOMETER_BEEP_VOLUME);
#endif //SOUND_DEBUG

#ifdef SCREEN_DEBUG
  SerialPort.println("update screen");
#endif //SCREEN_DEBUG

  screen.clearScreen();
  screen.schedulerScreen->enableShow();
#endif //HAVE_SCREEN

#ifdef HAVE_BLUETOOTH
  if (GnuSettings.VARIOMETER_ENABLE_BT) {
#ifdef BT_DEBUG
    SerialPort.setDebugOutput(true);
//    pinMode(0, INPUT_PULLUP);
    SerialPort.print("ESP32 SDK: ");
    SerialPort.println(ESP.getSdkVersion());
#endif //BT_DEBUG
    ble.begin("GnuVario-E");
    screen.btinfo->setBT();
  }
#endif //HAVE_BLUETOOTH

  ButtonScheduleur.Set_StatePage(STATE_PAGE_VARIO);
  /* init time */
  lastDisplayTimestamp  = millis(); 
  lastDisplayTimestamp2 = millis();
  time_deep_sleep       = lastDisplayTimestamp;
  sleepTimeoutSecs      = lastDisplayTimestamp;
  displayLowUpdateState = true; 
  displayUpdateState = true;
  MaxVoltage   = 0; 
}

double temprature=0;
double currentHeight = 0;
int    compteurErrorMPU = 0;

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
void createSDCardTrackFile(void);
#endif //defined(HAVE_SDCARD) && defined(HAVE_GPS)
void enableflightStartComponents(void);

//*****************************
//*****************************
void loop() {
//****************************  
//****************************

 /* if( vertaccel.readRawAccel(accel, quat) ){
    count++;
  }*/

/*  LOW UPDATE DISPLAY */
   if( millis() - lastDisplayTimestamp > DISPLAY_LOW_UPDATE ) {

     lastDisplayTimestamp = millis();
     displayLowUpdateState = true;
 /*    if (tmpint == 0) tmpint = 1000;
     else             tmpint = 0;*/
   }

// DISPLAY
   if( millis() - lastDisplayTimestamp2 > DISPLAY_UPDATE ) {

     lastDisplayTimestamp2 = millis();
     displayUpdateState = true;
 /*    if (tmpint == 0) tmpint = 1000;
     else             tmpint = 0;*/
   }

//**********************************************************
//  TRAITEMENT APPUIE SUR LES BOUTONS
//**********************************************************

/*******************************/
/*  Compute button             */
/*******************************/

  ButtonScheduleur.update();

  /*****************************/
  /* compute vertical velocity */
  /*****************************/

//**********************************************************
//  ACQUISITION DES DONNEES
//**********************************************************
#ifdef HAVE_ACCELEROMETER
#ifdef TWOWIRESCHEDULER
  if( twScheduler.havePressure() && twScheduler.haveAccel() ) {

    compteurErrorMPU = 0;
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
    tmpTemp += GnuSettings.COMPENSATION_TEMP; //MPU_COMP_TEMP;

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
//    DUMPLOG(LOG_TYPE_DEBUG,MS5611_DEBUG_LOG,realPressure);
    tmpAlti = ms5611.getAltitude(realPressure);
//    DUMPLOG(LOG_TYPE_DEBUG,MS5611_DEBUG_LOG,tmpAlti);
    tmpTemp = ms5611.readTemperature();
 //   DUMPLOG(LOG_TYPE_DEBUG,MS5611_DEBUG_LOG,tmpTemp);
    tmpTemp += MPU_COMP_TEMP;
//    DUMPLOG(LOG_TYPE_DEBUG,MS5611_DEBUG_LOG,tmpTemp);

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
  
 #ifdef PROG_DEBUG
    //SerialPort.println("Kalman Update");
#endif //PROG_DEBUG
 
    if (displayLowUpdateState) {
//      screen.tempDigit->setValue(tmpTemp);
//      screen.tunit->toDisplay();
    }

//**********************************************************
//  UPDATE BEEPER
//**********************************************************

#ifdef HAVE_SPEAKER
		beeper.setVelocity( kalmanvert.getVelocity() );
#endif //HAVE_SPEAKER

//**********************************************************
//  TEST INNACTIVITE
//**********************************************************

   if (abs(kalmanvert.getVelocity()) > GnuSettings.SLEEP_THRESHOLD_CPS) { 
     // reset sleep timeout watchdog if there is significant vertical motion
     sleepTimeoutSecs = millis();

   }
   else
   if ((GnuSettings.SLEEP_THRESHOLD_CPS != 0) && ((millis()-sleepTimeoutSecs) >= (GnuSettings.SLEEP_TIMEOUT_MINUTES*60*1000))) {
#ifdef MAIN_DEBUG       
     SerialPort.println("Timed out with no significant climb/sink, put MPU9250 and ESP8266 to sleep to minimize current draw");
     SerialPort.flush();
#endif          
     indicatePowerDown(); 
//     TRACELOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG);
     MESSLOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG,"Deep sleep - inactivite");
     deep_sleep("Power off");
   }   

//**********************************************************
//  TRAITEMENT DES DONNEES
//**********************************************************

   /* set history */
#if defined(HAVE_GPS) 
    if ((GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)	|| (GnuSettings.RATIO_CLIMB_RATE > 1)) history.setAlti(kalmanvert.getCalibratedPosition(), millis());
#endif

		double currentalti  = kalmanvert.getCalibratedPosition();
		double currentvario = kalmanvert.getVelocity();
//    DUMPLOG(LOG_TYPE_DEBUG,KALMAN_DEBUG_LOG,currentalti);
//    DUMPLOG(LOG_TYPE_DEBUG,KALMAN_DEBUG_LOG,currentvario);

#ifdef DATA_DEBUG
    SerialPort.print("Kalman Alti : ");
    SerialPort.println(currentalti);
    SerialPort.print("Kalman Vario : ");
    SerialPort.println(currentvario);
#endif //DATA_DEBUG

    /* set screen */

//**********************************************************
//  MAJ STATISTIQUE
//**********************************************************

    flystat.SetAlti(currentalti);
    flystat.SetVario(currentvario);

#ifdef HAVE_SCREEN

//**********************************************************
//  DISPLAY ALTI
//**********************************************************

#ifdef DATA_DEBUG
 //   SerialPort.print("altitude : ");
 //   SerialPort.println(currentalti);
#endif //DATA_DEBUG

    if (displayLowUpdateState) {
      screen.altiDigit->setValue(currentalti);
      #ifdef AGL_MANAGER_H
      aglManager.setAlti(currentalti);
      #endif
    }

//**********************************************************
//  DISPLAY VARIO
//**********************************************************
    
    if (GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE) {    
      if( history.haveNewClimbRate() ) {
        if (displayLowUpdateState) screen.varioDigit->setValue(history.getClimbRate(GnuSettings.SETTINGS_CLIMB_PERIOD_COUNT));
      }
    } else {
      if (displayLowUpdateState) screen.varioDigit->setValue(currentvario);    
    }

//**********************************************************
//  DISPLAY FINESSE / TAUX DE CHUTE MOYEN
//**********************************************************

    if( history.haveNewClimbRate() ) {
      double TmpTrend;
      TmpTrend = history.getClimbRate(GnuSettings.SETTINGS_CLIMB_PERIOD_COUNT);
#ifdef DATA_DEBUG
      SerialPort.print("Trend value : ");
      SerialPort.println(TmpTrend);
#endif //DATA_DEBUG
      
      if (displayLowUpdateState) {
        if (GnuSettings.RATIO_CLIMB_RATE > 1) {
          if (abs(TmpTrend) < 10) screen.trendDigit->setValue(abs(TmpTrend)); 
          else                    screen.trendDigit->setValue(9.9);
        }

#ifdef DATA_DEBUG
        SerialPort.println("display trendLevel");
#endif //DATA_DEBUG

        if (TmpTrend == 0)     screen.trendLevel->stateTREND(0);
        else if (TmpTrend > 0) screen.trendLevel->stateTREND(1);
        else                   screen.trendLevel->stateTREND(-1);
      }
    }  
#else
    if (GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE) {
      if( history.haveNewClimbRate() ) {
        if (displayLowUpdateState) screen.varioDigit->setValue(history.getClimbRate(GnuSettings.SETTINGS_CLIMB_PERIOD_COUNT));
      }
    else {
      if (displayLowUpdateState) screen.varioDigit->setValue(currentvario);
    }
#endif //HAVE_SCREEN
     
  } else {

    /**************************************************************/
    /*   ERREUR BAROMETRE / MPU                                   */
    /**************************************************************/
    
    SerialPort.println("ERREUR ERREUR BARO / ACCELEROMETRE");   

    compteurErrorMPU++;
    if (compteurErrorMPU > 20) {
      compteurErrorMPU = 20;

 //**********************************************************
//  DISABLE BEEPER
//**********************************************************

#ifdef HAVE_SPEAKER
      beeper.setVelocity( 0 );
#endif //HAVE_SPEAKER

      if (displayLowUpdateState) {
        screen.altiDigit->setValue(0);
        #ifdef AGL_MANAGER_H
        aglManager.setAlti(0);
        #endif
      }
      
      if (displayLowUpdateState) screen.varioDigit->setValue(0);    
    }
/*
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
      SerialPort.print("Alti Sans accelerometre : ");
      SerialPort.println(tmpAlti);
      SerialPort.print("Temperature sans accelerometre: ");
      SerialPort.println(tmpTemp);
#endif //DATA_DEBUG
    }*/
  }

//**********************************************************
//  EMISSION DES BIPS
//**********************************************************

  /*****************/
  /* update beeper */
  /*****************/
#ifdef HAVE_SPEAKER
  beeper.update();
#ifdef PROG_DEBUG
//    SerialPort.println("beeper update");
#endif //PROG_DEBUG
#endif //HAVE_SPEAKER


 //**********************************************************
//  EMISSIONTRAME BT
//  ACQUISITION GPS
//**********************************************************

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

#ifdef NMEAPARSER_DEBUG
        char tmpchar = c;
        SerialPort.print(tmpchar);
#endif //NMEAPARSER_DEBUG

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

#ifdef NMEAPARSER_DEBUG
          SerialPort.println("");
#endif //NMEAPARSER_DEBUG

      serialNmea.release();
#ifdef HAVE_SDCARD          
      fileIgc.flush();
#endif //HAVE_SDCARD
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


//**********************************************************
//  DETECTION FIX GPS / DEBUT DU VOL
//**********************************************************
    
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
          double tmpGpsAlti = nmeaParser.getAlti();
          aglManager.setAltiGps(tmpGpsAlti);

 //         DUMPLOG(LOG_TYPE_DEBUG,GPS_DEBUG_LOG,tmpGpsAlti);

          //Moyenne alti gps
          if (compteurGpsFix > 5) gpsAlti = (gpsAlti + tmpGpsAlti) / 2;
          else                    gpsAlti = tmpGpsAlti;       

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

          if (compteurGpsFix > NB_ACQUISITION_FIX_GPS) {
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
            kalmanvert.calibratePosition(gpsAlti+GnuSettings.COMPENSATION_GPSALTI);
/*            if (currentHeight == 0) kalmanvert.calibratePosition(gpsAlti+GnuSettings.COMPENSATION_GPSALTI);
            else                    kalmanvert.calibratePosition(gpsAlti+currentHeight);*/

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

        DUMP(kalmanvert.getVelocity());
        DUMP(GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD);
        DUMP(GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD);
        
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


//**********************************************************
//  MISE A JOUR ALTI AVEC GPS
//**********************************************************

  /* if no GPS, we can't calibrate, and we have juste to check flight start */
#ifndef HAVE_GPS
  if( variometerState == VARIOMETER_STATE_CALIBRATED ) { //already calibrated at start 
/*    if( (millis() > GnuSettings.FLIGHT_START_MIN_TIMESTAMP) &&
        (kalmanvert.getVelocity() < GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD || kalmanvert.getVelocity() > GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD) ) {
      variometerState = VARIOMETER_STATE_FLIGHT_STARTED;
      enableflightStartComponents();*/


      if( (millis() > GnuSettings.FLIGHT_START_MIN_TIMESTAMP) &&
          (((GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START) &&   
           ((kalmanvert.getVelocity() < GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD) || (kalmanvert.getVelocity() > GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD))) || 
           (!GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START))) {
//        variometerState = VARIOMETER_STATE_FLIGHT_STARTED;
        enableflightStartComponents();      
    }
  }
#endif // !HAVE_GPS

  /**********************************/
  /* update low freq screen objects */
  /**********************************/
#ifdef HAVE_SCREEN


//**********************************************************
//  DISPLAY TIME / DUREE DU VOL
//**********************************************************

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

//      DUMPLOG(LOG_TYPE_DEBUG,GPS_DEBUG_LOG,nmeaParser.time);

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
//    DUMPLOG(LOG_TYPE_DEBUG,GPS_DEBUG_LOG,nmeaParser.satelliteCount);
  }    
#endif //HAVE_GPS  

  /*****************/
  /* update screen */
  /*****************/

//**********************************************************
//  DISPLAY SPEED
//**********************************************************
  
#ifdef HAVE_GPS
  /* when getting speed from gps, display speed and ratio */

  if ((variometerState >= VARIOMETER_STATE_DATE_RECORDED ) && ( nmeaParser.haveNewSpeedValue() )) {

    double currentSpeed = nmeaParser.getSpeed();
    double ratio = history.getGlideRatio(currentSpeed, serialNmea.getReceiveTimestamp(), GnuSettings.SETTINGS_GLIDE_RATIO_PERIOD_COUNT);

#if defined(GPS_DEBUG) || defined(DATA_DEBUG)
          SerialPort.print("GpsSpeed : ");
          SerialPort.println(currentSpeed);
#endif //GPS_DEBUG

//     DUMPLOG(LOG_TYPE_DEBUG,GPS_DEBUG_LOG,currentSpeed);

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



 // }

//**********************************************************
//   DISPLAY LOW FRECQUENCE OBJECT
//********************************************************** 

  if (displayLowUpdateState) {

//**********************************************************
//  ACQUISITION / DISPLAY TENSION BATTERIE
//**********************************************************

#if defined(HAVE_SCREEN) && defined(HAVE_VOLTAGE_DIVISOR) 
//  int tmpVoltage = analogRead(VOLTAGE_DIVISOR_PIN);
//  if (maxVoltage < tmpVoltage) {maxVoltage = tmpVoltage;}

      /* update battery level */

      
#if defined(VOLTAGE_DIVISOR_DEBUG)
    int val = adc1_get_raw(ADC1_CHANNEL_7);

    SerialPort.print("Tension : ");
    SerialPort.println(val);
    if (compteurBoucle == 5) DUMPLOG(LOG_TYPE_DEBUG, VOLTAGE_DEBUG_LOG,val);
#endif //VOLTAGE_DIVISOR_DEBUG

  long TmpVoltage = 0;
  for(int i=0; i<10; i++) TmpVoltage += analogRead(VOLTAGE_DIVISOR_PIN);  
  TmpVoltage = TmpVoltage / 10;

  if (compteurBoucle == 4) {
    DUMPLOG(LOG_TYPE_DEBUG, VOLTAGE_DEBUG_LOG,TmpVoltage);
    compteurBoucle = 0;
  }
  else {
    compteurBoucle++;
  }
  
  if (TmpVoltage > MaxVoltage) MaxVoltage = TmpVoltage;
  
    if (MaxVoltage < 1750) {
      if (millis() - time_deep_sleep > 10000) {
        screen.ScreenViewMessage("Bat Low",3);
        indicatePowerDown(); 
//        TRACELOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG);
        MESSLOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG,"Deep sleep - Batterie low");
        DUMPLOG(LOG_TYPE_DEBUG, DEEPSLEEP_DEBUG_LOG,MaxVoltage);
        deep_sleep("Batt Low");  //protection batterie
      }
    } else {
      time_deep_sleep = millis();        
    }

    screen.batLevel->setVoltage(MaxVoltage);
    MaxVoltage   = 0;
//  batLevel.setVoltage( maxVoltage );
//  maxVoltage = 0;

#endif //HAVE_VOLTAGE_DIVISOR

//**********************************************************
//  DISPLAY STATE RECORD
//**********************************************************

    screen.recordIndicator->stateRECORD();
#ifdef PROG_DEBUG
//    SerialPort.println("Record Indicator : staterecord ");
    SerialPort.print("VarioState : ");
    SerialPort.println(variometerState);
#endif //PROG_DEBUG
      
  }


//**********************************************************
//  DISPLAY TEMPERATURE ESP32
//**********************************************************
/*
   if (displayLowUpdateState) {
    
#ifdef PROG_DEBUG
      SerialPort.print("Temperature: ");
      // Convert raw temperature in F to Celsius degrees
      SerialPort.print((temprature_sens_read() - 32) / 1.8);
      SerialPort.println(" C");
#endif //PROG_DEBUG
  }*/

//**********************************************************
//  DISPLAY BEARING
//**********************************************************

  if (displayLowUpdateState) {
    
    if (nmeaParser.haveBearing()) {

      double bearing    = nmeaParser.getBearing();
      String bearingStr = nmeaParser.Bearing_to_Ordinal(bearing);
#ifdef DATA_DEBUG
      SerialPort.print("Compas : ");
      SerialPort.print(bearing);
      SerialPort.print(" - ");
      SerialPort.println(bearingStr);
#endif //DATA_DEBUG     
      DUMPLOG(LOG_TYPE_DEBUG, DATA_DEBUG_LOG, bearing);
      DUMPLOG(LOG_TYPE_DEBUG, DATA_DEBUG_LOG, bearingStr);

     screen.gpsBearing->setValue(bearing); 
     screen.gpsBearingText->setValue(bearingStr); 
    }

    if (nmeaParser.haveLongitude()) {
      String longitude = nmeaParser.getLongitude();      
#ifdef DATA_DEBUG
      SerialPort.print("Longitude : ");
      SerialPort.println(longitude);
#endif //DATA_DEBUG     
      DUMPLOG(LOG_TYPE_DEBUG, DATA_DEBUG_LOG, longitude);
      #ifdef AGL_MANAGER_H
      aglManager.setLongitude(nmeaParser.getLong());
      #endif
//      screen.gpsLongDir->setValue(String(nmeaParser.getLongDir()));
//      screen.gpsLong->setValue(nmeaParser.getLong());
      screen.gpsLong->setValue(nmeaParser.getLongDegree());
    }

    if (nmeaParser.haveLatitude()) {
      String latitude = nmeaParser.getLatitude();
#ifdef DATA_DEBUG
      SerialPort.print("Latitude : ");
      SerialPort.println(latitude);
#endif //DATA_DEBUG     
      DUMPLOG(LOG_TYPE_DEBUG, DATA_DEBUG_LOG, latitude);
      #ifdef AGL_MANAGER_H
      aglManager.setLatitude(nmeaParser.getLat());
      #endif
//      screen.gpsLatDir->setValue(String(nmeaParser.getLatDir()));
//      screen.gpsLat->setValue(nmeaParser.getLat());
      screen.gpsLat->setValue(nmeaParser.getLatDegree());
    }
    #ifdef AGL_MANAGER_H
      currentHeight = aglManager.getHeight();
#ifdef PROG_DEBUG
      SerialPort.print("Height : ");
      SerialPort.println(currentHeight);
#endif //PROG_DEBUG     
      screen.heightDigit->setValue(currentHeight);
      #endif
  }
   
  displayLowUpdateState = false;

// Passes control to other tasks when called
  SysCall::yield();

//**********************************************************
//  UPDATE DISPLAY
//**********************************************************

#ifdef PROG_DEBUG
    //SerialPort.println("Update Screen");
#endif //PROG_DEBUG

  if (screen.schedulerScreen->displayStep()) {
    screen.updateScreen(); 
  }
 
#endif //HAVE_SCREEN

//**********************************************************
//  UPDATE STATISTIQUE
//**********************************************************

  flystat.Handle(); 

//*****************************************
//      FORCE L'ACTIVATION DE L'AMPLI          
//*****************************************  

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
