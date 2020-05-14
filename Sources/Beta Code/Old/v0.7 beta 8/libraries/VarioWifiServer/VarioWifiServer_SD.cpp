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

#include <SPI.h>

#include <VarioSettings.h>

#include <Update.h>

#include <ArduinoJson.h>

#include <esp32fota2.h>

extern VarioSettings GnuSettings;

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

SdFile uploadFile;

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

  SdFile dataFile;

  //test présence fichier index
  if (dataFile.open("www/index.htm", O_RDONLY))
  {
#ifdef WIFI_DEBUG
    SerialPort.println("SD Card initialized.");
#endif //WIFI_DEBUG

    hasSD = true;
    dataFile.close();
  }

  dataFile.open("/", O_RDONLY);
  dataFile.rewind();
  dataFile.close();
#ifdef WIFI_DEBUG
  // SerialPort.println("");
  // SerialPort.println("ListeDirectory");
  // listDirectory(dataFile, 0);
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

  wifiMulti.addAP(ssid_1, password_1); // add Wi-Fi networks you want to connect to, it connects strongest to weakest
  wifiMulti.addAP(ssid_2, password_2); // Adjust the values in the Network tab
  wifiMulti.addAP(ssid_3, password_3);
  wifiMulti.addAP(ssid_4, password_4); // You don't need 4 entries, this is for example!

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
void listDirectory(SdFile dir, int numTabs)
{
  /***********************************/

  char fBuffer[32];
  dir.rewind();

  while (true)
  {
    SdFile entry;

    if (!entry.openNext(&dir, O_RDONLY))
    {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++)
    {
      SerialPort.print('\t');
    }

    entry.getName(fBuffer, 30);

    SerialPort.print(fBuffer);
    if (entry.isDir())
    {
      SerialPort.println("/");
      listDirectory(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      SerialPort.print("\t\t");
      SerialPort.println(entry.fileSize(), DEC);
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

  String basePath = "www";

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

  SdFile dataFile;
  boolean tmpretour;
  tmpretour = dataFile.open(path.c_str(), O_RDONLY);
  if ((tmpretour) && (dataFile.isDir()))
  {
    dataFile.close();
    path += "/index.htm";
    dataType = "text/html";
    tmpretour = dataFile.open(path.c_str(), O_RDONLY);
  }

  if (!tmpretour)
  {
    return false;
  }

  if (server.hasArg("download"))
  {
    dataType = "application/octet-stream";
  }

  if (server.streamFile(dataFile, dataType) != dataFile.fileSize())
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

  String path;
  path = "/vols";

  SdFile dir;
  dir.open((char *)path.c_str(), O_READ); //O_RDONLY);

  path = String();
  if (!dir.isDir())
  {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewind();

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "");
  WiFiClient client = server.client();

  server.sendContent("[");
  for (int cnt = 0; true; ++cnt)
  {
    SdFile entry;
    if (!entry.openNext(&dir, O_READ))
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

    if (!entry.isDir())
    {
      int bytes = entry.fileSize();
      fsize = getFileSizeStringFromBytes(bytes);
    }
    else
    {
      fsize = "na";
    }

    output += "{\"type\":\"";

    output += (entry.isDir()) ? "dir" : "file";

    output += "\",\"name\":\"";

    char fBuffer[32];
    entry.getName(fBuffer, 30);
    output += fBuffer;

    output += "\",\"size\":\"";
    output += fsize;
    output += "\"";
    output += "}";
    server.sendContent(output);
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

  SdFile dataFile;

  if (!dataFile.open((char *)path.c_str()), O_RDONLY)
  {
#ifdef WIFI_DEBUG
    SerialPort.println("params.jso introuvable");
#endif
    dataFile.close();
    return returnFail("NO FILE named params.jso");
  }

  //gestion des CORS
  server.sendHeader("Access-Control-Allow-Origin", "*");

  if (server.streamFile(dataFile, dataType) != dataFile.fileSize())
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
  String path = "wifi.cfg";

#ifdef WIFI_DEBUG
  SerialPort.print("file name :");
  SerialPort.println(path.c_str());
#endif

  SdFile dataFile;
  if (!dataFile.open((char *)path.c_str(), O_RDONLY))

  {
#ifdef WIFI_DEBUG
    SerialPort.println("wifi.cfg introuvable");
#endif
    dataFile.close();
    return returnFail("NO FILE named wifi.cfg");
  }

  if (server.streamFile(dataFile, dataType) != dataFile.fileSize())
  {
    SerialPort.println("Sent less data than expected!");
  }

  dataFile.close();
  return;
}

// récupération en JSON de la liste des fichiers d'un répertoire
// ou de la racine de la carte SD
// récursivement
/***********************************/
void handlePrintDirectory()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handlePrintDirectory");
#endif

  String path;
  if (!server.hasArg("dir"))
  {
    path = "/";
  }
  else
  {
    path = server.arg("dir");
  }

#ifdef WIFI_DEBUG
  SerialPort.print("dir : ");
  SerialPort.println((char *)path.c_str());
#endif

  SdFile dir;
  dir.open((char *)path.c_str(), O_RDONLY);
  dir.rewind();

  if (!dir.isDir())
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
  SerialPort.println(output);
#endif
  server.sendContent(output);
  printDirectoryRecurse(path);
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
void printDirectoryRecurse(String path)
/***********************************/
{

  char fBuffer[32];
  SdFile dir;
  dir.open((char *)path.c_str(), O_RDONLY);
  dir.rewind();

  int tmpcnt = 0;

  for (int cnt = 0; true; ++cnt)
  {
    SdFile entry;
    boolean tmpRetour = entry.openNext(&dir, O_RDONLY);
    entry.getName(fBuffer, 30);
    String tmpName = fBuffer;
    if (!tmpRetour)
    {
      TRACE();
      break;
    }

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

    if (!entry.isDir())
    {
      int bytes = entry.fileSize();
      fsize = getFileSizeStringFromBytes(bytes);
    }
    else
    {
      fsize = "na";
    }

    output += "{\"type\":\"";

    output += (entry.isDir()) ? "dir" : "file";

    output += "\",\"name\":\"";

    entry.getName(fBuffer, 30);
    output += fBuffer;

    output += "\",\"size\":\"";
    output += fsize;
    output += "\"";

    if (entry.isDir())
    {
      output += ",\"contents\" :[";
#ifdef WIFI_DEBUG
      SerialPort.println(output);
#endif
      server.sendContent(output);
      entry.getName(fBuffer, 30);
      printDirectoryRecurse(path + "/" + fBuffer);

      output = "]";
    }
    output += "}";

#ifdef WIFI_DEBUG
    SerialPort.println(output);
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

  SdFile dataFile;
  if (!dataFile.open((char *)path.c_str(), O_RDONLY))
  {
    return returnFail("NO FILE");
  }

  //gestion des CORS
  server.sendHeader("Access-Control-Allow-Origin", "*");

  String dataType = "application/octet-stream";

  if (server.streamFile(dataFile, dataType) != dataFile.fileSize())
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
#ifdef WIFI_DEBUG
  SerialPort.println("handleFileUpload");
#endif

  if (server.uri() != "/upload")
  {
    return returnFail("BAD URL");
  }

  if (server.method() == HTTP_OPTIONS)
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    returnOK();
  }

  boolean tmpReturn = false;

  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    DUMP(upload.filename);
    if (SDHAL_SD.exists((char *)upload.filename.c_str()))
    {
      SDHAL_SD.remove((char *)upload.filename.c_str());
      TRACE();
    }
    tmpReturn = uploadFile.open(upload.filename.c_str(), O_WRONLY | O_CREAT);
    if (!tmpReturn)
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

    if (tmpReturn)
    {
      uploadFile.write(upload.buf, upload.currentSize);
      uploadFile.flush();

#ifdef WIFI_DEBUG
      SerialPort.print("Upload: WRITE, Bytes: ");
      SerialPort.println(upload.currentSize);
#endif
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    TRACE();

    if (tmpReturn)
    {
      uploadFile.flush();
      uploadFile.close();

#ifdef WIFI_DEBUG
      SerialPort.print("Upload: END, Size: ");
      SerialPort.println(upload.totalSize);
#endif
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
    uploadFile.flush();
    uploadFile.close();
    SDHAL_SD.remove((char *)upload.filename.c_str());

    return returnFail("Upload aborted");
  }

  returnOK();
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
  WiFiClient client = server.client();

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

  SdFile fileSD;
  fileSD.open((char *)path.c_str(), O_RDONLY);
  if (!fileSD.isDir())
  {
    fileSD.close();
    SDHAL_SD.remove((char *)path.c_str());
    TRACE();
    return;
  }

  fileSD.rewind();

  while (true)
  {
    SdFile entry;
    if (!entry.openNext(&fileSD, O_RDONLY))

    {
      break;
    }

    char fBuffer[32];
    entry.getName(fBuffer, 30);
    String entryPath = path + "/" + fBuffer;
    if (entry.isDir())
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
void handleNotFound()
{
/***********************************/
#ifdef WIFI_DEBUG
  SerialPort.println("handleNotFound");
#endif

  if (hasSD && loadFromSdCard(server.uri()))
  {
    return;
  }

  if (server.method() == HTTP_OPTIONS)
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    returnOK();
  }

  String message = ""; //"SDCARD Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
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
SdFile UpdateFile;
#endif

/***********************************/
void handleFileUpdate()
{ // update ESP32
/***********************************/
//  boolean tmpReturn = false;
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

  SdFile dataFile;
  dataFile.open(path.c_str(), O_RDONLY);
  SdFile dataFile2;
  dataFile2.open(pathBak.c_str(), O_RDWR | O_CREAT);

  while ((n = dataFile.read(buf, sizeof(buf))) > 0)
  {
    dataFile2.write(buf, n);
  }
  dataFile.close();
  dataFile2.close();

  SDHAL_SD.remove((char *)path.c_str());

  if (!dataFile.open(path.c_str(), O_RDWR | O_CREAT))
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

  SdFile dataFile;
  dataFile.open(path.c_str(), O_RDONLY);
  SdFile dataFile2;
  dataFile2.open(pathBak.c_str(), O_RDWR | O_CREAT);

  while ((n = dataFile.read(buf, sizeof(buf))) > 0)
  {
    dataFile2.write(buf, n);
  }
  dataFile.close();
  dataFile2.close();

  SDHAL_SD.remove((char *)path.c_str());

  if (!dataFile.open(path.c_str(), O_RDWR | O_CREAT))
  {
    return returnFail("NO FILE");
  }

  dataFile.println(content);

  dataFile.close();
  return returnOK();
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

/***********************************
void handleCreate() {
// ***********************************
  if (server.args() == 0) {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  if (path == "/" || SDHAL.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }

  if (path.indexOf('.') > 0) {
    File fileSD = SDHAL.open((char *)path.c_str(), FILE_WRITE);
    if (fileSD) {
      fileSD.write("\0");
      fileSD.close();
    }
  } else {
    SDHAL.mkdir((char *)path.c_str());
  }
  returnOK();
}
*/

VarioWifiServer varioWifiServer;

#endif //WEBSERVER_SDCARD