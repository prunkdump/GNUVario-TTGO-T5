
#ifndef _MPP_SPIFFS_FILE_HANDLER_H_
#define _MPP_SPIFFS_FILE_HANDLER_H_

#if defined ESP8266 || defined ESP32

#include "mpp-logger.h"

#if defined ESP8266 || defined ESP32
#ifdef ESP8266
#include <FS.h>
#else
#include "SPIFFS.h"
#endif
#endif

class SpiffsFileLogHandler : public LogHandler {
  protected:
    const char *log_name;
    size_t log_size;
    int log_backup;
    File openLog(void);
    int logRotate(void);

  public:
    SpiffsFileLogHandler(LogFormatter *formatter, const char *filename, size_t maxSize, int backupCount);
    void send(const char *msg);
    void sendRaw(const char *msg);
};

#endif

#endif
