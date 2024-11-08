#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <mutex>
#include <map>

/**
 * @enum LogLevel
 * @brief Enumeration for log levels.
 * 
 * This enum defines the various log levels that can be used when logging messages.
 */
enum LogLevel {
    DEBUG,   ///< Debug level for detailed diagnostic messages.
    INFO,    ///< Informational messages.
    WARNING, ///< Warning messages indicating potential issues.
    ERROR    ///< Error messages indicating failures or problems.
};

/**
 * @brief Logs a message to a specified file with a timestamp and thread ID.
 * 
 * This function appends a log entry to the specified file, including the current 
 * timestamp, the thread ID, and the log level. If the DEBUG macro is not defined, 
 * the function will return immediately without logging anything.
 * 
 * @param file_name The name of the file to which the log message will be appended.
 * @param level The log level of the message (e.g., DEBUG, INFO, WARNING, ERROR).
 * @param message The log message to be recorded.
 */
void log(const std::string& file_name, LogLevel level, const std::string& message);

#endif // DEBUG_H
