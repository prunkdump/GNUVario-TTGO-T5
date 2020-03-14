/* VarioESP32WebServer -- 
 *
 * Copyright 2019 Pascal - jpg63
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

/* 
 *********************************************************************************
 *********************************************************************************
 *                                                                               *
 *                           VarioESP32WebServer                                 *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0    27/12/19                                                            *
 *                                                                               *
 *********************************************************************************/

#include "VarioESP32WebServer.h"
#include "ESP32WebServer.h"
#include <Arduino.h>
#include <esp32-hal-log.h>
#include <libb64/cencode.h>
#include "WiFiServer.h"
#include "WiFiClient.h"
#include "FS.h"
#include "detail/RequestHandlersImpl.h"
#include "mbedtls/md5.h"

VarioESP32WebServer::VarioESP32WebServer(IPAddress addr, int port) : ESP32WebServer(addr, port)
{
}

VarioESP32WebServer::VarioESP32WebServer(int port) : ESP32WebServer(port)
{
}