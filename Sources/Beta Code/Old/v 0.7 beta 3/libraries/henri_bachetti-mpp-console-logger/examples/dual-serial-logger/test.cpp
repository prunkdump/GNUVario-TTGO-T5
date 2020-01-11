
#include <Arduino.h>

#include "mpp-console.h"
#include "mpp-logger.h"
#include "debug.h"
#include "test.h"

void test_func(void)
{
  log_printf("***** setting level to ERROR");
  log_setLevel(ERROR);
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
  delay(1000);

  log_printf("***** setting level to INFO");
  log_setLevel(INFO);
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
  delay(1000);

  log_printf("***** setting level to DEBUG");
  log_setLevel(DEBUG);
  log_debug("This is a %s message", "debug");
  log_info("This is an %s message", "info");
  log_warning("This is a %s message", "warning");
  log_error("This is an %s message", "error");
  log_critical("This is a %s message", "critical");
  delay(1000);
}

