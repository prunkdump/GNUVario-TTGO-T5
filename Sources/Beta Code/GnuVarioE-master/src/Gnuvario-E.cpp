#include <Arduino.h>

/*******************/
/* Version         */
/*******************/

#define VERSION 0
#define SUB_VERSION 8
#define BETA_CODE 3
#define DEVNAME "JPG63/MICELPA/RATAMUSE"
#define AUTHOR "J" //J=JPG63  P=PUNKDUMP  M=MICHELPA

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
* v0.8 Beta 1   21/03/20             Réécriture Main prog                                             *
*               23/03/20             Maj lib json et arduinotrace                                     *
*               05/04/20             Correction problème maj compensation alti via AGL                *
*                                    Correction blocage du MPU                                        *
*               08/04/20             Maj titre en français                                            *                     
*               09/04/20             Maj aduinoJson                                                   *
*                                    Ajout parametre language, sensibilité vario                      *
*               11/04/20             Nouvelle classe Language                                         *
*                                    Nouvelle font                                                    *
*               18/04/20             Ajout reglage de la sensibilité du vario - reglage kalman        *
*               27/04/20             Correction écran 1.54''                                          *
*               29/04/20             Changement de font - modification screenDigit                    *
*               03/05/20             Correction bug d'affichage                                       *
*               06/05/20             Correction déclenchement du vol                                  *
*               08/05/20             Compas Magnetique                                                *
*                                    Correction affichage écran 2.91''                                *
*                                    correction champs trop grand dans statistique                    *
* v0.8 beta 2   15/05/20             Changement de librairie pour sdcard                              *
*               16/05/20             Maj lib arduinojson                                              *
*               16/05/20             Ajout raffraichissement écran toutes les 15min                   *
*               17/05/20             Maj lib Adafruit_GFX_Library                                     *
*                                    Nettoyage lib BLE                                                *
*                                    Ajout position titre                                             *
*                                    Ajout lib sqlite3                                                *
*                                    Ajout VarioSqlFlight                                             *
*               23/05/20             Correction bug d'affichage (valeur > affichage)                  *
*                                    correction bug affichage alternatif finesse / taux de chute      *
*                                    Alternance compas GPS / compas magnétique                        *
* 																	 AJOUT - Réglage sensibilité filtre kalman et vario               *                                         
*                                      pas concluant                                                  *
* 0.8 Beta 3    26/05/20             Correction gestion d'erreur MPU                                  *
*                                    Amélioration du son - test cosine                                *
*                                    Ajout carnet de vol                                              *
*                                    Maj lib GFX                                                      *
*                16/06/20            Changement lib BT  - NimBLE                                      *
*                                    Ajout paramètre calibrated_gps pour IGC et BT                    *
*                                    Correction paramètre trame BT                                    *
*                                    Ajout fichier langue "anglais"                                   *
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
*                                                                                                     *        
* v0.8                                                                                                *       
* AJOUT - Espaces aeriens                                                                             *
* BUG   - Grésillement Buzzer                                                                         * 
* AJOUT - Indication niveau de batterie                                                               *
* Ajout - Indication charge batterie                                                                  *
* BUG   - altitude enregistré non compensé                                                            *
* AJOUT - écran charge batterie au démarrage                                                          *
* AJOUT - alti GPS                                                                                    *
* BUG   - derive alti                                                                                 *                                                                                
* BUG   - trame BT                                                                                    *
*                                                                                                     *
* VX.X                                                                                                *
* Paramètrage des écrans                                                                              *
* Gérer le son via le DAC                                                                             *
* revoir volume du son ToneESP32                                                                      *
* Refaire gestion du son (parametrage via xctracer)                                                   *
* Boussole graphique                                                                                  *                                                                       
* Création dynamique des objets screen                                                                *
* Sens et vitesse du vent                                                                             *
* verifier fonctionnement BT - trame non complete                                                     *
* Espace aérien                                                                                       *
* 2 Altitudes                                                                                         *
* Recupération vol via USB                                                                            *                                                                                        
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
 * - Ajout du mode écran 2.9'' en portrait                              *
 * - Calibration des accèlerometres                                     *
 * - AGL                                                                *
 * - Calibration manuel du baro via l'AGL                               *
 * - Declenchement de vol manuellement                                  *
 * - Compensation du GPS via l'AGL                                      *
 *                                                                      *
 * Version 0.8                                                          *
 *  - Gestion Multilangue                                               *
 *  - Nouvelle font d'affichage                                         *
 *  - Sensibilité du vario réglable                                     *
 *  - Compas Magnétique + compas GPS                                    *
 *  - Raffraichissement écran toutes les 15min                          *
 *  - Carnet de vol                                                     *
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
*               AGL/                                                   *
*                               ..      - Fichier topo                 *
*               VOLS/                   - repertoire des traces        *
*                       19101200.IGC                                   *
*                       19101201.IGC                                   *
*               WWW/                    - repertoire du site Web       *
*                       INDEX.HTM                                      *
*                       INDEXB.JS                                      *
*                       INDEXB~1.MAP                                   *
*               RECORD00.CAL            - fichier de calibration       *
*               PARAMS.JSO              - fichier de configuration     *
*               LOG.CFG                 - Paramètres de debug          *
*               VARIOCAL.CFG            - Paramètres de calibration    *
*               WIFI.CFG                - Paramètres Wifi              *
*                                                                      *
************************************************************************/

/***********************************************************************
*                                                                      *
*                           Mise à jour Manuel                         *
*                                                                      *
* Mise à jour via la carte SD, nom du fichier : update.bin             *                                                                                                           
* Mise à jour site web embarqué               : dossier wwwnew         *
*                                                                      *
************************************************************************/

/**************************************************************************************************************
 *              Liens utiles                                                                                  *
 *                                                                                                            *
 * https://learn.sparkfun.com/tutorials/9dof-razor-imu-m0-hookup-guide/all#libraries-and-example-firmware     *        
 *                                                                                                            *
 * AGL : https://vps.skybean.eu/agl/                                                                          *
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

//*****************************
// DEBBUGAGE                  *
//*****************************
#include <DebugConfig.h>
#include <VarioLog.h>

#ifdef PROG_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#ifndef ARDUINOTRACE_SERIAL
#define ARDUINOTRACE_SERIAL SerialPort
#endif

#include <ArduinoTrace.h>
//#include "myassert.h"

#if defined(ESP32)
//static const char* TAG = "GnuvarioE";
#include "esp_log.h"
#endif //ESP32

//#include <SysCall.h>

//#define TEST_SD

//*******************************
// GESTION DU MATERIEL          *
//*******************************

#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif //HAVE_SDCARD

#include <Update.h>
#include <SD_Update.h>

#include <VarioHardwareManager.h>

#ifdef HAVE_SPEAKER
#include <toneHAL.h>
#include <beeper.h>
#endif //HAVE_SPEAKER

#include <GPSSentence.h>

/*#ifdef HAVE_GPS
#include <SerialNmea.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <NmeaParser.h>
#endif //HAVE_GPS*/

#include <VarioButton.h>

//#include <Utility.h>

//#include <driver/adc.h>

/*****************/
/* screen        */
/*****************/

#include <varioscreenGxEPD.h>

//*******************************
// GESTION DES DONNES           *
//*******************************

#include <VarioData.h>
VarioData varioData;

#include <VarioSettings.h>
#include <VarioLanguage.h>

//*******************************
// GESTION WIFI                 *
//*******************************

#ifdef HAVE_WIFI
#include <VarioWifiServer.h>
#include <esp32fota2.h>
#endif //HAVE_WIFI

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
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
#elif defined(ETHERNETWEBSERVER)
EthernetServer server(80);
#elif defined(ESPRESSIFWEBSERVEUR)
WebServer server(80);
#else  //ESP32WEBSERVEUR
VarioWebServer server(80);
#endif //ESP32WEBSERVEUR
#endif

esp32FOTA2 esp32FOTA("Gnuvario" + String(VARIOSCREEN_SIZE), VERSION, SUB_VERSION, BETA_CODE); //esp32-fota-http", 0,6,0);

#endif //HAVE_WIFI

//****************************
//****************************
void setup()
{
  //****************************
  //****************************

#if defined(ENABLE_DEBUG)
  SerialPort.begin(115200);

  // Wait for USB Serial
  while (!SerialPort)
  {
    //    SysCall::yield();
  }
#endif

// *******************************************************
// *   TEST_SD
// *******************************************************
#ifdef TEST_SD
  delay(5000);
  TestSDCARD(true);
#endif

  /*****************************/
  /*  Init Alimentation        */
  /*****************************/
  varioHardwareManager.initAlim();

  /*****************************/
  /* wait for devices power on */
  /*****************************/
#ifdef PROG_DEBUG
  delay(5000);
#else
  delay(VARIOMETER_POWER_ON_DELAY);
#endif

  /************************/
  /*    BOOT SEQUENCE     */
  /************************/

  varioData.init(VERSION, SUB_VERSION, BETA_CODE, String(DEVNAME));

  varioHardwareManager.init();

  /******************/
  /* Init Speaker   */
  /******************/
  varioHardwareManager.initSpeaker();

  /******************/
  /* Init SDCARD    */
  /******************/

#ifdef TEST_SD
  varioData.initSettings(false);
#else
  varioData.initSettings(true);
#endif //TEST_SD

  //**********************************************
  // Charge le fichier de langue
  //**********************************************
  varioLanguage.init(GnuSettings.LANGUAGE);

#ifdef HAVE_SDCARD
  SerialPort.print("TITRE_TIME : ");
  SerialPort.println(varioLanguage.getText(TITRE_TIME));
#endif

// *********************************************
// Bip de démarrage
//**********************************************
#ifdef HAVE_SPEAKER
  if (GnuSettings.ALARM_VARIOBEGIN)
    beeper.generateTone(2000, 300);
#endif //HAVE_SPEAKER

  /*********************/
  /*    init logger    */
  /*********************/

  varioData.initLog();

  //***********************************************
  // INIT AGL
  //***********************************************

  varioData.initAGL();

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

#if defined(HAVE_SDCARD) && defined(HAVE_WIFI)
  esp32FOTA.UpdateWwwDirectory();
#endif //HAVE_SDCARD

  /***************/
  /* init button */
  /***************/

  varioHardwareManager.initButton();

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

  screen.ScreenViewInit(VERSION, SUB_VERSION, AUTHOR, BETA_CODE);

#endif //HAVE_SCREEN

  //***********************************************
  // INIT Sound
  //      init Beeper avec les valeurs personnelles
  //      init Volume
  //***********************************************

  varioHardwareManager.initSound();

  //***********************************************
  // INIT MPU / MS5611
  //***********************************************

  varioHardwareManager.initImu();

  //***********************************************
  // Première mesure d'altitude
  //***********************************************

  double firstAlti = varioHardwareManager.firstAlti();
#ifdef DATA_DEBUG
  SerialPort.print("first alti : ");
  SerialPort.println(firstAlti);
#endif //KALMAN_DEBUG

  //***********************************************
  // Calibration
  //***********************************************

  if (ButtonScheduleur.Get_StatePage() == STATE_PAGE_CALIBRATION)
    screen.ScreenViewMessage("Calibration", 5);

    //***********************************************
    // Affiche l'écran de statistique
    //***********************************************

#ifdef HAVE_SCREEN
  // Affichage Statistique
  varioData.flystat.Display();
  screen.ScreenViewStat();

  unsigned long TmplastDisplayTimestamp = millis();
  int compteur = 0;
  while (compteur < GnuSettings.DISPLAY_STAT_DURATION)
  {

    if (millis() - TmplastDisplayTimestamp > 1000)
    {

      TmplastDisplayTimestamp = millis();
      compteur++;

      //    Messure d'altitude
      firstAlti = varioHardwareManager.getAlti();
    }
  }

#endif //HAVE_SCREEN

  //***********************************************
  // Initialise le filtre de Kalman
  //***********************************************

  varioData.initKalman(firstAlti);

  varioData.compteurGpsFix = 0;

  //***********************************************
  // Initialise l'integration (climb rate)
  //***********************************************

  if (GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)
    varioData.history.init(firstAlti, millis());

  //***********************************************
  // Initialise GPS
  //***********************************************

  varioHardwareManager.initGps();

  //***********************************************
  // Affiche la première page
  //***********************************************

#ifdef HAVE_SCREEN

  screen.ScreenViewPage(0, true);
  screen.volLevel->setVolume(toneHAL.getVolume());
  screen.updateScreen();

#ifdef SOUND_DEBUG
  SerialPort.print("ToneHal Volume Sound : ");
  SerialPort.println(toneHAL.getVolume()); //GnuSettings.VARIOMETER_BEEP_VOLUME);
#endif                                     //SOUND_DEBUG

#ifdef SCREEN_DEBUG
  SerialPort.println("update screen");
#endif //SCREEN_DEBUG

  screen.clearScreen();
  screen.schedulerScreen->enableShow();
#endif //HAVE_SCREEN

  //***********************************************
  // Initialisation BT
  //***********************************************

#if defined(HAVE_BLUETOOTH)
  if (varioHardwareManager.initBt())
  {
    TRACE();
    screen.btinfo->setBT();
  }
  else
  {
    TRACE();
    screen.btinfo->unsetBT();
  }
#endif //HAVE_BLUETOOTH

  ButtonScheduleur.Set_StatePage(STATE_PAGE_VARIO);

  //***********************************************
  // Initialisation Time
  //***********************************************

  varioData.initTime();
}

double temprature = 0;

//*****************************
//*****************************
void loop()
{
  //****************************
  //****************************

  //****************************
  // Gestion Des chrono
  //***************************

  /*  LOW UPDATE DISPLAY */
  if (millis() - varioData.lastDisplayTimestamp > DISPLAY_LOW_UPDATE)
  {
    varioData.lastDisplayTimestamp = millis();
    varioData.displayLowUpdateState = true;
  }

  // DISPLAY
  if (millis() - varioData.lastDisplayTimestamp2 > DISPLAY_UPDATE)
  {
    varioData.lastDisplayTimestamp2 = millis();
    varioData.displayUpdateState = true;
  }

  //**********************************************************
  //  TRAITEMENT DU SON
  //**********************************************************

  toneHAL.update();

  //**********************************************************
  //  TRAITEMENT DES BOUTONS
  //**********************************************************

  ButtonScheduleur.update();

  //**********************************************************
  //  ACQUISITION DES DONNEES
  //**********************************************************

  varioData.update();

  //**********************************************************
  //  TEST INNACTIVITE
  //**********************************************************
  varioHardwareManager.testInactivity(varioData.getVelocity());

  //**********************************************************
  //  MISE A JOUR DE L'ECRAN
  //**********************************************************

  if (varioData.displayLowUpdateState)
  {

    //**********************************************************
    //  DISPLAY ALTI
    //**********************************************************

    screen.altiDigit->setValue(varioData.getCalibratedAlti());

    //**********************************************************
    //  DISPLAY VARIO
    //**********************************************************

    if (GnuSettings.VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)
    {
      if (varioData.haveNewClimbRate())
      {
        double tmpvalue = varioData.getClimbRate();
#if (VARIOSCREEN_SIZE == 154)
        if (tmpvalue > 9.9)
          tmpvalue = 9.9;
        if (tmpvalue < -9.9)
          tmpvalue = -9.9;
#else
        if (tmpvalue > 99.9)
          tmpvalue = 99.9;
        if (tmpvalue < -99.9)
          tmpvalue = -99.9;
#endif
        screen.varioDigit->setValue(tmpvalue);

        //       screen.varioDigit->setValue(varioData.getClimbRate());
      }
    }
    else
    {

      double tmpvalue = varioData.getVelocity();
#if (VARIOSCREEN_SIZE == 154)
      if (tmpvalue > 9.9)
        tmpvalue = 9.9;
      if (tmpvalue < -9.9)
        tmpvalue = -9.9;
#else
      if (tmpvalue > 99.9)
        tmpvalue = 99.9;
      if (tmpvalue < -99.9)
        tmpvalue = -99.9;
#endif
      screen.varioDigit->setValue(tmpvalue);

      //      screen.varioDigit->setValue(varioData.getVelocity());
    }

    //**********************************************************
    //  DISPLAY FINESSE / TAUX DE CHUTE MOYEN
    //**********************************************************

    if (varioData.haveNewClimbRate())
    {
      if (GnuSettings.RATIO_CLIMB_RATE > 1)
        screen.trendDigit->setValue(varioData.getTrend());
      else
        screen.trendDigit->setValue(0);
      screen.trendLevel->stateTREND(varioData.getStateTrend());
    }
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
  //  EMISSION TRAME BT
  //  ACQUISITION GPS
  //**********************************************************

#ifdef HAVE_GPS
  //avec gps le bluetooth est mis a jour en meme temps que la trame gps
  /**************/
  /* update GPS */
  /**************/

  varioData.updateGps();

  //**********************************************************
  //  DETECTION FIX GPS / DEBUT DU VOL
  //**********************************************************

  varioData.updateState();
#else //HAVE_GPS

//sans gps, le bluetooth doit etre mis a jour manuellement
#ifdef VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE
  varioHardwareManager.varioBle->bluetoothNMEA.begin(varioData.kalmanvert.getCalibratedPosition(), varioData.kalmanvert.getVelocity());
#else
  varioHardwareManager.varioBle->bluetoothNMEA.begin(varioData.kalmanvert.getPosition(), varioData.kalmanvert.getVelocity());
#endif

  // * if no GPS, we can't calibrate, and we have juste to check flight start */

  //**********************************************************
  //  DETECTION FIX GPS / DEBUT DU VOL
  //**********************************************************

  varioData.updateState();

#endif //HAVE_GPS

  /********************/
  /* update Bluetooth */
  /********************/

#if defined(HAVE_BLUETOOTH)
  if (varioData.updateBluetooth())
  {
#ifdef GPS_DEBUG
    SerialPort.println("Update BLE");
#endif //GPS_DEBUG
  }
#endif // HAVE_BLUETOOTH 

#ifdef HAVE_SCREEN
  if ((varioData.gpsFix > 0) && (varioData.gpsFix < 3))
    screen.recordIndicator->setActifGPSFIX();
  if (varioData.gpsFix == 2)
    screen.fixgpsinfo->setFixGps();

#endif //HAVE_SCREEN

    ////////////////////////////////////

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

  if (varioData.displayLowUpdateState)
  {
    if (nmeaParser.haveDate())
    {

      /* set time */
#if defined(GPS_DEBUG) || defined(DATA_DEBUG)
      SerialPort.print("Time : ");
      SerialPort.println(nmeaParser.time);
#endif //GPS_DEBUG

      //      DUMPLOG(LOG_TYPE_DEBUG,GPS_DEBUG_LOG,nmeaParser.time);

      screen.screenTime->setTime(nmeaParser.time);
      screen.screenTime->correctTimeZone(GnuSettings.VARIOMETER_TIME_ZONE);
      if (varioData.getVariometerState() == VARIOMETER_STATE_FLIGHT_STARTED)
      {
        screen.screenElapsedTime->setCurrentTime(screen.screenTime->getTime());
        varioData.flystat.SetTime(screen.screenTime->getTime());
        varioData.flystat.SetDuration(screen.screenElapsedTime->getTime());
      }
      else
      {
        screen.screenElapsedTime->setCurrentTime(screen.screenTime->getTime());
      }
    }

    /* update satelite count */
    screen.satLevel->setSatelliteCount(nmeaParser.satelliteCount);
#ifdef GPS_DEBUG
    SerialPort.print("Sat : ");
    SerialPort.println(nmeaParser.satelliteCount);
#endif //GPS_DEBUG \
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

  if (varioData.updateSpeed())
  {
    screen.speedDigit->setValue(varioData.currentSpeed);
    screen.ratioDigit->setValue(varioData.ratio);
  }
  else
  {
    screen.ratioDigit->setValue(0);
  }

#endif //HAVE_GPS

  //**********************************************************
  //   DISPLAY LOW FRECQUENCE OBJECT
  //**********************************************************

  if (varioData.displayLowUpdateState)
  {

    //**********************************************************
    //  ACQUISITION / DISPLAY TENSION BATTERIE
    //**********************************************************

#if defined(HAVE_SCREEN) && defined(HAVE_VOLTAGE_DIVISOR)
    varioData.updateVoltage();
    screen.batLevel->setVoltage(varioData.voltage);

#endif //HAVE_VOLTAGE_DIVISOR

    //**********************************************************
    //  DISPLAY STATE RECORD
    //**********************************************************

    screen.recordIndicator->stateRECORD();
#ifdef PROG_DEBUG
    //    SerialPort.println("Record Indicator : staterecord ");
    SerialPort.print("VarioState : ");
    SerialPort.println(varioData.getVariometerState());
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

  if (varioData.displayLowUpdateState)
  {

    int tmpcap = varioData.getCap();
    if (tmpcap > 0)
    {
      String bearingStr = nmeaParser.Bearing_to_Ordinal(tmpcap);
#ifdef DATA_DEBUG
      SerialPort.print("Compas : ");
      SerialPort.print(tmpcap);
      SerialPort.print(" - ");
      SerialPort.println(bearingStr);
#endif //DATA_DEBUG
      DUMPLOG(LOG_TYPE_DEBUG, DATA_DEBUG_LOG, tmpcap);
      DUMPLOG(LOG_TYPE_DEBUG, DATA_DEBUG_LOG, bearingStr);

      screen.gpsBearing->setValue(tmpcap);
      screen.gpsBearingText->setValue(bearingStr);
#if (VARIOSCREEN_SIZE == 291)
      screen.bearing->setValue(tmpcap);
      screen.bearingText->setValue(bearingStr);
#endif
    }

    if (nmeaParser.haveLongitude())
    {
      String longitude = nmeaParser.getLongitude();
#ifdef DATA_DEBUG
      SerialPort.print("Longitude : ");
      SerialPort.println(longitude);
#endif //DATA_DEBUG
      DUMPLOG(LOG_TYPE_DEBUG, DATA_DEBUG_LOG, longitude);
#ifdef AGL_MANAGER_H
      varioData.aglManager.setLongitude(nmeaParser.getLong());
#endif
      //      screen.gpsLongDir->setValue(String(nmeaParser.getLongDir()));
      //      screen.gpsLong->setValue(nmeaParser.getLong());
      screen.gpsLong->setValue(nmeaParser.getLongDegree());
    }

    if (nmeaParser.haveLatitude())
    {
      String latitude = nmeaParser.getLatitude();
#ifdef DATA_DEBUG
      SerialPort.print("Latitude : ");
      SerialPort.println(latitude);
#endif //DATA_DEBUG
      DUMPLOG(LOG_TYPE_DEBUG, DATA_DEBUG_LOG, latitude);
#ifdef AGL_MANAGER_H
      varioData.aglManager.setLatitude(nmeaParser.getLat());
#endif
      //      screen.gpsLatDir->setValue(String(nmeaParser.getLatDir()));
      //      screen.gpsLat->setValue(nmeaParser.getLat());
      screen.gpsLat->setValue(nmeaParser.getLatDegree());
    }

#ifdef AGL_MANAGER_H
    varioData.currentHeight = varioData.aglManager.getHeight();
#ifdef PROG_DEBUG
    SerialPort.print("Height : ");
    SerialPort.println(varioData.currentHeight);
#endif //PROG_DEBUG
    screen.heightDigit->setValue(varioData.currentHeight);
#endif

    if (nmeaParser.haveNewAltiValue())
    {
      varioData.gpsAlti = nmeaParser.getAlti();
      varioData.aglManager.setAltiGps(varioData.gpsAlti);
    }
  }

  varioData.displayLowUpdateState = false;

  // Passes control to other tasks when called
  //  SysCall::yield();

  //**********************************************************
  //  UPDATE DISPLAY
  //**********************************************************

#ifdef PROG_DEBUG
  //SerialPort.println("Update Screen");
#endif //PROG_DEBUG

  if (screen.schedulerScreen->displayStep())
  {
    screen.updateScreen();
  }

#endif //HAVE_SCREEN

  //**********************************************************
  //  UPDATE STATISTIQUE
  //**********************************************************

  varioData.flystat.Handle();

  //*****************************************
  //      FORCE L'ACTIVATION DE L'AMPLI
  //*****************************************

#ifdef HAVE_AUDIO_AMPLI
//  toneHAL.enableAmpli();
#endif

  /*******************************/
  /*******************************/
}
