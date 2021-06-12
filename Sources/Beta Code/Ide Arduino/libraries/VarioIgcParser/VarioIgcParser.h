#ifndef VARIO_IGC_PARSER_H
#define VARIO_IGC_PARSER_H

#include <HardwareConfig.h>
#include <VarioSettings.h>
#include <igcdata.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif

class VarioIgcParser
{

private:
    boolean isParsed = false;
    String filePath;
    struct igcdata myIgcData;
    void correctTimeZone(String &hms);

public:
    VarioIgcParser(String path);
    ~VarioIgcParser();
    boolean parseFile();
    String getFilename();
    igcdata getIgcdata();
};

extern VarioSettings GnuSettings;

#endif