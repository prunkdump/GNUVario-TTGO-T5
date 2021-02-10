#include <SPI.h>
#include <SD.h>
#include <ReadLines.h>

char line1[RL_MAX_CHARS];

void setup() {
    Serial.begin(9600);

    while (!SD.begin(D8)) {
        Serial.println("initialization failed!");
        delay(1000);
    }
}

void loop() {
    RL.readLines("sample-file.txt", [](char* line) {
        Serial.println(String(line));
    });
}