
#include <stdarg.h>

#include "mpp-console.h"

Stream *console_output;

void console_putc(char c)
{
  if (console_output) {
    console_output->print(c);
  }
}

void console_write(const char *s)
{
  if (console_output) {
    console_output->print(s);
  }
}

void console_puts(const char *s)
{
  if (console_output) {
    console_output->println(s);
  }
}

void console_printf(const char *format, ...)
{
  if (console_output) {
    va_list ap;
    va_start (ap, format);
    console_vprintf(format, ap);
    va_end (ap);
  }
}

