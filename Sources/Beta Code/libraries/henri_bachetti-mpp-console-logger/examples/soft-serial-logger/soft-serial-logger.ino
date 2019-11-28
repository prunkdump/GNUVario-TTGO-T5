
/*
 * Dual hardware & software serial logging demo
 * - console outputs to Serial
 * - logs output to SoftwareSerial
 * - logging level is changed during execution (see test.cpp)
 * - the sketch can run on ARDUINO, ESP8266 and ESP32
 * - SoftwareSerial is not very reliable on ESP32.
 */

#include <SoftwareSerial.h>

#include "mpp-console.h"
#include "mpp-logger.h"
#include "debug.h"
#include "test.h"

#ifdef ESP32
#define LED_BUILTIN     2
#endif

Logger LOGGER("test");
#if defined ESP8266 || defined ESP32
// 14 & 12 are D5 D6 on NodeMCU
SoftwareSerial swSerial(14, 12, false, 256);
#else
SoftwareSerial swSerial(10, 11);
#endif

void setup()
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("Dual hardware & software serial logging demo\n");
  swSerial.begin(57600);
  LOGGER.init(&swSerial);
  pinMode(LED_BUILTIN, OUTPUT);
  LOGGER.setLevel(DEBUG);
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  log_info("##### Calling test func\n");
  test_func();
}

