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

#include "VarioWifi.h"
#include "VarioWebHandler.h"

#include <ESPmDNS.h>
#include <WiFiMulti.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//************************************************************
// DISPLAY SCREEN
//************************************************************
#define ENABLE_DISPLAY_WEBSERVER

#ifdef ENABLE_DISPLAY_WEBSERVER
#include <varioscreenGxEPD.h>
#endif

#include <VarioData.h>

WiFiMulti wifiMulti;
AsyncWebServer server(80);

VarioWebHandler varioWebHandler;

bool VarioWifi::begin()
{

    if (!isIndexFileExist())
    {
        return false;
    }

#ifdef MEMORY_DEBUG
    Serial.println("Free heap connectToWifi");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    if (!connectToWifi())
    {
        return false;
    }

#ifdef MEMORY_DEBUG
    Serial.println("Free heap setClock");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    setClock();

#ifdef MEMORY_DEBUG
    Serial.println("Free heap setMDNS");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    setMDNS();

    checkDbVersion();

    esp32FOTA.checkURL = GnuSettings.URL_UPDATE; //"http://gnuvario-e.yj.fr/update/firmware.json";

#ifdef MEMORY_DEBUG
    Serial.println("Free heap startWebServer");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    startWebServer();

    return true;
}

bool VarioWifi::connectToWifi()
{
    char ssid_1[50];
    char password_1[50];

    char ssid_2[50];
    char password_2[50];

    char ssid_3[50];
    char password_3[50];

    char ssid_4[50];
    char password_4[50];

    GnuSettings.VARIOMETER_SSID_1.toCharArray(ssid_1, sizeof(ssid_1));
    GnuSettings.VARIOMETER_PASSWORD_1.toCharArray(password_1, sizeof(password_1));

    GnuSettings.VARIOMETER_SSID_2.toCharArray(ssid_2, sizeof(ssid_2));
    GnuSettings.VARIOMETER_PASSWORD_2.toCharArray(password_2, sizeof(password_2));

    GnuSettings.VARIOMETER_SSID_3.toCharArray(ssid_3, sizeof(ssid_3));
    GnuSettings.VARIOMETER_PASSWORD_3.toCharArray(password_3, sizeof(password_3));

    GnuSettings.VARIOMETER_SSID_4.toCharArray(ssid_4, sizeof(ssid_4));
    GnuSettings.VARIOMETER_PASSWORD_4.toCharArray(password_4, sizeof(password_4));

#ifdef WIFI_DEBUG
    SerialPort.print("ssid_1 : ");
    SerialPort.println(ssid_1);
    SerialPort.print("password_1 : ");
    SerialPort.println(password_1);

    SerialPort.print("ssid_2 : ");
    SerialPort.println(ssid_2);
    SerialPort.print("password_2 : ");
    SerialPort.println(password_2);

    SerialPort.print("ssid_3 : ");
    SerialPort.println(ssid_3);
    SerialPort.print("password_3 : ");
    SerialPort.println(password_3);

    SerialPort.print("ssid_4 : ");
    SerialPort.println(ssid_4);
    SerialPort.print("password_4 : ");
    SerialPort.println(password_4);
#endif //WIFI_DEBUG

    if (ssid_1[0] != '\0' && password_1[0] != '\0')
    {
        wifiMulti.addAP(ssid_1, password_1); // add Wi-Fi networks you want to connect to, it connects strongest to weakest
    }
    if (ssid_2[0] != '\0' && password_2[0] != '\0')
    {
        wifiMulti.addAP(ssid_2, password_2);
    }
    if (ssid_3[0] != '\0' && password_3[0] != '\0')
    {
        wifiMulti.addAP(ssid_3, password_3);
    }
    if (ssid_4[0] != '\0' && password_4[0] != '\0')
    {
        wifiMulti.addAP(ssid_4, password_4);
    }

    SerialPort.println("Connecting ...");
#ifdef ENABLE_DISPLAY_WEBSERVER
    screen.ScreenViewWifi("", "");
#endif //ENABLE_DISPLAY_WEBSERVER

    int counter = 0;
    while (wifiMulti.run() != WL_CONNECTED)
    { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
        delay(250);
        SerialPort.print('.');
        counter++;
        if (counter > 30)
        {
            SerialPort.println(F("Error Wifi not found!"));
            ESP.restart();
        }
    }
    SerialPort.println("\nConnected to " + WiFi.SSID() + " Use IP address: " + WiFi.localIP().toString()); // Report which SSID and IP is in use
                                                                                                           // The logical name http://fileserver.local will also access the device if you have 'Bonjour' running or your system supports multicast dns
#ifdef ENABLE_DISPLAY_WEBSERVER
    screen.ScreenViewWifi(WiFi.SSID().substring(0, 12), WiFi.localIP().toString());
#endif //ENABLE_DISPLAY_WEBSERVER

    return true;
}

bool VarioWifi::checkDbVersion()
{
#ifdef WIFI_DEBUG
    SerialPort.println("checkDbVersion");
#endif

    // if (SDHAL_SD.exists("/dist.gz"))
    // {
    //     tarGzExpander(SD, "/dist.gz", SD, "/tmp");
    // }

    if (!SDHAL_SD.exists("/db"))
    {
        SDHAL_SD.mkdir("/db");
        File myOrigFile;
        File myDestFile;

        if (SDHAL_SD.exists("/www/db/vol.db") && !SDHAL_SD.exists("/db/vol.db"))
        {
            myDestFile = SDHAL_SD.open("/db/vol.db", FILE_WRITE);
            myOrigFile = SDHAL_SD.open("/www/db/vol.db", FILE_READ);
            if (myDestFile && myOrigFile)
            {
                int data;
                while ((data = myOrigFile.read()) >= 0)
                {
                    myDestFile.write(data);
                }
                myDestFile.close();
                myOrigFile.close();
            }
            else
            {
                return false;
            }
        }
    }

    char path[] = "/www/sql";
    File dir;

    dir = SDHAL_SD.open(path, FILE_READ);

    if (!dir.isDirectory())
    {
        dir.close();
        return false;
    }
    dir.rewindDirectory();

    VarioSqlFlight varioSqlFlight;

    for (int cnt = 0; true; ++cnt)
    {
        File entry;
        entry = dir.openNextFile(FILE_READ);
        if (!entry)
        {
            TRACE();
            break;
        }

        TRACE();
        String tmpFullName = entry.name();
        String version = tmpFullName.substring(tmpFullName.lastIndexOf("/") + 1);
        version = version.substring(0, version.lastIndexOf("."));
        Serial.println("avant migration");
        Serial.println(ESP.getFreeHeap());
        varioSqlFlight.executeMigration(version, entry.readString());
        Serial.println("apres migration");
        Serial.println(ESP.getFreeHeap());
#ifdef WIFI_DEBUG
        SerialPort.println(version);
#endif
    }

    dir.close();
    TRACE();
    return true;
}

bool VarioWifi::isIndexFileExist()
{
    File dataFile;

    //test présence fichier index
    if (SDHAL_SD.exists("/www/index.htm"))
    {
#ifdef WIFI_DEBUG
        SerialPort.println("SD Card: initialized, index.htm found");
#endif //WIFI_DEBUG
        return true;
    }
    else
    {
#ifdef WIFI_DEBUG
        SerialPort.println("SD Card: failed to read index.htm.");
#endif //WIFI_DEBUG
        return false;
    }
}

/***********************************/
void VarioWifi::setClock()
{
    /***********************************/
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

#ifdef WIFI_DEBUG
    SerialPort.print(F("Waiting for NTP time sync: "));
#endif

    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        delay(500);
#ifdef WIFI_DEBUG
        SerialPort.print(F("."));
#endif
        yield();
        nowSecs = time(nullptr);
    }

#ifdef WIFI_DEBUG
    SerialPort.println();
#endif
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

#ifdef WIFI_DEBUG
    SerialPort.print(F("Current time: "));
    SerialPort.print(asctime(&timeinfo));
#endif
}

void VarioWifi::setMDNS()
{
    if (MDNS.begin(servername))
    {
        MDNS.addService("http", "tcp", 80);
#ifdef WIFI_DEBUG
        SerialPort.println("MDNS responder started");
        SerialPort.print("You can now connect to http://");
        SerialPort.print(servername);
        SerialPort.println(".local");
#endif
    }
    else
    {
        SerialPort.println(F("Error setting up MDNS responder!"));
        ESP.restart();
    }
}

void VarioWifi::startWebServer()
{

    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });

    // téléchargements de "mes vols" = contenu du repertoire vols
    server.on("/flights", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleListFlights(request));
    });

    //récupération de la liste du contenu de la carte SD
    server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handlePrintDirectory(request));
    });

    //récupération du contenu du fichier param
    server.on("/params", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleParams(request));
    });

    server.on(
        "/params", HTTP_POST, [](AsyncWebServerRequest *request) {
            // le reponse est envoyé par le handler sur le body
        },
        NULL, varioWebHandler.handleSaveParams);

    // mise à jour OTA
    server.on(
        "/fupdate", HTTP_POST, [](AsyncWebServerRequest *request) {
            // le reponse est envoyé par le handler sur le upload
        },
        varioWebHandler.handleOtaUpdate);

    //telechargement d'un fichier dont le nom complet avec chemin se trouve en param
    server.on("/file", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleFileDownload(request));
    });

    //suppression d'un fichier dont le nom complet avec chemin se trouve en param
    server.on("/file", HTTP_DELETE, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleFileDelete(request));
    });

    // upload d'un fichier, le chemin de destination se trouve dans le nom du fichier posté
    server.on(
        "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
            // le reponse est envoyé par le handler sur le upload
        },
        varioWebHandler.handleFileUpload);

    // sauvegarde du contenu du fichier wifi
    server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
            // le reponse est envoyé par le handler sur le body
        },
        NULL, varioWebHandler.handleSaveWifi);

    //creation d'un fichier ou d'un repertoire
    server.on(
        "/create", HTTP_PUT, [](AsyncWebServerRequest *request) {
            // le reponse est envoyé par le handler sur le upload
        },
        NULL, varioWebHandler.handleFileCreate);

    //récupération du contenu du fichier wifi
    server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleWifi(request));
    });

    //récupération du contenu du fichier preference
    server.on("/webconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleWebConfig(request));
    });

    // sauvegarde du contenu du fichier preference
    server.on(
        "/webconfig", HTTP_POST, [](AsyncWebServerRequest *request) {
            // le reponse est envoyé par le handler sur le body
        },
        NULL, varioWebHandler.handleSaveWebConfig);

    // parsage d'un fichier IGC
    server.on("/parseigc", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleParseIgc(request));
    });

    // téléchargements de "mes vols" en BDD
    server.on("/flightsbdd", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleGetFlights(request));
    });

    // enregistrement un vol en BDD
    server.on(
        "/flightsbdd", HTTP_POST, [](AsyncWebServerRequest *request) {
            // le reponse est envoyé par le handler sur le body
        },
        NULL, varioWebHandler.handleSetFlight);

    // suppression d'un vol en BDD
    server.on("/flightsbdd", HTTP_DELETE, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleDelFlight(request));
    });

    //recuperation des versions de firmware
    server.on("/firmwareversion", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleFirmwareVersion(request));
    });

    //Mise à jour via internet
    server.on("/upgradeweb", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleUpgradeWeb(request));
    });

    // récupération de la liste des sites
    server.on("/site", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleGetSites(request));
    });

    // sauvegarde d'un site
    server.on(
        "/site", HTTP_POST, [](AsyncWebServerRequest *request) {
            // le reponse est envoyé par le handler sur le body
        },
        NULL, varioWebHandler.handleSetSite);

    // suppression d'un site
    server.on("/site", HTTP_DELETE, [](AsyncWebServerRequest *request) {
        request->send(varioWebHandler.handleDelSite(request));
    });

    //default web dir "/www"
    server.serveStatic("/", SD, "/www/").setDefaultFile("index.htm");

    server.onNotFound([](AsyncWebServerRequest *request) {
        Serial.printf("NOT_FOUND: ");
        if (request->method() == HTTP_GET)
            Serial.printf("GET");
        else if (request->method() == HTTP_POST)
            Serial.printf("POST");
        else if (request->method() == HTTP_DELETE)
            Serial.printf("DELETE");
        else if (request->method() == HTTP_PUT)
            Serial.printf("PUT");
        else if (request->method() == HTTP_PATCH)
            Serial.printf("PATCH");
        else if (request->method() == HTTP_HEAD)
            Serial.printf("HEAD");
        else if (request->method() == HTTP_OPTIONS)
            Serial.printf("OPTIONS");
        else
            Serial.printf("UNKNOWN");
        Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

        if (request->contentLength())
        {
            Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
            Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
        }

        int headers = request->headers();
        int i;
        for (i = 0; i < headers; i++)
        {
            AsyncWebHeader *h = request->getHeader(i);
            Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
        }

        int params = request->params();
        for (i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);
            if (p->isFile())
            {
                Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
            }
            else if (p->isPost())
            {
                Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
            }
            else
            {
                Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
            }
        }

        if (request->method() == HTTP_OPTIONS)
        {
            request->send(200);
        }
        else
        {
            request->send(404);
        }
    });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    server.begin();
}