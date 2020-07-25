#include "ReadLines.h"

// private

std::function<void (char*, int)> ReadLines::createLineHandler(void (*handler)(char*)) {
    return [&handler](char *line, int) {
        handler(line);
    };
}

// public

ReadLines::ReadLines() {

}

int ReadLines::readLines(File file, std::function<void (char*, int)> handler) {
  if (file) {
    int noOfLines = 0;
    while (file.available()) {
      char line[RL_MAX_CHARS];
      char c = file.read();
      char cIndex = 0;
      while (c != '\n' && cIndex < RL_MAX_CHARS) {
        line[cIndex] = c;
        line[cIndex + 1] = '\0';
        cIndex++;
        if (!file.available()) break;
        c = file.read();
      }
      handler(line, noOfLines);
      noOfLines++;
    }
    return noOfLines;
  }
  else {
    return -1;
  }
}

int ReadLines::readLines(File file, void (*handler)(char*)) {
    return readLines(file, createLineHandler(handler));
}

int ReadLines::readLines(char *filePath, std::function<void (char*, int)> handler) {
    File file = SD.open(filePath);
    int noOfLines = readLines(file, handler);
    file.close();
    return noOfLines;
}

int ReadLines::readLines(char *filePath, void (*handler)(char*)) {
    return readLines(filePath, createLineHandler(handler));
}

ReadLines RL;