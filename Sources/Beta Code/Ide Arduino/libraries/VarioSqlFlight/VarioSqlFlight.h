#ifndef VARIO_SQL_FLIGHT_H
#define VARIO_SQL_FLIGHT_H

#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

#include <RingBuf.h>
#include <sqlite3.h>
#include <igcdata.h>

class VarioSqlFlight
{

private:
    String dbPath;
    sqlite3 *myDb;
    boolean isOpened;
    boolean haveNextFlight;
    sqlite3_stmt *nextFlightRes;
    int openDb(const char *filename, sqlite3 **db);
    void closeDb();
    char *zErrMsg = 0;
    int db_exec(sqlite3 *db, const char *sql);
    const char *data = "Callback function called";
    static int callback(void *data, int argc, char **argv, char **azColName);

public:
    VarioSqlFlight();
    ~VarioSqlFlight();
    bool insertFlight(igcdata myIgcData);
    bool updateFlight(uint8_t id, igcdata myIgcData);
    bool delFlight(uint8_t id);
    bool updateFlightMap(uint8_t id, String data);
    String getSites();
    bool insertSite(String data);
    bool updateSite(uint8_t id, String data);
    bool deleteSite(uint8_t id);
    bool initGetFlightsQuery(uint16_t limit, uint16_t offset);
    //String getNextFlight();
    bool getNextFlight(bool &firstline, RingBuf<char, 1024> &buffer);
    void executeMigration(String version, String sql);
    String escapeJson(const String &s);
};
#endif