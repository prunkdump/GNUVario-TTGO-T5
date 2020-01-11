
#include <Arduino.h>
#include "mpp-logger.h"
//#define LOG_DEBUG
#include "mpp_local.h"

LogHandler::LogHandler(LogFormatter *formatter, const char *name)
{
  my_formatter = formatter;
  my_name = name;
}

StreamLogHandler::StreamLogHandler(LogFormatter *formatter, Stream *serial) :
LogHandler(formatter, "StreamLogHandler")
{
  log_serial = serial;
}

void StreamLogHandler::setSerial(Stream *serial)
{
  log_serial = serial;
  debug_print("%s@%x::setSerial log_serial=%x\n", my_name, this, log_serial);
}

void StreamLogHandler::send(const char *msg)
{
  debug_print("%s@%x::send log_serial=%x\n", my_name, this, log_serial);
  if (log_serial) {
    logger_out(log_serial, msg);
  }
}

void StreamLogHandler::sendRaw(const char *msg)
{
  debug_print("%s@%x::send log_serial=%x\n", my_name, this, log_serial);
  if (log_serial) {
    logger_outRaw(log_serial, msg);
  }
}
