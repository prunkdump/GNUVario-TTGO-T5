/* VarioWifi 
 *
 * Copyright 2020 Pascal MICHEL
 * 
 * This file is part of GNUVario.
 *
 * GNUVario is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNUVario is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef VARIO_WIFI_H
#define VARIO_WIFI_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <HardwareConfig.h>
#include <sdcardHAL.h>
#include <VarioSettings.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <esp32fota2.h>
#include <VarioIgcParser.h>
#include <VarioSqlFlight.h>
#include <ESP32-targz.h>

#ifdef WIFI_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

class VarioWifi

{
public:
    bool begin();

private:
    bool connectToWifi();
    bool isIndexFileExist();
    void setClock(void);
    bool checkDbVersion();
    void setMDNS();
    void startWebServer();
};

// extern AsyncWebServer server;
extern VarioSettings GnuSettings;
#endif //VARIO_WIFI_H