/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#ifndef WAL_H
#define WAL_H

#include "../utils/types.h"
#include "../red_black_tree/red_black.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

/**
 * @class write_ahead_log
 * @brief A class that handles the write-ahead log (WAL) operations.
 *
 * The write-ahead log is used to ensure durability of the data by logging operations before they are
 * written to the main storage (e.g., database files). This class allows appending entries to the WAL,
 * clearing the log, and repopulating a memtable from the log in case of recovery.
 */
class write_ahead_log
{

public:
    /**
     * @brief Constructs a write_ahead_log object with a specified filename.
     *
     * Opens the log file in read/write mode. If the file does not exist, it will be created.
     *
     * @param filename The path to the write-ahead log file.
     */
    write_ahead_log(const std::string &filename);

    /**
     * @brief Destructor for the write_ahead_log class.
     *
     * Closes the WAL file when the object is destroyed.
     */
    ~write_ahead_log();

    /**
     * @brief Appends content to the write-ahead log.
     *
     * This method writes a string content to the end of the WAL file.
     *
     * @param content The content to append to the log.
     */
    void append(const std::string &content);

    /**
     * @brief Clears the content of the write-ahead log.
     *
     * Truncates the WAL file to zero size, effectively removing all entries from the log.
     */
    void clear();

    /**
     * @brief Repopulates the memtable from the write-ahead log.
     *
     * This method reads the WAL file and inserts key-value pairs into the provided
     * red-black tree (memtable), restoring the state of the memtable after a crash or restart.
     *
     * @param memtable The red-black tree (memtable) to repopulate.
     */
    void repopulate_memtable(red_black_tree &memtable) const;

private:
    std::string filename;   ///< The path to the WAL file.
    std::ofstream wal_file; ///< The stream object used for writing to the WAL file.
};

#endif // WAL_H
