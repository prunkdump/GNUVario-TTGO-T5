/* WifiServer_SD -- 
 *
 * Copyright 2016-2020 Jean-philippe, Pascal
 * 
 * This file is part of GNUVario-E
 *
 * GNUVario is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNUVario is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
/*                      Recodage de l'update                                     */
/*    1.2.7  11/12/19   Changement de nom                                        */
/*    1.2.8  12/12/19   Librairie VarioWebServer                                 */
/*    1.2.9  27/12/19   Modif ESP32WebServer par VarioESP32WebServer             */
/*    1.2.10 28/12/19   Modification HandleUpdate                                */
/*    1.2.11 23/01/20   Ajout setClock()																				 */
/*    1.2.12 25/01/20   Correction Upload                                        */
/*                      Ajout HandleFirmwareVersion / HandleUpgradeWeb           */
/*    1.2.13 27/01/20   Correction serveur.on handlefirwareversion               */
/*    1.2.14 04/02/20   modif esp32FOTA.checkURL																 */
/*                      Ajout écran d'infoi lors de l'update Internet            */
/*    1.2.15 13/02/20   Nettoyage code mort, refactoring                         */
/*    1.2.13 16/02/20   refactoring variowifiserver                              */
/*                                                                               */
/*********************************************************************************/

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#if defined(WEBSERVER_SDCARD)

#ifdef WIFI_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include <VarioWifiServer_SD.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

#include <VarioSettings.h>

#include <Update.h>

#include <ArduinoJson.h>

#include <esp32fota2.h>

extern VarioSettings GnuSettings;

#include <VarioIgcParser.h>

#include <VarioSqlFlight.h>

#define TAG "server"

bool SD_present = false;
char ServerVersion[20]; // = "3.4";

#ifdef _HARDWARECONFIG_H_
IPAddress local_IP(LOCAL_IP); // Set your server's fixed IP address here
IPAddress gateway(GATEWAY);   // Set your network Gateway usually your Router base address
IPAddress subnet(SUBNET);     // Set your network sub-network mask here
IPAddress dns(DNS);           // Set your network DNS usually your Router base address
#else
IPAddress local_IP(192, 168, 1, 150); // Set your server's fixed IP address here
IPAddress gateway(192, 168, 1, 1);    // Set your network Gateway usually your Router base address
IPAddress subnet(255, 255, 255, 0);   // Set your network sub-network mask here
IPAddress dns(192, 168, 1, 1);        // Set your network DNS usually your Router base address
#endif

char ssid_1[50];
char password_1[50];

char ssid_2[50];
char password_2[50];

char ssid_3[50];
char password_3[50];

char ssid_4[50];
char password_4[50];

File uploadFile;

//************************************************************
// DISPLAY SCREEN
//************************************************************
#define ENABLE_DISPLAY_WEBSERVER

#ifdef ENABLE_DISPLAY_WEBSERVER
#include <varioscreenGxEPD.h>
#endif

static bool hasSD = false;

//************************************************************
// CLASSE VARIOWIFISERVER
//************************************************************

//INIT Classe VarioWifiServer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
boolean VarioWifiServer::begin(void)
{
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  GnuSettings.VARIOMETER_SSID_1.toCharArray(ssid_1, sizeof(ssid_1));
  GnuSettings.VARIOMETER_PASSWORD_1.toCharArray(password_1, sizeof(password_1));

  GnuSettings.VARIOMETER_SSID_2.toCharArray(ssid_2, sizeof(ssid_2));
  GnuSettings.VARIOMETER_PASSWORD_2.toCharArray(password_2, sizeof(password_2));

  GnuSettings.VARIOMETER_SSID_3.toCharArray(ssid_3, sizeof(ssid_3));
  GnuSettings.VARIOMETER_PASSWORD_3.toCharArray(password_3, sizeof(password_3));

  GnuSettings.VARIOMETER_SSID_4.toCharArray(ssid_4, sizeof(ssid_4));
  GnuSettings.VARIOMETER_PASSWORD_4.toCharArray(password_4, sizeof(password_4));

#ifdef WIFI_DEBUG
  SerialPort.print("ssid_1 : ");
  SerialPort.println(ssid_1);
  SerialPort.print("password_1 : ");
  SerialPort.println(password_1);

  SerialPort.print("ssid_2 : ");
  SerialPort.println(ssid_2);
  SerialPort.print("password_2 : ");
  SerialPort.println(password_2);

  SerialPort.print("ssid_3 : ");
  SerialPort.println(ssid_3);
  SerialPort.print("password_3 : ");
  SerialPort.println(password_3);

  SerialPort.print("ssid_4 : ");
  SerialPort.println(ssid_4);
  SerialPort.print("password_4 : ");
  SerialPort.println(password_4);
#endif //WIFI_DEBUG

  File dataFile;

  //test présence fichier index
  if (dataFile = SDHAL_SD.open("/www/index.htm", FILE_READ))
  {
#ifdef WIFI_DEBUG
    SerialPort.println("SD Card initialized.");
#endif //WIFI_DEBUG

    hasSD = true;
    dataFile.close();
  }

#ifdef WIFI_DEBUG
  dataFile = SDHAL_SD.open("/", FILE_READ);
  dataFile.rewindDirectory();
  // SerialPort.println("");
  // SerialPort.println("ListeDirectory");
  // listDirectory(dataFile, 0);
  dataFile.close();
#endif //WIFI_DEBUG

  if (hasSD != true)
  {
#ifdef WIFI_DEBUG
    SerialPort.println("Erreur SDCARD");
#endif
    return false;
  }

#ifdef WIFI_DEBUG
  SerialPort.println("Begin Wifi done!");
#endif

#ifdef WIFI_DEBUG
  SerialPort.print("GnuVario-E Firmware Version : ");
  SerialPort.println(GnuSettings.getVersion());

  SerialPort.print("GnuVario-E Type d'écran : ");
  SerialPort.println(GnuSettings.getScreenModel());

  SerialPort.print("Serveur WEB : ");
  SerialPort.println(WEBSERVERTYPE);

#endif //WIFI_DEBUG

  // Init URL web server
  esp32FOTA.checkURL = GnuSettings.URL_UPDATE; //"http://gnuvario-e.yj.fr/update/firmware.json";

  return true;
};

/***********************************/
void VarioWifiServer::disableWifi(void)
{
  /***********************************/
  WiFi.mode(WIFI_OFF);
}

/***********************************/
void VarioWifiServer::disableBT(void)
{
  /***********************************/
}

void VarioWifiServer::disableAll(void)
{
  disableWifi();
  disableBT();
}

//Connection au wifi
/***********************************/
void VarioWifiServer::connect(void)
{
/***********************************/
#ifdef IP_FIXE
  if (!WiFi.config(local_IP, gateway, subnet, dns))
  { //WiFi.config(ip, gateway, subnet, dns1, dns2);
    SerialPort.println("WiFi STATION Failed to configure Correctly");
  }
#endif //IP_FIXE

  if (ssid_1[0] != '\0' && password_1[0] != '\0')
  {
    wifiMulti.addAP(ssid_1, password_1); // add Wi-Fi networks you want to connect to, it connects strongest to weakest
  }
  if (ssid_2[0] != '\0' && password_2[0] != '\0')
  {
    wifiMulti.addAP(ssid_2, password_2);
  }
  if (ssid_3[0] != '\0' && password_3[0] != '\0')
  {
    wifiMulti.addAP(ssid_3, password_3);
  }
  if (ssid_4[0] != '\0' && password_4[0] != '\0')
  {
    wifiMulti.addAP(ssid_4, password_4);
  }

  SerialPort.println("Connecting ...");
#ifdef ENABLE_DISPLAY_WEBSERVER
  screen.ScreenViewWifi("", "");
#endif //ENABLE_DISPLAY_WEBSERVER

  int counter = 0;
  while (wifiMulti.run() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    SerialPort.print('.');
    counter++;
    if (counter > 30)
    {
      SerialPort.println(F("Error Wifi not found!"));
      ESP.restart();
    }
  }
  SerialPort.println("\nConnected to " + WiFi.SSID() + " Use IP address: " + WiFi.localIP().toString()); // Report which SSID and IP is in use
  // The logical name http://fileserver.local will also access the device if you have 'Bonjour' running or your system supports multicast dns
#ifdef ENABLE_DISPLAY_WEBSERVER
  screen.ScreenViewWifi(WiFi.SSID().substring(0, 12), WiFi.localIP().toString());
#endif //ENABLE_DISPLAY_WEBSERVER

  setClock();

  if (MDNS.begin(servername))
  {
    MDNS.addService("http", "tcp", 80);
    SerialPort.println("MDNS responder started");
    SerialPort.print("You can now connect to http://");
    SerialPort.print(servername);
    SerialPort.println(".local");
  }
  else
  {
    SerialPort.println(F("Error setting up MDNS responder!"));
    ESP.restart();
  }

  checkDbVersion();
}

//Démarrage du serveur Web
/***********************************/
void VarioWifiServer::start(void)
{
  /***********************************/

  //////////////////////////////////////
  //   Liste des routes spécifiques   //
  //////////////////////////////////////

  // téléchargements de "mes vols" = contenu du repertoire vols
  server.on("/flights", HTTP_GET, handleListFlights);

  // téléchargements de "mes vols" en BDD
  server.on("/flightsbdd", HTTP_GET, handleGetFlights);

  // enregistrement un vol en BDD
  server.on("/flightsbdd", HTTP_POST, handleSetFlight);

  // enregistrement un vol en BDD
  server.on("/flightsbdd", HTTP_DELETE, handleDelFlight);

  //récupération de la liste du contenu de la carte SD
  server.on("/list", HTTP_GET, handlePrintDirectory);

  //récupération du contenu du fichier param
  server.on("/params", HTTP_GET, handleParams);

  // sauvegarde des parametres
  server.on("/params", HTTP_POST, handleSaveParams);

  //telechargement d'un fichier dont le nom complet avec chemin se trouve en param
  server.on("/file", HTTP_GET, handleFileDownload);

  //suppression d'un fichier dont le nom complet avec chemin se trouve en param
  server.on("/file", HTTP_DELETE, handleFileDelete);

  //creation d'un fichier ou d'un repertoire
  server.on("/create", HTTP_PUT, handleFileCreate);

  //recuperation des versions de firmware
  server.on("/firmwareversion", HTTP_GET, handleFirmwareVersion);

  //Mise à jour via internet
  server.on("/upgradeweb", HTTP_GET, handleUpgradeWeb);

  // upload d'un fichier, le chemin de destination se trouve dans le nom du fichier posté
  server.on(
      "/upload", HTTP_POST, []() {
        //   returnOK();
        server.send(200);
      },
      handleFileUpload);

  // mise à jour OTA
  server.on(
      "/fupdate", HTTP_POST, []() { server.send(200); }, handleFileUpdate);

  //récupération du contenu du fichier param
  server.on("/wifi", HTTP_GET, handleWifi);

  // sauvegarde du contenu du fichier param
  server.on("/wifi", HTTP_POST, handleSaveWifi);

  //récupération du contenu du fichier preference
  server.on("/webconfig", HTTP_GET, handleWebConfig);

  // sauvegarde du contenu du fichier preference
  server.on("/webconfig", HTTP_POST, handleSaveWebConfig);

  // sauvegarde du contenu du fichier preference
  server.on("/parseigc", HTTP_GET, handleParseIgc);

  // récupération de la liste des sites
  server.on("/site", HTTP_GET, handleGetSites);

  // sauvegarde d'un site
  server.on("/site", HTTP_POST, handleSetSite);

  // suppression d'un site
  server.on("/site", HTTP_DELETE, handleDelSite);

  //Aucun route spécifique, on regarde si il s'agit d'un fichier du répertoire www
  server.onNotFound(handleNotFound);

  ///////////////////////////// End of Request commands

  server.begin();
  SerialPort.println("HTTP server started");
#ifdef ENABLE_DISPLAY_WEBSERVER
  screen.ScreenViewWifi("START", "");
#endif //ENABLE_DISPLAY_WEBSERVER
}

/***********************************/
void VarioWifiServer::setClock()
{
  /***********************************/
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

#ifdef WIFI_DEBUG
  SerialPort.print(F("Waiting for NTP time sync: "));
#endif

  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    delay(500);
#ifdef WIFI_DEBUG
    SerialPort.print(F("."));
#endif
    yield();
    nowSecs = time(nullptr);
  }

#ifdef WIFI_DEBUG
  SerialPort.println();
#endif
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);

#ifdef WIFI_DEBUG
  SerialPort.print(F("Current time: "));
  SerialPort.print(asctime(&timeinfo));
#endif
}

/***********************************/
void VarioWifiServer::handleClient(void)
{
  /***********************************/
  server.handleClient(); // Listen for client connections
}

// Retour 200 = succes
/***********************************/
void returnOK()
{
  /***********************************/
  server.send(200, "text/plain", "");
}

// Retour 500 = erreur serveur
/***********************************/
void returnFail(String msg)
{
  /***********************************/
  server.send(500, "text/plain", msg + "\r\n");
}

/***********************************/
void listDirectory(File dir, int numTabs)
{
  /***********************************/

  dir.rewindDirectory();

  while (true)
  {
    File entry;

    if (!(entry = dir.openNextFile(FILE_READ)))
    {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++)
    {
      SerialPort.print('\t');
    }

    Serial.print(entry.name());
    if (entry.isDirectory())
    {
      Serial.println("/");
      listDirectory(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      SerialPort.print("\t\t");
      SerialPort.println(entry.size(), DEC);
    }

    entry.close();
  }
}

//////////////////////////////////////
//          GENERIC HANDLERS        //
//////////////////////////////////////

// envoi du contenu d'un fichier avec les bons entetes si trouvé

/***********************************/
bool loadFromSdCard(String path)
{
  /***********************************/
  String dataType = "text/plain";

  String basePath = "/www";

  path = basePath + path;

  if (path.endsWith("/"))
  {
    //si l'url est de type "dossier" on rajoute index.htm
    path += "index.htm";
  }

  if (path.endsWith(".src"))
  {
    path = path.substring(0, path.lastIndexOf("."));
  }
  else if (path.endsWith(".htm"))
  {
    dataType = "text/html";
  }
  else if (path.endsWith(".css"))
  {
    dataType = "text/css";
  }
  else if (path.endsWith(".js"))
  {
    dataType = "application/javascript";
  }
  else if (path.endsWith(".png"))
  {
    dataType = "image/png";
  }
  else if (path.endsWith(".gif"))
  {
    dataType = "image/gif";
  }
  else if (path.endsWith(".jpg"))
  {
    dataType = "image/jpeg";
  }
  else if (path.endsWith(".ico"))
  {
    dataType = "image/x-icon";
  }
  else if (path.endsWith(".xml"))
  {
    dataType = "text/xml";
  }
  else if (path.endsWith(".pdf"))
  {
    dataType = "application/pdf";
  }
  else if (path.endsWith(".zip"))
  {
    dataType = "application/zip";
  }
  else if (path.endsWith(".svg"))
  {
    dataType = "image/svg+xml";
  }
  else if (path.endsWith(".igc"))
  {
    dataType = "text/plain";
  }

#ifdef HAVE_SDCARD

  File dataFile;

  if (!(dataFile = SDHAL_SD.open(path.c_str(), FILE_READ)))
  {
    return false;
  };
  if (dataFile.isDirectory())
  {
    dataFile.close();
    path += "/index.htm";
    dataType = "text/html";
    if (!(dataFile = SDHAL_SD.open(path.c_str(), FILE_READ)))
    {
      return false;
    };
  }

  if (server.hasArg("download"))
  {
    dataType = "application/octet-stream";
  }

  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
#ifdef WIFI_DEBUG
    SerialPort.println("Sent less data than expected!");
#endif
  }

  dataFile.close();
  return true;
#else
  return false;
#endif
}

//////////////////////////////////////
//       END  GENERIC HANDLERS       //
//////////////////////////////////////

//////////////////////////////////////
//          SPECIFIC HANDLERS       //
//////////////////////////////////////

// Récupération de la liste des fichiers du répertoire vols au format json
/***********************************/
void handleListFlights()
{
  /***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleListFlights");
#endif

  String path = "/vols";

  File dir;
  dir = SDHAL_SD.open((char *)path.c_str(), FILE_READ);

  if (!dir.isDirectory())
  {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "");

  //  WiFiClient client = server.client();

  server.sendContent("[");
  for (int cnt = 0; true; ++cnt)
  {
    File entry;
    entry = dir.openNextFile(FILE_READ);
    if (!entry)
    {
      TRACE();
      break;
    }

    TRACE();
    String output;
    if (cnt > 0)
    {
      output = ',';
    }

    String fsize = "";

    if (!entry.isDirectory())
    {
      int bytes = entry.size();
      fsize = getFileSizeStringFromBytes(bytes);
      output += "{\"type\":\"";

      output += (entry.isDirectory()) ? "dir" : "file";

      output += "\",\"name\":\"";

      output += entry.name();

      output += "\",\"size\":\"";
      output += fsize;
      output += "\"";
      //VarioIgcParser varioIgcParser;
      // varioIgcParser.parseFile("/vols/" + String(entry.name()));
      //output += ", \"info\": " + varioIgcParser.getJson();
      output += "}";
      server.sendContent(output);
    }
    else
    {
      fsize = "na";
    }

    // output += "{\"type\":\"";

    // output += (entry.isDirectory()) ? "dir" : "file";

    // output += "\",\"name\":\"";

    // output += entry.name();

    // output += "\",\"size\":\"";
    // output += fsize;
    // output += "\"";
    // //VarioIgcParser varioIgcParser;
    // // varioIgcParser.parseFile("/vols/" + String(entry.name()));
    // //output += ", \"info\": " + varioIgcParser.getJson();
    // output += "}";
    // server.sendContent(output);
    entry.close();
  }
  server.sendContent("]");

  //correction bug chunk transfer webserver
  server.sendContent("");
  server.client().stop();
  dir.close();
  TRACE();
}

// Récupération du contenu du fichier parameters.jso
/*****************************/
void handleParams()
/*****************************/
{
#ifdef WIFI_DEBUG
  SerialPort.println("handleParams");
#endif
  if (server.uri() != "/params")
  {
    return returnFail("BAD ARGS");
  }

  String dataType = "application/json";
  String path = "/params.jso";

  File dataFile;

  if (!(dataFile = SDHAL_SD.open((char *)path.c_str(), FILE_READ)))
  {
#ifdef WIFI_DEBUG
    SerialPort.println("params.jso introuvable");
#endif
    dataFile.close();
    return returnFail("NO FILE named params.jso");
  }

  //gestion des CORS
  server.sendHeader("Access-Control-Allow-Origin", "*");

  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
    SerialPort.println("Sent less data than expected!");
  }

  dataFile.close();

  return;
}

// Récupération du contenu du fichier prefs.jso
/*****************************/
void handleWebConfig()
/*****************************/
{
#ifdef WIFI_DEBUG
  SerialPort.println("handleWebConfig");
#endif
  if (server.uri() != "/webconfig")
  {
    return returnFail("BAD ARGS");
  }

  String dataType = "application/json";
  String path = "/prefs.jso";

  File dataFile;

  if (!(dataFile = SDHAL_SD.open((char *)path.c_str(), FILE_READ)))
  {
#ifdef WIFI_DEBUG
    SerialPort.println("prefs.jso introuvable");
#endif
    dataFile.close();

    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "");
#ifdef WIFI_DEBUG
    SerialPort.println("[");
#endif
    server.sendContent("{}");

    //correction bug chunk transfer webserver
    server.sendContent("");
    server.client().stop();

    return;
  }

  //gestion des CORS
  server.sendHeader("Access-Control-Allow-Origin", "*");

  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
    SerialPort.println("Sent less data than expected!");
  }

  dataFile.close();

  return;
}

// Récupération du contenu du fichier wifi.cfg
/*****************************/
void handleWifi()
/*****************************/
{
#ifdef WIFI_DEBUG
  SerialPort.println("handleWifi");
#endif
  if (server.uri() != "/wifi")
  {
    return returnFail("BAD ARGS");
  }

  String dataType = "text/plain";
  String path = "/wifi.cfg";

#ifdef WIFI_DEBUG
  SerialPort.print("file name :");
  SerialPort.println(path.c_str());
#endif

  File dataFile;
  if (!(dataFile = SDHAL_SD.open((char *)path.c_str(), FILE_READ)))
  {
#ifdef WIFI_DEBUG
    SerialPort.println("wifi.cfg introuvable");
#endif
    dataFile.close();
    return returnFail("NO FILE named wifi.cfg");
  }

  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
    SerialPort.println("Sent less data than expected!");
  }

  dataFile.close();
  return;
}

// récupération en JSON de la liste des fichiers d'un répertoire
// ou de la racine de la carte SD
// récursivement ou non
/***********************************/
void handlePrintDirectory()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handlePrintDirectory");
#endif

  String path;
  boolean isRecursive = true;

  if (!server.hasArg("dir"))
  {
    path = "/";
  }
  else
  {
    path = server.arg("dir");
  }

  if (server.hasArg("norecursive"))
  {
    isRecursive = false;
  }

#ifdef WIFI_DEBUG
  SerialPort.print("dir : ");
  SerialPort.println((char *)path.c_str());
#endif

  File dir;
  dir = SDHAL_SD.open((char *)path.c_str(), FILE_READ);
  dir.rewindDirectory();

  if (!dir.isDirectory())
  {
#ifdef WIFI_DEBUG
    SerialPort.println("Not directory");
#endif

    dir.close();
    return returnFail("NOT DIR");
  }

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "");
#ifdef WIFI_DEBUG
  SerialPort.println("[");
#endif

  server.sendContent("[");

  String output;
  output += "{\"type\":\"";
  output += "dir";
  output += "\",\"name\":\"";
  output += path;
  output += "\",\"size\":\"";
  output += "na";
  output += "\"";

  output += ",\"contents\" :[";
#ifdef WIFI_DEBUG
  //SerialPort.println(output);
#endif
  server.sendContent(output);
  printDirectoryRecurse(path, isRecursive);
  output = "]";

  output += "}";
  server.sendContent(output);
  // printDirectoryRecurse(path);

  server.sendContent("]");

  //correction bug chunk transfer webserver
  server.sendContent("");
  server.client().stop();
#ifdef WIFI_DEBUG
  SerialPort.println("]");
#endif
  dir.close();
}

/***********************************/
void printDirectoryRecurse(String path, boolean isRecursive)
/***********************************/
{

  File dir;
  dir = SDHAL_SD.open((char *)path.c_str(), FILE_READ);
  dir.rewindDirectory();

  int tmpcnt = 0;

  for (int cnt = 0; true; ++cnt)
  {
    File entry;
    if (!(entry = dir.openNextFile(FILE_READ)))
    {
      // TRACE();
      break;
    }
    String tmpName = entry.name();

    if (tmpName.equalsIgnoreCase("SYSTEM~1") || tmpName.startsWith(".")) //equalsIgnoreCase(".TRASH~1"))
    {
      continue;
    }

    String output;
    if (tmpcnt > 0)
    {
      output = ',';
    }

    String fsize = "";

    if (!entry.isDirectory())
    {
      int bytes = entry.size();
      fsize = getFileSizeStringFromBytes(bytes);
    }
    else
    {
      fsize = "na";
    }

    output += "{\"type\":\"";

    output += (entry.isDirectory()) ? "dir" : "file";

    output += "\",\"name\":\"";
    String tmpFullName = entry.name();
    output += tmpFullName.substring(tmpFullName.lastIndexOf("/") + 1);

    output += "\",\"size\":\"";
    output += fsize;
    output += "\"";

    if (entry.isDirectory())
    {
      output += ",\"contents\" :[";
#ifdef WIFI_DEBUG
      // SerialPort.println(output);
#endif
      server.sendContent(output);

      if (isRecursive)
      {
        printDirectoryRecurse(entry.name(), isRecursive);
      }
      output = "]";
    }
    output += "}";

#ifdef WIFI_DEBUG
    //SerialPort.println(output);
#endif

    server.sendContent(output);
    entry.close();
    tmpcnt++;
  }
}

// téléchargement d'un fichier dont le chemin+ nom se trouve en param
/***********************************/
void handleFileDownload()
{
  /***********************************/

  //handleFileDownload
#ifdef WIFI_DEBUG
  SerialPort.println("handleFileDownload");
#endif

  if (server.uri() != "/file")
  {
    return returnFail("BAD URL");
  }
  if (server.args() == 0)
  {
    return returnFail("BAD ARGS");
  }

  String path = server.arg(0);
  DUMP(path);

  File dataFile;
  if (!(dataFile = SDHAL_SD.open((char *)path.c_str(), FILE_READ)))
  {
    return returnFail("NO FILE");
  }

  //gestion des CORS
  server.sendHeader("Access-Control-Allow-Origin", "*");

  String dataType = "application/octet-stream";

  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
    SerialPort.println("Sent less data than expected!");
  }

  dataFile.close();
}

// upload d'un fichier dont le chemin + nom se trouve en param dans le nommage du fichier
/***********************************/
void handleFileUpload()
{
  /***********************************/
  // #ifdef WIFI_DEBUG
  //   SerialPort.println("handleFileUpload");
  // #endif

  if (server.uri() != "/upload")
  {
    return returnFail("BAD URL");
  }

  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    DUMP(upload.filename);
    if (SDHAL_SD.exists((char *)upload.filename.c_str()))
    {
      SDHAL_SD.remove((char *)upload.filename.c_str());
      //TRACE();
    }

    uploadFile = SDHAL_SD.open(upload.filename.c_str(), FILE_WRITE);
    if (!uploadFile)
    {
#ifdef WIFI_DEBUG
      SerialPort.print("Impossible de créer le fichier : ");
      SerialPort.println(upload.filename);
#endif
    }

#ifdef WIFI_DEBUG
    SerialPort.print("Upload: START, filename: ");
    SerialPort.println(upload.filename);
#endif
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {

    if (uploadFile)
    {
      uploadFile.write(upload.buf, upload.currentSize);
      // uploadFile.flush();

#ifdef WIFI_DEBUG
      SerialPort.print("Upload: WRITE, Bytes: ");
      SerialPort.println(upload.currentSize);
#endif
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    TRACE();

    if (uploadFile)
    {
      // uploadFile.flush();
      uploadFile.close();

#ifdef WIFI_DEBUG
      SerialPort.print("Upload: END, Size: ");
      SerialPort.println(upload.totalSize);
#endif
      return returnOK();
    }
  }
  else if (upload.status == UPLOAD_FILE_ABORTED)
  {
// gestion du abort
#ifdef WIFI_DEBUG
    SerialPort.print("Upload: ABORT, Size: ");
    SerialPort.println(upload.currentSize);
#endif
    //on va supprimer le fichier partiel
    // uploadFile.flush();
    uploadFile.close();
    SDHAL_SD.remove((char *)upload.filename.c_str());

    return returnFail("Upload aborted");
  }
}

/***********************************/
void handleFirmwareVersion()
{
  /***********************************/
  /*
	//recuperation des versions de firmware
	server.on("/firmwareversion", HTTP_GET, handleFirmwareVersion);
*/
#ifdef WIFI_DEBUG
  SerialPort.println("handleFirmwareVersion");
#endif

  if (server.uri() != "/firmwareversion")
  {
    returnFail("BAD URL");
  }

  String output = esp32FOTA.getHTTPVersion();
#ifdef WIFI_DEBUG
  SerialPort.println(output);
#endif

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "");
  //WiFiClient client = server.client();

  server.sendContent(output);

  //correction bug chunk transfer webserver
  server.sendContent("");
  server.client().stop();
  TRACE();
}

/***********************************/
void handleUpgradeWeb()
{
  /***********************************/

  /*
	//  Mise à jour via internet
  //	server.on("/upgradeweb", HTTP_POST, handleUpgradeWeb);
  */

  if (server.uri() != "/upgradeweb")
  {
    returnFail("BAD URL");
  }

  if (server.args() == 0)
  {
    return returnFail("BAD ARGS");
  }

  bool betaversion = server.arg(0);

  int8_t updatedNeeded = esp32FOTA.execHTTPcheck(betaversion);
  if (updatedNeeded == 1)
  {
#ifdef WIFI_DEBUG
    SerialPort.println("############Update firmware########");
#endif

    returnOK();
    esp32FOTA.execOTA();
  }
  else
  {
    return returnFail("could not update");
  }
}

// suppression d'un fichier dont le chemin + nom se trouve en param
/**********************************/
void handleFileDelete()
{
  /**********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleFileDelete");
#endif

  if (server.uri() != "/file")
  {
    TRACE();
    return returnFail("BAD URL");
  }

  DUMP(server.args());
  DUMP(server.arg(0));

  if (server.args() == 0)
  {
    TRACE();
    return returnFail("BAD ARGS");
  }

  String path = server.arg(0);
  if (!SDHAL_SD.exists((char *)path.c_str()))
  {
    TRACE();
    return returnFail("FILE DOES NOT EXIST");
  }
  TRACE();
  DUMP(path);
  deleteRecursive(path);
  returnOK();
}

/***********************************/
void deleteRecursive(String path)
{
  /***********************************/

  File fileSD;

  fileSD = SDHAL_SD.open((char *)path.c_str(), FILE_READ);
  if (!fileSD.isDirectory())
  {
    fileSD.close();
    SDHAL_SD.remove((char *)path.c_str());
    TRACE();
    return;
  }

  fileSD.rewindDirectory();

  while (true)
  {
    File entry;
    if (!(entry = fileSD.openNextFile(FILE_READ)))

    {
      break;
    }

    String entryPath = entry.name();
    if (entry.isDirectory())
    {
      entry.close();
      deleteRecursive(entryPath);
    }
    else
    {
      entry.close();
      SDHAL_SD.remove((char *)entryPath.c_str());
    }
    yield();
  }

  SDHAL_SD.rmdir((char *)path.c_str());
  fileSD.close();
}

/***********************************/
void handleFileCreate()
{
  /***********************************/
  if (server.args() == 0)
  {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  if (path == "/" || SDHAL_SD.exists((char *)path.c_str()))
  {
    returnFail("BAD PATH");
    return;
  }

  if (path.indexOf('.') > 0)
  {
    File fileSD = SDHAL_SD.open((char *)path.c_str(), FILE_WRITE);
    if (fileSD)
    {
      fileSD.print("\0");
      fileSD.close();
    }
  }
  else
  {
    SDHAL_SD.mkdir((char *)path.c_str());
  }
  returnOK();
}

/***********************************/
void handleNotFound()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleNotFound");
#endif

  if (server.method() == HTTP_OPTIONS)
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    server.sendHeader("Access-Control-Allow-Methods", "POST,GET,PUT,OPTION,DELETE");
    return returnOK();
  }

  if (hasSD && loadFromSdCard(server.uri()))
  {
    return;
  }

  String message = ""; //"SDCARD Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  if (server.method() == HTTP_GET)
  {
    message += "GET";
  }
  else if (server.method() == HTTP_POST)
  {
    message += "POST";
  }
  else if (server.method() == HTTP_DELETE)
  {
    message += "DELETE";
  }
  else if (server.method() == HTTP_OPTIONS)
  {
    message += "OPTIONS";
  }
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  SerialPort.print(message);
}

#ifdef HAVE_SDCARD
File UpdateFile;
#endif

/***********************************/
void handleFileUpdate()
{ // update ESP32
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleFileUpdate");

  SerialPort.println("File update begin");
#endif //WIFI_DEBUG

  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    Serial.printf("Update: %s\n", upload.filename.c_str());

    if (!Update.begin(UPDATE_SIZE_UNKNOWN))
    { //start with max available size
      Update.printError(SerialPort);
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    /* flashing firmware to ESP*/
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
    {
      Update.printError(SerialPort);
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (Update.end(true))
    { //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);

      SerialPort.println("RESTART ESP32");
      SerialPort.flush();

      returnOK();

      ESP_LOGI("GnuVario-E", "RESTART ESP32");
#ifdef ENABLE_DISPLAY_WEBSERVER
      screen.ScreenViewReboot();
#endif
      ESP.restart();
    }
    else
    {
      Update.printError(SerialPort);
      return returnFail("could not update");
    }
  }
}

// sauvegarde de la configuration
/***********************************/
void handleSaveParams()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleSaveParams");
#endif

  if (server.uri() != "/params")
  {
#ifdef WIFI_DEBUG
    SerialPort.println("Erreur : BAD ARGS");
#endif
    return returnFail("BAD ARGS");
  }

  String path = "/params.jso";
  String pathBak = "/params.bak";
  String content = server.arg(0);

  if (SDHAL_SD.exists((char *)pathBak.c_str()))
  {
    SDHAL_SD.remove((char *)pathBak.c_str());
  }
  size_t n;
  uint8_t buf[64];

  File dataFile;
  dataFile = SDHAL_SD.open(path.c_str(), FILE_READ);
  File dataFile2;
  dataFile2 = SDHAL_SD.open(pathBak.c_str(), FILE_WRITE);

  while ((n = dataFile.read(buf, sizeof(buf))) > 0)
  {
    dataFile2.write(buf, n);
  }
  dataFile.close();
  dataFile2.close();

  SDHAL_SD.remove((char *)path.c_str());

  if (!(dataFile = SDHAL_SD.open(path.c_str(), FILE_WRITE)))
  {
    return returnFail("NO FILE");
  }

  dataFile.println(content);

  dataFile.close();
  return returnOK();
}

// sauvegarde des preferences
/***********************************/
void handleSaveWebConfig()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleSaveParams");
#endif

  if (server.uri() != "/webconfig")
  {
#ifdef WIFI_DEBUG
    SerialPort.println("Erreur : BAD ARGS");
#endif
    return returnFail("BAD ARGS");
  }

  String path = "/prefs.jso";
  String content = server.arg(0);

  File dataFile;

  SDHAL_SD.remove((char *)path.c_str());

  if (!(dataFile = SDHAL_SD.open(path.c_str(), FILE_WRITE)))
  {
    return returnFail("NO FILE");
  }

  dataFile.println(content);

  dataFile.close();

  return returnOK();
}

// sauvegarde de la configuration wifi
/***********************************/
void handleSaveWifi()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleSaveWifi");
#endif

  if (server.uri() != "/wifi")
  {
#ifdef WIFI_DEBUG
    SerialPort.println("Erreur : BAD ARGS");
#endif
    return returnFail("BAD ARGS");
  }

  String path = "/wifi.cfg";
  String pathBak = "/wifi.bak";
  String content = server.arg(0);

  if (SDHAL_SD.exists((char *)pathBak.c_str()))
  {
    SDHAL_SD.remove((char *)pathBak.c_str());
  }
  size_t n;
  uint8_t buf[64];

  File dataFile;
  dataFile = SDHAL_SD.open(path.c_str(), FILE_READ);
  File dataFile2;
  dataFile2 = SDHAL_SD.open(pathBak.c_str(), FILE_READ);

  while ((n = dataFile.read(buf, sizeof(buf))) > 0)
  {
    dataFile2.write(buf, n);
  }
  dataFile.close();
  dataFile2.close();

  SDHAL_SD.remove((char *)path.c_str());

  if (!(dataFile = SDHAL_SD.open(path.c_str(), FILE_WRITE)))
  {
    return returnFail("NO FILE");
  }

  dataFile.println(content);

  dataFile.close();
  return returnOK();
}

// parsage d'un fichier IGC
/***********************************/
void handleParseIgc()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleParseIgc");
#endif

  if (server.uri() != "/parseigc")
  {
    return returnFail("BAD URL");
  }
  if (server.args() == 0)
  {
    return returnFail("BAD ARGS");
  }

  String path = server.arg(0);

  File dataFile;
  server.sendHeader("Access-Control-Allow-Origin", "*");
  //test présence fichier
  if (dataFile = SDHAL_SD.open(path, FILE_READ))
  {
    //parsage du fichier IGC
    VarioIgcParser varioIgcParser;
    varioIgcParser.parseFile(path);

    VarioSqlFlight varioSqlFlight;
    varioSqlFlight.insertFlight(varioIgcParser.getJson());

    String tmpFullName = dataFile.name();
    dataFile.close();

    String filename = tmpFullName.substring(tmpFullName.lastIndexOf("/") + 1);
    if (!SDHAL_SD.exists("/vols/parsed"))
    {
      SDHAL_SD.mkdir("/vols/parsed");
    }
    SDHAL_SD.rename(path, "/vols/parsed/" + filename);

    return returnOK();
  }
  return returnFail("NO");
}

// Récupération de la liste des vols en bdd
/***********************************/
void handleGetFlights()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleGetFlights");
#endif
  if (server.uri() != "/flightsbdd")
  {
    return returnFail("BAD URL");
  }

  uint8_t offset;
  uint8_t limit;
  if (server.hasArg("offset"))
  {
    offset = server.arg("offset").toInt();
  }
  else
  {
    offset = 0;
  }
  if (server.hasArg("limit"))
  {
    limit = server.arg("limit").toInt();
  }
  else
  {
    limit = 20;
  }

  VarioSqlFlight varioSqlFlight;

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "");
  server.sendContent("[");

  //WiFiClient client = server.client();
  varioSqlFlight.initGetFlightsQuery(limit, offset);
  String unvol = varioSqlFlight.getNextFlight();
  boolean firstLine = true;
  while (unvol != "")
  {
    if (firstLine)
    {
      firstLine = false;
    }
    else
    {
      unvol = "," + unvol;
    }
    server.sendContent(unvol);
    unvol = varioSqlFlight.getNextFlight();
  }
  server.sendContent("]");

  //correction bug chunk transfer webserver
  server.sendContent("");
  server.client().stop();
}

// Enregistrement d'un vol
/***********************************/
void handleSetFlight()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleSetFlight");
#endif

  if (server.uri() != "/flightsbdd")
  {
    return returnFail("BAD URL");
  }

  VarioSqlFlight varioSqlFlight;
  String content;

  if (server.hasArg("id"))
  {
    uint8_t id = server.arg("id").toInt();
    content = server.arg(1);

#ifdef WIFI_DEBUG
    SerialPort.println(id);
    SerialPort.println(content);
#endif

    varioSqlFlight.updateFlight(id, content);
  }
  else
  {
    content = server.arg(0);

#ifdef WIFI_DEBUG
    SerialPort.println(content);
#endif

    varioSqlFlight.insertFlight(content);
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  return returnOK();
}

// Suppression d'un vol
/***********************************/
void handleDelFlight()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleDelFlight");
#endif

  if (server.uri() != "/flightsbdd")
  {
    return returnFail("BAD URL");
  }

  if (server.hasArg("filename"))
  {
    String filename = server.arg("filename");
    File dataFile;
    if (dataFile = SDHAL_SD.open("/vols/parsed/" + filename, FILE_READ))
    {
      dataFile.close();
      SDHAL_SD.rename("/vols/parsed/" + filename, "/vols/" + filename);
    }
  }

  VarioSqlFlight varioSqlFlight;

  if (server.hasArg("id"))
  {
    uint8_t id = server.arg("id").toInt();

#ifdef WIFI_DEBUG
    SerialPort.println(id);

#endif

    varioSqlFlight.delFlight(id);
  }

  server.sendHeader("Access-Control-Allow-Origin", "*");
  return returnOK();
}

// Récupération de la liste des sites
/***********************************/
void handleGetSites()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleGetSites");
#endif
  if (server.uri() != "/site")
  {
    return returnFail("BAD URL");
  }

  VarioSqlFlight varioSqlFlight;

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "");
  //WiFiClient client = server.client();

  server.sendContent(varioSqlFlight.getSites());

  //correction bug chunk transfer webserver
  server.sendContent("");
  server.client().stop();
}

// Enregistrement d'un site
/***********************************/
void handleSetSite()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleSetSite");
#endif

  if (server.uri() != "/site")
  {
    return returnFail("BAD URL");
  }

  VarioSqlFlight varioSqlFlight;
  String content;

  if (server.hasArg("id"))
  {
    uint8_t id = server.arg("id").toInt();
    content = server.arg(1);

#ifdef WIFI_DEBUG
    SerialPort.println(id);
    SerialPort.println(content);
#endif

    varioSqlFlight.updateSite(id, content);
  }
  else
  {
    content = server.arg(0);

#ifdef WIFI_DEBUG
    SerialPort.println(content);
#endif

    varioSqlFlight.insertSite(content);
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  return returnOK();
}

// Enregistrement d'un site
/***********************************/
void handleDelSite()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleSetSite");
#endif

  if (server.uri() != "/site")
  {
    return returnFail("BAD URL");
  }

  VarioSqlFlight varioSqlFlight;
  String content;

  if (server.hasArg("id"))
  {
    uint8_t id = server.arg("id").toInt();

#ifdef WIFI_DEBUG
    SerialPort.println(id);
    SerialPort.println(content);
#endif

    varioSqlFlight.deleteSite(id);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    return returnOK();
  }
  else
  {
    return returnFail("ID introuvable");
  }
}

//////////////////////////////////////
//      END SPECIFIC HANDLERS       //
//////////////////////////////////////

/***********************************/
String getFileSizeStringFromBytes(int bytes)
{
  // conversion taille
  /***********************************/
  String fsize = "";

  if (bytes < 1024)
    fsize = String(bytes) + " B";
  else if (bytes < (1024 * 1024))
    fsize = String(bytes / 1024.0, 3) + " KB";
  else if (bytes < (1024 * 1024 * 1024))
    fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
  else
    fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";

  return fsize;
}

bool checkDbVersion()
{
#ifdef WIFI_DEBUG
  SerialPort.println("checkDbVersion");
#endif

  if (!SDHAL_SD.exists("/db"))
  {
    SDHAL_SD.mkdir("/db");
    File myOrigFile;
    File myDestFile;

    if (SDHAL_SD.exists("/www/db/vol.db") && !SDHAL_SD.exists("/db/vol.db"))
    {
      myDestFile = SDHAL_SD.open("/db/vol.db", FILE_WRITE);
      myOrigFile = SDHAL_SD.open("/www/db/vol.db", FILE_READ);
      if (myDestFile && myOrigFile)
      {
        int data;
        while ((data = myOrigFile.read()) >= 0)
        {
          myDestFile.write(data);
        }
        myDestFile.close();
        myOrigFile.close();
      }
      else
      {
        return false;
      }
    }
  }
  String path;
  path = "/www/sql";

  File dir;
  dir = SDHAL_SD.open((char *)path.c_str(), FILE_READ);

  path = String();
  if (!dir.isDirectory())
  {
    dir.close();
    return false;
  }
  dir.rewindDirectory();
  VarioSqlFlight varioSqlFlight;

  for (int cnt = 0; true; ++cnt)
  {
    File entry;
    entry = dir.openNextFile(FILE_READ);
    if (!entry)
    {
      TRACE();
      break;
    }

    TRACE();
    String output;
    String tmpFullName = entry.name();
    String version = tmpFullName.substring(tmpFullName.lastIndexOf("/") + 1);
    version = version.substring(0, version.lastIndexOf("."));
    varioSqlFlight.executeMigration(version, entry.readString());

#ifdef WIFI_DEBUG
    SerialPort.println(version);
#endif
  }

  dir.close();
  TRACE();
  return true;
}

VarioWifiServer varioWifiServer;

#endif //WEBSERVER_SDCARD