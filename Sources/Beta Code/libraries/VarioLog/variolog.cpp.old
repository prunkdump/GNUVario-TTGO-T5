#include "Particle.h"

#include "SdCardLogHandlerRK.h"

// Define the debug logging level here
// 0 = Off
// 1 = Normal
// 2 = High
#define SDCARD_LOGHANDLER_DEBUG_LEVEL 1

// Don't change these, just change the debugging level above
// Note: must use Serial.printlnf here, not Log.info, as these are called from the log handler itself!
#if SDCARD_LOGHANDLER_DEBUG_LEVEL >= 1
#define DEBUG_NORMAL(x) Serial.printlnf x
#else
#define DEBUG_NORMAL(x)
#endif

#if SDCARD_LOGHANDLER_DEBUG_LEVEL >= 2
#define DEBUG_HIGH(x) Serial.printlnf x
#else
#define DEBUG_HIGH(x)
#endif




//
//
//

SdCardLogHandlerBuffer::SdCardLogHandlerBuffer(uint8_t *buf, size_t bufSize, SdFat &sd, uint8_t csPin, SPISettings spiSettings, LogLevel level, LogCategoryFilters filters) :
	StreamLogHandler(*this, level, filters), SdCardPrintHandler(sd, csPin, spiSettings), RingBuffer(buf, bufSize) {

	// This was the old default for SdCardLogHandler. The subclass SdCardPrintHandler now defaults to NULL.
	withWriteToStream(&Serial);
}

SdCardLogHandlerBuffer::~SdCardLogHandlerBuffer() {
}

void SdCardLogHandlerBuffer::setup() {
	// Add this handler into the system log manager
	LogManager::instance()->addHandler(this);
}

void SdCardLogHandlerBuffer::loop() {

	while(true) {
		uint8_t c;

		bool bResult = RingBuffer::read(&c);
		if (!bResult) {
			break;
		}
		SdCardPrintHandler::write(c);
	}
}


size_t SdCardLogHandlerBuffer::write(uint8_t c) {

	return RingBuffer::write(&c) ? 1 : 0;
}

//
//
//
SdCardPrintHandler::SdCardPrintHandler(SdFat &sd, uint8_t csPin, SPISettings spiSettings) : sd(sd), csPin(csPin), spiSettings(spiSettings){
}

SdCardPrintHandler::~SdCardPrintHandler() {

}


size_t SdCardPrintHandler::write(uint8_t c) {

	buf[bufOffset++] = c;
	if (bufOffset >= BUF_SIZE || c == '\n') {
		// Buffer is full or have the LF in CRLF, write it out
		writeBuf();
	}

	return 1;
}

void SdCardPrintHandler::scanCard() {
	DEBUG_HIGH(("scanCard"));
	needsScanCard = false;

	if (!lastBeginResult) {
		// Set the date time callback
		SdFile::dateTimeCallback(dateTimeCallback);

		pinMode(csPin, OUTPUT);
		lastBeginResult = sd.begin(csPin, spiSettings);
		if (!lastBeginResult) {
			DEBUG_HIGH(("sd.begin failed (no card or no reader)"));
			needsScanCard = true;
			lastCardCheck = millis();
			return;
		}
	}

	if (logsDirName != NULL && !sd.exists(logsDirName)) {
		DEBUG_NORMAL(("creating logs dir %s", logsDirName));
		if (!sd.mkdir(logsDirName)) {
			DEBUG_NORMAL(("mkdir failed"));
		}
	}

	if (logsDir.open(sd.vwd(), logsDirName, O_READ)) {
		DEBUG_HIGH(("opened logs dir %s", logsDirName));

		logsDir.rewind();

		FatFile tempFile;

		while (tempFile.openNext(&logsDir, O_READ)) {
			char name[64];
			tempFile.getName(name, sizeof(name));
			DEBUG_HIGH(("logs dir file %s", name));

			int num = atoi(name);
			if (num != 0) {
				fileNums.insert(num);
				if (num > lastFileNum) {
					lastFileNum = num;
				}
			}
			tempFile.close();
		}
		checkMaxFiles();

		if (!openLogFile()) {
			needsScanCard = true;
			lastCardCheck = millis();
		}
	}
	else {
		DEBUG_NORMAL(("failed to open logs dir"));
		needsScanCard = true;
		lastCardCheck = millis();
	}
}

const char *SdCardPrintHandler::getName(int num) {
	snprintf(nameBuf, sizeof(nameBuf), "%06u.txt", num);
	return nameBuf;
}

bool SdCardPrintHandler::openLogFile() {
	const char *name = getName(lastFileNum);
	if (curLogFile.open(&logsDir, name, O_RDWR | O_APPEND | O_CREAT)) {
		fileNums.insert(lastFileNum);
		DEBUG_HIGH(("using log file %s", name));
		return true;
	}
	else {
		DEBUG_HIGH(("failed to open log file %s", name));
		return false;
	}
}


void SdCardPrintHandler::checkMaxFiles() {
	auto it = fileNums.begin();

	while(fileNums.size() > maxFilesToKeep) {
		const char *name = getName(*it);
		DEBUG_NORMAL(("removing old log file %s", name));
		FatFile::remove(&logsDir, name);
		it = fileNums.erase(it);
	}
}


void SdCardPrintHandler::writeBuf() {

	if (writeToStream) {
		writeToStream->write(buf, bufOffset);
	}

	if (needsScanCard) {
		if (lastCardCheck == 0 || millis() - lastCardCheck >= cardCheckPeriod) {
			scanCard();
		}
	}

	if (lastBeginResult) {
		if (curLogFile.isOpen()) {
			if (curLogFile.write(buf, bufOffset) > 0) {
				if (syncEveryEntry) {
					curLogFile.sync();
				}

				if (curLogFile.fileSize() > desiredFileSize) {
					// File is too large now. Make a new one.
					curLogFile.close();
					lastFileNum++;
					DEBUG_NORMAL(("creating new log file %04d", lastFileNum));
					openLogFile();

					// Are there too many old files?
					checkMaxFiles();
				}
			}
			else {
				// Write failed
				DEBUG_NORMAL(("write to sd card failed"));
				curLogFile.close();
				logsDir.close();
				needsScanCard = true;
				lastBeginResult = false;
				lastCardCheck = 0;
			}
		}
	}

	// Start over at beginning of buffer
	bufOffset = 0;
}

// [static]
void SdCardPrintHandler::dateTimeCallback(uint16_t* date, uint16_t* time) {
	*date = FAT_DATE(Time.year(), Time.month(), Time.day());
	*time = FAT_TIME(Time.hour(), Time.minute(), Time.second());
}

