

#ifndef IGCDATA_H
#define IGCDATA_H

#include <Arduino.h>

struct igcdata
{
    String pilot = "";
    String wing = "";
    String flightDate = "";
    String startFlightTime = "";
    String endFlightTime = "";
    int16_t startHeight = 0;
    int16_t endHeight = 0;
    int16_t minHeight = 0;
    int16_t maxHeight = 0;
    double_t startLat = -999;
    double_t startLon = -999;
    double_t endLat = -999;
    double_t endLon = -999;
    String md5 = "";
    String filename = "";
    int16_t site_id = 0;
    String comment = "";
};

#endif