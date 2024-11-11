/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/


#ifndef LSM_TREE_H
#define LSM_TREE_H

#include "../utils/types.h"
#include "../bloom_filter/bloom.h"
#include "../red_black_tree/red_black.h"
#include "../wal/wal.h"
#include "level/level.h"
#include <string>
#include <list>
#include <vector>
#include <optional>

/**
 * @brief Path definitions for internal storage.
 */
#define WAL_PATH "../src/.internal_storage/wal.log"
#define SEGMENT_BASE "../src/.internal_storage/segments/"

/**
 * @class lsm_tree
 * @brief Implements a Log-Structured Merge Tree (LSM Tree) for efficient data storage and retrieval.
 *
 * This class is a basic implementation of an LSM tree, which organizes data into
 * levels and uses a write-ahead log (WAL) for durability. The tree stores data in memory
 * (memtable) and periodically flushes to disk as segments. It supports basic operations
 * like putting, getting, and removing key-value pairs, along with dropping the table.
 */
class lsm_tree
{
public:
    /**
     * @brief Default constructor for the LSM tree.
     *
     * Initializes the LSM tree with an empty memtable and WAL. Prepares the internal
     * structures for storing data and managing segments.
     */
    lsm_tree();

    /**
     * @brief Destructor for the LSM tree.
     *
     * Cleans up any resources used by the LSM tree, including flushing the memtable
     * to disk and closing the write-ahead log.
     */
    ~lsm_tree();

    /**
     * @brief Insert or update a key-value pair in the LSM tree.
     *
     * If the key already exists, the value is updated. If the memtable size exceeds
     * the limit, it is flushed to disk.
     *
     * @param key The key to insert or update.
     * @param value The value to associate with the key.
     */
    void put(const std::string &key, const std::string &value);

    /**
     * @brief Retrieve the value associated with a key.
     *
     * Searches through the memtable, segments, and WAL to find the most recent value
     * for the given key.
     *
     * @param key The key whose associated value is to be retrieved.
     * @return The value associated with the key.
     */
    std::string get(const std::string &key);

    /**
     * @brief Remove a key-value pair from the LSM tree.
     *
     * Marks the key for deletion in the memtable and WAL. The key will also be purged
     * from disk in the next compaction.
     *
     * @param key The key to remove.
     */
    void remove(const std::string &key);

    /**
     * @brief Drop all data in the LSM tree.
     *
     * Deletes all entries from the memtable, WAL, and segments, effectively resetting
     * the LSM tree.
     */
    void drop_table();

private:
    /**
     * @brief The maximum size of the memtable in bytes (64 MB).
     */
    static const uint64_t MEMTABLE_SIZE{67108864};

    /**
     * @brief The memtable stores key-value pairs in memory.
     */
    red_black_tree memtable;

    /**
     * @brief A list of segments at various levels, representing the on-disk storage.
     */
    std::list<std::pair<uint32_t, std::vector<level *>>> segments;

    /**
     * @brief The write-ahead log (WAL) for durability.
     */
    write_ahead_log wal;

    /**
     * @brief The current segment index.
     */
    uint16_t segment_i{0};

    /**
     * @brief Flush the memtable to disk as a new segment.
     *
     * This method writes the data in the memtable to disk, creating a new segment.
     * It also increments the segment index and manages WAL writes.
     */
    void flush_memtable_to_disk();

    /**
     * @brief Restore the database from disk.
     *
     * This method restores the state of the LSM tree from the disk, including the
     * segments and the memtable.
     */
    void restore_db();

    /**
     * @brief Restore the memtable from disk.
     *
     * This method restores the memtable from the disk, which is crucial for
     * recovering data between sessions.
     */
    void restore_memtable();

    /**
     * @brief Restore the segments from disk.
     *
     * This method loads the segments stored on disk into memory for future access.
     */
    void restore_segments();

    /**
     * @brief Compact the segments in the LSM tree.
     *
     * This method performs compaction by merging smaller segments into larger ones
     * to optimize space and retrieval times.
     */
    void compact();

    /**
     * @brief Search for a key across all segments.
     *
     * This method checks all segments on disk to find the most recent version of
     * a given key.
     *
     * @param target The key to search for.
     * @return The value associated with the key, if found.
     */
    std::optional<std::string> search_all_segments(const std::string &target);

    /**
     * @brief Generate a new path for a segment at a specific level.
     *
     * This method generates a path for the new segment in the given level of the
     * LSM tree, based on the current segment index.
     *
     * @param level_order The level number for the segment.
     * @return The path where the new segment will be stored.
     */
    std::string get_new_segment_path(uint16_t level_order);
};

#endif // LSM_TREE_H
