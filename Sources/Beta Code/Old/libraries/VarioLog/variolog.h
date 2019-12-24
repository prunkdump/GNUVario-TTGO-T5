#ifndef __SDCARDLOGHANDLERRK_H
#define __SDCARDLOGHANDLERRK_H

#include "Particle.h"
#include "RingBuffer.h"
#include "SdFat.h"

#include <set>


/**
 * @brief Class for writing a data stream to SD card
 *
 * You normally instantiate one of these as a global variable, passing in the SdFat object and the parameters
 * you'd normally pass to SdFat::begin().
 *
 * ~~~~{.c}
 * const int SD_CHIP_SELECT = A2;
 * SdFat sd;
 *
 * SdCardLogHandler logHandler(sd, SD_CHIP_SELECT, SPI_FULL_SPEED);
 * ~~~~
 *
 * You can pass additional options using the fluent-style methods beginning with "with" like withLogsDirName().
 *
 * This class is a subclass of Print, so you can use all of the overloads of print, println, and printf that are supported
 * by Print. Data is buffered until the \n, then written to the card, for performance reasons and to avoid splitting
 * a line between multiple files.
 */
class SdCardPrintHandler : public Print {
public:
	SdCardPrintHandler(SdFat &sd, uint8_t csPin, SPISettings spiSettings);
	virtual ~SdCardPrintHandler();

	/**
	 * @brief Sets the log directory name. Default: "logs"
	 *
	 * The logs must always be in a subdirectory, so make sure you set it to something, setting it to an empty string
	 * or NULL will disable logging.
	 *
	 * @param value The name to use instead of "logs" (const char *). This is not copied, as a constant string is normally
	 * passed in. If you calculate it, make sure you put it in a static or allocated buffer!
	 *
	 */
	inline SdCardPrintHandler &withLogsDirName(const char *value) { logsDirName = value; return *this; };

	/**
	 * @brief Desired file size in bytes. Default: 1000000 bytes (1 MB)
	 *
	 * Each log file will be approximately this size. It typically will be slightly larger than this, as the log is rotated
	 * when the size is exceeded. If changed, it will only be enforced for the current log file; old log files are not
	 * modified.
	 *
	 * @param value The maximum file size for a log file in bytes (size_t)
	 */
	inline SdCardPrintHandler &withDesiredFileSize(size_t value) { desiredFileSize = value; return *this; };

	/**
	 * @brief The number of files to keep. Default: 10
	 *
	 * The maximum number of log files to keep is enforced at startup, when a SD card is inserted, and when
	 * the current log file is full.
	 *
	 * @param value Number of files to kee. Values are 1 <= num <= 999999 (size_t)
	 */
	inline SdCardPrintHandler &withMaxFilesToKeep(size_t value) { maxFilesToKeep = value; return *this; };

	/**
	 * @brief The number of milliseconds to between checks to see if the SD card is present. Default: 10000
	 *
	 * When you remove the SD card, it's necessary to reinitialize the SdFat library and check for the card.
	 * Because of timeouts, this takes a while, so logging would become very slow if every attempt to log caused
	 * this delay. The card check period reduces the frequency of checking to see if a card has been inserted.
	 *
	 * Note that as long as you leave the card in, you won't experience this.

	 * @param value The time in milliseconds (unsigned ling)
	 */
	inline SdCardPrintHandler &withCardCheckPeriod(unsigned long value) { cardCheckPeriod = value; return *this; };

	/**
	 * @brief Set whether to sync the file system after every log entry. Default: true
	 *
	 * Setting this to false dramatically improves the performance, but it also makes it much more likely that
	 * in the case of a reboot, the last log messages will be lost. The SdFat library normally only flushes the
	 * file in 512 byte increments so if you log infrequently, you could lose a number of log messages.
	 *
	 * @param value The value to set (size_t)
	 */
	inline SdCardPrintHandler &withSyncEveryEntry(size_t value) { syncEveryEntry = value; return *this; };

	/**
	 * @brief The default is to log to Serial as well as SD card; to only log to SD card call this method.
	 *
	 * If you want to log to a different stream (like Serial1), use withWriteToStream() instead.
	 */
	inline SdCardPrintHandler &withNoSerialLogging() { writeToStream = NULL; return *this; };

	/**
	 * @brief Write to a different Stream, such as Serial1. Default: Serial
	 *
	 * @param value The stream to write log output to (such as &Serial1) or NULL to only write to the SD card.
	 *
	 * Only one stream is supported. Setting it again replaces the last setting.
	 */
	inline SdCardPrintHandler &withWriteToStream(Stream *value) { writeToStream = value; return *this; };


	/**
	 * @brief Virtual override for the StreamLogHandler to write data to the log
	 */
    virtual size_t write(uint8_t);

    /**
     * @brief Checks the result of the last time sd.begin() was called
     *
     * This call determines whether the last call to sd.begin() succeeded or not. You might do this if you also
     * need to call SdFat from your own code. This library needs to call sd.begin() internally to do it very
     * early, and also after the SD card is ejected.
     *
     * @return true if begin was successful
     */
    inline bool getLastBeginResult() const { return lastBeginResult; }

private:
    static const size_t BUF_SIZE = 128;  //!< size of buf[], the buffer to hold log messages. This improves write performance. Logs messages can be bigger than this.

    /**
     * Called to see if SD card is inserted, create the logs directory, and scan to see how many log files
     * there are. Removes excess log files of there are more than maxFilesToKeep.
     */
    void scanCard();

    /**
     * Writes the current buffer in buf of length bufOffset to the SD card then resets the bufOffset to 0
     *
     * If writeToStream is non-null, its write method is called to write out the buffer as well. This
     * default to &Serial, but you can set it to &Serial1, or other streams.
     */
    void writeBuf();

    /**
     * Formats the log file name. This is %06d.txt, and the string returned is a point to the shared
     * nameBuf member variables.
     */
    const char *getName(int num);

    /**
     * Opens the log file numbered lastFileNum using the pattern in getName
     */
    bool openLogFile();

    /**
     * Checks the number of log files in fileNums and if there are more than maxFilesToKeep, deletes the
     * lowest numbered.
     */
    void checkMaxFiles();

    /**
     * Callback passed to SdFat so it can determine the time for file timestamps
     *
     * Note: Timestamps will be in UTC not local time, and are unaffected by daylight saving.
     */
    static void dateTimeCallback(uint16_t* date, uint16_t* time);

    SdFat &sd; //!< The SdFat object (typically a globally allocated object passed into the constructor for this object
    uint8_t csPin; //!< The CS/SS pin for the SD card reader passed into the constructor for this object
    SPISettings spiSettings; //!< SPI_FULL_SPEED or SPI_HALF_SPEED passed into the constructor for this object

    const char *logsDirName = "logs"; //!< Name of the logs directory, override using withLogsDirName()
    size_t desiredFileSize = 1000000;  //!< Desired log file size, override using withDesiredFileSize()
    size_t maxFilesToKeep = 10; //!< Maximum number of log files to keep, override using withMaxFilesToKeep()
    unsigned long cardCheckPeriod = 10000; //!< How often to check when there's no SD card, override using withCardCheckPeriod (in milliseconds)
    bool syncEveryEntry = true; //!< Whether to sync the filesystem after each log entry. Override using withSyncEveryEntry().
    Stream *writeToStream = NULL; //!< Write to another Stream in addition to SD, override using withWriteToStream().

    size_t bufOffset = 0; //!< Offset we're currently writing to in buf
    uint8_t buf[BUF_SIZE];  //!< Buffer to hold partial log message.
    char nameBuf[12]; //!< Shared buffer returned by getName()
    bool needsScanCard = true; //!< true if we need to call scanCard() on the next log message
    bool lastBeginResult = false; //!< Last result from sd.begin(). Will be false if there's not a valid SD card inserted.
    int lastFileNum = 1; //!< Last file number found, and the one we're writing to.
	FatFile logsDir; //!< FatFile for the logs directory (logsDirName, default is "logs")
    FatFile curLogFile; //!< FatFile for the file we're currently writing to
	std::set<int> fileNums; //!< set of file numbers in the logs directory
	unsigned long lastCardCheck = 0; //!< millis() value at last time we checked for an SD card, see also cardCheckPeriod
};


/**
 * @brief Class for logging to SD card
 *
 * You normally instantiate one of these as a global variable, passing in the SdFat object and the parameters
 * you'd normally pass to SdFat::begin(). You can optionally pass the LogLevel and LogCategoryFilters parameters
 * you'd pass to the LogHandler constructor.
 *
 * ~~~~{.c}
 * const int SD_CHIP_SELECT = A2;
 * SdFat sd;
 *
 * SdCardLogHandler logHandler(sd, SD_CHIP_SELECT, SPI_FULL_SPEED);
 * ~~~~
 *
 * You can pass additional options using the fluent-style methods beginning with "with" like withLogsDirName().
 */
class SdCardLogHandlerBuffer : public StreamLogHandler, public SdCardPrintHandler, public RingBuffer<uint8_t> {
public:
	/**
	 * @brief Constructor. The object is normally instantiated as a global object.
	 *
	 * @param buf Ring buffer pointer
	 * @param bufSize Ring buffer size
	 * @param sd The SdFat object, normally allocated a global object.
	 * @param csPin The pin used for the SPI chip select for the SD card reader
	 * @param spiSettings Usually either SPI_FULL_SPEED or SPI_HALF_SPEED. You can also use a SPISettings object.
	 * @param level  (optional, default is LOG_LEVEL_INFO)
	 * @param filters (optional, default is none)
	 */
	SdCardLogHandlerBuffer(uint8_t *buf, size_t bufSize, SdFat &sd, uint8_t csPin, SPISettings spiSettings, LogLevel level = LOG_LEVEL_INFO, LogCategoryFilters filters = {});
	virtual ~SdCardLogHandlerBuffer();

	/**
	 * @brief Must be called from setup (added in 0.0.6)
	 *
	 * On mesh devices, it's not safe to set up the log handler at global object construction time and you will likely
	 * fault.
	 */
	void setup();

    /**
     * @brief Must be called from loop (added in 0.1.0)
     *
     * This method must be called from loop(), ideally on every call to loop. The reason is
     * that it's not really safe to call SPI from the log handler, and it's best to buffer the data and
     * write it out from loop to avoid SPI conflicts.
     */
    void loop();

	/**
	 * @brief Virtual override for the StreamLogHandler to write data to the log
	 */
    virtual size_t write(uint8_t);

protected:

};

template<size_t BUFFER_SIZE>
class SdCardLogHandler : public SdCardLogHandlerBuffer {
public:
	/**
	 * @brief Constructor. The object is normally instantiated as a global object.
	 *
	 * @param sd The SdFat object, normally allocated a global object.
	 * @param csPin The pin used for the SPI chip select for the SD card reader
	 * @param spiSettings Usually either SPI_FULL_SPEED or SPI_HALF_SPEED. You can also use a SPISettings object.
	 * @param level  (optional, default is LOG_LEVEL_INFO)
	 * @param filters (optional, default is none)
	 */
	explicit SdCardLogHandler(SdFat &sd, uint8_t csPin, SPISettings spiSettings, LogLevel level = LOG_LEVEL_INFO, LogCategoryFilters filters = {}) :
		SdCardLogHandlerBuffer(ringBuffer, sizeof(ringBuffer), sd, csPin, spiSettings, level, filters) {};

protected:
	uint8_t ringBuffer[BUFFER_SIZE];
};


#endif /* __SDCARDLOGHANDLERRK_H */
