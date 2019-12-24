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
/*                                                                               */
/*********************************************************************************/

#ifndef variowifiServer_SD_h
#define variowifiServer_SD_h

#include <HardwareConfig.h>

#include <VarioWifiServer.h>

//#define ESP32WEBSERVEUR 
//#define ESPASYNCWEBSERVER
//#define ETHERNETWEBSERVER

#if defined(WEBSERVER_SDCARD)

#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>      // Built-in
#include <ESP8266WiFiMulti.h> // Built-in
#include <ESP8266WebServer.h> // Built-in
#include <ESP8266mDNS.h>

#define WEBSERVERTYPE "ESP8266"

#else //ESP32

#ifdef ESP32WEBSERVEUR 
#include <WiFiMulti.h> // Built-in
//#include <WiFiClient.h>
#include <ESP32WebServer.h>    // https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
#include <WiFi.h>
#include <ESPmDNS.h>

#define WEBSERVERTYPE "ESP32WebServer"

#elif defined(ESPASYNCWEBSERVER)

#include <WiFi.h>
#include <WiFiMulti.h> // Built-in
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#define WEBSERVERTYPE "ESPAsyncWebServer"

#elif defined(ETHERNETWEBSERVER)

//#include <WiFi.h>
#include <WiFiMulti.h> // Built-in
#include <Ethernet.h>
#include <ESPmDNS.h>

#define WEBSERVERTYPE "Ethernet"

#else //ESP32WEBSERVEUR

#include <WiFi.h>      // Built-in
#include <WiFiMulti.h> // Built-in
//  #include <ESP32WebServer.h>    // https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
#include <WiFiClient.h>
#include <VarioWebServer.h>
#include <ESPmDNS.h>
//  #include "FS.h"

#define WEBSERVERTYPE "VarioWebServer"

#endif //ESP32WEBSERVEUR
#endif //ESP8266

/*#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif //HAVE_SDCARD*/

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

#ifdef ESP8266
extern ESP8266WiFiMulti wifiMulti;
extern ESP8266WebServer server;
#else
extern WiFiMulti wifiMulti;
#ifdef ESP32WEBSERVEUR 
extern ESP32WebServer server;
#elif defined(ESPASYNCWEBSERVER)
  AsyncWebServer server; 
#elif defined(ETHERNETWEBSERVER)
extern EthernetServer server; 
#else //ESP32WEBSERVEUR
extern VarioWebServer server;
#endif //ESP32WEBSERVEUR
#endif

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
  void handleClient(void);
};

void returnOK();
void returnFail(String msg);
//void listDirectory(File dir, int numTabs);

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

extern VarioWifiServer varioWifiServer;

#endif //WEBSERVER_SDCARD

#endif
