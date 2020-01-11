
#include <Arduino.h>
#include <stdarg.h>

#include "mpp-logger.h"

void logger_out(Stream *serial, const char *msg)
{
  for (const char *p = msg ; *p != 0 ; p++) {
    if (*p == '\n') serial->write('\r');
    serial->write(*p);
  }
  serial->write("\r\n");
}

void logger_outRaw(Stream *serial, const char *msg)
{
  for (const char *p = msg ; *p != 0 ; p++) {
    if (*p == '\n') serial->write('\r');
    serial->write(*p);
  }
}
