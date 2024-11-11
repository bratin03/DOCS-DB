/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#ifndef LEVEL_H
#define LEVEL_H

#include "../../bloom_filter/bloom.h"
#include "../../red_black_tree/red_black.h"
#include "../../utils/types.h"
#include <string>
#include <queue>
#include <ios>
#include <filesystem>

/**
 * @class level
 * @brief Represents a level in the LSM tree where data is stored in SST files.
 *
 * The `level` class provides the functionality for creating, managing, and searching
 * SST files. It supports operations such as creating a new SST from a Memtable,
 * merging two SST files, and searching for data in the SST.
 */
class level
{

public:
    /**
     * @brief Constructor for creating a new SST from the flushed Memtable.
     *
     * Initializes the bloom filter and the index. Populates the SST file with data
     * from the Memtable.
     *
     * @param path The path where the SST file will be stored.
     * @param bloom_size The size of the bloom filter.
     * @param memtable The Memtable from which the SST will be created.
     */
    level(const std::string &path, long bloom_size, red_black_tree &memtable);

    /**
     * @brief Constructor for merging two existing SSTs into a new one.
     *
     * Merges data from two SSTs and writes the merged content to the new SST file.
     *
     * @param path The path where the merged SST will be stored.
     * @param sst_a Pointer to the first SST to be merged.
     * @param sst_b Pointer to the second SST to be merged.
     * @param bloom_size The size of the bloom filter.
     */
    level(const std::string &path, level *sst_a, level *sst_b, long bloom_size);

    /**
     * @brief Constructor for creating an SST from an existing segment file.
     *
     * This constructor is used when repopulating the SST data into memory after
     * a database restart. It repopulates the bloom filter and the index.
     *
     * @param path The path where the segment file is located.
     * @param bloom_size The size of the bloom filter.
     */
    level(const std::string &path, long bloom_size);

    /**
     * @brief Destructor for the level class.
     *
     * Cleans up resources by deleting the index and removing the segment file.
     */
    ~level();

    /**
     * @brief Search for a key in the SST.
     *
     * This method first checks the bloom filter. If the key is likely to exist, it uses
     * the sparse index to find a nearby position and searches from there.
     *
     * @param target The key to search for.
     *
     * @return The value associated with the key if found, otherwise an empty optional.
     */
    std::optional<std::string> search(const std::string &target) const;

    /**
     * @brief Create a filename based on the segment ID and the level of the segment.
     *
     * The filename will be formatted as "00001_00010" where the first part is
     * the segment ID and the second part is the level order.
     *
     * @param id The segment ID.
     * @param level_order The level order of the segment.
     *
     * @return The generated filename as a string.
     */
    static std::string create_filename_based_on_level(uint16_t id, uint16_t level_order);

    /**
     * @brief Extract the segment ID and level from the path.
     *
     * The segment filename is expected to be in the format "00001_00010". This
     * function extracts the ID and level order from that format.
     *
     * @param path The path of the segment file.
     *
     * @return A pair containing the segment ID and level order.
     */
    static std::pair<uint16_t, uint16_t> extract_id_level_from_path(const std::string &path);

    /**
     * @brief Collect all stored segment files in the specified path and return a list
     *        of segments organized by their level hierarchy.
     *
     * @param path The directory path where segment files are stored.
     * @param memtable_size The size of the Memtable used for bloom filter size.
     *
     * @return A pair containing the largest segment ID and a list of segment levels.
     */
    static std::pair<uint16_t, std::list<std::pair<uint32_t, std::vector<level *>>>> collect_levels(const std::string &path, uint64_t memtable_size);

    /**
     * @brief Delete all segment files in the specified directory.
     *
     * This method removes all SST segment files in the given directory.
     *
     * @param path The directory path where segment files are located.
     */
    static void delete_all_segments(const std::string &path);

    /**
     * @brief Get the name (path) of the level.
     *
     * @return The path of the level.
     */
    std::string get_name() const;

private:
    static const uint64_t SPARSITY_FACTOR{128}; ///< The sparsity factor for the index

    std::string path;     ///< The path to the SST file for this level
    bloom_filter bloom;   ///< The bloom filter used for this level
    red_black_tree index; ///< The sparse index for this level

    /**
     * @brief Create a new SST from the Memtable and write it to disk.
     *
     * This method iterates over the nodes of the Memtable, writes them to the SST file,
     * and populates the bloom filter and sparse index.
     *
     * @param memtable The Memtable whose data will be written to the SST.
     */
    void create_sst_from_memtable(red_black_tree &memtable);

    /**
     * @brief Merge two SSTs and write the merged content to a new SST.
     *
     * This method merges the data from two SSTs, ensuring that duplicates and deleted
     * entries are removed. The result is written to the current SST.
     *
     * @param sst_a Pointer to the first SST to be merged.
     * @param sst_b Pointer to the second SST to be merged.
     */
    void merge_sst_values(level *sst_a, level *sst_b);

    /**
     * @brief Repopulate the bloom filter and sparse index from an existing SST.
     *
     * This method reads the SST file, populates the bloom filter, and inserts entries
     * into the sparse index based on the file content.
     */
    void repopulate_bloom_and_index();

    /**
     * @brief Delete the segment file corresponding to the current level.
     *
     * This method removes the SST segment file from the filesystem.
     */
    void delete_segment_file();
};

#endif // LEVEL_H
