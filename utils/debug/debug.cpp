#include "debug.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <mutex>
#include <map>
#include <thread>
#include <sstream>

static std::map<std::string, std::mutex> file_mutexes; ///< Mutexes for each log file

/**
 * @brief Logs a message to a specified file with a timestamp, thread ID, and log level.
 *
 * This function locks the appropriate mutex for the specified file to ensure
 * thread-safe writing. It opens the file in append mode, writes the current
 * timestamp, thread ID, log level, and message to the file, and then closes the file.
 *
 * If the file cannot be opened, an error message will be printed to standard error.
 * The function will only perform logging if the DEBUG macro is defined.
 *
 * Level is one of DEBUG, INFO, WARNING, or ERROR.
 * 
 * @param file_name The name of the file to log the message.
 * @param level The log level (DEBUG, INFO, WARNING, ERROR).
 * @param message The message to log.
 */
void log(const std::string &file_name, LogLevel level, const std::string &message)
{
    // Get the mutex for the specific file to ensure thread safety
    std::lock_guard<std::mutex> guard(file_mutexes[file_name]);

    std::ofstream log_file;

    // Open the file in append mode
    log_file.open(file_name, std::ios::app);

    // Check if the file was opened successfully
    if (!log_file.is_open())
    {
        std::cerr << "Error opening log file: " << file_name << std::endl;
        return;
    }

    // Get current time and format it as a string
    std::time_t now = std::time(nullptr);
    char buf[20]; // YYYY-MM-DD HH:MM:SS
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    // Get thread ID
    std::thread::id thread_id = std::this_thread::get_id();
    std::ostringstream thread_id_stream;
    thread_id_stream << thread_id;

    // Map LogLevel to string
    std::string level_str;
    switch (level)
    {
    case DEBUG:
        level_str = "DEBUG";
        break;
    case INFO:
        level_str = "INFO";
        break;
    case WARNING:
        level_str = "WARNING";
        break;
    case ERROR:
        level_str = "ERROR";
        break;
    default:
        level_str = "UNKNOWN";
        break;
    }

    // Write the timestamp, thread ID, level, and message to the file
    log_file << buf << " [" << thread_id_stream.str() << "] [" << level_str << "] " << message << std::endl;

    // Close the file
    log_file.close();
}
