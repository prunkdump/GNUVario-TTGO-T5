
#include <Arduino.h>
#include <stdarg.h>

#include "mpp-console.h"

#ifdef AVR

static FILE uartout = {0} ;

static int console_putchar (char c, FILE *stream)
{
  if (c == '\n') console_output->write('\r');
  console_output->write(c);
  return 0;
}

#endif

#if defined ESP8266 || defined ESP32

static int console_putchar(int c)
{
  if (c == '\n') console_output->write('\r');
  console_output->write(c);
  return 0;
}

#endif

void console_vprintf(const char *format, va_list ap)
{
#if defined ESP8266
  ets_vprintf(console_putchar, format, ap);
#elif defined ESP32
  char loc_buf[MPP_MAX_BUF_LEN];
  char *temp = loc_buf;
  size_t len = vsnprintf(NULL, 0, format, ap);
  if (len >= sizeof(loc_buf)) {
    temp = new char[len+1];
    if (temp == NULL) {
      return;
    }
  }
  len = vsnprintf(temp, len+1, format, ap);
  for (const char *p = temp ; *p != 0 ; p++) {
    console_putchar(*p);
  }
  if (len >= sizeof(loc_buf)) {
      delete[] temp;
  }
#else
  vprintf(format, ap);
#endif
}

void console_init(Stream *serial)
{
#ifdef AVR
  fdev_setup_stream (&uartout, console_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &uartout;
#endif
#if defined ESP8266 || defined ESP32
  if (serial == &Serial) {
    Serial.setDebugOutput(true);
  }
#endif
#if defined ESP8266
  else if (serial == &Serial1) {
    Serial1.setDebugOutput(true);
  }
#endif
#if defined ESP32
  else if (serial == &Serial2) {
    Serial2.setDebugOutput(true);
  }
#endif
  console_output = serial;
}

