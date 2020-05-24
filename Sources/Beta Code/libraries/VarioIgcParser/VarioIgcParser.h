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
    String getJson();
    String getFilename();
};

#endif