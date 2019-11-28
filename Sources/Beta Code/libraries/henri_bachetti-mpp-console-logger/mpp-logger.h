
#ifndef _MPP_LOGGER_H_
#define _MPP_LOGGER_H_

#include <Arduino.h>
#include "mpp-console.h"

#define DEBUG           5
#define INFO            4
#define WARNING         3
#define ERROR           2
#define CRITICAL        1

#ifndef MPP_MAX_BUF_LEN
#define MPP_MAX_BUF_LEN     80
#endif

class Stream;
class Logger;
class LogFormatter;

class LogHandler {
  protected:
    LogFormatter *my_formatter;
    const char *my_name;

  public:
    LogHandler(LogFormatter *formatter, const char *name);
    const char *getName(void) {return my_name;}
    virtual void send(const char *msg) {}
    virtual void sendRaw(const char *msg) {}
};

class StreamLogHandler : public LogHandler {
  protected:
    Stream *log_serial;

  public:
    StreamLogHandler(LogFormatter *formatter, Stream *serial);
    void setSerial(Stream *serial);
    Stream *getSerial(void)                     {return log_serial;};
    void send(const char *msg);
    void sendRaw(const char *msg);
};

class LogFormatter {
  protected:
    Logger *my_logger;
    const char *my_name;
    LogHandler *log_handler;
    StreamLogHandler default_handler;

  public:
    LogFormatter(Logger *logger, const char *name);
    const char *getName(void) {return my_name;}
    void setHandler(LogHandler *handler);
    LogHandler *getHandler(void)                {return log_handler;}
    void logRaw(const char *format, va_list ap);
    virtual void log(const char *level, const char *format, va_list ap) {};
};

class BaseFormatter : public LogFormatter {
  public:
    BaseFormatter(Logger *logger);
    void log(const char *level, const char *format, va_list ap);
};

class LogNameFormatter : public LogFormatter {
  public:
    LogNameFormatter(Logger *logger);
    void log(const char *level, const char *format, va_list ap);
};

class LogTimestampFormatter : public LogFormatter {
  public:
    LogTimestampFormatter(Logger *logger);
    void log(const char *level, const char *format, va_list ap);
};

class LogTimeDateFormatter : public LogFormatter {
  public:
    LogTimeDateFormatter(Logger *logger);
    void log(const char *level, const char *format, va_list ap);
};

class Logger {
  private:
    const char *log_name;
    uint32_t log_level;
    Stream *log_serial;
    BaseFormatter default_formatter;
    LogFormatter *log_formatter;
    void checkFormatter(void);
    void logRaw(const char *format, ...);
    void vlog(const char *level, const char *format, va_list ap);
    void vlog(const char *level, const __FlashStringHelper *format, va_list ap);

  public:
    Logger(const char *name);
    void init(Stream *serial);
    const char *getName(void)                    {return log_name;}
    void setLevel(int level)                     {log_level = level;}
    void setFormatter(LogFormatter *formatter);
    void log(const char *format, ...);
    void log(const __FlashStringHelper *format, ...);
    void debug(const char *format, ...);
    void debug(const __FlashStringHelper *format, ...);
    void info(const char *format, ...);
    void info(const __FlashStringHelper *format, ...);
    void warning(const char *format, ...);
    void warning(const __FlashStringHelper *format, ...);
    void error(const char *format, ...);
    void error(const __FlashStringHelper *format, ...);
    void critical(const char *format, ...);
    void critical(const __FlashStringHelper *format, ...);
    void hexDump(const char *message, const uint8_t *addr, int len);
};

void logger_out(Stream *serial, const char *msg);
void logger_outRaw(Stream *serial, const char *msg);

#endif

