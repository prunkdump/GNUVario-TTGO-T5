#ifndef VARIO_IGC_PARSER_H
#define VARIO_IGC_PARSER_H

#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

class VarioIgcParser
{

private:
    boolean isParsed = false;
    String pilot;
    String wing;
    String flightDate;
    String startFlightTime;
    String endFlightTime;
    int16_t startHeight;
    int16_t endHeight;
    int16_t minHeight;
    int16_t maxHeight;
    double_t startLat = -999;
    double_t startLon = -999;
    double_t endLat = -999;
    double_t endLon = -999;
    String md5;
    String filename;

public:
    VarioIgcParser();
    boolean parseFile(String path);
    String getPilot();
    String getWing();
    String getFlightDate();
    String getStartFlightTime();
    String getEndFlightTime();
    String getMD5();
    int16_t getStartHeight();
    int16_t getEndHeight();
    int16_t getMinHeight();
    int16_t getMaxHeight();
    double_t getStartLat();
    double_t getStartLon();
    double_t getEndLat();
    double_t getEndLon();
    String getJson();
    String getFilename();
};

#endif