#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <mutex>
#include <map>

using namespace std;

/**
 * @brief The log levels that can be used to categorize log messages.
 */
#define DEBUG 0
#define INFO 1
#define WARNING 2
#define ERROR 3


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
void log(const string& file_name, int level, const string& message);

#endif // DEBUG_H
