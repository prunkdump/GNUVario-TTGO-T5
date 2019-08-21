/*********************************************************************************/
/*                                                                               */
/*                           Libraries wifiServer                                 */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    31/07/19                                                            */
/*    1.1    16/08/19   Ajout Affichage sur écran                                */
/*                                                                               */
/*********************************************************************************/

#include <Arduino.h>
#include <wifiServer.h>

#include <sdcardHAL.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>
#include <SPI.h>

#include <VarioSettings.h>

extern VarioSettings GnuSettings;

#define TAG "server"

bool   SD_present = false;
char   ServerVersion[20]; // = "3.4";

#ifdef _HARDWARECONFIG_H_
IPAddress local_IP(LOCAL_IP); // Set your server's fixed IP address here
IPAddress gateway(GATEWAY);    // Set your network Gateway usually your Router base address
IPAddress subnet(SUBNET);   // Set your network sub-network mask here
IPAddress dns(DNS);           // Set your network DNS usually your Router base address
#else
IPAddress local_IP(192, 168, 1, 150); // Set your server's fixed IP address here
IPAddress gateway(192, 168, 1, 1);    // Set your network Gateway usually your Router base address
IPAddress subnet(255, 255, 255, 0);   // Set your network sub-network mask here
IPAddress dns(192,168,1,1);           // Set your network DNS usually your Router base address	
#endif

char ssid_1[50];
char password_1[50];

char ssid_2[50];
char password_2[50];

char ssid_3[50];
char password_3[50];

char ssid_4[50];
char password_4[50];


#include "CSS.h"

//************************************************************
// DISPLAY SCREEN
//************************************************************
#define ENABLE_DISPLAY_WEBSERVER

#ifdef ENABLE_DISPLAY_WEBSERVER
#include <varioscreenGxEPD.h>
#endif

// All supporting functions from here...
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void HomePage(){
  SendHTML_Header();
  webpage += F("<a href='/download'><button>Download</button></a>");
  webpage += F("<a href='/upload'><button>Upload</button></a>");
//  webpage += F("<a href='/stream'><button>Stream</button></a>");
  webpage += F("<a href='/delete'><button>Delete</button></a>");
  webpage += F("<a href='/dir'><button>Directory</button></a>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop(); // Stop is needed because no content length was sent
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Download(){ // This gets called twice, the first pass selects the input, the second pass then processes the command line arguments
  if (server.args() > 0 ) { // Arguments were received
    if (server.hasArg("download")) SD_file_download(server.arg(0));
  }
  else SelectInput("Enter filename to download","download","download");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SD_file_download(String filename){
  if (SD_present) { 

    char copy[250];
    String scopy = "/"+filename;
    scopy.toCharArray(copy, 250);
    
    File download = SDHAL.open(copy);
    if (download) {
      server.sendHeader("Content-Type", "text/text");
      server.sendHeader("Content-Disposition", "attachment; filename="+filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    } else ReportFileNotPresent("download"); 
  } else ReportSDNotPresent();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Upload(){
  SerialPort.println("File upload stage-1");
  append_page_header();
  webpage += F("<h3>Select File to Upload</h3>"); 
  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<input class='buttons' style='width:40%' type='file' name='fupload' id = 'fupload' value=''><br>");
  webpage += F("<br><button class='buttons' style='width:10%' type='submit'>Upload File</button><br>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  SerialPort.println("File upload stage-2");
  server.send(200, "text/html",webpage);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
File UploadFile; 
void handleFileUpload(){ // upload a new file to the Filing system
  SerialPort.println("File upload stage-3");
  HTTPUpload& uploadfile = server.upload(); // See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            // For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if(uploadfile.status == UPLOAD_FILE_START)
  {
    SerialPort.println("File upload stage-4");
    String filename = uploadfile.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    SerialPort.print("Upload File Name: "); SerialPort.println(filename);

    char copy[250];
    String scopy = filename;
    scopy.toCharArray(copy, 250);

    SDHAL.remove(copy);                         // Remove a previous version, otherwise data is appended the file again
    UploadFile = SDHAL.open(copy, FILE_WRITE);  // Open the file for writing in SPIFFS (create it, if doesn't exist)
    filename = String();
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    SerialPort.println("File upload stage-5");
    if(UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  } 
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if(UploadFile)          // If the file was successfully created
    {                                    
      UploadFile.close();   // Close the file again
      SerialPort.print("Upload Size: "); SerialPort.println(uploadfile.totalSize);
      webpage = "";
      append_page_header();
      webpage += F("<h3>File was successfully uploaded</h3>"); 
      webpage += F("<h2>Uploaded File Name: "); webpage += uploadfile.filename+"</h2>";
      webpage += F("<h2>File Size: "); webpage += file_size(uploadfile.totalSize) + "</h2><br>"; 
      append_page_footer();
      server.send(200,"text/html",webpage);
    } 
    else
    {
      ReportCouldNotCreateFile("upload");
    }
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SD_dir(){
#ifdef SDCARD_DEBUG
  SerialPort.println("SD_Dir");
#endif //SDCARD_DEBUG

  if (SD_present) { 
#ifdef SDCARD_DEBUG
    SerialPort.println("SD_Dir - SD_prent");
#endif //SDCARD_DEBUG

    File root = SDHAL.open("/");
    if (root) {
#ifdef SDCARD_DEBUG
			SerialPort.println("SD_Dir - root");
#endif //SDCARD_DEBUG
	
      root.rewindDirectory();
      SendHTML_Header();
      webpage += F("<h3 class='rcorners_m'>SD Card Contents</h3><br>");
      webpage += F("<table align='center'>");
      webpage += F("<tr><th>Name/Type</th><th style='width:20%'>Type File/Dir</th><th>File Size</th></tr>");
      printDirectory("/",0);
      webpage += F("</table>");
      SendHTML_Content();
      root.close();
    }
    else 
    {
      SendHTML_Header();
      webpage += F("<h3>No Files Found</h3>");
    }
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();   // Stop is needed because no content length was sent
  } else ReportSDNotPresent();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printDirectory(const char * dirname, uint8_t levels){
#ifdef SDCARD_DEBUG
    SerialPort.println("printDirectory");
#endif //SDCARD_DEBUG

  File root = SDHAL.open(dirname);

//#ifdef ESP8266
  root.rewindDirectory(); //Only needed for ESP8266
//#endif

  if(!root){
#ifdef SDCARD_DEBUG
    SerialPort.println("printDirectory - !root");
#endif //SDCARD_DEBUG
		
    return;
  }
  if(!root.isDirectory()){
#ifdef SDCARD_DEBUG
    SerialPort.println("printDirectory - !root.isDirectory");
#endif //SDCARD_DEBUG

    return;
  }
  File fileDir = root.openNextFile();
  while(fileDir){
    if (webpage.length() > 1000) {
      SendHTML_Content();
    }
    if(fileDir.isDirectory()){
      SerialPort.println(String(fileDir.isDirectory()?"Dir ":"File ")+String(fileDir.name()));
      webpage += "<tr><td>"+String(fileDir.isDirectory()?"Dir":"File")+"</td><td>"+String(fileDir.name())+"</td><td></td></tr>";
      printDirectory(fileDir.name(), levels-1);
    }
    else
    {
      //Serial.print(String(file.name())+"\t");
      webpage += "<tr><td>"+String(fileDir.name())+"</td>";
      SerialPort.print(String(fileDir.isDirectory()?"Dir ":"File ")+String(fileDir.name())+"\t");
      webpage += "<td>"+String(fileDir.isDirectory()?"Dir":"File")+"</td>";
      int bytes = fileDir.size();
      String fsize = "";
      if (bytes < 1024)                     fsize = String(bytes)+" B";
      else if(bytes < (1024 * 1024))        fsize = String(bytes/1024.0,3)+" KB";
      else if(bytes < (1024 * 1024 * 1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
      else                                  fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
      webpage += "<td>"+fsize+"</td></tr>";
      SerialPort.println(String(fsize));
    }
    fileDir = root.openNextFile();
  }
  fileDir.close();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Stream(){
  if (server.args() > 0 ) { // Arguments were received
    if (server.hasArg("stream")) SD_file_stream(server.arg(0));
  }
  else SelectInput("Enter a File to Stream","stream","stream");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SD_file_stream(String filename) { 
  if (SD_present) { 

    char copy[250];
    String scopy = "/"+filename;
    scopy.toCharArray(copy, 250);
    
    File dataFile = SDHAL.open(copy, FILE_READ); // Now read data from SD Card 
    SerialPort.print("Streaming file: "); SerialPort.println(filename);
    if (dataFile) { 
      if (dataFile.available()) { // If data is available and present 
        String dataType = "application/octet-stream"; 
        if (server.streamFile(dataFile, dataType) != dataFile.size()) {SerialPort.print(F("Sent less data than expected!")); } 
      }
      dataFile.close(); // close the file: 
    } else ReportFileNotPresent("Cstream");
  } else ReportSDNotPresent(); 
}   
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Delete(){
  if (server.args() > 0 ) { // Arguments were received
    if (server.hasArg("delete")) SD_file_delete(server.arg(0));
  }
  else SelectInput("Select a File to Delete","delete","delete");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SD_file_delete(String filename) { // Delete the file 
  if (SD_present) { 
    SendHTML_Header();
    
    char copy[250];
    String scopy = "/"+filename;
    scopy.toCharArray(copy, 250);
    
    File dataFile = SDHAL.open(copy, FILE_READ); // Now read data from SD Card 
    SerialPort.print("Deleting file: "); SerialPort.println(filename);
    if (dataFile)
    {
      if (SDHAL.remove(copy)) {
        SerialPort.println(F("File deleted successfully"));
        webpage += "<h3>File '"+filename+"' has been erased</h3>"; 
        webpage += F("<a href='/delete'>[Back]</a><br><br>");
      }
      else
      { 
        webpage += F("<h3>File was not deleted - error</h3>");
        webpage += F("<a href='delete'>[Back]</a><br><br>");
      }
    } else ReportFileNotPresent("delete");
    append_page_footer(); 
    SendHTML_Content();
    SendHTML_Stop();
  } else ReportSDNotPresent();
} 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Header(){
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate"); 
  server.sendHeader("Pragma", "no-cache"); 
  server.sendHeader("Expires", "-1"); 
  server.setContentLength(CONTENT_LENGTH_UNKNOWN); 
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves. 
  append_page_header();
  server.sendContent(webpage);
  webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Content(){
  server.sendContent(webpage);
  webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Stop(){
  server.sendContent("");
  server.client().stop(); // Stop is needed because no content length was sent
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SelectInput(String heading1, String command, String arg_calling_name){
  SendHTML_Header();
  webpage += F("<h3>"); webpage += heading1 + "</h3>"; 
  webpage += F("<FORM action='/"); webpage += command + "' method='post'>"; // Must match the calling argument e.g. '/chart' calls '/chart' after selection but with arguments!
  webpage += F("<input type='text' name='"); webpage += arg_calling_name; webpage += F("' value=''><br>");
  webpage += F("<type='submit' name='"); webpage += arg_calling_name; webpage += F("' value=''><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportSDNotPresent(){
  SendHTML_Header();
  webpage += F("<h3>No SD Card present</h3>"); 
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportFileNotPresent(String target){
  SendHTML_Header();
  webpage += F("<h3>File does not exist</h3>"); 
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ReportCouldNotCreateFile(String target){
  SendHTML_Header();
  webpage += F("<h3>Could Not Create Uploaded File (write-protected?)</h3>"); 
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
String file_size(int bytes){
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}

boolean WifiServer::begin(void) {
	
	GnuSettings.VARIOMETER_SSID_1.toCharArray(ssid_1,sizeof(ssid_1));
	GnuSettings.VARIOMETER_PASSWORD_1.toCharArray(password_1,sizeof(password_1));

	GnuSettings.VARIOMETER_SSID_2.toCharArray(ssid_2,sizeof(ssid_2));
	GnuSettings.VARIOMETER_PASSWORD_2.toCharArray(password_2,sizeof(password_2));

	GnuSettings.VARIOMETER_SSID_3.toCharArray(ssid_3,sizeof(ssid_3));
	GnuSettings.VARIOMETER_PASSWORD_3.toCharArray(password_3,sizeof(password_3));

	GnuSettings.VARIOMETER_SSID_4.toCharArray(ssid_4,sizeof(ssid_4));
	GnuSettings.VARIOMETER_PASSWORD_4.toCharArray(password_4,sizeof(password_4));
	
	return true;
};

void WifiServer::disableWifi(void) {
	WiFi.mode(WIFI_OFF);
}

void WifiServer::disableBT(void) {
}
		
void WifiServer::disableAll(void) {
	disableWifi();
	disableBT();
}

   
void WifiServer::connect(void) {
#ifdef IP_FIXE
 if (!WiFi.config(local_IP, gateway, subnet, dns)) { //WiFi.config(ip, gateway, subnet, dns1, dns2);
    SerialPort.println("WiFi STATION Failed to configure Correctly"); 
  } 
#endif //IP_FIXE	
	
  wifiMulti.addAP(ssid_1, password_1);  // add Wi-Fi networks you want to connect to, it connects strongest to weakest
  wifiMulti.addAP(ssid_2, password_2);  // Adjust the values in the Network tab
  wifiMulti.addAP(ssid_3, password_3);
  wifiMulti.addAP(ssid_4, password_4);  // You don't need 4 entries, this is for example!
  
  SerialPort.println("Connecting ...");
#ifdef ENABLE_DISPLAY_WEBSERVER
	screen.ScreenViewWifi("", "");
#endif //ENABLE_DISPLAY_WEBSERVER
	
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250); SerialPort.print('.');
  }
  SerialPort.println("\nConnected to "+WiFi.SSID()+" Use IP address: "+WiFi.localIP().toString()); // Report which SSID and IP is in use
  // The logical name http://fileserver.local will also access the device if you have 'Bonjour' running or your system supports multicast dns
#ifdef ENABLE_DISPLAY_WEBSERVER
	screen.ScreenViewWifi(WiFi.SSID(), WiFi.localIP().toString());
#endif //ENABLE_DISPLAY_WEBSERVER
  if (!MDNS.begin(servername)) {          // Set your preferred server name, if you use "myserver" the address would be http://myserver.local/
    SerialPort.println(F("Error setting up MDNS responder!")); 
    ESP.restart(); 
  } 
}

void WifiServer::start(void) {

  // Note: Using the ESP32 and SD_Card readers requires a 1K to 4K7 pull-up to 3v3 on the MISO line, otherwise they do-not function.
  //----------------------------------------------------------------------   
  ///////////////////////////// Server Commands 
  server.on("/",         HomePage);
  server.on("/dir",      SD_dir);
  server.on("/upload",   File_Upload);
  server.on("/fupload",  HTTP_POST,[](){ server.send(200);}, handleFileUpload);
  server.on("/download", File_Download);
//  server.on("/stream",   File_Stream);
  server.on("/delete",   File_Delete);
  
/*
   server.on("/",         server_homePage);
   server.on("/dir",      server_listDir);
   server.on("/upload",   server_fileUpload);
   server.on("/fupload",  HTTP_POST,[](){ server.send(200);}, server_handleFileUpload);
   server.on("/download", server_fileDownload);
   server.on("/delete",   server_fileDelete);
   server.on("/datalog",  server_downloadDataLog);
*/		
	
  ///////////////////////////// End of Request commands
  server.begin();
  SerialPort.println("HTTP server started");
#ifdef ENABLE_DISPLAY_WEBSERVER
	screen.ScreenViewWifi("START", "");
#endif //ENABLE_DISPLAY_WEBSERVER
}

void WifiServer::handleClient(void){
	server.handleClient(); // Listen for client connections
}


WifiServer wifiServer; 
