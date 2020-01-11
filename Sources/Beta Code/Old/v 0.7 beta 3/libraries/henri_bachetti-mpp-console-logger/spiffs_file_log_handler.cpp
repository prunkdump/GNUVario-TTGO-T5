
#if defined ESP32 || defined ESP8266

#if defined ESP32
#include <sdkconfig.h>
#else
#include <spiffs/spiffs_config.h>
#endif

#include "mpp-console.h"
#include "mpp-spiffs_log_handler.h"
//#define LOG_DEBUG
#include "mpp_local.h"

#ifndef SPIFFS_OBJ_NAME_LEN
#define SPIFFS_OBJ_NAME_LEN 32
#endif

SpiffsFileLogHandler::SpiffsFileLogHandler(LogFormatter *formatter, const char *filename, size_t maxSize, int backupCount) :
LogHandler(formatter, "SpiffsFileLogHandler")
{
  log_name = filename;
  log_size = maxSize;
  log_backup = backupCount;
}

File SpiffsFileLogHandler::openLog(void)
{
  File f = SPIFFS.open(log_name, "a");
  if (!f) {
    debug_print("openLog %s: error\n", log_name);
    return f;
  }
  if (f.size() > log_size) {
    debug_print("openLog %s %ld > %ld\n", log_name, f.size(), log_size);
    f.close();
    logRotate();
    File f = SPIFFS.open(log_name, "a");
    if (!f) {
      debug_print("openLog %s: error\n", log_name);
      return f;
    }
  }
  return f;
}

int SpiffsFileLogHandler::logRotate(void)
{
  int count = 0, fno, flast = 0;
  size_t size;
  
#if defined ESP32
  File dir = SPIFFS.open("/");
  if (!dir) {
    printf("error opening /\n");
    return 0;
  }
#else
  Dir dir = SPIFFS.openDir("");
#endif
#if defined ESP32
  File file = dir.openNextFile();
  while (file) {
    size = file.size();
    char fname[SPIFFS_OBJ_NAME_LEN];
    strcpy(fname, file.name());
#else
  while (dir.next()) {
    size = dir.fileSize();
    char fname[SPIFFS_OBJ_NAME_LEN];
    strcpy(fname, dir.fileName().c_str());
#endif
    if (!strncmp(fname, log_name, strlen(log_name))) {
      debug_print("found %s %ld\n", fname, size);
      char *p = strrchr(fname, '.');
      char *ext = p+1;
      fno = atoi(ext);
      if (fno != 0) {
        if (fno > flast) {
          flast = fno;
        }
      }
    }
#if defined ESP32
    file = dir.openNextFile();
#endif
  }
  klog("SpiffsFileLogHandler::logRotate: %d files\n", flast);
  if (flast != 0) {
    for (fno = flast ; fno >= log_backup ; fno--) {
      char name[SPIFFS_OBJ_NAME_LEN];
      sprintf(name, "%s.%d", log_name, fno);
      debug_print("%s: delete\n", name);
      if (SPIFFS.remove(name) != true) {
        debug_print("%s: cannot delete\n", name);
      }
      debug_print("%s: successfully deleted\n", name);
      flast--;
    }
    for (fno = flast ; fno > 0 ; fno--) {
      char name[SPIFFS_OBJ_NAME_LEN];
      char newname[SPIFFS_OBJ_NAME_LEN];
      sprintf(name, "%s.%d", log_name, fno);
      sprintf(newname, "%s.%d", log_name, fno+1);
      debug_print("%s: rename to %s\n", name, newname);
      if (SPIFFS.rename(name, newname) != true) {
        debug_print("%s: cannot rename to %s\n", name, newname);
      }
      debug_print("%s: successfully renamed to %s\n", name, newname);
    }
  }
  char newname[SPIFFS_OBJ_NAME_LEN];
  sprintf(newname, "%s.%d", log_name, fno+1);
  debug_print("%s: rename to %s\n", log_name, newname);
  if (SPIFFS.rename(log_name, newname) != true) {
    debug_print("%s: cannot rename to %s\n", log_name, newname);
  }
  debug_print("%s: successfully renamed to %s\n", log_name, newname);
  return 0;
}

void SpiffsFileLogHandler::send(const char *msg)
{
  File f = openLog();
  size_t len = strlen(msg);
  size_t n = f.println(msg);
  f.close();
  debug_print("%s: %s\n", log_name, msg);
  debug_print("%s: %d/%d bytes written\n", log_name, n, len);
}

void SpiffsFileLogHandler::sendRaw(const char *msg)
{
  File f = openLog();
  size_t len = strlen(msg);
  size_t n = f.print(msg);
  f.close();
  debug_print("%s: %s\n", log_name, msg);
  debug_print("%s: %d/%d bytes written\n", log_name, n, len);
}

#endif

