// log.h

#ifndef LOG_H
#define LOG_H

#include <string>
#include <fstream>
#include <mutex>
#include <ctime>

/**
 * @class Logger
 * @brief Provides logging functionality with timestamps.
 */
class Logger {
public:
    /**
     * @brief Constructor that opens the log file.
     * @param filename Path to the log file.
     */
    Logger(const std::string& filename);

    /**
     * @brief Destructor that closes the log file.
     */
    ~Logger();

    /**
     * @brief Logs a message with a timestamp.
     * @param message The message to log.
     */
    void log(const std::string& message);

private:
    std::ofstream logFile; ///< Output file stream for logging.
    std::mutex logMutex;   ///< Mutex for thread-safe logging.

    /**
     * @brief Retrieves the current time as a string.
     * @return Current time in "YYYY-MM-DD HH:MM:SS" format.
     */
    std::string getCurrentTime() const;
};

#endif // LOG_H
