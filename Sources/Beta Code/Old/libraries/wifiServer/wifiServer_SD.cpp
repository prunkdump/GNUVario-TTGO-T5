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
/*                                                                               */
/*********************************************************************************/

//Arg nº0 –> plain: {"GnuvarioE":{"version":"0.6 beta 2"},"Systeme":{"BT_ENABLE":"0","NO_RECORD":"0","RATIO_CLIMB_RATE":"2","VARIOMETER_SENT_LXNAV_SENTENCE":"1","ALARM_SDCARD":"1","ALARM_GPSFIX":"1","ALARM_FLYBEGIN":"1"},"General":{"VARIOMETER_PILOT_NAME":"Magali","VARIOMETER_GLIDER_NAME":"MAC-PARA Muse 3","VARIOMETER_TIME_ZONE":"2","VARIOMETER_BEEP_VOLUME":"8"},"Vario":{"VARIOMETER_SINKING_THRESHOLD":"-2.0","VARIOMETER_CLIMBING_THRESHOLD":"0.2","VARIOMETER_NEAR_CLIMBING_SENSITIVITY":"0.5","VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM":"0","VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP":"0","VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE":"0","SETTINGS_CLIMB_PERIOD_COUNT":"10","SETTINGS_GLIDE_RATIO_PERIOD_COUNT":"20","RATIO_MAX_VALUE":"30.0","RATIO_MIN_SPEED":"10.0"},"Flight start":{"FLIGHT_START_MIN_TIMESTAMP":"15000","FLIGHT_START_VARIO_LOW_THRESHOLD":"-0.5","FLIGHT_START_VARIO_HIGH_THRESHOLD":"0.5","FLIGHT_START_MIN_SPEED":"8.0","VARIOMETER_RECORD_WHEN_FLIGHT_START":"0"},"calibration":{"VERTACCEL_GYRO_CAL_BIAS_00":"0x00","VERTACCEL_GYRO_CAL_BIAS_01":"0x00","VERTACCEL_GYRO_CAL_BIAS_02":"0x00","VERTACCEL_GYRO_CAL_BIAS_03":"0x00","VERTACCEL_GYRO_CAL_BIAS_04":"0x00","VERTACCEL_GYRO_CAL_BIAS_05":"0x00","VERTACCEL_GYRO_CAL_BIAS_06":"0x00","VERTACCEL_GYRO_CAL_BIAS_07":"0x00","VERTACCEL_GYRO_CAL_BIAS_08":"0x00","VERTACCEL_GYRO_CAL_BIAS_09":"0x00","VERTACCEL_GYRO_CAL_BIAS_10":"0x00","VERTACCEL_GYRO_CAL_BIAS_11":"0x00","VERTACCEL_ACCEL_CAL_BIAS_00":"0","VERTACCEL_ACCEL_CAL_BIAS_01":"0","VERTACCEL_ACCEL_CAL_BIAS_02":"0","VERTACCEL_ACCEL_CAL_SCALE":"0","VERTACCEL_MAG_CAL_BIAS_00":"0","VERTACCEL_MAG_CAL_BIAS_01":"0","VERTACCEL_MAG_CAL_BIAS_02":"0","VERTACCEL_MAG_CAL_PROJ_SCALE":"-16689","VERTACCEL_ACCEL_CAL_BIAS_MULTIPLIER":"7","VERTACCEL_MAG_CAL_BIAS_MULTIPLIER":"5"},"Wifi":{"wifi_1":{"SSID":"your_SSID1","PASSWORD":"your_PASSWORD_for SSID1"},"wifi_2":{"SSID":"your_SSID2","PASSWORD":"your_PASSWORD_for SSID2"},"wifi_3":{"SSID":"your_SSID3","PASSWORD":"your_PASSWORD_for SSID3"},"wifi_4":{"SSID":"your_SSID4","PASSWORD":"your_PASSWORD_for SSID4"}}}

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#if defined(WEBSERVER_SDCARD)

#include <wifiServer_SD.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

#include <SPI.h>

#include <VarioSettings.h>

#include <Update.h>

#include <ArduinoJson.h>

extern VarioSettings GnuSettings;

void listDirectory(File dir, int numTabs);

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
// CLASSE WIFISERVER
//************************************************************

//INIT Classe WifiServer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
boolean WifiServer::begin(void)
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

  File dataFile = SDHAL.open("www/index.htm");
  if (dataFile)
  {
//    if (SDHAL.exists("www/index.html")) {
#ifdef WIFI_DEBUG
    SerialPort.println("SD Card initialized.");
#endif //WIFI_DEBUG

    hasSD = true;
    dataFile.close();
  }

  dataFile = SDHAL.open("/");

  dataFile.rewindDirectory();

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

  return true;
};

/***********************************/
void WifiServer::disableWifi(void)
{
  /***********************************/
  WiFi.mode(WIFI_OFF);
}

/***********************************/
void WifiServer::disableBT(void)
{
  /***********************************/
}

void WifiServer::disableAll(void)
{
  disableWifi();
  disableBT();
}

//Connection au wifi
/***********************************/
void WifiServer::connect(void)
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
void WifiServer::start(void)
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
void WifiServer::handleClient(void)
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

  File dataFile = SDHAL.open(path.c_str());
  if (dataFile.isDirectory())
  {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SDHAL.open(path.c_str());
  }

  if (!dataFile)
  {
    return false;
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

  String path;
  path = "/vols";
  File dir = SDHAL.open((char *)path.c_str());
  path = String();
  if (!dir.isDirectory())
  {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/json", "");
  WiFiClient client = server.client();

  server.sendContent("[");
  for (int cnt = 0; true; ++cnt)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      break;
    }

    String output;
    if (cnt > 0)
    {
      output = ',';
    }

    String fsize = "";
    if (!entry.isDirectory())
    {
      int bytes = entry.size();

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
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += entry.name();
    output += "\",\"size\":\"";
    output += fsize;
    output += "\"";
    output += "}";
    server.sendContent(output);
    entry.close();
  }
  server.sendContent("]");
  dir.close();
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

  File dataFile = SDHAL.open((char *)path.c_str());

  if (!dataFile)
  {
#ifdef WIFI_DEBUG
    SerialPort.println("params.jso introuvable");
#endif
    dataFile.close();
    return returnFail("NO FILE named params.jso");
  }

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
  String path = "wifi.cfg";

#ifdef WIFI_DEBUG
  SerialPort.print("file name :");
  SerialPort.println(path.c_str());
#endif

  File dataFile = SDHAL.open((char *)path.c_str());

  if (!dataFile)
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

  File dir = SDHAL.open((char *)path.c_str());
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
  server.send(200, "text/json", "");
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
  File dir = SDHAL.open((char *)path.c_str());
  dir.rewindDirectory();
  int tmpcnt = 0;

  for (int cnt = 0; true; ++cnt)
  {

    File entry = dir.openNextFile();
    String tmpName = entry.name();
    if (!entry)
    {
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
    if (!entry.isDirectory())
    {
      int bytes = entry.size();

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
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += entry.name();
    output += "\",\"size\":\"";
    output += fsize;
    output += "\"";
    if (entry.isDirectory())
    {
      output += ",\"contents\" :[";
#ifdef WIFI_DEBUG
      SerialPort.println(output);
#endif
      server.sendContent(output);
      printDirectoryRecurse(path + "/" + entry.name());
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
  File dataFile = SDHAL.open(path.c_str());

  if (!dataFile)
  {
    return returnFail("NO FILE");
  }

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
#ifdef WIFI_DEBUG
  SerialPort.println("handleFileUpload");
#endif

  if (server.uri() != "/upload")
  {
    return;
  }

  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    if (SDHAL.exists((char *)upload.filename.c_str()))
    {
      SDHAL.remove((char *)upload.filename.c_str());
    }
    uploadFile = SDHAL.open(upload.filename.c_str(), FILE_WRITE);
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
    }
#ifdef WIFI_DEBUG
    SerialPort.print("Upload: WRITE, Bytes: ");
    SerialPort.println(upload.currentSize);
#endif
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (uploadFile)
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
    return returnFail("BAD URL");
  }
  if (server.args() == 0)
  {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  if (!SDHAL.exists((char *)path.c_str()))
  {
    return returnFail("FILE DOES NOT EXIST");
  }
  deleteRecursive(path);
  returnOK();
}

/***********************************/
void deleteRecursive(String path)
{
  /***********************************/
  File fileSD = SDHAL.open((char *)path.c_str());
  if (!fileSD.isDirectory())
  {
    fileSD.close();
    SDHAL.remove((char *)path.c_str());
    return;
  }

  fileSD.rewindDirectory();
  while (true)
  {
    File entry = fileSD.openNextFile();
    if (!entry)
    {
      break;
    }
    String entryPath = path + "/" + entry.name();
    if (entry.isDirectory())
    {
      entry.close();
      deleteRecursive(entryPath);
    }
    else
    {
      entry.close();
      SDHAL.remove((char *)entryPath.c_str());
    }
    yield();
  }

  SDHAL.rmdir((char *)path.c_str());
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
File UpdateFile;
#endif

// Mise à jour OTA
/***********************************/
void handleFileUpdate()
{ // update ESP32
/***********************************/
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

    SDHAL.remove(copy);                        // Remove a previous version, otherwise data is appended the file again
    UpdateFile = SDHAL.open(copy, FILE_WRITE); // Open the file for writing in SPIFFS (create it, if doesn't exist)
    filename = String();

    if (!Update.begin(UPDATE_SIZE_UNKNOWN))
    { //start with max available size
      Update.printError(SerialPort);
    }
  }
  else if (updatefile.status == UPLOAD_FILE_WRITE)
  {
    SerialPort.println("File update stage-5");
    if (UpdateFile)
      UpdateFile.write(updatefile.buf, updatefile.currentSize); // Write the received bytes to the file
    /* flashing firmware to ESP*/
    if (Update.write(updatefile.buf, updatefile.currentSize) != updatefile.currentSize)
    {
      Update.printError(SerialPort);
    }
  }
  else if (updatefile.status == UPLOAD_FILE_END)
  {
    if (UpdateFile) // If the file was successfully created
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

  if (SDHAL.exists((char *)pathBak.c_str()))
  {
    SDHAL.remove((char *)pathBak.c_str());
  }
  size_t n;
  uint8_t buf[64];
  File dataFile = SDHAL.open(path.c_str(), FILE_READ);
  File dataFile2 = SDHAL.open(pathBak.c_str(), FILE_WRITE);
  while ((n = dataFile.read(buf, sizeof(buf))) > 0)
  {
    dataFile2.write(buf, n);
  }
  dataFile.close();
  dataFile2.close();

  SDHAL.remove((char *)path.c_str());

  dataFile = SDHAL.open(path.c_str(), FILE_WRITE);

  if (!dataFile)
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

  if (SDHAL.exists((char *)pathBak.c_str()))
  {
    SDHAL.remove((char *)pathBak.c_str());
  }
  size_t n;
  uint8_t buf[64];
  File dataFile = SDHAL.open(path.c_str(), FILE_READ);
  File dataFile2 = SDHAL.open(pathBak.c_str(), FILE_WRITE);
  while ((n = dataFile.read(buf, sizeof(buf))) > 0)
  {
    dataFile2.write(buf, n);
  }
  dataFile.close();
  dataFile2.close();

  SDHAL.remove((char *)path.c_str());

  dataFile = SDHAL.open(path.c_str(), FILE_WRITE);

  if (!dataFile)
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

WifiServer wifiServer;

#endif //WEBSERVER_SDCARD
