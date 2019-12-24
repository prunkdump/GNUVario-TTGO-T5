
/*
 * dual hardware serial logging demo
 * - console outputs to Serial
 * - logs output to Serial2
 * - logging level is changed during execution (see test.cpp)
 * - the sketch can run on ARDUINO MEGA (Serial2), ESP8266 (Serial1) and ESP32 (Serial2)
 */

#include "mpp-console.h"
#include "mpp-logger.h"
#include "debug.h"
#include "test.h"

#ifdef ESP32
#define LED_BUILTIN     2
#endif

#ifdef STM32F401xE
HardwareSerial Serial1(USART1);
#endif

Logger LOGGER("test");

void setup()
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("Dual hardware serial logging demo\n");
#ifdef ESP8266
  Serial1.begin(115200);
  LOGGER.init(&Serial1);
#elif defined STM32F401xE
  Serial1.begin(115200);
  LOGGER.init(&Serial1);
#else
  Serial2.begin(115200);
  LOGGER.init(&Serial2);
#endif
  pinMode(LED_BUILTIN, OUTPUT);
  LOGGER.setLevel(DEBUG);

#if defined ESP32
#include <esp_log.h>
  // Tools / Core Debug Level have to be selected
  static const char *TAG = "example";
  esp_log_level_set(TAG, ESP_LOG_VERBOSE);
  ESP_LOGI(TAG, "output from %s\n", "ESP32_LOG");
#endif
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

