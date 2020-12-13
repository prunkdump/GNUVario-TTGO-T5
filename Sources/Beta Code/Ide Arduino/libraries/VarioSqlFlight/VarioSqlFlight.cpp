#include <Arduino.h>

#include <HardwareConfig.h>
#include <DebugConfig.h>

#ifdef WIFI_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

#include "VarioSqlFlight.h"
#include <ArduinoJson.h>

VarioSqlFlight::VarioSqlFlight()
{

#ifdef SQL_DEBUG
    SerialPort.println("VarioSqlFlight");
#endif //SQL_DEBUG

    dbPath = "/sd/db/vol.db";
    isOpened = false;

#ifdef SQL_DEBUG
    SerialPort.println("sqlite3_initialize");
#endif //SQL_DEBUG

    sqlite3_initialize();

    haveNextFlight = false;
}

VarioSqlFlight::~VarioSqlFlight()
{
    closeDb();
    sqlite3_shutdown();
}

int VarioSqlFlight::callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    Serial.printf("%s: ", (const char *)data);
    for (i = 0; i < argc; i++)
    {
        Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    Serial.printf("\n");
    return 0;
}

int VarioSqlFlight::openDb(const char *filename, sqlite3 **db)
{
    int rc = sqlite3_open(filename, db);
    if (rc)
    {
#ifdef SQL_DEBUG
        Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
#endif //SQL_DEBUG
        return rc;
    }
    else
    {
#ifdef SQL_DEBUG
        Serial.printf("Opened database successfully\n");
#endif //SQL_DEBUG
    }
    isOpened = true;
    return rc;
}

void VarioSqlFlight::closeDb()
{
    if (isOpened)
    {
#ifdef SQL_DEBUG
        Serial.printf("close db");
#endif //SQL_DEBUG
        sqlite3_close(myDb);
        isOpened = false;
    }
}

int VarioSqlFlight::db_exec(sqlite3 *db, const char *sql)
{
#ifdef SQL_DEBUG
    Serial.println(sql);
    long start = micros();
#endif //SQL_DEBUG
    int rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("SQL error: %s\n", zErrMsg);
#endif //SQL_DEBUG
        sqlite3_free(zErrMsg);
    }
    else
    {
#ifdef SQL_DEBUG
        Serial.printf("Operation done successfully\n");
#endif //SQL_DEBUG
    }
#ifdef SQL_DEBUG
    Serial.print(F("Time taken:"));
    Serial.println(micros() - start);
#endif //SQL_DEBUG
    return rc;
}

bool VarioSqlFlight::insertFlight(igcdata myIgcData)
{
#ifdef MEMORY_DEBUG
    Serial.println("Free heap debut insertFlight");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    // #ifdef SQL_DEBUG
    //     SerialPort.println(data);
    // #endif //SQL_DEBUG

#ifdef MEMORY_DEBUG
    Serial.println("Free heap insertFlight fin deserialization");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    int rc;
    sqlite3_stmt *res;
    const char *tail;

#ifdef SQL_DEBUG
    SerialPort.println("openDb");
#endif //SQL_DEBUG

    if (!isOpened)
    {
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return false;
        }
    }

#ifdef MEMORY_DEBUG
    Serial.println("Free heap insertFlight fin opendb");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    String sql;

    if (myIgcData.filename != "" && myIgcData.md5 != "")
    {
        sql = "DELETE FROM flight WHERE filename = ? AND md5 = ?";

        rc = sqlite3_prepare_v2(myDb, (char *)sql.c_str(), sql.length(), &res, &tail);
        if (rc != SQLITE_OK)
        {
#ifdef SQL_DEBUG
            Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
            closeDb();
            return false;
        }

#ifdef SQL_DEBUG
        SerialPort.println("Début binding");
#endif //SQL_DEBUG

        sqlite3_bind_text(res, 1, (char *)myIgcData.filename.c_str(), myIgcData.filename.length(), SQLITE_STATIC);
        sqlite3_bind_text(res, 2, (char *)myIgcData.md5.c_str(), myIgcData.md5.length(), SQLITE_STATIC);

        if (sqlite3_step(res) != SQLITE_DONE)
        {
            closeDb();
            return false;
        }

        sqlite3_clear_bindings(res);

        rc = sqlite3_reset(res);
        if (rc != SQLITE_OK)
        {
            closeDb();
            return false;
        }

        rc = sqlite3_finalize(res);
        if (rc != SQLITE_OK)
        {
#ifdef SQL_DEBUG
            SerialPort.println("cannot finalize");
#endif //SQL_DEBUG
            closeDb();
            return false;
        }
    }

#ifdef MEMORY_DEBUG
    Serial.println("Free heap fin delete  insertFlight");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    sql = "INSERT INTO flight (";
    sql = sql + "pilot, wing, flight_date, start_flight_time, end_flight_time, start_height, end_height, min_height, max_height, start_lat, start_lon, end_lat, end_lon";

    if (myIgcData.filename != "" && myIgcData.md5 != "")
    {
        sql = sql + ", filename, md5";
    }
    else
    {
        sql = sql + ", site_id, comment";
    }

    sql = sql + ") VALUES (?, ?, ? ,?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    rc = sqlite3_prepare_v2(myDb, (char *)sql.c_str(), sql.length(), &res, &tail);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    SerialPort.println("Début binding");
#endif //SQL_DEBUG

    sqlite3_bind_text(res, 1, (char *)myIgcData.pilot.c_str(), myIgcData.pilot.length(), SQLITE_STATIC);
    sqlite3_bind_text(res, 2, (char *)myIgcData.wing.c_str(), myIgcData.wing.length(), SQLITE_STATIC);
    sqlite3_bind_text(res, 3, (char *)myIgcData.flightDate.c_str(), myIgcData.flightDate.length(), SQLITE_STATIC);
    sqlite3_bind_text(res, 4, (char *)myIgcData.startFlightTime.c_str(), myIgcData.startFlightTime.length(), SQLITE_STATIC);
    sqlite3_bind_text(res, 5, (char *)myIgcData.endFlightTime.c_str(), myIgcData.endFlightTime.length(), SQLITE_STATIC);
    sqlite3_bind_int(res, 6, myIgcData.startHeight);
    sqlite3_bind_int(res, 7, myIgcData.endHeight);
    sqlite3_bind_int(res, 8, myIgcData.minHeight);
    sqlite3_bind_int(res, 9, myIgcData.maxHeight);
    sqlite3_bind_double(res, 10, myIgcData.startLat);
    sqlite3_bind_double(res, 11, myIgcData.startLon);
    sqlite3_bind_double(res, 12, myIgcData.endLat);
    sqlite3_bind_double(res, 13, myIgcData.endLon);
    if (myIgcData.filename != "" && myIgcData.md5 != "")
    {
        sqlite3_bind_text(res, 14, (char *)myIgcData.filename.c_str(), myIgcData.filename.length(), SQLITE_STATIC);
        sqlite3_bind_text(res, 15, (char *)myIgcData.md5.c_str(), myIgcData.md5.length(), SQLITE_STATIC);
    }
    else
    {
        sqlite3_bind_int(res, 14, myIgcData.site_id);
        sqlite3_bind_text(res, 15, (char *)myIgcData.comment.c_str(), myIgcData.comment.length(), SQLITE_STATIC);
    }

#ifdef SQL_DEBUG
    Serial.printf("Début step");
#endif //SQL_DEBUG

    if (sqlite3_step(res) != SQLITE_DONE)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
    sqlite3_clear_bindings(res);
    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("reset failed");
#endif //SQL_DEBUG
        closeDb();
        return false;
    }
#ifdef SQL_DEBUG
    Serial.printf("sqlite3_finalize");
#endif //SQL_DEBUG
    sqlite3_finalize(res);

    return true;
}

bool VarioSqlFlight::delFlight(uint8_t id)
{
#ifdef SQL_DEBUG
    SerialPort.println(id);
#endif //SQL_DEBUG
    int rc;
    sqlite3_stmt *res;
    const char *tail;

#ifdef SQL_DEBUG
    SerialPort.println("openDb");
#endif //SQL_DEBUG

    if (!isOpened)
    {
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return false;
        }
    }

    String sql = F("DELETE FROM flight WHERE id = ?");

    rc = sqlite3_prepare_v2(myDb, (char *)sql.c_str(), sql.length(), &res, &tail);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    SerialPort.println("Début binding");
#endif //SQL_DEBUG

    sqlite3_bind_int(res, 1, id);

#ifdef SQL_DEBUG
    Serial.printf("Début step");
#endif //SQL_DEBUG

    if (sqlite3_step(res) != SQLITE_DONE)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
    sqlite3_clear_bindings(res);
    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("reset failed");
#endif //SQL_DEBUG
        closeDb();
        return false;
    }
#ifdef SQL_DEBUG
    Serial.printf("sqlite3_finalize");
#endif //SQL_DEBUG
    sqlite3_finalize(res);

    return true;
}

bool VarioSqlFlight::updateFlight(uint8_t id, igcdata myIgcData)
{
#ifdef SQL_DEBUG
    SerialPort.println("updateFlight");
    SerialPort.println(data);
#endif //SQL_DEBUG

    int rc;
    sqlite3_stmt *res;
    const char *tail;

#ifdef SQL_DEBUG
    SerialPort.println("openDb");
#endif //SQL_DEBUG

    if (!isOpened)
    {
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return false;
        }
    }

    String sql = F("UPDATE flight SET site_id = ?, comment = ? ");

    if (myIgcData.pilot != "")
    {
        sql = sql + ", pilot = ?, wing = ?, flight_date = ?, start_flight_time = ?, end_flight_time = ?, start_height = ?, end_height = ?, min_height = ?, max_height = ?, start_lat = ?, start_lon = ?, end_lat = ?, end_lon = ? ";
    }

    sql = sql + " WHERE id = ?";

    rc = sqlite3_prepare_v2(myDb, (char *)sql.c_str(), sql.length(), &res, &tail);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    SerialPort.println("Début binding");
#endif //SQL_DEBUG

    sqlite3_bind_int(res, 1, myIgcData.site_id);
    sqlite3_bind_text(res, 2, (char *)myIgcData.comment.c_str(), myIgcData.comment.length(), SQLITE_STATIC);

    if (myIgcData.pilot != "")
    {
        sqlite3_bind_text(res, 3, (char *)myIgcData.pilot.c_str(), myIgcData.pilot.length(), SQLITE_STATIC);
        sqlite3_bind_text(res, 4, (char *)myIgcData.wing.c_str(), myIgcData.wing.length(), SQLITE_STATIC);
        sqlite3_bind_text(res, 5, (char *)myIgcData.flightDate.c_str(), myIgcData.flightDate.length(), SQLITE_STATIC);
        sqlite3_bind_text(res, 6, (char *)myIgcData.startFlightTime.c_str(), myIgcData.startFlightTime.length(), SQLITE_STATIC);
        sqlite3_bind_text(res, 7, (char *)myIgcData.endFlightTime.c_str(), myIgcData.endFlightTime.length(), SQLITE_STATIC);
        sqlite3_bind_int(res, 8, myIgcData.startHeight);
        sqlite3_bind_int(res, 9, myIgcData.endHeight);
        sqlite3_bind_int(res, 10, myIgcData.minHeight);
        sqlite3_bind_int(res, 11, myIgcData.maxHeight);
        sqlite3_bind_double(res, 12, myIgcData.startLat);
        sqlite3_bind_double(res, 13, myIgcData.startLon);
        sqlite3_bind_double(res, 14, myIgcData.endLat);
        sqlite3_bind_double(res, 15, myIgcData.endLon);
        sqlite3_bind_int(res, 16, id);
    }
    else
    {
        sqlite3_bind_int(res, 3, id);
    }

#ifdef SQL_DEBUG
    Serial.printf("Début step");
#endif //SQL_DEBUG

    if (sqlite3_step(res) != SQLITE_DONE)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
    sqlite3_clear_bindings(res);
    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("reset failed");
#endif //SQL_DEBUG
        closeDb();
        return false;
    }
#ifdef SQL_DEBUG
    Serial.printf("sqlite3_finalize");
#endif //SQL_DEBUG
    sqlite3_finalize(res);

    return true;
}

bool VarioSqlFlight::updateFlightMap(uint8_t id, String data)
{
    int rc;
    sqlite3_stmt *res;
    const char *tail;

#ifdef SQL_DEBUG
    SerialPort.println("openDb");
#endif //SQL_DEBUG

    if (!isOpened)
    {
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return false;
        }
    }

    String sql = F("UPDATE flight SET minimap = ? WHERE id = ?");

    rc = sqlite3_prepare_v2(myDb, (char *)sql.c_str(), sql.length(), &res, &tail);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    SerialPort.println("Début binding");
#endif //SQL_DEBUG

    sqlite3_bind_blob(res, 1, (char *)data.c_str(), strlen((char *)data.c_str()), SQLITE_STATIC);
    sqlite3_bind_int(res, 2, id);

#ifdef SQL_DEBUG
    Serial.printf("Début step");
#endif //SQL_DEBUG

    if (sqlite3_step(res) != SQLITE_DONE)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
    sqlite3_clear_bindings(res);
    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("reset failed");
#endif //SQL_DEBUG
        closeDb();
        return false;
    }
#ifdef SQL_DEBUG
    Serial.printf("sqlite3_finalize");
#endif //SQL_DEBUG
    sqlite3_finalize(res);

    return true;
}

String VarioSqlFlight::getSites()
{
    String output = "";
    DynamicJsonDocument doc(4096);

    int rc;
    sqlite3_stmt *res;
    const char *tail;

#ifdef SQL_DEBUG
    SerialPort.println("openDb");
#endif //SQL_DEBUG

    if (!isOpened)
    {
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return output;
        }
    }

    String sql = F("SELECT id, lib, comment, lat, lon FROM site");

    rc = sqlite3_prepare_v2(myDb, sql.c_str(), 1000, &res, &tail);
    if (rc != SQLITE_OK)
    {
        closeDb();
        return output;
    }

    while (sqlite3_step(res) == SQLITE_ROW)
    {
        // rec_count = sqlite3_column_int(res, 0);
        // if (rec_count > 5000)
        // {
        //     sqlite3_finalize(res);
        //     return;
        // }
        //doc.add(i);
        JsonObject obj1 = doc.createNestedObject();
        obj1["id"] = sqlite3_column_int(res, 0);
        obj1["lib"] = String((char *)sqlite3_column_text(res, 1));
        obj1["comment"] = String((char *)sqlite3_column_text(res, 2));
        obj1["lat"] = sqlite3_column_double(res, 3);
        obj1["lon"] = sqlite3_column_double(res, 4);
    }
    serializeJson(doc, output);
    sqlite3_finalize(res);
    return output;
}

bool VarioSqlFlight::insertSite(String data)
{
#ifdef SQL_DEBUG
    SerialPort.println(data);
#endif //SQL_DEBUG

    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, data);
    if (err)
    {
#ifdef SQL_DEBUG
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.c_str());
#endif //SQL_DEBUG

        return false;
    }

    int rc;
    sqlite3_stmt *res;
    const char *tail;

#ifdef SQL_DEBUG
    SerialPort.println("openDb");
#endif //SQL_DEBUG

    if (!isOpened)
    {
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return false;
        }
    }

    String sql = F("INSERT INTO site (lib, comment, lat, lon) VALUES (?, ?, ? ,?)");

    rc = sqlite3_prepare_v2(myDb, (char *)sql.c_str(), sql.length(), &res, &tail);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    SerialPort.println("Début binding");
#endif //SQL_DEBUG

    sqlite3_bind_text(res, 1, doc["lib"], strlen(doc["lib"]), SQLITE_STATIC);
    sqlite3_bind_text(res, 2, doc["comment"], strlen(doc["comment"]), SQLITE_STATIC);
    sqlite3_bind_double(res, 3, doc["lat"]);
    sqlite3_bind_double(res, 4, doc["lon"]);

#ifdef SQL_DEBUG
    Serial.printf("Début step");
#endif //SQL_DEBUG

    if (sqlite3_step(res) != SQLITE_DONE)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
    sqlite3_clear_bindings(res);
    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("reset failed");
#endif //SQL_DEBUG
        closeDb();
        return false;
    }
#ifdef SQL_DEBUG
    Serial.printf("sqlite3_finalize");
#endif //SQL_DEBUG
    sqlite3_finalize(res);

    return true;
}

bool VarioSqlFlight::updateSite(uint8_t id, String data)
{
#ifdef SQL_DEBUG
    SerialPort.println(data);
#endif //SQL_DEBUG

    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, data);
    if (err)
    {
#ifdef SQL_DEBUG
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.c_str());
#endif //SQL_DEBUG

        return false;
    }

    int rc;
    sqlite3_stmt *res;
    const char *tail;

#ifdef SQL_DEBUG
    SerialPort.println("openDb");
#endif //SQL_DEBUG

    if (!isOpened)
    {
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return false;
        }
    }

    String sql = F("UPDATE site SET lib = ?, comment = ?, lat = ?, lon = ? WHERE id = ?");

    rc = sqlite3_prepare_v2(myDb, (char *)sql.c_str(), sql.length(), &res, &tail);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    SerialPort.println("Début binding");
#endif //SQL_DEBUG

    sqlite3_bind_text(res, 1, doc["lib"], strlen(doc["lib"]), SQLITE_STATIC);
    sqlite3_bind_text(res, 2, doc["comment"], strlen(doc["comment"]), SQLITE_STATIC);
    sqlite3_bind_double(res, 3, doc["lat"]);
    sqlite3_bind_double(res, 4, doc["lon"]);
    sqlite3_bind_int(res, 5, id);

#ifdef SQL_DEBUG
    Serial.printf("Début step");
#endif //SQL_DEBUG

    if (sqlite3_step(res) != SQLITE_DONE)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
    sqlite3_clear_bindings(res);
    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("reset failed");
#endif //SQL_DEBUG
        closeDb();
        return false;
    }
#ifdef SQL_DEBUG
    Serial.printf("sqlite3_finalize");
#endif //SQL_DEBUG
    sqlite3_finalize(res);

    return true;
}

bool VarioSqlFlight::deleteSite(uint8_t id)
{
#ifdef SQL_DEBUG
    SerialPort.println(id);
#endif //SQL_DEBUG

    int rc;
    sqlite3_stmt *res;
    const char *tail;

#ifdef SQL_DEBUG
    SerialPort.println("openDb");
#endif //SQL_DEBUG

    if (!isOpened)
    {
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return false;
        }
    }

    String sql = F("DELETE FROM site WHERE id = ?");

    rc = sqlite3_prepare_v2(myDb, (char *)sql.c_str(), sql.length(), &res, &tail);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    SerialPort.println("Début binding");
#endif //SQL_DEBUG

    sqlite3_bind_int(res, 1, id);

#ifdef SQL_DEBUG
    Serial.printf("Début step");
#endif //SQL_DEBUG

    if (sqlite3_step(res) != SQLITE_DONE)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

#ifdef SQL_DEBUG
    Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
    sqlite3_clear_bindings(res);
    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("reset failed");
#endif //SQL_DEBUG
        closeDb();
        return false;
    }
#ifdef SQL_DEBUG
    Serial.printf("sqlite3_finalize");
#endif //SQL_DEBUG
    sqlite3_finalize(res);

    return true;
}

bool VarioSqlFlight::initGetFlightsQuery(uint16_t limit, uint16_t offset)
{
#ifdef MEMORY_DEBUG
    Serial.println("Free heap initGetFlightsQuery");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    haveNextFlight = false;

    int rc;
    // sqlite3_stmt *res;
    const char *tail;

    if (!isOpened)
    {
#ifdef SQL_DEBUG
        SerialPort.println("openDb");
#endif //SQL_DEBUG
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return false;
        }
    }

#ifdef MEMORY_DEBUG
    Serial.println("Free heap initGetFlightsQuery after opendb");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    //String sql = F("SELECT f.id, f.site_id, f.filename, f.md5, f.pilot, f.wing, f.flight_date, f.start_flight_time, f.end_flight_time, f.start_height, f.end_height, f.min_height, f.max_height, f.start_lat, f.start_lon, f.end_lat, f.end_lon, f.comment, f.minimap, s.lib FROM flight f LEFT JOIN site s ON(s.id = f.site_id) ORDER BY f.flight_date DESC, f.start_flight_time ASC LIMIT ?  OFFSET ?");
    //String sql = F("SELECT f.id, f.site_id, f.filename, f.md5, f.pilot, f.wing, f.flight_date, f.start_flight_time, f.end_flight_time, f.start_height, f.end_height, f.min_height, f.max_height, f.start_lat, f.start_lon, f.end_lat, f.end_lon, f.comment, f.minimap, 'toto' AS lib FROM flight f");
    String sql = F("SELECT f.id, f.site_id, f.filename,  f.pilot, f.wing, f.flight_date, f.start_flight_time, f.end_flight_time, f.start_height, f.end_height, f.min_height, f.max_height, f.start_lat, f.start_lon, f.end_lat, f.end_lon, f.comment, s.lib FROM flight f LEFT JOIN site s ON(s.id = f.site_id) LIMIT ?  OFFSET ?");

#ifdef SQL_DEBUG
    // SerialPort.println(sql);
#endif //SQL_DEBUG

    rc = sqlite3_prepare_v2(myDb, sql.c_str(), sql.length(), &nextFlightRes, &tail);

    sqlite3_bind_int(nextFlightRes, 1, limit);
    sqlite3_bind_int(nextFlightRes, 2, offset);

    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return false;
    }

    haveNextFlight = true;

#ifdef MEMORY_DEBUG
    Serial.println("Free heap FIN initGetFlightsQuery");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    return true;
}

// String VarioSqlFlight::getNextFlight()
// {
// #ifdef SQL_DEBUG
//     SerialPort.println("getNextFlight");
// #endif //SQL_DEBUG

//     DynamicJsonDocument doc(4096);

//     // create an object
//     JsonObject obj1 = doc.to<JsonObject>();

//     int step_res;

//     if (haveNextFlight)
//     {
//         step_res = sqlite3_step(nextFlightRes);

//         if (step_res == SQLITE_ROW)
//         {
//             //JsonObject obj1 = doc.createNestedObject();
//             obj1["id"] = sqlite3_column_int(nextFlightRes, 0);
//             obj1["site_id"] = sqlite3_column_int(nextFlightRes, 1);
//             obj1["filename"] = String((char *)sqlite3_column_text(nextFlightRes, 2));
//             obj1["md5"] = String((char *)sqlite3_column_text(nextFlightRes, 3));
//             obj1["pilot"] = String((char *)sqlite3_column_text(nextFlightRes, 4));
//             obj1["wing"] = String((char *)sqlite3_column_text(nextFlightRes, 5));
//             obj1["flight_date"] = String((char *)sqlite3_column_text(nextFlightRes, 6));
//             obj1["start_flight_time"] = String((char *)sqlite3_column_text(nextFlightRes, 7));
//             obj1["end_flight_time"] = String((char *)sqlite3_column_text(nextFlightRes, 8));
//             obj1["start_height"] = sqlite3_column_double(nextFlightRes, 9);
//             obj1["end_height"] = sqlite3_column_double(nextFlightRes, 10);
//             obj1["min_height"] = sqlite3_column_double(nextFlightRes, 11);
//             obj1["max_height"] = sqlite3_column_double(nextFlightRes, 12);
//             obj1["start_lat"] = sqlite3_column_double(nextFlightRes, 13);
//             obj1["start_lon"] = sqlite3_column_double(nextFlightRes, 14);
//             obj1["end_lat"] = sqlite3_column_double(nextFlightRes, 15);
//             obj1["end_lon"] = sqlite3_column_double(nextFlightRes, 16);
//             obj1["comment"] = String((char *)sqlite3_column_text(nextFlightRes, 17));
//             obj1["minimap"] = String((char *)sqlite3_column_text(nextFlightRes, 18));
//             obj1["site_lib"] = String((char *)sqlite3_column_text(nextFlightRes, 19));

//             String unvol;
//             serializeJson(obj1, unvol);

// #ifdef SQL_DEBUG
//             // Serial.println(unvol);
// #endif //SQL_DEBUG
//             return unvol;
//         }
//         else if (step_res == SQLITE_ERROR)
//         {
// #ifdef SQL_DEBUG
//             Serial.println("ERROR ICI");
// #endif //SQL_DEBUG
//             haveNextFlight = false;
//             sqlite3_clear_bindings(nextFlightRes);
//             sqlite3_finalize(nextFlightRes);
//             closeDb();
//             return "";
//         }
//         else
//         {
// #ifdef SQL_DEBUG
//             Serial.println("Plus aucun vol" + String(step_res));
// #endif //SQL_DEBUG
//             haveNextFlight = false;
//             sqlite3_clear_bindings(nextFlightRes);
//             sqlite3_finalize(nextFlightRes);
//             closeDb();
//             return "";
//         }
//     }
//     else
//     {
//         sqlite3_clear_bindings(nextFlightRes);
//         sqlite3_finalize(nextFlightRes);
//         closeDb();
// #ifdef SQL_DEBUG
//         Serial.println("Aucun vol");
// #endif //SQL_DEBUG
//         return "";
//     }
// }

bool VarioSqlFlight::getNextFlight(bool &firstline, RingBuf<char, 1024> &buffer)
{
#ifdef SQL_DEBUG
    SerialPort.println("getNextFlight");
#endif //SQL_DEBUG

#ifdef MEMORY_DEBUG
    Serial.println("Free heap getNextFlight");
    Serial.println(ESP.getFreeHeap());
#endif //MEMORY_DEBUG

    // DynamicJsonDocument doc(4096);

    // // create an object
    // JsonObject obj1 = doc.to<JsonObject>();

    int step_res;

    if (haveNextFlight)
    {
        step_res = sqlite3_step(nextFlightRes);

        if (step_res == SQLITE_ROW)
        {
            // //JsonObject obj1 = doc.createNestedObject();
            // obj1["id"] = sqlite3_column_int(nextFlightRes, 0);
            // obj1["site_id"] = sqlite3_column_int(nextFlightRes, 1);
            // obj1["filename"] = String((char *)sqlite3_column_text(nextFlightRes, 2));
            // obj1["md5"] = String((char *)sqlite3_column_text(nextFlightRes, 3));
            // obj1["pilot"] = String((char *)sqlite3_column_text(nextFlightRes, 4));
            // obj1["wing"] = String((char *)sqlite3_column_text(nextFlightRes, 5));
            // obj1["flight_date"] = String((char *)sqlite3_column_text(nextFlightRes, 6));
            // obj1["start_flight_time"] = String((char *)sqlite3_column_text(nextFlightRes, 7));
            // obj1["end_flight_time"] = String((char *)sqlite3_column_text(nextFlightRes, 8));
            // obj1["start_height"] = sqlite3_column_double(nextFlightRes, 9);
            // obj1["end_height"] = sqlite3_column_double(nextFlightRes, 10);
            // obj1["min_height"] = sqlite3_column_double(nextFlightRes, 11);
            // obj1["max_height"] = sqlite3_column_double(nextFlightRes, 12);
            // obj1["start_lat"] = sqlite3_column_double(nextFlightRes, 13);
            // obj1["start_lon"] = sqlite3_column_double(nextFlightRes, 14);
            // obj1["end_lat"] = sqlite3_column_double(nextFlightRes, 15);
            // obj1["end_lon"] = sqlite3_column_double(nextFlightRes, 16);
            // obj1["comment"] = String((char *)sqlite3_column_text(nextFlightRes, 17));
            // obj1["minimap"] = String((char *)sqlite3_column_text(nextFlightRes, 18));
            // obj1["site_lib"] = String((char *)sqlite3_column_text(nextFlightRes, 19));

            char unvol[2048] = "";

            String tmp;
            strcat(unvol, "{");

            tmp = "\"id\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 0)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"site_id\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 1)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"filename\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 2)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            // tmp = "\"md5\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 3)));
            // strcat(unvol, tmp.c_str());
            // strcat(unvol, ", ");

            tmp = "\"pilot\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 3)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"wing\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 4)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"flight_date\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 5)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"start_flight_time\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 6)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"end_flight_time\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 7)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"start_height\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 8)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"end_height\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 9)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"min_height\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 10)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"max_height\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 11)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"start_lat\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 12)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"start_lon\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 13)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"end_lat\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 14)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"end_lon\" : " + escapeJson(String(sqlite3_column_int(nextFlightRes, 15)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            tmp = "\"comment\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 16)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, ", ");

            // tmp = "\"minimap\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 18)));
            // strcat(unvol, tmp.c_str());
            // strcat(unvol, ", ");

            tmp = "\"site_lib\" : " + escapeJson(String((char *)sqlite3_column_text(nextFlightRes, 17)));
            strcat(unvol, tmp.c_str());
            strcat(unvol, "}");

            // serializeJson(obj1, unvol);

#ifdef SQL_DEBUG
            Serial.print("unvol : ");
            Serial.println(unvol);
#endif //SQL_DEBUG
            if (!firstline)
            {
                buffer.push(',');
            }
            else
            {
                firstline = false;
            }

            int i = 0;
            while (unvol[i] != '\0')
            {
                // SerialPort.print("enqueue");
                // SerialPort.println(unvol[i]);
                buffer.push(unvol[i]);
                i++;
            }
            // SerialPort.println("ICICICICICICIC");
            return true;
        }
        else if (step_res == SQLITE_ERROR)
        {
#ifdef SQL_DEBUG
            Serial.println("ERROR ICI");
#endif //SQL_DEBUG
            haveNextFlight = false;
            sqlite3_clear_bindings(nextFlightRes);
            sqlite3_finalize(nextFlightRes);
            closeDb();
            return false;
        }
        else
        {
#ifdef SQL_DEBUG
            Serial.print("Plus aucun vol");
            Serial.println(step_res);
#endif //SQL_DEBUG
            haveNextFlight = false;
            sqlite3_clear_bindings(nextFlightRes);
            sqlite3_finalize(nextFlightRes);
            closeDb();
            return false;
        }
    }
    else
    {
        sqlite3_clear_bindings(nextFlightRes);
        sqlite3_finalize(nextFlightRes);
        closeDb();
#ifdef SQL_DEBUG
        Serial.println("Aucun vol");
#endif //SQL_DEBUG
        return false;
    }
}

void VarioSqlFlight::executeMigration(String version, String sql)
{
    int rc;
    sqlite3_stmt *res;
    const char *tail;

    if (!isOpened)
    {
        if (openDb((char *)dbPath.c_str(), &myDb))
        {
            return;
        }
    }

    String sqlSelect = "SELECT COUNT(version) AS NB FROM version WHERE version = ?";

    rc = sqlite3_prepare_v2(myDb, sqlSelect.c_str(), 1000, &res, &tail);
    if (rc != SQLITE_OK)
    {
        closeDb();
        return;
    }
    sqlite3_bind_text(res, 1, (char *)version.c_str(), version.length(), SQLITE_STATIC);

    while (sqlite3_step(res) == SQLITE_ROW)
    {
#ifdef SQL_DEBUG
        Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
        if (sqlite3_column_int(res, 0) == 1)
        {
#ifdef SQL_DEBUG
            Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
            sqlite3_clear_bindings(res);
            rc = sqlite3_reset(res);
            if (rc != SQLITE_OK)
            {
                closeDb();
            }
            sqlite3_finalize(res);
            return;
        }
    }

    sqlite3_clear_bindings(res);

    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK)
    {
        closeDb();
        return;
    }

    sqlite3_finalize(res);

    rc = sqlite3_prepare_v2(myDb, sql.c_str(), 1000, &res, &tail);
    if (rc != SQLITE_OK)
    {
        closeDb();
        return;
    }

#ifdef SQL_DEBUG
    Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG

    if (sqlite3_step(res) != SQLITE_DONE)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return;
    }

    sqlite3_finalize(res);

    //insertion de l'enregistrement de version
    String sql2 = "INSERT INTO version (version) VALUES (?)";

    rc = sqlite3_prepare_v2(myDb, (char *)sql2.c_str(), sql2.length(), &res, &tail);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR preparing sql: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return;
    }

#ifdef SQL_DEBUG
    SerialPort.println("Début binding");
#endif //SQL_DEBUG

    sqlite3_bind_text(res, 1, (char *)version.c_str(), version.length(), SQLITE_STATIC);

#ifdef SQL_DEBUG
    Serial.printf("Début step");
#endif //SQL_DEBUG

    if (sqlite3_step(res) != SQLITE_DONE)
    {
#ifdef SQL_DEBUG
        Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(myDb));
#endif //SQL_DEBUG
        closeDb();
        return;
    }

#ifdef SQL_DEBUG
    Serial.printf("sqlite3_clear_bindings");
#endif //SQL_DEBUG
    sqlite3_clear_bindings(res);
    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK)
    {
#ifdef SQL_DEBUG
        Serial.printf("reset failed");
#endif //SQL_DEBUG
        closeDb();
        return;
    }
#ifdef SQL_DEBUG
    Serial.printf("sqlite3_finalize");
#endif //SQL_DEBUG
    sqlite3_finalize(res);
}

String VarioSqlFlight::escapeJson(const String &s)
{
    if (s == NULL || s.length() == 0)
    {
        return "\"\"";
    }

    char c = 0;
    int i;
    int len = s.length();
    String sb;
    String t;

    sb.concat('"');
    for (i = 0; i < len; i += 1)
    {
        c = s.charAt(i);

        switch (c)
        {
        case '\\':
        case '"':
            sb.concat('\\');
            sb.concat(c);
            break;
        case '/':
            //                if (b == '<') {
            sb.concat('\\');
            //                }
            sb.concat(c);
            break;
        case '\b':
            sb.concat("\\b");
            break;
        case '\t':
            sb.concat("\\t");
            break;
        case '\n':
            sb.concat("\\n");
            break;
        case '\f':
            sb.concat("\\f");
            break;
        case '\r':
            sb.concat("\\r");
            break;
        default:
            if (c < ' ')
            {
                t = "000" + String(c, HEX); // Integer.toHexString(c);
                sb.concat("\\u" + t.substring(t.length() - 4));
            }
            else
            {
                sb.concat(c);
            }
        }
    }

    sb.concat('"');

    return sb;
}
