
/*
   basic custom logging handler demo
   - console and logs output done by puts
   - logging level is: DEBUG (change to INFO, WARNING, ERROR, CRITICAL to see the effect)
   - the sketch can run on ARDUINO, ESP8266 and ESP32
*/

#include "mpp-console.h"
#include "mpp-logger.h"
#include "debug.h"

class MyLogHandler : public LogHandler {
  public:
    MyLogHandler(LogFormatter *formatter);
    void send(const char *msg);
};

MyLogHandler::MyLogHandler(LogFormatter *formatter) :
  LogHandler(formatter, "MyLogHandler")
{
}

void MyLogHandler::send(const char *msg)
{
  printf("MyLogHandler: %s\n", msg);
}

static Logger LOGGER("test");
LogNameFormatter formatter(&LOGGER);
MyLogHandler handler(&formatter);

void setup()
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("Basic logging handler demo\n");
  LOGGER.init(NULL);
  LOGGER.setLevel(DEBUG);
  formatter.setHandler(&handler);
  LOGGER.setFormatter(&formatter);
}

void loop()
{
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
  delay(2000);
}
