
#ifndef _MPP_SDHAL_FILE_HANDLER_H_
#define _MPP_SDHAL_FILE_HANDLER_H_

#include "mpp-logger.h"

#include <sdcardHAL.h>

class SdFatFileLogHandler : public LogHandler {
protected:
		char *log_name;
    size_t log_size;
    int log_backup;
    SdCardHAL *log_sd;
#if defined ESP8266 || defined ESP32
    File openLog(void);
#else
    SdFile openLog(void);
#endif
    int logRotate(void);

  public:
    SdFatFileLogHandler(LogFormatter *formatter, SdCardHAL *sd, char *filename, size_t maxSize, int backupCount);
    void send(const char *msg);
    void sendRaw(const char *msg);
};

#endif
