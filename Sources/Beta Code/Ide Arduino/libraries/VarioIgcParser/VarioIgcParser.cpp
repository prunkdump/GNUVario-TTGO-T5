/* VarioIgcParser -- 
 *
 * Copyright 2020 MichelPa / Jpg63
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
 *                                                                               *
 *                           VarioIgcParser                                      *
 *                                                                               *
 *  version    Date     Description                                              *
 *    1.0                                                                        *
 *    1.1      14/11/20   json doc externe                                       *
 *                                                                               *
 *********************************************************************************
 */

#include <Arduino.h>

#include <HardwareConfig.h>
#include <DebugConfig.h>

// #define JSONDOCEXTERN

// #ifdef JSONDOCEXTERN
// #include <VarioSettings.h>
// #endif

#ifdef WIFI_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include "VarioIgcParser.h"
#include <MD5Builder.h>
#include <ArduinoJson.h>

QueueHandle_t xQueueParse;

VarioIgcParser::VarioIgcParser(String path)
{
#ifdef MEMORY_DEBUG
    Serial.println("Free heap VarioIgcParser constructor");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    filePath = path;
}

VarioIgcParser::~VarioIgcParser()
{
#ifdef MEMORY_DEBUG
    Serial.println("Free heap VarioIgcParser destructor");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG
}

boolean VarioIgcParser::parseFile()
{
    const char IGCHeaderDATE[] PROGMEM = "HFDTE";
    const char IGCHeaderDATENEW[] PROGMEM = "HFDTEDATE:";
    const char IGCHeaderPILOT[] PROGMEM = "HFPLTPILOTINCHARGE:";
    const char IGCHeaderGLIDER[] PROGMEM = "HFGTYGLIDERTYPE:";

    // TickType_t delay = (1) / portTICK_PERIOD_MS;
    File dataFile;
    String buffer;
    char tmpBuffer[100];
    uint8_t tmpBufferPos = 0;
    boolean startHeightSet = false;
    boolean startFlightTimeSet = false;

    uint32_t dataToSend = 1;

    myIgcData.minHeight = 9999;
    myIgcData.maxHeight = -9999;

    isParsed = false;

    if (!(dataFile = SDHAL_SD.open((char *)filePath.c_str(), FILE_READ)))
    {
        return false;
    }
    String tmpFullName = dataFile.name();
    myIgcData.filename = tmpFullName.substring(tmpFullName.lastIndexOf("/") + 1);

    MD5Builder md5b;
    md5b.begin();
    uint16_t size = dataFile.size();
    while (size > 0)
    {
        uint16_t curSize = min(size, (uint16_t)4096);
        md5b.addStream(dataFile, curSize);
        size -= curSize;

        xQueueSend(xQueueParse, &dataToSend, 0);
        // vTaskDelay(delay);
#ifdef WIFI_DEBUG
        SerialPort.print(".");
#endif //WIFI_DEBUG
    }

    md5b.calculate();
    myIgcData.md5 = md5b.toString();
#ifdef WIFI_DEBUG
    SerialPort.println(myIgcData.md5);
#endif //WIFI_DEBUG

    //on ferme pour libérer la mémoire
    dataFile.close();
    if (!(dataFile = SDHAL_SD.open((char *)filePath.c_str(), FILE_READ)))
    {
        return false;
    }

    //retour au debut du fichier
    dataFile.seek(0);

    while (dataFile.available())
    {
        xQueueSend(xQueueParse, &dataToSend, 0);
#ifdef WIFI_DEBUG
        SerialPort.print("-");
#endif //WIFI_DEBUG
        tmpBufferPos = 0;
        while (dataFile.available() && dataFile.peek() != '\n' && tmpBufferPos < 99) // note how this also prevents the buffer from overflowing (49 max to leave space for '\0'!)
        {
            tmpBuffer[tmpBufferPos] = dataFile.read();
            tmpBufferPos++;
        }
        if (tmpBufferPos > 0)
        {
            //on lit aussi le \n qui traine
            dataFile.read();

            tmpBuffer[tmpBufferPos] = '\0';
            buffer = String(tmpBuffer);
            buffer.trim();
#ifdef WIFI_DEBUG
            SerialPort.print(".");
            // SerialPort.print(" buffer : ");
            // SerialPort.println(buffer);
#endif //WIFI_DEBUG
            if (buffer.startsWith(IGCHeaderDATENEW))
            {
                // date de la trace
                myIgcData.flightDate = buffer.substring(10);
                myIgcData.flightDate.trim();
                //inversion pour format americain
                myIgcData.flightDate = "20" + myIgcData.flightDate.substring(4, 6) + "-" + myIgcData.flightDate.substring(2, 4) + "-" + myIgcData.flightDate.substring(0, 2);
#ifdef WIFI_DEBUG
                SerialPort.print("flightDate : ");
                SerialPort.println(myIgcData.flightDate);
#endif //WIFI_DEBUG
            }
            else if (buffer.startsWith(IGCHeaderDATE))
            {
                // date de la trace
                myIgcData.flightDate = buffer.substring(5);
                myIgcData.flightDate.trim();
                //inversion pour format americain
                myIgcData.flightDate = "20" + myIgcData.flightDate.substring(4, 6) + "-" + myIgcData.flightDate.substring(2, 4) + "-" + myIgcData.flightDate.substring(0, 2);
#ifdef WIFI_DEBUG
                SerialPort.print("flightDate : ");
                SerialPort.println(myIgcData.flightDate);
#endif //WIFI_DEBUG
            }
            else if (buffer.startsWith(IGCHeaderPILOT))
            {
                //nom du pilote
                myIgcData.pilot = buffer.substring(19);
                myIgcData.pilot.trim();
#ifdef WIFI_DEBUG
                SerialPort.print("pilot : ");
                SerialPort.println(myIgcData.pilot);
#endif //WIFI_DEBUG
            }
            else if (buffer.startsWith(IGCHeaderGLIDER))
            {
                //nom de la voile
                myIgcData.wing = buffer.substring(16);
                myIgcData.wing.trim();
#ifdef WIFI_DEBUG
                SerialPort.print("wing : ");
                SerialPort.println(myIgcData.wing);
#endif //WIFI_DEBUG
            }
            else if (buffer.startsWith("B"))
            {
                //trame trace
                // B1243314503488N00351234EA0088400927
                // B 12 43 31 4503488N 00351234E A 00884 00927
                //lat lon DDMMmmmN  DDDMMmmmE
                String hms = buffer.substring(1, 7);
                hms = hms.substring(0, 2) + ":" + hms.substring(2, 4) + ":" + hms.substring(4, 6);
                int16_t nPos = buffer.indexOf("N");
                int16_t ePos = buffer.indexOf("E");
                uint8_t aPos = buffer.indexOf("A");
                String lat = "";
                String lon = "";
                if (nPos != -1 && ePos != -1)
                {
                    lat = buffer.substring(7, nPos);
                    lon = buffer.substring(nPos + 1, ePos);
                }
                int16_t height = buffer.substring(aPos + 1, aPos + 6).toInt();

                if (!startHeightSet)
                {
                    startHeightSet = true;
                    myIgcData.startHeight = height;
                    if (nPos != -1 && ePos != -1)
                    {
                        myIgcData.startLat = lat.substring(0, 2).toDouble() + lat.substring(3, 8).toDouble() / 60000;
                        myIgcData.startLon = lon.substring(0, 3).toDouble() + lon.substring(3, 8).toDouble() / 60000;
                    }
                }
                // correction de l'heure en fonction du param de timezone
                // potentiellement faux si mal réglé
                correctTimeZone(hms);
                if (!startFlightTimeSet)
                {
                    startFlightTimeSet = true;
                    myIgcData.startFlightTime = hms;
                }
                myIgcData.endFlightTime = hms;
                myIgcData.minHeight = min(height, myIgcData.minHeight);
                myIgcData.maxHeight = max(height, myIgcData.maxHeight);
                myIgcData.endHeight = height;
                if (nPos != -1 && ePos != -1)
                {
                    myIgcData.endLat = lat.substring(0, 2).toDouble() + lat.substring(3, 8).toDouble() / 60000;
                    myIgcData.endLon = lon.substring(0, 3).toDouble() + lon.substring(3, 8).toDouble() / 60000;
                }
            }
        }
    }

    dataFile.close();

#ifdef WIFI_DEBUG
    SerialPort.print("startFlightTime : ");
    SerialPort.println(myIgcData.startFlightTime);
    SerialPort.print("endFlightTime : ");
    SerialPort.println(myIgcData.endFlightTime);
    SerialPort.print("startHeight : ");
    SerialPort.println(myIgcData.startHeight);
    SerialPort.print("minHeight : ");
    SerialPort.println(myIgcData.minHeight);
    SerialPort.print("maxHeight : ");
    SerialPort.println(myIgcData.maxHeight);
    SerialPort.print("endHeight : ");
    SerialPort.println(myIgcData.endHeight);
#endif //WIFI_DEBUG

    isParsed = true;

    return true;
}

igcdata VarioIgcParser::getIgcdata()
{
    return myIgcData;
}

void VarioIgcParser::correctTimeZone(String &hms)
{
    //hms = "18:57:12"
    uint8_t UTCDrift = GnuSettings.VARIOMETER_TIME_ZONE;
    uint8_t hour = atoi((char *)hms.substring(0, 2).c_str());
    hour += UTCDrift;
    if (hour < 0)
    {
        hour += 24;
    }
    if (hour >= 24)
    {
        hour -= 24;
    }
    char newHMS[9];
    sprintf(newHMS, "%02d%s", hour, hms.substring(2, 8));
    hms = String(newHMS);
}