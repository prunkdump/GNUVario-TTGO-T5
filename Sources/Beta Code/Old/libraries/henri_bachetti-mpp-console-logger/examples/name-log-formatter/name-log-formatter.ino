
/*
 * log name formatter logging demo
 * - console and logs output to Serial
 * - the logger name is printed at the begining of each message
 * - logging level is: DEBUG (change to INFO, WARNING, ERROR, CRITICAL to see the effect)
 */

#include "mpp-console.h"
#include "mpp-logger.h"
#include "debug.h"

static Logger LOGGER("test");
LogNameFormatter formatter(&LOGGER);

void setup()
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("Log name formatter logging demo\n");
  LOGGER.init(&Serial);
  LOGGER.setLevel(DEBUG);
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

