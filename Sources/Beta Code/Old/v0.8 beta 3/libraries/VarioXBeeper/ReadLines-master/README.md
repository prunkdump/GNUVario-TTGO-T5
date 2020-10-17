# ReadLines

An Arduino library for reading a file, line by line, from an SD card.

## Supported Boards

This library does not contain board-specific code, but it only been tested on

- [ESP8266 with Micro SD Card adapter](https://www.instructables.com/id/SD-Card-Module-With-ESP8266/)

If you have used it successfully on a different board, please indicate, so this list can be updated.

## Usage with inline callback function

```cpp
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
    RL.readLines("sample-file.txt", [](char* line, int index) {
        Serial.println(String(line) + " " + String(index));
    });
}
```

## Usage with pre-declared callback function

```cpp
/** add above setup code here */

void loop() {
    RL.readLines("sample-file.txt", &handleEachLine);
}

void handleEachLine(char line[], int lineIndex) {
    Serial.println(String(line) + " " + String(index));
}
```

## Usage (Method Overrides)

You may omit the 2nd argument `int lineIndex` in the `RL.readLines` callback function, so the definition for `handleEachLine` becomes:

```cpp
void handleEachLine(char line[]) {
    Serial.println(String(line));
}
```

You may also pass in `File` object as the 1st argument in the `RL.readLines` callback function, like:

```cpp
void loop() {
    File txtFile = SD.open("setup.txt");
    RL.readLines(txtFile, &handleEachLine);
    txtFile.close();
}

void handleEachLine(char line[]) {
    Serial.println(String(line));
}
```