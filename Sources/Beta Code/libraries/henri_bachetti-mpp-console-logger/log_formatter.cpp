
#include <Arduino.h>
#include <Time.h>
#include <time.h>

#include "mpp-logger.h"
//#define LOG_DEBUG
#include "mpp_local.h"

LogFormatter::LogFormatter(Logger *logger, const char *name) :
default_handler(this, 0)
{
  my_logger = logger;
  my_name = name;
  log_handler = &default_handler;
}

void LogFormatter::setHandler(LogHandler *handler)
{
  debug_print("%s: setHandler %s\n", handler->getName());
  log_handler = handler;
}

void LogFormatter::logRaw(const char *format, va_list ap)
{
  debug_print("%s: log to (%x)\n", my_name, log_handler);
  if (log_handler) {
    char buf[MPP_MAX_BUF_LEN];
    vsnprintf(buf, MPP_MAX_BUF_LEN, format, ap);
    debug_print("%s: log to %s\n", my_name, log_handler->getName());
    log_handler->sendRaw(buf);
  }
}

BaseFormatter::BaseFormatter(Logger *logger) :
LogFormatter(logger, "BaseFormatter")
{
}

void BaseFormatter::log(const char *level, const char *format, va_list ap)
{
  debug_print("%s: log to (%x)\n", my_name, log_handler);
  if (log_handler) {
    char buf[MPP_MAX_BUF_LEN];
    int len = snprintf(buf, MPP_MAX_BUF_LEN, "%s - ", level ? level : "");
    vsnprintf(buf+len, MPP_MAX_BUF_LEN-len, format, ap);
    debug_print("%s: log to %s\n", my_name, log_handler->getName());
    log_handler->send(buf);
  }
}

LogNameFormatter::LogNameFormatter(Logger *logger) :
LogFormatter(logger, "LogNameFormatter")
{
}

void LogNameFormatter::log(const char *level, const char *format, va_list ap)
{
  if (log_handler) {
    char buf[MPP_MAX_BUF_LEN];
    int len = snprintf(buf, MPP_MAX_BUF_LEN, "%s - %s - ", my_logger->getName(), level ? level : "");
    vsnprintf(buf+len, MPP_MAX_BUF_LEN-len, format, ap);
    debug_print("%s: log to %s (%x)\n", my_name, log_handler->getName(), log_handler);
    log_handler->send(buf);
  }
}

LogTimestampFormatter::LogTimestampFormatter(Logger *logger) :
LogFormatter(logger, "LogTimestampFormatter")
{
}

void LogTimestampFormatter::log(const char *level, const char *format, va_list ap)
{
  debug_print("%s: log to (%x)\n", my_name, log_handler);
  if (log_handler) {
    char buf[MPP_MAX_BUF_LEN];
    int len = snprintf(buf, MPP_MAX_BUF_LEN, "%lu - %s - %s - ", millis(), my_logger->getName(), level ? level : "");
    vsnprintf(buf+len, MPP_MAX_BUF_LEN-len, format, ap);
    debug_print("%s: log to %s\n", my_name, log_handler->getName());
    log_handler->send(buf);
  }
}

LogTimeDateFormatter::LogTimeDateFormatter(Logger *logger) :
LogFormatter(logger, "LogTimeDateFormatter")
{
}

void LogTimeDateFormatter::log(const char *level, const char *format, va_list ap)
{
  debug_print("%s: log to (%x)\n", my_name, log_handler);
  if (log_handler) {
    char buf[MPP_MAX_BUF_LEN];
    time_t t;
    char *p;

    t = millis()*1000;   //now();
    char *dt = ctime(&t);
    p = dt + strlen(dt)-1;
    if (*p == '\n') {
      *p = '\0';
    }
    int len = snprintf(buf, MPP_MAX_BUF_LEN, "%s - %s - %s - ", dt, my_logger->getName(), level ? level : "");
    vsnprintf(buf+len, MPP_MAX_BUF_LEN-len, format, ap);
    debug_print("%s: log to %s\n", my_name, log_handler->getName());
    log_handler->send(buf);
  }
}
