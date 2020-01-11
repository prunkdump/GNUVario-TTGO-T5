
/*
   spiffs logging demo
   - console outputs to Serial
   - logs output to spiffs
   - logging level is: DEBUG (change to INFO, WARNING, ERROR, CRITICAL to see the effect)
   - the sketch can run only on ESP32
   - select tools/Partition Scheme to match your needs

   A  log file named test.log is created in the SPIFFS file system
   The maximum log file size is 10000 bytes
   When the maximum size is reached the log file is copied int test.log.1
   5 backups files are kept : (test.log.1, test.log2, test.log3, test.log4, test.log5)

   At boot, the log files are listed and the content of test.log is printed.

*/

#if !defined ESP32 && !defined ESP8266
#error "This sketch requires ESP8266 or ESP32"
#endif

#if defined ESP8266
#include <ESP8266WiFi.h>
#endif

#include "mpp-console.h"
#include "mpp-logger.h"
#include "mpp-spiffs_log_handler.h"
#include "debug.h"

Logger LOGGER("test");
LogNameFormatter formatter(&LOGGER);
SpiffsFileLogHandler handler(&formatter, "/test.log", 10000, 5);

void printLog(void)
{
  puts("########## CONTENT OF TEST.LOG ##########");
  File f = SPIFFS.open("/test.log", "r");
  if (!f) {
    puts("log file not found");
  }
  char buf[80];
  int c, n = 0;
  while ((c = f.read()) > 0) {
    buf[n++] = c;
    if (c == '\n' || n == 79) {
      buf[n] = '\0';
      printf(buf);
      n = 0;
    }
  }
  if (n > 0) {
    printf(buf);
  }
  puts("########## END OF CONTENT OF TEST.LOG ##########");
}

void mount(void)
{
  int count;

#ifdef ESP32
  if (!SPIFFS.begin(true)) {
#else
  struct FSInfo fs_info;
  if (SPIFFS.begin() != true) {
#endif
    printf("SPIFFS mount failed\n");
    return;
  }
  printf("\nSPIFFS mounted\n");
#ifdef ESP32
  printf("%ld / %ld bytes used\n", SPIFFS.usedBytes(), SPIFFS.totalBytes());
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
#else
  if (SPIFFS.begin() != true) {
    SPIFFS.info(fs_info);
    printf("%ld / %ld bytes used\n", fs_info.usedBytes, fs_info.totalBytes);
    Dir dir = SPIFFS.openDir("");
    while (dir.next()) {
      printf("%s: %d bytes\n", dir.fileName().c_str(), dir.fileSize());
    }
  }
#endif
  printLog();
}

void setup()
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("SPIFFS logging demo\n");
  mount();
  LOGGER.init(NULL);
  LOGGER.setLevel(DEBUG);
  formatter.setHandler(&handler);
  LOGGER.setFormatter(&formatter);
}

#define BUF_LEN     128

void loop()
{
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
  uint8_t buf[BUF_LEN];
  for (int i = 0 ; i < BUF_LEN ; i++) {
    buf[i] = i;
  }
  log_dump("buf", buf, BUF_LEN);
  delay(1000);
}
