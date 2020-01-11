
#include <mpp-console.h>
#include <mpp-logger.h>
#include <mpp-sd_log_handler.h>

extern Logger LOGGER;
extern Logger ACCESS;

#define LOGS_ACTIVE

#ifndef LOGS_ACTIVE
#define log_setLevel
#define log_debug
#define log_info
#define log_warning
#define log_error
#define log_critical
#else
#define log_setLevel    LOGGER.setLevel
#define log_debug       LOGGER.debug
#define log_info        LOGGER.info
#define log_warning     LOGGER.warning
#define log_error       LOGGER.error
#define log_critical    LOGGER.critical
#define log_dump        DEBUG_LOG.hexDump
#endif

