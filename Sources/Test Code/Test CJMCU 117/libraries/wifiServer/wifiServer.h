/*********************************************************************************/
/*                                                                               */
/*                           Libraries wifiServer                                 */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    31/07/19                                                            */
/*    1.1    16/08/19   Ajout Affichage sur écran                                */
/*                                                                               */
/*********************************************************************************/
#ifndef wifiServer_h
#define wifiServer_h

#include <Arduino.h>

#ifdef ESP8266
  #include <ESP8266WiFi.h>       // Built-in
  #include <ESP8266WiFiMulti.h>  // Built-in
  #include <ESP8266WebServer.h>  // Built-in
  #include <ESP8266mDNS.h>
#else
  #include <WiFi.h>              // Built-in
  #include <WiFiMulti.h>         // Built-in
  #include <ESP32WebServer.h>    // https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
  #include <ESPmDNS.h>
//  #include "FS.h"
#endif

//#define ServerVersion "1.0"
extern char   ServerVersion[20];
extern String webpage;
extern bool   SD_present;

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
  extern ESP32WebServer server;
#endif

class WifiServer {

  public:

	  boolean begin(void);
		void disableWifi(void);
		void disableBT(void);
		void disableAll(void);
		void connect(void);
		void start(void);
		void handleClient(void);
};

void HomePage(void);
void File_Download(void);
void SD_file_download(String filename);
void File_Upload();
void handleFileUpload();
void SD_dir();
void printDirectory(const char * dirname, uint8_t levels);
void File_Stream();
void SD_file_stream(String filename);
void File_Delete();
void SD_file_delete(String filename); 
void SendHTML_Header();
void SendHTML_Content();
void SendHTML_Stop();
void SelectInput(String heading1, String command, String arg_calling_name);
void ReportSDNotPresent();
void ReportFileNotPresent(String target);
void ReportCouldNotCreateFile(String target);
String file_size(int bytes);

extern WifiServer wifiServer; 

#endif
	