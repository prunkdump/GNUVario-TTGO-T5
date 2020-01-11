/* mpp-sdhal_file_log_handler -- 
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * This file is part of GnuVario-E.
 *
 * ToneHAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ToneHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*********************************************************************************/
/*                                                                               */
/*             mpp-sdhal_log_handler                                             */
/*                                                                               */
/*  version    Date        Description                                           */
/*    1.0      25/11/19                                                          */
/*    1.1      29/11/19    Modif changement librairie sdfat                      */
/*                                                                               */
/*********************************************************************************/

#ifndef _MPP_SDHAL_FILE_HANDLER_H_
#define _MPP_SDHAL_FILE_HANDLER_H_

#include "mpp-logger.h"

#include <sdcardHAL.h>

#include <HardwareConfig.h>

class SdFatFileLogHandler : public LogHandler {
protected:
		char *log_name;
    size_t log_size;
    int log_backup;
#ifdef SDFAT_LIB
		SdFile *log_sd;
#else
    SdCardHAL *log_sd;
#endif //SDFAT_LIB
		
#if defined ESP8266 || defined ESP32
#ifdef SDFAT_LIB
    SdFile openLog(void);
#else
    File openLog(void);
#endif //SDFAT_LIB
#else
    SdFile openLog(void);
#endif
    int logRotate(void);

  public:
#ifdef SDFAT_LIB
    SdFatFileLogHandler(LogFormatter *formatter, char *filename, size_t maxSize, int backupCount);
#else
    SdFatFileLogHandler(LogFormatter *formatter, SdCardHAL *sd, char *filename, size_t maxSize, int backupCount);
#endif //SDFAT_LIB
    void send(const char *msg);
    void sendRaw(const char *msg);
};

#endif
