#ifndef ReadLines_h
#define ReadLines_h

#include <Arduino.h>
#include <SD.h>

#define RL_MAX_CHARS 128

class ReadLines
{
    private:

        std::function<void (char*, int)> createLineHandler(void (*handler)(char*));

    public:

        ReadLines();

        int readLines(File file, std::function<void (char* line, int index)> handler);

        int readLines(File file, void (*handler)(char* line));

        int readLines(char *filePath, std::function<void (char* line, int index)> handler);

        int readLines(char *filePath, void (*handler)(char* line));
};

extern ReadLines RL;

#endif