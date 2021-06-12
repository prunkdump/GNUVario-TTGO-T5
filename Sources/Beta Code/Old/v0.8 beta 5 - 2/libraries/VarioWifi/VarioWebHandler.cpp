#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include "VarioWebHandler.h"

//************************************************************
// DISPLAY SCREEN
//************************************************************
#define ENABLE_DISPLAY_WEBSERVER

#ifdef ENABLE_DISPLAY_WEBSERVER
#include <varioscreenGxEPD.h>
#endif

File uploadFile;
File varioParamFile;
File wifiParamFile;
File webParamFile;
VarioSqlFlightHelper varioSqlFlightHelper;

AsyncResponseStream *VarioWebHandler::handleListFlights(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("application/json");

    String path = "/vols";

    File dir;
    dir = SDHAL_SD.open((char *)path.c_str(), FILE_READ);

    if (!dir.isDirectory())
    {
        dir.close();
    }
    dir.rewindDirectory();

    response->print("[");

    bool first = true;
    File entry;
    while (entry = dir.openNextFile(FILE_READ))
    {
        String output;
        String fsize = "";

        if (!entry.isDirectory())
        {
            if (first)
            {
                first = false;
            }
            else
            {
                output = ',';
            }
            int bytes = entry.size();
            fsize = getFileSizeStringFromBytes(bytes);

            output += "{\"type\":\"";
            output += (entry.isDirectory()) ? "dir" : "file";
            output += "\",\"name\":\"";
            output += entry.name();
            output += "\",\"size\":\"";
            output += fsize;
            output += "\"";
            output += "}";

            response->print(output);
        }

        entry.close();
    }

    response->print("]");

    //correction bug chunk transfer webserver
    // server.sendContent("");
    dir.close();

    return response;
}

AsyncResponseStream *VarioWebHandler::handlePrintDirectory(AsyncWebServerRequest *request)
{
    /***********************************/
#ifdef WIFI_DEBUG
    SerialPort.println("handlePrintDirectory");
#endif

    AsyncResponseStream *response = request->beginResponseStream("application/json");

    String path;
    boolean isRecursive = true;

    if (request->hasParam("dir"))
    {
        AsyncWebParameter *p = request->getParam("dir");
        path = p->value().c_str();
    }
    else
    {
        path = "/";
    }

    if (request->hasParam("dir"))
    {
        isRecursive = false;
    }

#ifdef WIFI_DEBUG
    SerialPort.print("dir : ");
    SerialPort.println((char *)path.c_str());
#endif

    File dir;
    dir = SDHAL_SD.open((char *)path.c_str(), FILE_READ);
    dir.rewindDirectory();

    if (!dir.isDirectory())
    {
#ifdef WIFI_DEBUG
        SerialPort.println("Not directory");
#endif

        dir.close();
        //response->setCode(404);
        request->send(500, "text/plain", "NOT DIR");

        return response;
    }

#ifdef WIFI_DEBUG
    SerialPort.println("[");
#endif

    response->print("[");

    String output;
    output += "{\"type\":\"";
    output += "dir";
    output += "\",\"name\":\"";
    output += path;
    output += "\",\"size\":\"";
    output += "na";
    output += "\"";

    output += ",\"contents\" :[";
#ifdef WIFI_DEBUG
    //SerialPort.println(output);
#endif
    response->print(output);
    printDirectoryRecurse(response, path, isRecursive);
    output = "]";

    output += "}";
    response->print(output);
    // printDirectoryRecurse(path);

    response->print("]");

    //correction bug chunk transfer webserver
    // server.sendContent("");
    // server.client().stop();
#ifdef WIFI_DEBUG
    SerialPort.println("]");
#endif
    dir.close();

    return response;
}

/***********************************/
void VarioWebHandler::printDirectoryRecurse(AsyncResponseStream *response, String path, boolean isRecursive)
/***********************************/
{

    File dir;
    dir = SDHAL_SD.open((char *)path.c_str(), FILE_READ);
    dir.rewindDirectory();

    int tmpcnt = 0;

    for (int cnt = 0; true; ++cnt)
    {
        File entry;
        if (!(entry = dir.openNextFile(FILE_READ)))
        {
            // TRACE();
            break;
        }
        String tmpName = entry.name();

        if (tmpName.equalsIgnoreCase("SYSTEM~1") || tmpName.startsWith(".")) //equalsIgnoreCase(".TRASH~1"))
        {
            continue;
        }

        String output;
        if (tmpcnt > 0)
        {
            output = ',';
        }

        String fsize = "";

        if (!entry.isDirectory())
        {
            int bytes = entry.size();
            fsize = getFileSizeStringFromBytes(bytes);
        }
        else
        {
            fsize = "na";
        }

        output += "{\"type\":\"";

        output += (entry.isDirectory()) ? "dir" : "file";

        output += "\",\"name\":\"";
        String tmpFullName = entry.name();
        output += tmpFullName.substring(tmpFullName.lastIndexOf("/") + 1);

        output += "\",\"size\":\"";
        output += fsize;
        output += "\"";

        if (entry.isDirectory())
        {
            output += ",\"contents\" :[";
#ifdef WIFI_DEBUG
            // SerialPort.println(output);
#endif
            response->print(output);

            if (isRecursive)
            {
                printDirectoryRecurse(response, entry.name(), isRecursive);
            }
            output = "]";
        }
        output += "}";

#ifdef WIFI_DEBUG
        //SerialPort.println(output);
#endif

        response->print(output);
        entry.close();
        tmpcnt++;
    }
}

AsyncWebServerResponse *VarioWebHandler::handleParams(AsyncWebServerRequest *request)
{
#ifdef WIFI_DEBUG
    SerialPort.println("handleParams");
#endif
    AsyncWebServerResponse *response;
    String path = "/params.jso";
    if (!SDHAL_SD.exists(path))
    {
        response = request->beginResponse(404);
    }
    else
    {
        response = request->beginResponse(SD, "/params.jso", "application/json");
    }

    return response;
}

void VarioWebHandler::handleSaveParams(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
#ifdef WIFI_DEBUG
    SerialPort.println("handleSaveParams");
#endif
    String path = "/params.jso";
    String pathBak = "/params.bak";

    if (!index)
    {
        backupFile(path, pathBak);
        if (!(varioParamFile = SDHAL_SD.open(path.c_str(), FILE_WRITE)))
        {
            request->send(500, "text/plain", "NO FILE");
            return;
        }
    }

    varioParamFile.write(data, len);

    if (index + len == total)
    {
        varioParamFile.close();
        request->send(200);
    }

    return;
}

void VarioWebHandler::handleOtaUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{

    if (!index)
    {
        Serial.printf("UploadStart: %s\n", filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH))
        { //start with max available size
            Update.printError(Serial);
            request->send(500, "text/plain", "UPDATE FAIL");
            return;
        }
    }
    if (len)
    {
        if (Update.write(data, len) != len)
        {
            Update.printError(Serial);
            request->send(500, "text/plain", "UPDATE FAIL");
            return;
        }
    }

    if (final)
    {
        Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);

        if (Update.end(true))
        { //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", index + len);
            request->send(200);
            Serial.println("RESTART ESP32");
            Serial.flush();

            ESP_LOGI("GnuVario-E", "RESTART ESP32");
#ifdef ENABLE_DISPLAY_WEBSERVER
            screen.ScreenViewReboot();
#endif
            ESP.restart();
        }
        else
        {
            Update.printError(Serial);
            request->send(500, "text/plain", "UPDATE FAIL");
            return;
        }
    }
}

//telechargement d'un fichier dont le nom complet avec chemin se trouve en param
AsyncWebServerResponse *VarioWebHandler::handleFileDownload(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response;
    String path;
    if (request->hasParam("path"))
    {
        AsyncWebParameter *p = request->getParam("path");
        path = p->value().c_str();
    }
    else
    {
        response = request->beginResponse(500, "text/plain", "BAD ARGS");
        return response;
    }

    if (!SDHAL_SD.exists((char *)path.c_str()))
    {
        response = request->beginResponse(500, "text/plain", "NO FILE");
        return response;
    }
    response = request->beginResponse(SD, path, "application/octet-stream", true);

    return response;
}

//suppression d'un fichier dont le nom complet avec chemin se trouve en param
AsyncWebServerResponse *VarioWebHandler::handleFileDelete(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response;
    String path;
    if (request->hasParam("path"))
    {
        AsyncWebParameter *p = request->getParam("path");
        path = p->value().c_str();
    }
    else
    {
        response = request->beginResponse(500, "text/plain", "BAD ARGS");
        return response;
    }

    if (!SDHAL_SD.exists((char *)path.c_str()))
    {
        response = request->beginResponse(500, "text/plain", "NO FILE");
        return response;
    }

    SdCardHAL::deleteRecursive(path);

    response = request->beginResponse(200, "text/plain", "OK");
    return response;
}

// upload d'un fichier, le chemin de destination se trouve dans le nom du fichier posté
void VarioWebHandler::handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{

    if (!index)
    {
        if (SDHAL_SD.exists(filename))
        {
            SDHAL_SD.remove(filename);
        }

        Serial.printf("UploadStart: %s\n", filename.c_str());
        uploadFile = SDHAL_SD.open(filename, FILE_WRITE);
        if (!uploadFile)
        {
            //return error
            request->send(500, "text/plain", "UPLOAD FAIL");
            return;
        }
    }

    if (len && uploadFile)
    {
        if (uploadFile.write(data, len) != len)
        {
            //return error
            Serial.printf("Upload fail");
            request->send(500, "text/plain", "UPLOAD FAIL");
            return;
        }
    }

    if (final && uploadFile)
    {
        Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
        uploadFile.close();
        request->send(200);
    }
}

void VarioWebHandler::handleFileCreate(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String path;

    if (request->hasParam("path"))
    {
        AsyncWebParameter *p = request->getParam("path");
        path = p->value().c_str();
    }
    else
    {
        request->send(500, "text/plain", "BAD ARGS");

        return;
    }

    if (path == "/" || SDHAL_SD.exists((char *)path.c_str()))
    {
        request->send(500, "text/plain", "BAD PATH");
        return;
    }

    if (path.indexOf('.') > 0)
    {
        File fileSD = SDHAL_SD.open((char *)path.c_str(), FILE_WRITE);
        if (fileSD)
        {
            fileSD.print("\0");
            fileSD.close();
        }
    }
    else
    {
        SDHAL_SD.mkdir((char *)path.c_str());
    }
    request->send(200, "text/plain", "OK");
    return;
}

//récupération du contenu du fichier wifi
AsyncWebServerResponse *VarioWebHandler::handleWifi(AsyncWebServerRequest *request)
{
#ifdef WIFI_DEBUG
    SerialPort.println("handleParams - handle Wifi");
#endif

    AsyncWebServerResponse *response = request->beginResponse(SD, "/wifi.cfg", "text/plain");

    return response;
}

// sauvegarde des parametres
void VarioWebHandler::handleSaveWifi(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{

#ifdef WIFI_DEBUG
    SerialPort.println("handleSaveParams - Save Wifi");
#endif

    String path = "/wifi.cfg";
    String pathBak = "/wifi.bak";

    if (!index)
    {
			
#ifdef WIFI_DEBUG
				SerialPort.println("handleSaveParams - Backup");
#endif
			
        backupFile(path, pathBak);
        if (!(wifiParamFile = SDHAL_SD.open(path.c_str(), FILE_WRITE)))
        {

#ifdef WIFI_DEBUG
						SerialPort.println("Erreur impossible d'ouvrir wifi.cfg");
#endif

            request->send(500, "text/plain", "NO FILE");
            return;
        }
    }

#ifdef WIFI_DEBUG
	  SerialPort.println("Save config wifi");
#endif

    wifiParamFile.write(data, len);

    if (index + len == total)
    {
        wifiParamFile.close();
        request->send(200);
    }

    return;
}

AsyncWebServerResponse *VarioWebHandler::handleWebConfig(AsyncWebServerRequest *request)
{
#ifdef WIFI_DEBUG
    SerialPort.println("handleParams Web config");
#endif
    AsyncWebServerResponse *response;

    if (!SDHAL_SD.exists("/prefs.jso"))
    {
        response = request->beginResponse(404, "text/plain", "NO FILE");
    }
    else
    {
        response = request->beginResponse(SD, "/prefs.jso", "application/json");
    }

    return response;
}

// sauvegarde du contenu du fichier preference
void VarioWebHandler::handleSaveWebConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{

#ifdef WIFI_DEBUG
    SerialPort.println("handleSaveParams");
#endif

    String path = "/prefs.jso";
    String pathBak = "/prefs.bak";

    if (!index)
    {
        backupFile(path, pathBak);
        if (!(webParamFile = SDHAL_SD.open(path.c_str(), FILE_WRITE)))
        {
            request->send(500, "text/plain", "NO FILE");
            return;
        }
    }

    webParamFile.write(data, len);

    if (index + len == total)
    {
        webParamFile.close();
        request->send(200);
    }

    return;
}

// parsage d'un fichier IGC
AsyncWebServerResponse *VarioWebHandler::handleParseIgc(AsyncWebServerRequest *request)
{
#ifdef WIFI_DEBUG
    SerialPort.println("handleParseIgc");
#endif
    AsyncWebServerResponse *response;
    uint32_t dataToSend = 1;
    String path;

    if (request->hasParam("path"))
    {
        AsyncWebParameter *p = request->getParam("path");
        path = p->value().c_str();
    }

    else
    {
        response = request->beginResponse(500, "text/plain", "BAD ARGS");
        return response;
    }

    if (!xQueueParse)
    {
        xQueueParse = xQueueCreate(5, sizeof(uint32_t));
#ifdef WIFI_DEBUG
        SerialPort.println("xQueueParse created");
#endif
        if (!xQueueParse)
        {
#ifdef WIFI_DEBUG
            SerialPort.println("xQueueParse failed creation");
#endif
        }
    }

    xQueueSend(xQueueParse, &dataToSend, 0);

    // const TickType_t delay = (100) / portTICK_PERIOD_MS;
    // vTaskDelay(delay);

    TaskHandle_t taskParse;
    xTaskCreate(
        _doParseIgcAndInsert,   /* Task function. */
        "doParseIgcAndInsert",  /* String with name of task. */
        18000,                  /* Stack size in bytes. */
        (void *)(path.c_str()), /* Parameter passed as input of the task */
        1,                      /* Priority of the task. */
        &taskParse);            /* Task handle. */

    response = request->beginChunkedResponse("text/plain", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
        //Write up to "maxLen" bytes into "buffer" and return the amount written.
        //index equals the amount of bytes that have been already sent
        //You will be asked for more data until 0 is returned
        //Keep in mind that you can not delay or yield waiting for more data!

        uint32_t element;
        if (!(xQueueReceive(xQueueParse, &element, portMAX_DELAY) == pdTRUE))
        {
#ifdef WIFI_DEBUG
            SerialPort.println("RIEN DANS LA QUEUE");
#endif
            //on force le renvoi de la valeur  1
            element = 1;
        }
        else
        {
#ifdef WIFI_DEBUG
            SerialPort.print("VALEUR DE LA QUEUE");
            SerialPort.println(element);
#endif
        };

        if (element == 0)
        {
            // la queue contient l'element 0 qui signifie que le traitement est terminé
            vQueueDelete(xQueueParse);
            xQueueParse = NULL;
#ifdef WIFI_DEBUG
            SerialPort.println("handleParseIgc RETURN 0");
#endif
            return 0;
        }
        else if (element == 2)
        {
            //la queue contient l'element 2 qui signifie que le traitement a échoué
            // on met 0 dans la queue pour terminer la requete au prochain passage
            uint32_t dataToSend = 0;
            xQueueSend(xQueueParse, &dataToSend, 0);

            return snprintf((char *)buffer, maxLen, "ERROR");
        }
        else
        {
            // la queue contient l'element 1 qui signifie que le traitement est en cours
            return snprintf((char *)buffer, maxLen, ".");
        }
    });

    return response;
}

AsyncWebServerResponse *VarioWebHandler::handleGetFlights(AsyncWebServerRequest *request)
{
    int16_t offset;
    int16_t limit;

    if (request->hasParam("offset"))
    {
        AsyncWebParameter *p = request->getParam("offset");
        offset = p->value().toInt();
    }
    else
    {
        offset = 0;
    }

    if (request->hasParam("limit"))
    {
        AsyncWebParameter *p = request->getParam("limit");
        limit = p->value().toInt();
    }
    else
    {
        limit = 0;
    }

    varioSqlFlightHelper.init(limit, offset);

    AsyncWebServerResponse *response = request->beginChunkedResponse("application/json", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
        //Write up to "maxLen" bytes into "buffer" and return the amount written.
        //index equals the amount of bytes that have been already sent
        //You will be asked for more data until 0 is returned
        //Keep in mind that you can not delay or yield waiting for more data!
        return varioSqlFlightHelper.readData(buffer, maxLen);
    });

    return response;
}

String VarioWebHandler::getFileSizeStringFromBytes(int bytes)
{
    // conversion taille
    /***********************************/
    String fsize = "";

    if (bytes < 1024)
        fsize = String(bytes) + " B";
    else if (bytes < (1024 * 1024))
        fsize = String(bytes / 1024.0, 3) + " KB";
    else if (bytes < (1024 * 1024 * 1024))
        fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
    else
        fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";

    return fsize;
}

void VarioWebHandler::backupFile(String pathOrig, String pathBack)
{
    if (SDHAL_SD.exists((char *)pathBack.c_str()))
    {
        SDHAL_SD.remove((char *)pathBack.c_str());
    }
    size_t n;
    uint8_t buf[64];

    if (SDHAL_SD.exists((char *)pathOrig.c_str()))
    {
        File dataFile;
        dataFile = SDHAL_SD.open(pathOrig.c_str(), FILE_READ);
        File dataFile2;
        dataFile2 = SDHAL_SD.open(pathBack.c_str(), FILE_WRITE);

        while ((n = dataFile.read(buf, sizeof(buf))) > 0)
        {
            dataFile2.write(buf, n);
        }
        dataFile.close();
        dataFile2.close();

        SDHAL_SD.remove((char *)pathOrig.c_str());
    }
}

void VarioWebHandler::handleSetFlight(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
#ifdef WIFI_DEBUG
    SerialPort.println("handleSetFlight");
#endif

    VarioSqlFlight varioSqlFlight;
    char content[len];
    strncpy(content, (char *)data, len);
    content[len] = '\0';
    if (request->hasParam("id", false))
    {
        AsyncWebParameter *p = request->getParam("id");
        uint8_t id = p->value().toInt();

#ifdef WIFI_DEBUG
        SerialPort.println(id);
        SerialPort.println(content);
#endif
        varioSqlFlight.updateFlight(id, jsonToIgcdata(content));
    }
    else
    {

#ifdef WIFI_DEBUG
        SerialPort.println(content);
#endif

        varioSqlFlight.insertFlight(jsonToIgcdata(content));
    }

    request->send(200);

    return;
}

AsyncWebServerResponse *VarioWebHandler::handleDelFlight(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response;

    if (request->hasParam("filename"))
    {
        AsyncWebParameter *p = request->getParam("filename");
        String filename = p->value().c_str();

        File dataFile;
        if (dataFile = SDHAL_SD.open("/vols/parsed/" + filename, FILE_READ))
        {
            dataFile.close();
            SDHAL_SD.rename("/vols/parsed/" + filename, "/vols/" + filename);
        }
    }

    VarioSqlFlight varioSqlFlight;

    if (request->hasParam("id"))
    {
        AsyncWebParameter *p = request->getParam("id");
        uint8_t id = p->value().toInt();

#ifdef WIFI_DEBUG
        SerialPort.println(id);

#endif

        varioSqlFlight.delFlight(id);
    }

    response = request->beginResponse(200, "text/plain", "OK");
    return response;
}

AsyncWebServerResponse *VarioWebHandler::handleFirmwareVersion(AsyncWebServerRequest *request)
{
    //recuperation des versions de firmware

#ifdef WIFI_DEBUG
    SerialPort.println("handleFirmwareVersion");
#endif

    AsyncWebServerResponse *response = request->beginChunkedResponse("application/json", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
        //Write up to "maxLen" bytes into "buffer" and return the amount written.
        //index equals the amount of bytes that have been already sent
        //You will be asked for more data until 0 is returned
        //Keep in mind that you can not delay or yield waiting for more data!
        if (!index)
        {
            String output = esp32FOTA.getHTTPVersion();
            memcpy(buffer, (char *)output.c_str(), output.length());
            return output.length();
        }
        return 0;
    });

    return response;
}

AsyncWebServerResponse *VarioWebHandler::handleUpgradeWeb(AsyncWebServerRequest *request)
{

    AsyncWebServerResponse *response;

    bool betaversion;
    if (request->hasParam("beta"))
    {
        AsyncWebParameter *p = request->getParam("beta");
        betaversion = p->value().toInt();
    }
    else
    {
        response = request->beginResponse(500, "text/plain", "BAD ARGS");
        return response;
    }

    int8_t updatedNeeded = esp32FOTA.execHTTPcheck(betaversion);
    if (updatedNeeded == 1)
    {
#ifdef WIFI_DEBUG
        SerialPort.println("############Update firmware########");
#endif

        response = request->beginResponse(200);
        esp32FOTA.execOTA();
        return response;
    }
    else
    {
        response = request->beginResponse(500, "text/plain", "could not update");
        return response;
    }
}

AsyncWebServerResponse *VarioWebHandler::handleGetSites(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    VarioSqlFlight varioSqlFlight;

    response->print(varioSqlFlight.getSites());

    return response;
}

void VarioWebHandler::handleSetSite(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
#ifdef WIFI_DEBUG
    SerialPort.println("handleSetSite");
#endif

    VarioSqlFlight varioSqlFlight;
    char content[len];
    strncpy(content, (char *)data, len);
    content[len] = '\0';
    if (request->hasParam("id", false))
    {
        AsyncWebParameter *p = request->getParam("id");
        uint8_t id = p->value().toInt();

#ifdef WIFI_DEBUG
        SerialPort.println("Update Site");
				SerialPort.print("ID : ");
        SerialPort.println(id);
				SerialPort.print("CONTENT : ");
        SerialPort.println(content);
				
#endif
        varioSqlFlight.updateSite(id, content);
    }
    else
    {

#ifdef WIFI_DEBUG
        SerialPort.println("Insert Site");
				SerialPort.print("CONTENT : ");
        SerialPort.println(content);
#endif

        if (!varioSqlFlight.insertSite(content))
        {
            request->send(500);

            return;
        };
    }

    request->send(200);

    return;
}

AsyncWebServerResponse *VarioWebHandler::handleDelSite(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response;

    VarioSqlFlight varioSqlFlight;

    if (request->hasParam("id"))
    {
        AsyncWebParameter *p = request->getParam("id");
        uint8_t id = p->value().toInt();

#ifdef WIFI_DEBUG
        SerialPort.println(id);

#endif

        varioSqlFlight.deleteSite(id);
    }

    response = request->beginResponse(200, "text/plain", "OK");

    return response;
}

void VarioWebHandler::_doParseIgcAndInsert(void *parameter)
{
    const String ParsedPath PROGMEM = "/vols/parsed";
    uint32_t dataToSend = 0;

#ifdef WIFI_DEBUG
    SerialPort.println("_doParseIgcAndInsert");
#endif
    String path = String((char *)parameter);
    File dataFile;
    //test présence fichier
    if (dataFile = SDHAL_SD.open(path, FILE_READ))
    {
        String tmpFullName = dataFile.name();
        dataFile.close();

        //parsage du fichier IGC
        VarioIgcParser varioIgcParser(path);
        varioIgcParser.parseFile();

        VarioSqlFlight varioSqlFlight;
        if (varioSqlFlight.insertFlight(varioIgcParser.getIgcdata()))
        {
            String filename = tmpFullName.substring(tmpFullName.lastIndexOf("/") + 1);
#ifdef WIFI_DEBUG
            SerialPort.print("Path origine: ");
            SerialPort.println(path);

            SerialPort.print("Filename a deplacer: ");
            SerialPort.println(filename);
#endif
            if (!SDHAL_SD.exists(ParsedPath))
            {
                SDHAL_SD.mkdir(ParsedPath);
            }

            if (SDHAL_SD.exists(ParsedPath + "/" + filename))
            {
                SDHAL_SD.remove(ParsedPath + "/" + filename);
            }

            SDHAL_SD.rename(path, ParsedPath + "/" + filename);
#ifdef WIFI_DEBUG
            SerialPort.println("Deplacement du fichier termine");
#endif
            // response = request->beginResponse(200, "text/plain", "OK");
            // return response;
        }
        else
        {
#ifdef WIFI_DEBUG
            SerialPort.println("ECHEC de l insertion");
#endif
            dataToSend = 2;

            // response = request->beginResponse(500, "text/plain", "CANNOT INSERT FLIGHT");
            // return response;
        }

        varioIgcParser.~VarioIgcParser();
        varioSqlFlight.~VarioSqlFlight();
    }

    xQueueSend(xQueueParse, &dataToSend, 0);
    vTaskDelete(NULL);

    return;
}

igcdata VarioWebHandler::jsonToIgcdata(String data)
{
    igcdata myIgcData;

		GnuSettings.doc.clear();

//    DynamicJsonDocument doc(1024);
    DeserializationError err = deserializeJson(GnuSettings.doc, data);
    if (err)
    {
#ifdef SQL_DEBUG
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.c_str());
#endif //SQL_DEBUG

        return myIgcData;
    }

    if (GnuSettings.doc.containsKey("pilot"))
    {
        myIgcData.pilot = GnuSettings.doc["pilot"].as<String>();
    }
    if (GnuSettings.doc.containsKey("wing"))
    {
        myIgcData.wing = GnuSettings.doc["wing"].as<String>();
    }
    if (GnuSettings.doc.containsKey("flightDate"))
    {
        myIgcData.flightDate = GnuSettings.doc["flightDate"].as<String>();
    }
    if (GnuSettings.doc.containsKey("startFlightTime"))
    {
        myIgcData.startFlightTime = GnuSettings.doc["startFlightTime"].as<String>();
    }
    if (GnuSettings.doc.containsKey("endFlightTime"))
    {
        myIgcData.endFlightTime = GnuSettings.doc["endFlightTime"].as<String>();
    }
    if (GnuSettings.doc.containsKey("startHeight"))
    {
        myIgcData.startHeight = GnuSettings.doc["startHeight"];
    }
    if (GnuSettings.doc.containsKey("endHeight"))
    {
        myIgcData.endHeight = GnuSettings.doc["endHeight"];
    }
    if (GnuSettings.doc.containsKey("minHeight"))
    {
        myIgcData.minHeight = GnuSettings.doc["minHeight"];
    }
    if (GnuSettings.doc.containsKey("maxHeight"))
    {
        myIgcData.maxHeight = GnuSettings.doc["maxHeight"];
    }
    if (GnuSettings.doc.containsKey("startLat"))
    {
        myIgcData.startLat = GnuSettings.doc["startLat"];
    }
    if (GnuSettings.doc.containsKey("startLon"))
    {
        myIgcData.startLon = GnuSettings.doc["startLon"];
    }
    if (GnuSettings.doc.containsKey("endLat"))
    {
        myIgcData.endLat = GnuSettings.doc["endLat"];
    }
    if (GnuSettings.doc.containsKey("endLon"))
    {
        myIgcData.endLon = GnuSettings.doc["endLon"];
    }
    if (GnuSettings.doc.containsKey("md5"))
    {
        myIgcData.md5 = GnuSettings.doc["md5"].as<String>();
    }
    if (GnuSettings.doc.containsKey("filename"))
    {
        myIgcData.filename = GnuSettings.doc["filename"].as<String>();
    }
    if (GnuSettings.doc.containsKey("site_id"))
    {
        myIgcData.site_id = GnuSettings.doc["site_id"];
    }
    if (GnuSettings.doc.containsKey("comment"))
    {
        myIgcData.comment = GnuSettings.doc["comment"].as<String>();
    }

    return myIgcData;
}