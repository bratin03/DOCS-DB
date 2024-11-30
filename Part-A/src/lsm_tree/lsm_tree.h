/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @mainpage DOCS-DB Documentation - Part A
 *
 * @section overview Overview
 * DOCS-DB is a high-performance, persistent key-value storage system designed for scalability and reliability. It offers efficient access to data through its robust architecture, which includes in-memory structures and disk-based storage mechanisms. DOCS-DB ensures high throughput and low latency, making it suitable for a wide range of applications, from small-scale systems to large distributed environments.
 *
 * @section architecture Architecture
 * DOCS-DB leverages an underlying Red-Black Tree (RB_tree) for memory management, providing efficient in-memory storage and fast lookups. To ensure data persistence, DOCS-DB utilizes a Log-Structured Merge Tree (LSM Tree) for on-disk storage, offering optimal write performance and efficient compaction strategies.
 *
 * @section features Key Features
 * - **Red-Black Tree (RB_tree)**: Used for efficient in-memory data storage and management. The RB_tree allows for logarithmic time complexity for key lookups, insertions, and deletions.
 * - **Log-Structured Merge Tree (LSM Tree)**: Employed for persistent storage, the LSM tree allows DOCS-DB to handle high write loads while providing efficient disk-based data retrieval.
 * - **Bloom Filter**: A probabilistic data structure used to quickly test whether a key exists in the database. This helps to reduce unnecessary disk lookups, thereby improving read performance.
 * - **Memtable**: An in-memory data structure that acts as a write buffer before data is flushed to disk. It helps to speed up write operations by allowing writes to be stored in memory temporarily.
 * - **Write-Ahead Log (WAL)**: Ensures data durability by logging all changes before they are written to the database. This prevents data loss in the event of a crash or system failure.
 * - **Stable Storage**: Ensures that data is eventually written to stable storage, providing fault tolerance and reliability.

 * @section performance Performance Considerations
 * DOCS-DB has been designed with performance as a top priority. By using the LSM tree combined with Bloom filters, memtables, and write-ahead logging, the system is optimized for both write-heavy and read-heavy workloads. The use of a Red-Black Tree in memory ensures that read operations are fast and efficient.
 *
 * @section usecases Use Cases
 * DOCS-DB is well-suited for scenarios where high write throughput is required, such as logging, time-series data, or event-driven architectures. It is also ideal for applications that need to support large-scale data with the need for efficient querying, such as caching layers or NoSQL database systems.
 *
 */

/**
 * @file lsm_tree.h
 * @brief Definition of the LSM tree class.
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
#include <shared_mutex>
#include <mutex>

/**
 * @brief Path definitions for internal storage.
 */
#define WAL_PATH "/tmp/docs-db/.internal_storage/wal.log"
#define SEGMENT_BASE "/tmp/docs-db/.internal_storage/segments/"

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

    /**
     * @brief Mutex for thread-safe access to the LSM tree.
     */
    mutable std::shared_mutex tree_mutex;
};

#endif // LSM_TREE_H
