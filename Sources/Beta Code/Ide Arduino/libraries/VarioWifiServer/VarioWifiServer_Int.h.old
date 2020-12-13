/*********************************************************************************/
/*                                                                               */
/*                           Libraries VarioWifiServer                           */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    31/07/19                                                            */
/*    1.1    16/08/19   Ajout Affichage sur écran                                */
/*    1.1.1  17/08/19   Ajout message de debuggage                               */
/*    1.2    21.08/19   Ajout Update ESP32                                       */
/*    1.2.1  28/08/19		Modification de l'organisation des menus							 	 */
/*    1.2.2  11/12/19   Changement de nom                                        */
/*                                                                               */
/*********************************************************************************/
#ifndef VarioWifiServer_Int_h
#define VarioWifiServer_Int_h

#include <Arduino.h>
#include <HardwareConfig.h>

#if not defined(WEBSERVER_SDCARD)


#include <WiFi.h>              // Built-in
#include <WiFiMulti.h>         // Built-in
//  #include <ESP32WebServer.h>    // https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
//	#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
//  #include "FS.h"

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



extern WiFiMulti wifiMulti;
extern WebServer server;


class VarioWifiServer {

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
void File_Update();
void handleFileUpdate();
void File_Delete();
void SD_file_delete(String filename); 
void SendHTML_Header();
void SendHTML_Content();
void SendHTML_Stop();
void SelectInput(String heading1, String command, String arg_calling_name);
void ReportSDNotPresent();
void ReportFileNotPresent(String target);
void ReportCouldNotCreateFile(String target);
void ReportCouldNotUpdateFile(String target);
String file_size(int bytes);

extern VarioWifiServer varioWifiServer; 

#endif //WEBSERVER_SDCARD

#endif
	