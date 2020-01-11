
/*
 * basic console display demo
 * - console outputs to Serial
 * - various console functions are used
 * - the sketch can run on ARDUINO, ESP8266 and ESP32
 */

#include "mpp-console.h"

#ifdef ESP32
#define LED_BUILTIN     2
#endif

void setup()
{
  Serial.begin(115200);
  console_init(&Serial);
  printf("Basic console display demo\n");
  pinMode(LED_BUILTIN, OUTPUT);
  console_write("output from console_write\r\n");
  console_puts("output from console_puts");
  printf("output from printf\n");
  puts("output from puts");
  const char *s = "output from putchar\n";
  for (const char *p = s ; *p ; p++) {
    putchar(*p);
  }
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
}

