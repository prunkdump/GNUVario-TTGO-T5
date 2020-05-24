/*********************************************************************************/
/*                                                                               */
/*                           Libraries wifiServer_SD                             */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0                                                                        */
/*    1.1    08/10/19   Ajout listDirectory                                      */
/*    1.2    20/10/19   Mise à jour par rapport au fichier main.cpp de Pascal    */
/*    1.2.1  22/10/19   Ajout printDirectoryRecurse                              */
/*    1.2.2  31/10/19   Réorganisation / nettoyage et renomage du code source    */
/*                      Correction bug chargement params.jso                     */
/*    1.2.3  03/11/19   Ajout handleSaveParams																	 */
/*    1.2.4  21/11/19   Correction Bug printDirectoryRecurse                     */
/*    1.2.5  29/11/19   Modif Sdfat                                              */
/*    1.2.6  02/12/19   Changement librairie webserveur                          */
/*    1.2.7  11/12/19   Changement de nom                                        */
/*    1.2.8  12/12/19   Librairie VarioWebServer                                 */
/*    1.2.9  27/12/19   Modif ESP32WebServer par VarioESP32WebServer             */
/*    1.2.10 28/12/19   Modification HandleUpdate                                */
/*    1.2.11 23/01/20   Ajout setClock()																				 */
/*    1.2.12 25/01/20   Correction Upload                                        */
/*                      Ajout HandleFirmwareVersion / handleUpgradeWeb           */
/*    1.2.13 16/02/20   refactoring variowifiserver                              */
/*                                                                               */
/*********************************************************************************/

#ifndef variowifiServer_SD_h
#define variowifiServer_SD_h

#include <HardwareConfig.h>

#include <VarioWifiServer.h>

#if defined(WEBSERVER_SDCARD)

#include <Arduino.h>

#include <WiFi.h>      // Built-in
#include <WiFiMulti.h> // Built-in
#include <WiFiClient.h>
#include <VarioWebServer.h>
#include <ESPmDNS.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

#define WEBSERVERTYPE "VarioWebServer"

//#define ServerVersion "1.0"
extern char ServerVersion[20];
extern String webpage;
extern bool SD_present;

extern char ssid_1[50];
extern char password_1[50];

extern char ssid_2[50];
extern char password_2[50];

extern char ssid_3[50];
extern char password_3[50];

extern char ssid_4[50];
extern char password_4[50];

extern WiFiMulti wifiMulti;
extern VarioWebServer server;

//////////////////////////////////////
//          CLASS WIFISERVER        //
//////////////////////////////////////

class VarioWifiServer
{

public:
  boolean begin(void);
  void disableWifi(void);
  void disableBT(void);
  void disableAll(void);
  void connect(void);
  void start(void);
  void setClock(void);
  void handleClient(void);
};

void returnOK();
void returnFail(String msg);

//////////////////////////////////////
//          GENERIC HANDLERS        //
//////////////////////////////////////

bool loadFromSdCard(String path);

//////////////////////////////////////
//          SPECIFIC HANDLERS       //
//////////////////////////////////////

void handleListFlights();
void handleParams();
void handleWifi();
void handlePrintDirectory();
void printDirectoryRecurse(String path);
void handleFileDownload();
void handleFileUpload();
void handleFileDelete();
void deleteRecursive(String path);
void handleNotFound();
void handleFileUpdate();
void handleSaveParams();
void handleSaveWifi();
void handleFirmwareVersion();
void handleUpgradeWeb();
void handleWebConfig();
void handleSaveWebConfig();

//////////////////////////////////////
//              HELPERS            //
/////////////////////////////////////

String getFileSizeStringFromBytes(int bytes);
void listDirectory(SdFile dir, int numTabs);

extern VarioWifiServer varioWifiServer;

#endif //WEBSERVER_SDCARD

#endif
