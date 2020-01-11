
/*
 * time & date formatter logging demo
 * - console and logs output to Serial
 * - a timestamp in and the logger name are printed at the begining of each message
 * - logging level is: DEBUG (change to INFO, WARNING, ERROR, CRITICAL to see the effect)
 * - the sketch can run on ARDUINO, ESP8266 and ESP32
 */

#include <Time.h>

#include "mpp-console.h"
#include "mpp-logger.h"
#include "debug.h"

Logger LOGGER("test");
LogTimeDateFormatter formatter(&LOGGER);

void setup()
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("Time & date formatter logging demo\n");
  LOGGER.init(&Serial);
  LOGGER.setLevel(DEBUG);
  LOGGER.setFormatter(&formatter);
  setTime(1552583988);
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
