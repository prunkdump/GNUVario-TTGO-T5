
/*
   ESP32 SPIFFS logging WebServer demo
   - console outputs to Serial
   - logs output to Serial and SPIFFS
   - logging level is: DEBUG (change to INFO, WARNING, ERROR, CRITICAL to see the effect)
   - the sketch can run only on ESP32

   A  log file named access.log is created in the SPIFFS file system
   WIFI and HTTP connections and logged to access.log
   The maximum log file size is 10000 bytes
   When the maximum size is reached the log file is copied int access.log.1
   5 backups files are kept : (access.log.1, access.log2, access.log3, access.log4, access.log5)

   The home pages shows a link to view the log files :
   
   LOG: /access.log.5 (10008 bytes) download
   LOG: /access.log.4 (10012 bytes) download
   LOG: /access.log.3 (10014 bytes) download
   LOG: /access.log.2 (10007 bytes) download
   LOG: /access.log.1 (10009 bytes) download
   LOG: /access.log (2500 bytes) download   

   Click on the filename to view.
   Click on "download" to download.
*/

#include <Time.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include <mpp-console.h>
#include "debug.h"

Logger LOGGER("http");
LogNameFormatter formatter(&LOGGER);
StreamLogHandler streamHandler(&formatter, &Serial);
Logger ACCESS("access");
LogTimeDateFormatter logFormatter(&ACCESS);
SpiffsFileLogHandler logHandler(&logFormatter, "/access.log", 10000, 5);

const char* ssid = "**********";
const char* password = "********************";
const char* host = "esp32spiffs";

WebServer server(80);

void mount(void)
{
  int count;
  size_t total = 0, used = 0;
  struct dirent *ent;

  if (!SPIFFS.begin(true)) {
    printf("SPIFFS mount failed\n");
    return;
  }
  printf("\nSPIFFS mounted\n");
  printf("%ld / %ld bytes used\n", SPIFFS.usedBytes(), SPIFFS.totalBytes());
  logFormatter.setHandler(&logHandler);
  log_info(F("SpiffsFileLogHandler installed"));
  File dir = SPIFFS.open("/");
  if (!dir) {
    printf("error opening /\n");
    return;
  }
  File file = dir.openNextFile();
  while (file) {
    printf("%s: %d bytes\n", file.name(), file.size());
    file = dir.openNextFile();
  }
  dir.close();
}

void setup(void)
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("ESP32 SPIFFS logging WebServer demo\n");
  LOGGER.init(&Serial);
  LOGGER.setLevel(DEBUG);
  formatter.setHandler(&streamHandler);
  LOGGER.setFormatter(&formatter);
  ACCESS.init(&Serial);
  ACCESS.setLevel(INFO);
  logFormatter.setHandler(&streamHandler);
  ACCESS.setFormatter(&logFormatter);
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

void log_ip_addr(const char *msg, IPAddress ip, const char *url)
{
  ACCESS.info("%s %d.%d.%d.%d", msg, ip[0], ip[1], ip[2], ip[3]);
  if (url) {
    ACCESS.info(url);
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
  File download = SPIFFS.open(filename, "r");
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
      File f = SPIFFS.open(name, "r");
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
    File dir = SPIFFS.open("/");
    if (!dir) {
      return returnFail("NOT DIR");
      return;
    }
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text / html", "");
    File file = dir.openNextFile();
    while (file) {
      LOGGER.debug("handleLogs: % s", file.name());
      if (strstr(file.name(), ".log")) {
        char buf[80];
        snprintf(buf, 80, "<p>LOG: <a href=\"/logs?name=%s\">%s</a> (%lu bytes) ", file.name(), file.name(), file.size());
        server.sendContent(buf);
        snprintf(buf, 80, "<a href=\"/logs?download=%s\">download</a></p>", file.name());
        server.sendContent(buf);
      }
      file = dir.openNextFile();
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

