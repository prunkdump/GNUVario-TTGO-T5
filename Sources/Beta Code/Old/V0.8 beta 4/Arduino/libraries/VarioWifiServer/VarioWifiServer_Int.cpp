/*********************************************************************************/
/*                                                                               */
/*                           Libraries wifiServer                                 */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0    31/07/19                                                            */
/*    1.1    16/08/19   Ajout Affichage sur écran                                */
/*    1.1.1  17/08/19   Ajout message de debuggage                               */
/*    1.2    21.08/19   Ajout Update ESP32                                       */
/*    1.2.1  28/08/19		Modification de l'organisation des menus							 	 */
/*    1.2.2  04/10/19   Ajout reboot après l'update                              */
/*    1.2.3  11/12/19   Changement de nom                                        */
/*                                                                               */
/*********************************************************************************/

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#if not defined(WEBSERVER_SDCARD)

#include <VarioWifiServer_Int.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

#include <SPI.h>

#include <VarioSettings.h>

#include <Update.h>

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
  webpage += F("<a href='/dir'><button>Directory</button></a>");
  webpage += F("<a href='/download'><button>Download</button></a>");
  webpage += F("<a href='/upload'><button>Upload</button></a>");
//  webpage += F("<a href='/stream'><button>Stream</button></a>");
  webpage += F("<a href='/delete'><button>Delete</button></a>");
  webpage += F("<a href='/Update'><button>Update</button></a>");
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
#ifdef HAVE_SDCARD
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
#else
	ReportSDNotPresent();
#endif
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
#ifdef HAVE_SDCARD
File UploadFile; 
#endif

/***********************************************************************/
void handleFileUpload(){ // upload a new file to the Filing system
/***********************************************************************/
#ifdef HAVE_SDCARD
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
#else
	ReportSDNotPresent();
#endif	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SD_dir(){
#ifdef HAVE_SDCARD
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
#else
	ReportSDNotPresent();
#endif

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printDirectory(const char * dirname, uint8_t levels){
#ifdef HAVE_SDCARD
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
#else
	ReportSDNotPresent();
#endif

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
#ifdef HAVE_SDCARD
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
#else
	ReportSDNotPresent();
#endif
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
#ifdef HAVE_SDCARD
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
#else
	ReportSDNotPresent();
#endif
} 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void File_Update(){
  SerialPort.println("File update stage-1");
  append_page_header();
  webpage += F("<h3>Select File to Update</h3>"); 
  webpage += F("<FORM action='/fupdate' method='post' enctype='multipart/form-data'>");
  webpage += F("<input class='buttons' style='width:40%' type='file' name='fupdate' id = 'fupdate' value=''><br>");
  webpage += F("<br><button class='buttons' style='width:10%' type='submit'>Update File</button><br>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  SerialPort.println("File update stage-2");
  server.send(200, "text/html",webpage);}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef HAVE_SDCARD
File UpdateFile; 
#endif

void handleFileUpdate(){ // upload a new file to the Filing system
#ifdef HAVE_SDCARD
  SerialPort.println("File update stage-3");
  HTTPUpload& updatefile = server.upload(); // See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            // For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if(updatefile.status == UPLOAD_FILE_START)
  {		
    SerialPort.println("File update stage-4");
/*    String filename = updatefile.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    SerialPort.print("Update File Name: "); SerialPort.println(filename);

    char copy[250];
    String scopy = filename;
    scopy.toCharArray(copy, 250);

    SDHAL.remove(copy);                         // Remove a previous version, otherwise data is appended the file again
    UpdateFile = SDHAL.open(copy, FILE_WRITE);  // Open the file for writing in SPIFFS (create it, if doesn't exist)
    filename = String();*/
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(SerialPort);
    }
  }
  else if (updatefile.status == UPLOAD_FILE_WRITE)
  {
    SerialPort.println("File update stage-5");
//    if(UpdateFile) UpdateFile.write(updatefile.buf, updatefile.currentSize); // Write the received bytes to the file
      /* flashing firmware to ESP*/
      if (Update.write(updatefile.buf, updatefile.currentSize) != updatefile.currentSize) {
        Update.printError(SerialPort);
      }
  } 
  else if (updatefile.status == UPLOAD_FILE_END)
  {
 /*   if(UpdateFile)          // If the file was successfully created
    {                                    
      UpdateFile.close();   // Close the file again
      SerialPort.print("Update Size: "); SerialPort.println(updatefile.totalSize);
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
      ReportCouldNotCreateFile("update");
    }*/
		
    if (Update.end(true)) { //true to set the size to the current progress
      SerialPort.printf("Update Success: %u\nRebooting...\n", updatefile.totalSize);
      webpage = "";
      append_page_header();
      webpage += F("<h3>ESP32 was successfully updated</h3>"); 
      webpage += F("<h2>Updated File Name: "); webpage += updatefile.filename+"</h2>";
      webpage += F("<h2>File Size: "); webpage += file_size(updatefile.totalSize) + "</h2><br>"; 
      append_page_footer();
      server.send(200,"text/html",webpage);	

			SerialPort.println("RESTART ESP32");
			SerialPort.flush();
			ESP_LOGI("GnuVario-E", "RESTART ESP32");
			screen.ScreenViewReboot();
			ESP.restart();		
			
    } else {
      Update.printError(SerialPort);
      ReportCouldNotUpdateFile("update");
    }	
  }
#else
	ReportSDNotPresent();
#endif
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
void ReportCouldNotUpdateFile(String target){
  SendHTML_Header();
  webpage += F("<h3>Could Not Update ESP32 (write-protected?)</h3>"); 
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
	
}//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
String file_size(int bytes){
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}

/*
 * Server Index Page
 */
 
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";
 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
boolean VarioWifiServer::begin(void) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	GnuSettings.VARIOMETER_SSID_1.toCharArray(ssid_1,sizeof(ssid_1));
	GnuSettings.VARIOMETER_PASSWORD_1.toCharArray(password_1,sizeof(password_1));

#ifdef WIFI_DEBUG
  SerialPort.print("ssid_1 : ");
  SerialPort.println(ssid_1);
  SerialPort.print("password_1 : ");
  SerialPort.println(password_1);
#endif //WIFI_DEBUG

	GnuSettings.VARIOMETER_SSID_2.toCharArray(ssid_2,sizeof(ssid_2));
	GnuSettings.VARIOMETER_PASSWORD_2.toCharArray(password_2,sizeof(password_2));

#ifdef WIFI_DEBUG
  SerialPort.print("ssid_2 : ");
  SerialPort.println(ssid_2);
  SerialPort.print("password_2 : ");
  SerialPort.println(password_2);
#endif //WIFI_DEBUG

	GnuSettings.VARIOMETER_SSID_3.toCharArray(ssid_3,sizeof(ssid_3));
	GnuSettings.VARIOMETER_PASSWORD_3.toCharArray(password_3,sizeof(password_3));
	
#ifdef WIFI_DEBUG
  SerialPort.print("ssid_3 : ");
  SerialPort.println(ssid_3);
  SerialPort.print("password_3 : ");
  SerialPort.println(password_3);
#endif //WIFI_DEBUG

	GnuSettings.VARIOMETER_SSID_4.toCharArray(ssid_4,sizeof(ssid_4));
	GnuSettings.VARIOMETER_PASSWORD_4.toCharArray(password_4,sizeof(password_4));

#ifdef WIFI_DEBUG
  SerialPort.print("ssid_4 : ");
  SerialPort.println(ssid_4);
  SerialPort.print("password_4 : ");
  SerialPort.println(password_4);
#endif //WIFI_DEBUG
	
	return true;
};

void VarioWifiServer::disableWifi(void) {
	WiFi.mode(WIFI_OFF);
}

void VarioWifiServer::disableBT(void) {
}
		
void VarioWifiServer::disableAll(void) {
	disableWifi();
	disableBT();
}

   
void VarioWifiServer::connect(void) {
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
	
	int counter = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250); SerialPort.print('.');
		counter++;
		if (counter > 30) {
			SerialPort.println(F("Error Wifi not found!")); 
			ESP.restart(); 
		}
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

void VarioWifiServer::start(void) {

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
  server.on("/update",   File_Update);
  server.on("/fupdate",  HTTP_POST,[](){ server.send(200);}, handleFileUpdate);
  
/*  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });	
  // *handling uploading firmware file *
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      // * flashing firmware to ESP*
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });	*/
	
	
	
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

void VarioWifiServer::handleClient(void){
	server.handleClient(); // Listen for client connections
}

VarioWifiServer varioWifiServer; 

#endif //WEBSERVER_SDCARD
