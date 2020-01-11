
/*
   ESP32 SDFAT logging WebServer demo
   - console outputs to Serial
   - logs output to Serial and SDFAT
   - logging level is: DEBUG (change to INFO, WARNING, ERROR, CRITICAL to see the effect)
   - the sketch can run only on ESP32

   A  log file named access.log is created in the SDFAT file system
   WIFI and HTTP connections are logged to access.log
   The maximum log file size is 10000 bytes
   When the maximum size is reached the log file is copied int access.log.1
   5 backups files are kept : (access.log.1, access.log2, access.log3, access.log4, access.log5)

   A  log file named debug.log is created in the SDFAT file system
   Some binary data is logged to debug.log at boot
   The maximum log file size is 10000 bytes
   When the maximum size is reached the log file is copied int debug.log.1
   5 backups files are kept : (debug.log.1, debug.log2, debug.log3, debug.log4, debug.log5)

   The home pages shows a link to view the log files :

   LOG: /access.log.5 (10008 bytes) download
   LOG: /access.log.4 (10012 bytes) download
   LOG: /access.log.3 (10014 bytes) download
   LOG: /access.log.2 (10007 bytes) download
   LOG: /access.log.1 (10009 bytes) download
   LOG: /access.log (2500 bytes) download
   LOG: /debug.log (3500 bytes) download

   Click on the filename to view.
   Click on "download" to download.
*/

#include <Time.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPI.h>
#include <SD.h>

#include <mpp-console.h>
#include "debug.h"

Logger LOGGER("http");
LogNameFormatter formatter(&LOGGER);
StreamLogHandler streamHandler(&formatter, &Serial);
Logger ACCESS_LOG("access");
LogTimeDateFormatter logFormatter(&ACCESS_LOG);
SdFatFileLogHandler logHandler(&logFormatter, &SD, "/access.log", 10000, 5);
Logger DEBUG_LOG("debug");
LogTimeDateFormatter debugLogFormatter(&DEBUG_LOG);
SdFatFileLogHandler debugLogHandler(&debugLogFormatter, &SD, "/debug.log", 10000, 5);

const char* ssid = "**********";
const char* password = "********************";
const char* host = "esp32sdfat";

WebServer server(80);

void mount(void)
{
  if (SD.begin(SS)) {
    log_info("SD Card initialized.");
    logFormatter.setHandler(&logHandler);
    debugLogFormatter.setHandler(&debugLogHandler);
    log_info(F("SdFatFileLogHandler installed"));
  }
  log_some_binary_data();
}

void setup(void)
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("ESP32 SDFAT logging WebServer demo\n");
  LOGGER.init(&Serial);
  LOGGER.setLevel(DEBUG);
  formatter.setHandler(&streamHandler);
  LOGGER.setFormatter(&formatter);
  ACCESS_LOG.init(&Serial);
  ACCESS_LOG.setLevel(INFO);
  logFormatter.setHandler(&streamHandler);
  ACCESS_LOG.setFormatter(&logFormatter);
  DEBUG_LOG.init(&Serial);
  DEBUG_LOG.setLevel(INFO);
  debugLogFormatter.setHandler(&streamHandler);
  DEBUG_LOG.setFormatter(&debugLogFormatter);
  mount();
  setTime(1552922658);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  log_info(F("Connecting to %s"), ssid);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    putchar('.');
    delay(500);
  }
  putchar('\n');
  log_ip_addr("Connected! IP address:", WiFi.localIP(), 0);

  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    log_info("MDNS responder started");
    log_info("You can now connect to http://%s.local", host);
  }
  server.on("/", HTTP_GET, handleRoot);
  server.on("/logs", HTTP_GET, handleLogs);
  server.onNotFound(handleNotFound);
  server.begin();
  log_info("HTTP server started");
}

#define BUF_LEN     128

void log_some_binary_data(void)
{
  uint8_t buf[BUF_LEN];
  for (int i = 0 ; i < BUF_LEN ; i++) {
    buf[i] = i;
  }
  log_dump("buf", buf, BUF_LEN);
}

void log_ip_addr(const char *msg, IPAddress ip, const char *url)
{
  ACCESS_LOG.info("%s %d.%d.%d.%d", msg, ip[0], ip[1], ip[2], ip[3]);
  if (url) {
    ACCESS_LOG.info(url);
  }
}

void returnOK() {
  server.send(200, "text/plain", "");
}

void returnFail(String msg) {
  server.send(500, "text/plain", msg + "\r\n");
}

void handleRoot()
{
  WiFiClient client = server.client();
  log_ip_addr("Connection from", client.remoteIP(), server.uri().c_str());
  LOGGER.debug("handleRoot");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  char buf[80];
  snprintf(buf, 80, "<p><a href=\"/logs\">LOGS</a> ");
  server.sendContent(buf);
}

void download(String filename) {
  LOGGER.debug("download: % s", filename.c_str());
  File download = SD.open("/" + filename);
  if (download) {
    server.sendHeader("Content-Type", "text/text");
    server.sendHeader("Content-Disposition", "attachment; filename=" + filename);
    server.sendHeader("Connection", "close");
    server.streamFile(download, "application/octet-stream");
    download.close();
  }
  else {
    return returnFail("NOT FOUND");
  }
}

void handleLogs()
{
  WiFiClient client = server.client();
  log_ip_addr("Connection from", client.remoteIP(), server.uri().c_str());
  if (server.args() != 0) {
    if (server.hasArg("download")) {
      download(server.arg(0));
    }
    else if (server.hasArg("name")) {
      String name = server.arg(0);
      LOGGER.debug("handleLogs: % s", name.c_str());
      File f = SD.open(name);
      if (!f) {
        return returnFail("NOT FOUND");
      }
      server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      server.send(200, "text/html", "");
      char buf[80];
      int c, n = 0;
      server.sendContent("<pre>");
      while ((c = f.read()) > 0) {
        buf[n++] = c;
        if (c == '\n' || n == 79) {
          buf[n] = '\0';
          LOGGER.debug("handleLogs: % d bytes", n);
          server.sendContent(buf);
          n = 0;
        }
      }
      if (n > 0) {
        LOGGER.debug("handleLogs: % d bytes", n);
        server.sendContent(buf);
      }
      server.sendContent("</pre> ");
    }
  }
  else {
    File dir = SD.open("/");
    if (!dir.isDirectory()) {
      dir.close();
      return returnFail("NOT DIR");
    }
    dir.rewindDirectory();
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text / html", "");
    for (int cnt = 0; true; ++cnt) {
      File entry = dir.openNextFile();
      if (!entry) {
        break;
      }
      LOGGER.debug("handleLogs: % s", entry.name());
      if (strstr(entry.name(), ".log")) {
        char buf[80];
        snprintf(buf, 80, "<p>LOG: <a href=\"/logs?name=%s\">%s</a> (%lu bytes) ", entry.name(), entry.name(), entry.size());
        server.sendContent(buf);
        snprintf(buf, 80, "<a href=\"/logs?download=%s\">download</a></p>", entry.name());
        server.sendContent(buf);
      }
      entry.close();
    }
    dir.close();
  }
}

void handleNotFound()
{
  char srv_addr[16];
  char cli_addr[16];

  log_debug(F("handleNotFound"));
  WiFiClient client = server.client();
  IPAddress ip = WiFi.localIP();
  snprintf(srv_addr, 16, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  ip = client.remoteIP();
  snprintf(cli_addr, 16, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  String message("<!DOCTYPE HTML>\n<html><head>\n<title>404 Not Found</title>\n</head><body>");
  message += "\n\n<p>Sorry ";
  message += cli_addr;
  message += "</p>\n";
  message += "\n<h1>Not Found</h1>\n<p>The requested URL ";
  message += server.uri();
  for (uint8_t i = 0 ; i < server.args(); i++) {
    message += i == 0 ? '?' : '&';
    message += server.argName(i) + "=" + server.arg(i);
  }
  message += " was not found on this server.</p>\n<hr>\n<address>ESP32 Server at ";
  message += srv_addr;
  message += ", Method: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "</address>\n</body></html>";
  server.send(404, "text/html", message);
}

void loop(void) {
  server.handleClient();
}
