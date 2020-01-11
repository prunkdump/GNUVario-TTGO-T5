
#ifndef _MPP_CONSOLE_H_
#define _MPP_CONSOLE_H_

#include <Arduino.h>

#ifndef MPP_MAX_BUF_LEN
#define MPP_MAX_BUF_LEN     80
#endif

class Stream;

extern Stream *console_output;

void console_vprintf(const char *format, va_list ap);
void console_init(Stream *serial);
void console_printf(const char *format, ...);
void console_putc(char c);
void console_write(const char *s);
void console_puts(const char *s);

#if defined ESP8266 || defined ESP32
#define printf console_printf
#define puts console_puts
#define putchar console_putc
#endif

#endif

