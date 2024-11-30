/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @file wal.cpp
 * @brief Implementation of the write_ahead_log class.
 */

#include "wal.h"

/**
 * @class write_ahead_log
 * @brief Represents a write-ahead log (WAL) for persisting operations before they are committed to the database.
 *
 * This class provides methods for appending new entries to the WAL, clearing it, and repopulating a memtable
 * from the WAL after a crash or restart.
 */
write_ahead_log::write_ahead_log(const std::string &filename)
{
    this->filename = filename;
    wal_file.open(filename, std::ios::in | std::ios::out | std::ios::app);
}

/**
 * @brief Destructor for the write_ahead_log class.
 *
 * This is the default destructor, which does not require any additional implementation.
 */
write_ahead_log::~write_ahead_log() = default;

/**
 * @brief Appends content to the write-ahead log file.
 *
 * This method writes a string content to the WAL file. It ensures the content is added to the end of the file.
 *
 * @param content The string content to be appended to the log.
 */
void write_ahead_log::append(const std::string &content)
{
    if (wal_file.is_open())
    {
        wal_file << content;
    }
}

/**
 * @brief Clears the content of the write-ahead log.
 *
 * This method truncates the WAL file to 0 size, effectively clearing all the entries stored in it.
 */
void write_ahead_log::clear()
{
    std::filesystem::resize_file(filename, 0);
}

/**
 * @brief Repopulates the memtable from the write-ahead log.
 *
 * This method reads the WAL file line by line, extracting key-value pairs, and inserts them into the given
 * red-black tree (memtable). This process is used to restore the state of the memtable after a crash or restart.
 *
 * @param memtable The red-black tree (memtable) to repopulate from the WAL.
 */
void write_ahead_log::repopulate_memtable(red_black_tree &memtable) const
{
    std::ifstream wal_tmp;
    wal_tmp.open(filename);

    std::string line;
    std::string key, value;

    while (std::getline(wal_tmp, line))
    {
        size_t seperator_pos = line.find(SEPERATOR);
        key = line.substr(0, seperator_pos);
        value = line.substr(seperator_pos + 1, line.size());
        if (value == TOMBSTONE)
        {
            memtable.remove(key);
        }
        else
        {
            memtable.insert(kv_pair{key, value});
        }
        memtable.insert(kv_pair{key, value});
    }

    wal_tmp.close();
}
