
#ifndef _MPP_SD_FILE_HANDLER_H_
#define _MPP_SD_FILE_HANDLER_H_

#include "mpp-logger.h"

#if defined ESP8266
#include "SD.h"
#elif defined ESP32
#include <FS.h>
#include <SD.h>
#else
#include <SdFat.h>
#endif

class SdFatFileLogHandler : public LogHandler {
protected:
    const char *log_name;
    size_t log_size;
    int log_backup;
#if defined ESP8266
    SDClass *log_sd;
#elif defined ESP32
    SDFS *log_sd;
#else
    SdFat *log_sd;
#endif
#if defined ESP8266 || defined ESP32
    File openLog(void);
#else
    SdFile openLog(void);
#endif
    int logRotate(void);

  public:
#if defined ESP8266
    SdFatFileLogHandler(LogFormatter *formatter, SDClass *sd, const char *filename, size_t maxSize, int backupCount);
#elif defined ESP32
    SdFatFileLogHandler(LogFormatter *formatter, SDFS *sd, const char *filename, size_t maxSize, int backupCount);
#else
    SdFatFileLogHandler(LogFormatter *formatter, SdFat *sd, const char *filename, size_t maxSize, int backupCount);
#endif
    void send(const char *msg);
    void sendRaw(const char *msg);
};

#endif
