
/*
   sdfat logging demo
   - console outputs to Serial
   - logs output to sdfat
   - logging level is: DEBUG (change to INFO, WARNING, ERROR, CRITICAL to see the effect)
   - the sketch can run only on ESP32
   - select tools/Partition Scheme to match your needs

   A  log file named test.log is created in the SDFAT file system
   The maximum log file size is 10000 bytes
   When the maximum size is reached the log file is copied int test.log.1
   5 backups files are kept : (test.log.1, test.log2, test.log3, test.log4, test.log5)
*/

#ifndef ESP32
#error "This sketch requires ESP32"
#endif

#include "mpp-console.h"
#include "mpp-logger.h"
#include "mpp-sd_log_handler.h"
#include "debug.h"

Logger LOGGER("test");
LogTimeDateFormatter formatter(&LOGGER);
SdFatFileLogHandler handler(&LOGGER, &SD, "/test.log", 10000, 5);

void createFile(const char *name)
{
  File f = SD.open(name, FILE_WRITE);
  if (!f) {
    log_error("%s: open error", name);
    return;
  }
  char s[] = "AZERTYUIOPQSDFGHJKLMWXCVBN\n";
  for (int i = 0 ; i < 10 ; i++) {
    if (f.print(s) != 27) {
      log_error("write error");
    }
  }
  f.close();
  log_info("%s: created");
}

void mount(void)
{
  if (SD.begin(SS)) {
    log_info("SD Card initialized.");
    LOGGER.setHandler(&handler);
  }
  log_info("SDFAT mounted");
  File dir = SD.open("/");
  if (!dir) {
    log_error("file /: error\n");
    return;
  }
  dir.rewindDirectory();
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    log_debug("found %s %ld\n", entry.name(), entry.size());
    entry.close();
    yield();
  }
  dir.close();
}

void setup()
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("SDFAT logging demo\n");
  LOGGER.init(&Serial);
  LOGGER.setLevel(DEBUG);
  LOGGER.setFormatter(&formatter);
  mount();
  createFile("test.log.6");
  createFile("test.log.7");
  createFile("test.log.8");
}

void loop()
{
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
  delay(1000);
}

