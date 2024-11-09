#include "debug.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <mutex>
#include <map>
#include <thread>
#include <filesystem>
#include <sstream>

static map<string, mutex> file_mutexes; ///< Mutexes for each log file

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
void log(const string &file_name, int level, const string &message)
{
    // Get the mutex for the specific file to ensure thread safety
    lock_guard<mutex> guard(file_mutexes[file_name]);

    ofstream log_file;

    // Parse the file to get the directory structures
    // Create the directories if they don't exist
    filesystem::path path = file_name;
    filesystem::create_directories(path.parent_path());
    

    // Open the file in append mode
    log_file.open(file_name, ios::app);

    // Check if the file was opened successfully
    if (!log_file.is_open())
    {
        cerr << "Error opening log file: " << file_name << endl;
        return;
    }

    // Get current time and format it as a string
    time_t now = time(nullptr);
    char buf[20]; // YYYY-MM-DD HH:MM:SS
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Get thread ID
    thread::id thread_id = this_thread::get_id();
    ostringstream thread_id_stream;
    thread_id_stream << thread_id;

    // Map LogLevel to string
    string level_str;
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
    log_file << buf << " [" << thread_id_stream.str() << "] [" << level_str << "] " << message << endl;

    // Close the file
    log_file.close();
}
