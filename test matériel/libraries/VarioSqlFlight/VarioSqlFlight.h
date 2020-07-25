#ifndef VARIO_SQL_FLIGHT_H
#define VARIO_SQL_FLIGHT_H

#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

#include <sqlite3.h>

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
    bool insertFlight(String data);
    bool updateFlight(uint8_t id, String data);
    bool delFlight(uint8_t id);
    bool updateFlightMap(uint8_t id, String data);
    String getSites();
    bool insertSite(String data);
    bool updateSite(uint8_t id, String data);
    bool deleteSite(uint8_t id);
    bool initGetFlightsQuery(uint8_t limit, uint8_t offset);
    String getNextFlight();
    void executeMigration(String version, String sql);
};
#endif