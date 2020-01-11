
#include <stdarg.h>

#include "mpp-logger.h"
//#define LOG_DEBUG
#include "mpp_local.h"

Logger::Logger(const char *name) :
default_formatter(this)
{
  log_name = name;
  log_level = DEBUG;
  log_formatter = &default_formatter;
  log_serial = 0;
}

void Logger::init(Stream *serial)
{
  log_serial = serial;
  debug_print("Logger::init log_formatter %x\n", log_formatter);
  checkFormatter();
}

void Logger::checkFormatter(void)
{
  LogHandler *handler = log_formatter->getHandler();
  const char *name = handler->getName();
  if (!strcmp(name, "StreamLogHandler")) {
    Stream *serial = ((StreamLogHandler *)handler)->getSerial();
    if (!serial) {
      debug_print("Logger: set serial for %s (%x)\n", name, handler);
      ((StreamLogHandler *)handler)->setSerial(log_serial);
    }
  }
}

void Logger::setFormatter(LogFormatter *formatter)
{
  log_formatter = formatter;
  checkFormatter();
}

void Logger::logRaw(const char *format, ...)
{
  if (log_formatter) {
    debug_print("%s: log to %s (%x)\n", log_name, log_formatter->getName(), log_formatter);
    va_list ap;
    va_start (ap, format);
    log_formatter->logRaw(format, ap);
    va_end (ap);
  }
}

void Logger::vlog(const char *level, const char *format, va_list ap)
{
  if (log_formatter) {
    debug_print("%s: log to %s (%x)\n", log_name, log_formatter->getName(), log_formatter);
    log_formatter->log(level, format, ap);
  }
}

void Logger::vlog(const char *level, const __FlashStringHelper *format, va_list ap)
{
  if (log_formatter) {
    char fmt[MPP_MAX_BUF_LEN];
    size_t len = 63;

    PGM_P p = reinterpret_cast<PGM_P>(format);
    size_t n = 0;
    while (1) {
      unsigned char c = pgm_read_byte(p++);
      fmt[n++] = c;
      if (c == 0) break;
    }
    debug_print("%s: vlog to %s\n", log_name, log_formatter->getName());
    log_formatter->log(level, fmt, ap);
  }
}

void Logger::log(const char *format, ...)
{
  va_list ap;
  va_start (ap, format);
  vlog("NONE", format, ap);
  va_end (ap);
}

void Logger::log(const __FlashStringHelper *format, ...)
{
  va_list ap;
  va_start (ap, format);
  vlog("NONE", format, ap);
  va_end (ap);
}

void Logger::debug(const char *format, ...)
{
  if (log_level >= DEBUG) {
    va_list ap;
    va_start (ap, format);
    vlog("DEBUG", format, ap);
    va_end (ap);
  }
}

void Logger::debug(const __FlashStringHelper *format, ...)
{
  if (log_level >= DEBUG) {
    va_list ap;
    va_start (ap, format);
    vlog("DEBUG", format, ap);
    va_end (ap);
  }
}

void Logger::info(const char *format, ...)
{
  if (log_level >= INFO) {
    va_list ap;
    va_start (ap, format);
    vlog("INFO", format, ap);
    va_end (ap);
  }
}

void Logger::info(const __FlashStringHelper *format, ...)
{
  if (log_level >= INFO) {
    va_list ap;
    va_start (ap, format);
    vlog("INFO", format, ap);
    va_end (ap);
  }
}

void Logger::warning(const char *format, ...)
{
  if (log_level >= WARNING) {
    va_list ap;
    va_start (ap, format);
    vlog("WARNING", format, ap);
    va_end (ap);
  }
}

void Logger::warning(const __FlashStringHelper *format, ...)
{
  if (log_level >= WARNING) {
    va_list ap;
    va_start (ap, format);
    vlog("WARNING", format, ap);
    va_end (ap);
  }
}

void Logger::error(const char *format, ...)
{
  if (log_level >= ERROR) {
    va_list ap;
    va_start (ap, format);
    vlog("ERROR", format, ap);
    va_end (ap);
  }
}

void Logger::error(const __FlashStringHelper *format, ...)
{
  if (log_level >= ERROR) {
    va_list ap;
    va_start (ap, format);
    vlog("ERROR", format, ap);
    va_end (ap);
  }
}

void Logger::critical(const __FlashStringHelper *format, ...)
{
  if (log_level >= CRITICAL) {
    va_list ap;
    va_start (ap, format);
    vlog("CRITICAL", format, ap);
    va_end (ap);
  }
}

void Logger::critical(const char *format, ...)
{
  if (log_level >= CRITICAL) {
    va_list ap;
    va_start (ap, format);
    vlog("CRITICAL", format, ap);
    va_end (ap);
  }
}

void Logger::hexDump(const char *message, const uint8_t *addr, int len)
{
  int i;
  unsigned char buff[17];

  if (message != NULL) {
    logRaw("DEBUG - %s\n", message);
  }
  for (i = 0 ; i < len ; i++) {
    if ((i % 16) == 0) {
      if (i != 0)
        logRaw("  %s\n", buff);
      logRaw("  %04x ", i);
    }
    logRaw(" %02x", addr[i]);
    if ((addr[i] < 0x20) || (addr[i] > 0x7e)) {
      buff[i % 16] = '.';
    } else {
      buff[i % 16] = addr[i];
    }
    buff[(i % 16) + 1] = '\0';
  }
  while ((i % 16) != 0) {
      logRaw("   ");
      i++;
  }
  logRaw("  %s\n", buff);
}
