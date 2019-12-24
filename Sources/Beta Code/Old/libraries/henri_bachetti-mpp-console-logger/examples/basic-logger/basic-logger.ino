
/*
 * basic logging demo
 * - console and logs output to Serial
 * - logging level is: DEBUG (change to INFO, WARNING, ERROR, CRITICAL to see the effect)
 * - the sketch can run on ARDUINO, ESP8266 and ESP32
 */

#include "mpp-console.h"
#include "mpp-logger.h"
#include "debug.h"

#define BUF_LEN     128

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
  uint8_t buf[BUF_LEN];
  for (int i = 0 ; i < BUF_LEN ; i++) {
    buf[i] = i;
  }
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
  log_dump("buf", buf, BUF_LEN);
  delay(2000);
}

