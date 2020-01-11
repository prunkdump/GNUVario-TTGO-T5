# MPP-CONSOLE-LOGGER

Multipurpose Console Logger is a versatile library for ARDUINO, ESP8266, ESP32 and STM32.
composed of two modules :

 * a console module
 * a logger module

To be able to use this library with ESP8266, ESP32 and STM32 you must install the following board packages :

 * http://arduino.esp8266.com/versions/2.5.0/package_esp8266com_index.json
 * https://dl.espressif.com/dl/package_esp32_index.json
 * https://raw.githubusercontent.com/stm32duino/BoardManagerFiles/master/STM32/package_stm_index.json

See ARDUINO IDE / preferences.

## Console module

This module provides some console functions :

void console_init(Stream *serial);
void console_vprintf(const char *format, va_list ap);
void console_printf(const char *format, ...);
void console_putc(char c);
void console_write(const char *s);
void console_puts(const char *s);

The second role of this module is to activate the standard output so that the standard functions putchar, puts, printf, etc. can be used.

## Logger Module

This module defines classes which implement a flexible event logging system for applications and libraries.

Loggers expose the interface that application code directly uses.
Handlers send the log records (created by loggers) to the appropriate destination.
Formatters specify the layout of log records in the final output.

Logger : the main class.

LogHandler : an abstract handler class.

StreamLogHandler : a serial line handler class.

SdFatFileLogHandler : an SDFAT rotating log files generation class. (not on ESP8266, as SD filesystem does not support renaming)

SpiffsFileLogHandler : an SPIFFS rotating log files generation class.

LogFormatter : an abstract formatter class.

BaseFormatter : a basic formatter class.

LogNameFormatter : a formatter class to add the logger name to your messages.

LogTimestampFormatter : a formatter class to add a timestamp and the logger name to your messages.

LogTimeDateFormatter : a formatter class to add the date, time and the logger name to your messages.

## Logger class

**Logger::Logger(const char *name);**

Instanciates a logger for the module *name*.

**void Logger::init(Stream *serial);**

Initializes a logger for a *serial* line. *serial* is optionnal.

**const char *Logger::getName(void);**

Gets the name of the logger. It can be included in the messages by a formatter.

**void Logger::setLevel(int level);**

Sets the threshold for this logger to level. Logging messages which are less severe than level will be ignored. When a logger is created, the level is set to DEBUG (which causes all messages to be processed).

**void Logger::setFormatter(LogFormatter *formatter)**

Sets the Formatter for this logegr to *formatter*.

**void Logger::setFormatter(LogFormatter *formatter)**

Tell the logger to use the specified *formatter*.

**void Logger::send(const char *msg)**

Sends the message *msg*. This method is used by the formatter.

**void Logger::log(const char *format, ...)**
**void Logger::log(const __FlashStringHelper *format, ...)**

Logs a message with no level on this logger. The arguments are interpreted like the printf function.

**void Logger::debug(const char *format, ...)**
**void Logger::debug(const __FlashStringHelper *format, ...)**

Logs a message with DEBUG level on this logger.

**void Logger::info(const char *format, ...)**
**void Logger::info(const __FlashStringHelper *format, ...)**

Logs a message with INFO level on this logger.

**void Logger::warning(const char *format, ...)**
**void Logger::warning(const __FlashStringHelper *format, ...)**

Logs a message with WARNING level on this logger.

**void Logger::error(const char *format, ...)**
**void Logger::error(const __FlashStringHelper *format, ...)**

Logs a message with ERROR level on this logger.

**void Logger::critical(const char *format, ...)**
**void Logger::critical(const __FlashStringHelper *format, ...)**

Logs a message with CRITICAL level on this logger.

**void Logger::hexDump(const char *format, ...)**

Logs a binary dump with level DEBUG on this logger :
```
DEBUG - buf
  0000  00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f  ................
  0010  10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f  ................
  0020  20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f   !"#$&'()*+,-./
  0030  30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f  0123456789:;<=>?
  0040  40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f  @ABCDEFGHIJKLMNO
  0050  50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f  PQRSTUVWXYZ[\]^_
  0060  60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f  `abcdefghijklmno
  0070  70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f  pqrstuvwxyz{|}~.
```

## LogHandler virtual class

**LogHandler::LogHandler(LogFormatter *formatter, const char *name)**

Initializes the LogHandler instance with a *name*. *name* is set by the derived classes.

**const char *LogHandler::getName(void);**

Gets the name of the handler (its class name). This is usefull for debug.

**virtual void LogHandler::send(const char *msg)**

Sends the message *msg*. This method is virtual.

## StreamLogHandler class

**StreamLogHandler::StreamLogHandler(LogFormatter *formatter, Stream *serial);**

Initializes the StreamLogHandler instance for a given *serial* line.

**void StreamLogHandler::setSerial(Stream *serial);**

Changes the *serial* line for this instance.

**void StreamLogHandler::send(const char *msg)**

Sends the message *msg*. This method is used by the logger.

## SdFatFileLogHandler class

**SdFatFileLogHandler::SdFatFileLogHandler(LogFormatter *formatter, SDClass *sd, const char *filename, size_t maxSize, int backupCount);**

Initializes the SdFatFileLogHandler instance for a given file named *filename* on a filesystem named *sd*.
You can use the maxSize and backupCount values to allow the file to rollover at a predetermined size. When the size is about to be exceeded, the file is closed and a new file is silently opened for output.

**void SdFatFileLogHandler::send(const char *msg)**

Sends the message *msg*. This method is used by the logger.

## SpiffsFileLogHandler class

**SpiffsFileLogHandler::SpiffsFileLogHandler(LogFormatter *formatter, const char *filename, size_t maxSize, int backupCount);**

Initializes the SpiffsFileLogHandler instance for a given file named *filename* on a filesystem named *sd*.
You can use the maxSize and backupCount values to allow the file to rollover at a predetermined size. When the size is about to be exceeded, the file is closed and a new file is silently opened for output.

**void SpiffsFileLogHandler::send(const char *msg)**

Sends the message *msg*. This method is used by the logger.

## LogFormatter virtual class

**LogFormatter::LogFormatter(Logger *logger, const char *name)**

Initializes the LogFormatter instance with a *name*. *name* is set by the derived classes.

**const char *LogFormatter::getName(void);**

Gets the name of the formatter (its class name). This is usefull for debug.

**void LogFormatter::setHandler(LogHandler *handler)**

Tells the formatter to use the specified *handler*.

## BaseLogFormatter class

The BaseLogFormatter adds only the level name at the beginning of the message.

BaseFormatter::BaseFormatter(Logger *logger) :

Initializes the BaseFormatter instance.

## LogNameLogFormatter class

The BaseLogFormatter adds the logger name and the level name at the beginning of the message.

LogNameFormatter::LogNameFormatter(Logger *logger) :

Initializes the LogNameFormatter instance.

## LogTimestampFormatter class

The LogTimestampFormatter adds the logger name, the level name and a timestamp in milliseconds at the beginning of the message.

LogTimestampFormatter::LogTimestampFormatter(Logger *logger) :

Initializes the LogTimestampFormatter instance.

## LogTimeDateFormatter class

The LogTimeDateFormatter adds the logger name, the level name and the date and time at the beginning of the message.

LogTimeDateFormatter::LogTimeDateFormatter(Logger *logger) :

Initializes the LogTimeDateFormatter instance.

### INSTALLATION

Just unzip the archive in the folder named "libraries" in your Arduino folder.

### EXAMPLES

The library provides some examples :

|Example                    |Demo                                       |
|---------------------------|-------------------------------------------|
|basic-logger               |A basic serial logger                      |
|basic-logger-handler       |A basic serial logger with custom handler  |
|console                    |A basic console demo                       |
|dual-serial-logger         |A dual hardware serial logger              |
|ESP32-sdfat                |An SDFAT logger                            |
|ESP32-sdfat-http-server    |A WebServer with SDFAT logging             |
|ESP-spiffs                 |An ESP32 and ESP8266 SPIFFS logger         |
|ESP32-spiffs-http-server   |A WebServer with SPIFFS logging            |
|name-log-formatter         |A name formatter demo                      |
|soft-serial-logger         |A dual hardware and software serial logger |
|timedate-log-formatter     |A time&date formatter demo                 |
|timestamp-log-formatter    |A timestamp formatter demo                 |

The following examples are usable on the UNO, NANO or MINI platforms :

 * console
 * basic-logger
 * basic-logger-handler
 * name-log-formatter
 * timedate-log-formatter
 * timestamp-log-formatter
 * soft-serial-logger

The following examples are usable on the STM32 NUCLEO platforms :

 * console
 * basic-logger
 * basic-logger-handler
 * name-log-formatter
 * timedate-log-formatter
 * timestamp-log-formatter
 * dual-serial-logger

basic-logger :
The sketch uses 4478 bytes (13%) in FLASH meory and 418 bytes (20%) in RAM.

soft-serial-logger :
The sketch uses 6846 bytes (21%) in FLASH meory and 659 bytes (32%) in RAM.

It is possible to eliminate the logging code in the application with some macros :

```
#include "mpp-console.h"
#include "mpp-logger.h"
#include "debug.h"

#define LOGS_ACTIVE

#ifndef LOGS_ACTIVE
#define log_setLevel
#define log_debug
#define log_info
#define log_warning
#define log_error
#define log_critical
#define log_dump
#else
#define log_setLevel    LOGGER.setLevel
#define log_printf      LOGGER.log
#define log_debug       LOGGER.debug
#define log_info        LOGGER.info
#define log_warning     LOGGER.warning
#define log_error       LOGGER.error
#define log_critical    LOGGER.critical
#define log_dump        LOGGER.hexDump
#endif

static Logger LOGGER("test");

void setup()
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("Basic logging demo\n");
  LOGGER.init(&Serial);
  LOGGER.setLevel(DEBUG);
}

void loop()
{
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
  log_dump("buf", buf, BUF_LEN);
  delay(2000);
}
```
In this code if you comment the LOGS_ACTIVE directive, the logging code will not be compiled anymore :
```
//#define LOGS_ACTIVE
```

### TODO

Jobs to be done.

|Example                    |Demo                                                 |
|---------------------------|-----------------------------------------------------|
|MEGA-sdfat                 |An SDFAT logger on MEGA2560                          |
|MEGA-sdfat-http-server     |An Ethernet WebServer with SDFAT logging on MEGA2560 |


### BLOG
A description in french here : https://riton-duino.blogspot.com/2019/03/arduino-esp8266-et-esp32-une-librairie.html

