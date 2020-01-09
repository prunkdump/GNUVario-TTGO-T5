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
/*                                                                               */
/*********************************************************************************/

//Arg nº0 –> plain: {"GnuvarioE":{"version":"0.6 beta 2"},"Systeme":{"BT_ENABLE":"0","NO_RECORD":"0","RATIO_CLIMB_RATE":"2","VARIOMETER_SENT_LXNAV_SENTENCE":"1","ALARM_SDCARD":"1","ALARM_GPSFIX":"1","ALARM_FLYBEGIN":"1"},"General":{"VARIOMETER_PILOT_NAME":"Magali","VARIOMETER_GLIDER_NAME":"MAC-PARA Muse 3","VARIOMETER_TIME_ZONE":"2","VARIOMETER_BEEP_VOLUME":"8"},"Vario":{"VARIOMETER_SINKING_THRESHOLD":"-2.0","VARIOMETER_CLIMBING_THRESHOLD":"0.2","VARIOMETER_NEAR_CLIMBING_SENSITIVITY":"0.5","VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM":"0","VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP":"0","VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE":"0","SETTINGS_CLIMB_PERIOD_COUNT":"10","SETTINGS_GLIDE_RATIO_PERIOD_COUNT":"20","RATIO_MAX_VALUE":"30.0","RATIO_MIN_SPEED":"10.0"},"Flight start":{"FLIGHT_START_MIN_TIMESTAMP":"15000","FLIGHT_START_VARIO_LOW_THRESHOLD":"-0.5","FLIGHT_START_VARIO_HIGH_THRESHOLD":"0.5","FLIGHT_START_MIN_SPEED":"8.0","VARIOMETER_RECORD_WHEN_FLIGHT_START":"0"},"calibration":{"VERTACCEL_GYRO_CAL_BIAS_00":"0x00","VERTACCEL_GYRO_CAL_BIAS_01":"0x00","VERTACCEL_GYRO_CAL_BIAS_02":"0x00","VERTACCEL_GYRO_CAL_BIAS_03":"0x00","VERTACCEL_GYRO_CAL_BIAS_04":"0x00","VERTACCEL_GYRO_CAL_BIAS_05":"0x00","VERTACCEL_GYRO_CAL_BIAS_06":"0x00","VERTACCEL_GYRO_CAL_BIAS_07":"0x00","VERTACCEL_GYRO_CAL_BIAS_08":"0x00","VERTACCEL_GYRO_CAL_BIAS_09":"0x00","VERTACCEL_GYRO_CAL_BIAS_10":"0x00","VERTACCEL_GYRO_CAL_BIAS_11":"0x00","VERTACCEL_ACCEL_CAL_BIAS_00":"0","VERTACCEL_ACCEL_CAL_BIAS_01":"0","VERTACCEL_ACCEL_CAL_BIAS_02":"0","VERTACCEL_ACCEL_CAL_SCALE":"0","VERTACCEL_MAG_CAL_BIAS_00":"0","VERTACCEL_MAG_CAL_BIAS_01":"0","VERTACCEL_MAG_CAL_BIAS_02":"0","VERTACCEL_MAG_CAL_PROJ_SCALE":"-16689","VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER":"7","VERTACCEL_MAG_CAL_BIAS_MULTIPLIER":"5"},"Wifi":{"wifi_1":{"SSID":"your_SSID1","PASSWORD":"your_PASSWORD_for SSID1"},"wifi_2":{"SSID":"your_SSID2","PASSWORD":"your_PASSWORD_for SSID2"},"wifi_3":{"SSID":"your_SSID3","PASSWORD":"your_PASSWORD_for SSID3"},"wifi_4":{"SSID":"your_SSID4","PASSWORD":"your_PASSWORD_for SSID4"}}}

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

extern VarioSettings GnuSettings;

#ifdef SDFAT_LIB
void listDirectory(SdFile dir, int numTabs);
#else //SDFAT_LIB
void listDirectory(File dir, int numTabs);
#endif //SDFAT_LIB

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

#ifdef SDFAT_LIB
SdFile uploadFile;
#else //SDFAT_LIB
File uploadFile;
#endif //SDFAT_LIB

#include "CSS.h"

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

#ifdef WIFI_DEBUG
  SerialPort.print("ssid_1 : ");
  SerialPort.println(ssid_1);
  SerialPort.print("password_1 : ");
  SerialPort.println(password_1);
#endif //WIFI_DEBUG

  GnuSettings.VARIOMETER_SSID_2.toCharArray(ssid_2, sizeof(ssid_2));
  GnuSettings.VARIOMETER_PASSWORD_2.toCharArray(password_2, sizeof(password_2));

#ifdef WIFI_DEBUG
  SerialPort.print("ssid_2 : ");
  SerialPort.println(ssid_2);
  SerialPort.print("password_2 : ");
  SerialPort.println(password_2);
#endif //WIFI_DEBUG

  GnuSettings.VARIOMETER_SSID_3.toCharArray(ssid_3, sizeof(ssid_3));
  GnuSettings.VARIOMETER_PASSWORD_3.toCharArray(password_3, sizeof(password_3));

#ifdef WIFI_DEBUG
  SerialPort.print("ssid_3 : ");
  SerialPort.println(ssid_3);
  SerialPort.print("password_3 : ");
  SerialPort.println(password_3);
#endif //WIFI_DEBUG

  GnuSettings.VARIOMETER_SSID_4.toCharArray(ssid_4, sizeof(ssid_4));
  GnuSettings.VARIOMETER_PASSWORD_4.toCharArray(password_4, sizeof(password_4));

#ifdef WIFI_DEBUG
  SerialPort.print("ssid_4 : ");
  SerialPort.println(ssid_4);
  SerialPort.print("password_4 : ");
  SerialPort.println(password_4);
#endif //WIFI_DEBUG

#ifdef SDFAT_LIB
  SdFile dataFile;
  if (dataFile.open("www/index.htm", O_RDONLY))
#else //SDFAT_LIB
  File dataFile = SDHAL_SD.open("www/index.htm");
  if (dataFile)
#endif //SDFAT_LIB
  {
//    if (SDHAL.exists("www/index.html")) {
#ifdef WIFI_DEBUG
    SerialPort.println("SD Card initialized.");
#endif //WIFI_DEBUG

    hasSD = true;
    dataFile.close();
  }

#ifdef SDFAT_LIB
  dataFile.open("/", O_RDONLY);

  dataFile.rewind();
#else //SDFAT_FILE
  dataFile = SDHAL_SD.open("/");

  dataFile.rewindDirectory();
#endif //SDFAT_FILE

#ifdef WIFI_DEBUG

  SerialPort.println("");
  SerialPort.println("ListeDirectory");
  listDirectory(dataFile, 0);

/*  SerialPort.println("");
  SerialPort.println("printDirectoryRecurse");
	printDirectoryRecurse("/",0);
  SerialPort.println("");*/
#endif //WIFI_DEBUG \
       //  listDirectoryJson(dataFile, 0);

  //	printDirectoryToJson("/");

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
  dataFile.close();

#ifdef WIFI_DEBUG
    SerialPort.print("GnuVario-E Firmware Version : ");
		SerialPort.println(GnuSettings.getVersion());

    SerialPort.print("GnuVario-E Type d'écran : ");
		SerialPort.println(GnuSettings.getScreenModel());

    SerialPort.print("Serveur WEB : ");
		SerialPort.println(WEBSERVERTYPE);

/*		
#ifdef ESP8266
    SerialPort.print("Serveur WEB : ESP8266WebServer");
#else
#ifdef ESP32WEBSERVEUR 
    SerialPort.println("Serveur WEB : ESP32WebServer");
#elif defined(ESPASYNCWEBSERVER)
    SerialPort.println("Serveur WEB : AsyncWebServer");
#elif defined(ETHERNETWEBSERVER)
    SerialPort.println("Serveur WEB : EthernetServer");
#else //ESP32WEBSERVEUR
    SerialPort.println("Serveur WEB : VarioWebServer");
#endif //ESP32WEBSERVEUR
#endif		*/
		

#ifdef SDFAT_LIB
    SerialPort.print("Lib SDCard : SDFAT");
#elif defined(MYSD_LIB) 
    SerialPort.print("Lib SDCard : MYSD");
#else
    SerialPort.print("Lib SDCard : SD");
#endif	
		
#endif //WIFI_DEBUG

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
  //  if (SD.begin(SS)) {
  SerialPort.println("SD Card initialized.");
  hasSD = true;
  //  }

  //////////////////////////////////////
  //   Liste des routes spécifiques   //
  //////////////////////////////////////

  // téléchargements de "mes vols" = contenu du repertoire vols
  server.on("/flights", HTTP_GET, handleListFlights);

  //récupération de la liste du contenu de la carte SD
  server.on("/list", HTTP_GET, handlePrintDirectory); //printDirectory);

  //récupération du contenu du fichier param
  server.on("/params", HTTP_GET, handleParams);

  // sauvegarde des parametres
  server.on("/params", HTTP_POST, handleSaveParams);

  //telechargement d'un fichier dont le nom complet avec chemin se trouve en param
  server.on("/file", HTTP_GET, handleFileDownload);

  //suppression d'un fichier dont le nom complet avec chemin se trouve en param
  server.on("/file", HTTP_DELETE, handleFileDelete);

  // upload d'un fichier, le chemin de destination se trouve dans le nom du fichier posté
  server.on("/upload", HTTP_POST, []() {
    returnOK();
  },
            handleFileUpload);

  // mise à jour OTA
  server.on("/fupdate", HTTP_POST, []() { server.send(200); }, handleFileUpdate);

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

#ifdef SDFAT_LIB
/***********************************/
void listDirectory(SdFile dir, int numTabs)
{
  /***********************************/
  //	DynamicJsonBuffer jsonBuffer;
  //	DynamicJsonDocument doc(1024);
  //  JsonArray& array = jsonBuffer.createArray();
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
		
		entry.getName(fBuffer,30);

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

    /*    JsonObject& object = jsonBuffer.createObject();

    object["type"] = (entry.isDirectory()) ? "dir" : "file";
    object["name"] = String(entry.name());
    array.add(object);	*/

    entry.close();
  }
}
#else //SDFAT_LIB
/***********************************/
void listDirectory(File dir, int numTabs)
{
  /***********************************/
  //	DynamicJsonBuffer jsonBuffer;
  //	DynamicJsonDocument doc(1024);
  //  JsonArray& array = jsonBuffer.createArray();

  dir.rewindDirectory();

  while (true)
  {

    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++)
    {
      SerialPort.print('\t');
    }
    SerialPort.print(entry.name());
    if (entry.isDirectory())
    {
      SerialPort.println("/");
      listDirectory(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      SerialPort.print("\t\t");
      SerialPort.println(entry.size(), DEC);
    }

    /*    JsonObject& object = jsonBuffer.createObject();

    object["type"] = (entry.isDirectory()) ? "dir" : "file";
    object["name"] = String(entry.name());
    array.add(object);	*/

    entry.close();
  }
}
#endif //SDFAT_LIB

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

#ifdef HAVE_SDCARD

#ifdef SDFAT_LIB
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
#else //SDFAT_LIB
  File dataFile = SDHAL_SD.open(path.c_str());
  if (dataFile.isDirectory())
  {
		dataFile.close();
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SDHAL_SD.open(path.c_str());
  }

  if (!dataFile)

#endif // SDFAT_LIB

  {
    return false;
  }

  if (server.hasArg("download"))
  {
    dataType = "application/octet-stream";
  }

#ifdef SDFAT_LIB
  if (server.streamFile(dataFile, dataType) != dataFile.fileSize())
#else //SDFAT_LIB
  if (server.streamFile(dataFile, dataType) != dataFile.size())
#endif //SDFAT_LIB	
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
	
#ifdef SDFAT_LIB
  SdFile dir;
	dir.open((char *)path.c_str(), O_READ); //O_RDONLY);

  path = String();
  if (!dir.isDir())
  {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewind();
#else //SDFAT_LIB
  File dir = SDHAL_SD.open((char *)path.c_str());

  path = String();
  if (!dir.isDirectory())
  {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
#endif //SDFAT_LIB

	
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "");
  WiFiClient client = server.client();

  server.sendContent("[");
  for (int cnt = 0; true; ++cnt)
  {
#ifdef SDFAT_LIB
    SdFile entry;
    if (!entry.openNext(&dir, O_READ))
#else //SDFAT_FILE
    File entry = dir.openNextFile();
    if (!entry)
#endif //SDFAT_LIB
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
#ifdef SDFAT_LIB
    if (!entry.isDir())
    {
      int bytes = entry.fileSize();
#else //SDFAT_LIB
    if (!entry.isDirectory())
    {
      int bytes = entry.size();
#endif //SDFAT_LIB

      if (bytes < 1024)
        fsize = String(bytes) + " B";
      else if (bytes < (1024 * 1024))
        fsize = String(bytes / 1024.0, 3) + " KB";
      else if (bytes < (1024 * 1024 * 1024))
        fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
      else
        fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
    }
    else
    {
      fsize = "na";
    }

    output += "{\"type\":\"";
#ifdef SDFAT_LIB
    output += (entry.isDir()) ? "dir" : "file";
#else //SDFAT_LIB
    output += (entry.isDirectory()) ? "dir" : "file";
#endif //SDFAT_LIB
    output += "\",\"name\":\"";
#ifdef SDFAT_LIB
		char fBuffer[32];
		entry.getName(fBuffer,30);
    output += fBuffer;
#else //SDFAT_LIB
    output += entry.name();
#endif //SDFAT_LIB
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

#ifdef WIFI_DEBUG
  SerialPort.print("file name :");
  SerialPort.println(path.c_str());
#endif

#ifdef SDFAT_LIB
  SdFile dataFile;
  if (!dataFile.open((char *)path.c_str()), O_RDONLY)
#else //SDFAT_LIB
  File dataFile = SDHAL_SD.open((char *)path.c_str());

  if (!dataFile)
#endif  //SDFAT_LIB
  {
#ifdef WIFI_DEBUG
    SerialPort.println("params.jso introuvable");
#endif
    dataFile.close();
    return returnFail("NO FILE named params.jso");
  }

  server.sendHeader("Access-Control-Allow-Origin", "*");
#ifdef SDFAT_LIB
  if (server.streamFile(dataFile, dataType) != dataFile.fileSize())
#else //SDFAT_LIB
  if (server.streamFile(dataFile, dataType) != dataFile.size())
#endif //SDFAT_LIB
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

#ifdef SDFAT_LIB
  SdFile dataFile;
  if (!dataFile.open((char *)path.c_str(), O_RDONLY))
#else //SDFAT_LIB
  File dataFile = SDHAL_SD.open((char *)path.c_str());

  if (!dataFile)
#endif //SDFAT_LIB
  {
#ifdef WIFI_DEBUG
    SerialPort.println("wifi.cfg introuvable");
#endif
    dataFile.close();
    return returnFail("NO FILE named wifi.cfg");
  }

#ifdef SDFAT_LIB
  if (server.streamFile(dataFile, dataType) != dataFile.fileSize())
#else //SDFAT_LIB
   if (server.streamFile(dataFile, dataType) != dataFile.size())
#endif //SDFAT_LIB
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

#ifdef SDFAT_LIB
  SdFile dir;
	dir.open((char *)path.c_str(), O_RDONLY);
  dir.rewind();
  
  if (!dir.isDir())
#else //SDFAT_LIB
  File dir = SDHAL_SD.open((char *)path.c_str());
  dir.rewindDirectory();
  
  if (!dir.isDirectory())
#endif //SDFAT_LIB
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
#ifdef WIFI_DEBUG
//	SerialPort.println("printDirectoryRecurse");
#endif
#ifdef SDFAT_LIB
 	char fBuffer[32];	
  SdFile dir;
	
	dir.open((char *)path.c_str(), O_RDONLY);
  dir.rewind();
#else //SDFAT_LIB
  File dir = SDHAL_SD.open((char *)path.c_str());
  dir.rewindDirectory();
#endif //SDFAT_LIB
  int tmpcnt = 0;

  for (int cnt = 0; true; ++cnt)
  {

#ifdef SDFAT_LIB
    SdFile entry;
		boolean tmpRetour = entry.openNext(&dir, O_RDONLY);
		entry.getName(fBuffer,30);
    String tmpName = fBuffer;
    if (!tmpRetour)
#else //SDFAT_LIB
    File entry = dir.openNextFile();
    String tmpName = entry.name();
    if (!entry)
#endif //SDFAT_LIB
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
#ifdef SDFAT_LIB
    if (!entry.isDir())
    {
      int bytes = entry.fileSize();
#else //SDFAT_LIB
    if (!entry.isDirectory())
    {
      int bytes = entry.size();
#endif //SDFAT_LIB

      if (bytes < 1024)
        fsize = String(bytes) + " B";
      else if (bytes < (1024 * 1024))
        fsize = String(bytes / 1024.0, 3) + " KB";
      else if (bytes < (1024 * 1024 * 1024))
        fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
      else
        fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
    }
    else
    {
      fsize = "na";
    }

    output += "{\"type\":\"";
#ifdef SDFAT_LIB
    output += (entry.isDir()) ? "dir" : "file";
#else //SDFAT_LIB
    output += (entry.isDirectory()) ? "dir" : "file";
#endif //SDFAT_LIB
    output += "\",\"name\":\"";
#ifdef SDFAT_LIB
		entry.getName(fBuffer,30);
    output += fBuffer;
#else //SDFAT_LIB
    output += entry.name();
#endif //SDFAT_LIB
    output += "\",\"size\":\"";
    output += fsize;
    output += "\"";
#ifdef SDFAT_LIB
    if (entry.isDir())
#else //SDFAT_LIB
    if (entry.isDirectory())
#endif //SDFAT_LIB
    {
      output += ",\"contents\" :[";
#ifdef WIFI_DEBUG
      SerialPort.println(output);
#endif
      server.sendContent(output);
#ifdef SDFAT_LIB
			entry.getName(fBuffer,30);
      printDirectoryRecurse(path + "/" + fBuffer);
#else //SDFAT_LIB
      printDirectoryRecurse(path + "/" + entry.name());
#endif //SDFAT_LIB
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
    returnFail("BAD URL");
  }
  if (server.args() == 0)
  {
    return returnFail("BAD ARGS");
  }

  String path = server.arg(0);
#ifdef SDFAT_LIB
  SdFile dataFile;
  if (!dataFile.open((char *)path.c_str(), O_RDONLY))
#else //SDFAT_LIB
  File dataFile = SDHAL_SD.open(path.c_str());

  if (!dataFile)
#endif //SDFAT_LIB
  {
    return returnFail("NO FILE");
  }

  String dataType = "application/octet-stream";

#ifdef SDFAT_LIB
  if (server.streamFile(dataFile, dataType) != dataFile.fileSize())
#else //SDFAT_LIB
  if (server.streamFile(dataFile, dataType) != dataFile.size())
#endif //SDFAT_LIB
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
    return;
  }

  boolean tmpReturn = false;
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    if (SDHAL_SD.exists((char *)upload.filename.c_str()))
    {
      SDHAL_SD.remove((char *)upload.filename.c_str());
    }
#ifdef SDFAT_LIB
    tmpReturn = uploadFile.open(upload.filename.c_str(), O_WRONLY | O_CREAT);
#else //SDFAT_LIB
    uploadFile = SDHAL.open(upload.filename.c_str(), FILE_WRITE);
#endif //SDFAT_LIB
#ifdef WIFI_DEBUG
    SerialPort.print("Upload: START, filename: ");
    SerialPort.println(upload.filename);
#endif
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
#ifdef SDFAT_LIB
    if (tmpReturn)
#else //SDFAT_LIB
    if (uploadFile)
#endif //SDFAT_LIB
    {
      uploadFile.write(upload.buf, upload.currentSize);
    }
#ifdef WIFI_DEBUG
    SerialPort.print("Upload: WRITE, Bytes: ");
    SerialPort.println(upload.currentSize);
#endif
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
#ifdef SDFAT_LIB
    if (tmpReturn)
#else //SDFAT_LIB
    if (uploadFile)
#endif //SDFAT_LIB
    {
      uploadFile.close();
    }
#ifdef WIFI_DEBUG
    SerialPort.print("Upload: END, Size: ");
    SerialPort.println(upload.totalSize);
#endif
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  returnOK();
}


/*
/***********************************
void handleFileUpload() {
/***********************************

#ifdef WIFI_DEBUG
  SerialPort.println("handleFileUpload");
#endif

  if (server.uri()!= "/upload") {
		return;
	}

// Si le fichier existe on le supprime
	HTTPUpload &upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		if (SDHAL_SD.exists((char*)upload.filename.c_str())) {
			SDHAL_SD.remove((char*)upload.filename.c_str());
		}

#ifdef SDFAT_LIB

	//Création du fichier
		if (!uploadFile.open(upload.filename.c_str(), O_WRONLY | O_CREAT)) {
			returnFail("Cannot creat file");
			return;
		}

#else //SDFAT_LIB

		uploadFile = SDHAL_SD.open(upload.filename.c_str(), FILE_WRITE);

#endif //SDFAT_LIB

#ifdef WIFI_DEBUG
		SerialPort.print("Upload: START, filename: ");
		SerialPort.println(upload.filename);
#endif

	} 
  else if (upload.status == UPLOAD_FILE_WRITE)
	{
#ifdef SDFAT_LIB
		if (uploadFile.isOpen()) 
#else //SDFAT_LIB
		  if (uploadFile)
#endif //SDFAT_LIB
			{
				TRACE();
//				DUMP(upload.buf);
				DUMP(upload.currentSize);
				DUMP(upload.totalSize);
				uploadFile.write(upload.buf, upload.currentSize);

#ifdef WIFI_DEBUG

				SerialPort.print("Upload: WRITE, Bytes: ");
				SerialPort.println(upload.currentSize);
#endif
			}
	} 
	else if (upload.status == UPLOAD_FILE_END) 
	{
#ifdef SDFAT_LIB
		if (uploadFile.isOpen())
#else //SDFAT_LIB
		if (uploadFile)
#endif //SDFAT_LIB
			{
				uploadFile.close();
#ifdef WIFI_DEBUG
				SerialPort.print("Upload: END, Size: ");
				SerialPort.println(upload.totalSize);
#endif
			}
	}
	else if (upload.status == UPLOAD_FILE_ABORTED)
	{
		
		uploadFile.close();

#ifdef WIFI_DEBUG
		SerialPort.println("Upload: UPLOAD_FILE_ABORTED");
#endif
	}
	
	server.sendHeader("Access-Control-Allow-Origin","*");
	returnOK();
}
*/

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
#ifdef SDFAT_LIB
  SdFile fileSD;
	fileSD.open((char *)path.c_str(), O_RDONLY );
  if (!fileSD.isDir())
#else //SDFAT_LIB
  File fileSD = SDHAL_SD.open((char *)path.c_str());
  if (!fileSD.isDirectory())
#endif //SDFAT_LIB
  {
    fileSD.close();
    SDHAL_SD.remove((char *)path.c_str());
		TRACE();
    return;
  }

#ifdef SDFAT_LIB
  fileSD.rewind();
#else //SDFAT_LIB
  fileSD.rewindDirectory();
#endif //SDFAT_LIB
  while (true)
  {
		
#ifdef SDFAT_LIB
    SdFile entry;
    if (!entry.openNext(&fileSD, O_RDONLY))
#else //SDFAT_LIB
   File entry = fileSD.openNextFile();
    if (!entry)
#endif //SDFAT_LIB
    {
      break;
    }
#ifdef SDFAT_LIB		
 	  char fBuffer[32];	
		entry.getName(fBuffer,30);
    String entryPath = path + "/" + fBuffer;
    if (entry.isDir())
#else //SDFAT_LIB
    String entryPath = path + "/" + entry.name();
    if (entry.isDirectory())
#endif //SDFAT_LIB
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

  String message = "SDCARD Not Detected\n\n";
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
#ifdef SDFAT_LIB
SdFile UpdateFile;
#else //SDFAT_LIB
File UpdateFile;
#endif //SDFAT_LIB
#endif

// Mise à jour OTA

// upload d'un fichier dont le chemin + nom se trouve en param dans le nommage du fichier
/***********************************
void handleFileUpdate() {
/***********************************

#ifdef WIFI_DEBUG
  SerialPort.println("handleFileUpdate");
#endif

/*  if (server.uri()!= "/upload") {
		return;
	}*

// Si le fichier existe on le supprime
	HTTPUpload &upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
    SerialPort.setDebugOutput(true);
//        WiFiUDP::stop();   //All();
#ifdef WIFI_DEBUG
		SerialPort.printf("Update: %s\n", upload.filename.c_str());
#endif
		uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
		if(!Update.begin(maxSketchSpace)){//start with max available size
				Update.printError(SerialPort);
		}
	} 
  else if (upload.status == UPLOAD_FILE_WRITE)
	{
		if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
			Update.printError(SerialPort);
		}
		TRACE();
//				DUMP(upload.buf);
	} 
	else if (upload.status == UPLOAD_FILE_END) 
	{
		if(Update.end(true)){ //true to set the size to the current progress
#ifdef WIFI_DEBUG
			SerialPort.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
#endif
						
			SerialPort.println("RESTART ESP32");
			SerialPort.flush();

			returnOK();

			ESP_LOGI("GnuVario-E", "RESTART ESP32");
#ifdef ENABLE_DISPLAY_WEBSERVER
			screen.ScreenViewReboot();
#endif
			ESP.restart();			
		} else {
			Update.printError(SerialPort);
			SerialPort.setDebugOutput(false);
			return returnFail("could not update");
		}
		SerialPort.setDebugOutput(false);

#ifdef WIFI_DEBUG
				SerialPort.print("Upload: END, Size: ");
				SerialPort.println(upload.totalSize);
#endif
	}
	
	server.sendHeader("Access-Control-Allow-Origin","*");
	returnOK();
}*/



/***********************************/
void handleFileUpdate()
{ // update ESP32
/***********************************/
  boolean tmpReturn = false;
#ifdef WIFI_DEBUG
  SerialPort.println("handleFileUpdate");

  SerialPort.println("File update begin");
#endif //WIFI_DEBUG

	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		Serial.printf("Update: %s\n", upload.filename.c_str());
		
		if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
			Update.printError(SerialPort);
		}
	} else if (upload.status == UPLOAD_FILE_WRITE) {
		/* flashing firmware to ESP*/
		if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
			Update.printError(SerialPort);
		}
	} else if (upload.status == UPLOAD_FILE_END) {
		if (Update.end(true)) { //true to set the size to the current progress
			Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
			
			SerialPort.println("RESTART ESP32");
			SerialPort.flush();

			returnOK();

			ESP_LOGI("GnuVario-E", "RESTART ESP32");
#ifdef ENABLE_DISPLAY_WEBSERVER
			screen.ScreenViewReboot();
#endif
			ESP.restart();
		} else {
			Update.printError(SerialPort);
			return returnFail("could not update");
		}
	}
}


/*
  boolean tmpReturn = false;
#ifdef WIFI_DEBUG
  SerialPort.println("handleFileUpdate");

  SerialPort.println("File update stage-3");
#endif //WIFI_DEBUG

#ifdef HAVE_SDCARD

  HTTPUpload &updatefile = server.upload(); // See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            // For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if (updatefile.status == UPLOAD_FILE_START)
  {
#ifdef WIFI_DEBUG
    SerialPort.println("File update stage-4");
#endif //WIFI_DEBUG
    String filename = updatefile.filename;
    if (!filename.startsWith("/"))
      filename = "/" + filename;
#ifdef WIFI_DEBUG
    SerialPort.print("Update File Name: ");
    SerialPort.println(filename);
#endif //WIFI_DEBUG

    char copy[250];
    String scopy = filename;
    scopy.toCharArray(copy, 250);

    SDHAL_SD.remove(copy);                        // Remove a previous version, otherwise data is appended the file again
#ifdef SDFAT_LIB
    tmpReturn = UpdateFile.open(copy, O_WRONLY | O_CREAT); // Open the file for writing in SPIFFS (create it, if doesn't exist)
#else //SDFAT_LIB
    UpdateFile = SDHAL.open(copy, FILE_WRITE); // Open the file for writing in SPIFFS (create it, if doesn't exist)
#endif //SDFAT_LIB
    filename = String();

    if (!Update.begin(UPDATE_SIZE_UNKNOWN))
    { //start with max available size
      Update.printError(SerialPort);
    }
  }
  else if (updatefile.status == UPLOAD_FILE_WRITE)
  {
    SerialPort.println("File update stage-5");
#ifdef SDFAT_LIB
    if (tmpReturn)
#else //SDFAT_LIB
    if (UpdateFile)
#endif //SDFAT_LIB
      UpdateFile.write(updatefile.buf, updatefile.currentSize); // Write the received bytes to the file
    // flashing firmware to ESP
    if (Update.write(updatefile.buf, updatefile.currentSize) != updatefile.currentSize)
    {
      Update.printError(SerialPort);
    }
  }
  else if (updatefile.status == UPLOAD_FILE_END)
  {
#ifdef SDFAT_LIB
    if (tmpReturn) // If the file was successfully created
#else //SDFAT_LIB
    if (UpdateFile) // If the file was successfully created
#endif //SDFAT_LIB
    {
      UpdateFile.close(); // Close the file again
#ifdef WIFI_DEBUG
      SerialPort.print("Update Size: ");
      SerialPort.println(updatefile.totalSize);
#endif //WIFI_DEBUG
    }
    else
    {
      return returnFail("could not update");
    }

    if (Update.end(true))
    { //true to set the size to the current progress
#ifdef WIFI_DEBUG
      SerialPort.printf("Update Success: %u\nRebooting...\n", updatefile.totalSize);
#endif //WIFI_DEBUG

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
#else
  returnFail("no SD card");
#endif
}*/

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
#ifdef SDFAT_LIB
  SdFile dataFile;
	dataFile.open(path.c_str(), O_RDONLY);
  SdFile dataFile2;
	dataFile2.open(pathBak.c_str(), O_RDWR | O_CREAT);
#else //SDFAT_LIB
  File dataFile = SDHAL_SD.open(path.c_str(), FILE_READ);
  File dataFile2 = SDHAL_SD.open(pathBak.c_str(), FILE_WRITE);
#endif //SDFAT_LIB
  while ((n = dataFile.read(buf, sizeof(buf))) > 0)
  {
    dataFile2.write(buf, n);
  }
  dataFile.close();
  dataFile2.close();

  SDHAL_SD.remove((char *)path.c_str());

#ifdef SDFAT_LIB
  if (!dataFile.open(path.c_str(), O_RDWR | O_CREAT))
#else //SDFAT_LIB
  dataFile = SDHAL_SD.open(path.c_str(), FILE_WRITE);

  if (!dataFile)
#endif //SDFAT_LIB
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
#ifdef SDFAT_LIB
  SdFile dataFile;
	dataFile.open(path.c_str(), O_RDONLY);
  SdFile dataFile2;
	dataFile2.open(pathBak.c_str(), O_RDWR | O_CREAT);
#else //SDFAT_LIB
  File dataFile = SDHAL_SD.open(path.c_str(), FILE_READ);
  File dataFile2 = SDHAL_SD.open(pathBak.c_str(), FILE_WRITE);
#endif //SDFAT_LIB
  while ((n = dataFile.read(buf, sizeof(buf))) > 0)
  {
    dataFile2.write(buf, n);
  }
  dataFile.close();
  dataFile2.close();

  SDHAL_SD.remove((char *)path.c_str());

#ifdef SDFAT_LIB
  if (!dataFile.open(path.c_str(), O_RDWR | O_CREAT))
#else //SDFAT_LIB
  dataFile = SDHAL_SD.open(path.c_str(), FILE_WRITE);

  if (!dataFile)
#endif //SDFAT_LIB
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

/***********************************
void handleCreate() {
/***********************************
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
