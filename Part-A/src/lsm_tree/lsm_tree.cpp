/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include "lsm_tree.h"
#include <shared_mutex>

/**
 * @class lsm_tree
 * @brief Implements a Log-Structured Merge (LSM) Tree for key-value storage.
 *
 * The LSM Tree is a disk-based storage engine optimized for write-heavy workloads.
 * It uses a memtable for fast writes and flushes data to disk in segments (SSTs).
 * Periodically, segments are compacted to optimize read performance.
 */
lsm_tree::lsm_tree() : memtable(), wal(WAL_PATH)
{
    restore_db();
}

/**
 * @brief Destructor for the LSM Tree.
 *
 * This method ensures that the table is cleared and all resources are freed.
 */
lsm_tree::~lsm_tree()
{
    drop_table();
}

/**
 * @brief Inserts a key-value pair into the database.
 *
 * If the memtable exceeds the specified size (MEMTABLE_SIZE), it is flushed to disk,
 * a new segment is created, and segments are compacted. The operation is also logged
 * in the Write-Ahead Log (WAL), and the pair is inserted into the memtable.
 *
 * @param key The key to insert.
 * @param value The value associated with the key.
 */
void lsm_tree::put(const std::string &key, const std::string &value)
{

    std::unique_lock<std::shared_mutex> lock(tree_mutex);

    if (memtable.size() >= MEMTABLE_SIZE)
    {
        compact();
        flush_memtable_to_disk();
        wal.clear();
    }

    auto final_value = value;
    if (value == TOMBSTONE)
    {
        final_value = "";
    }

    kv_pair entry = {key, final_value};
    wal.append(entry.to_log_entry());
    memtable.insert(entry);
}

/**
 * @brief Retrieves a value associated with a key from the database.
 *
 * This method first checks the memtable for the key, and if not found, it searches
 * through the disk segments. If the key is found in any segment, the associated value
 * is returned.
 *
 * @param key The key to search for.
 * @return The value associated with the key, or an empty string if not found.
 */
std::string lsm_tree::get(const std::string &key)
{

    std::shared_lock<std::shared_mutex> lock(tree_mutex);

    std::optional<std::string> memtable_val = memtable.get(key);
    if (memtable_val.has_value())
    {
        return memtable_val.value() == TOMBSTONE ? "" : memtable_val.value();
    }

    std::optional<std::string> segment_val = search_all_segments(key);
    if (segment_val.has_value())
    {
        return segment_val.value() == TOMBSTONE ? "" : segment_val.value();
    }

    return "";
}

/**
 * @brief Marks a key as deleted by inserting the key with a special value.
 *
 * In the LSM Tree, deletion is achieved by inserting the key with the value
 * `TOMBSTONE`. This ensures that subsequent lookups will recognize the key as deleted.
 *
 * @param key The key to delete.
 */
void lsm_tree::remove(const std::string &key)
{

    put(key, TOMBSTONE);
}

/**
 * @brief Clears the database.
 *
 * This method deletes the memtable, clears the WAL, and removes all segments
 * from disk.
 */
void lsm_tree::drop_table()
{
    // std::unique_lock<std::shared_mutex> lock(tree_mutex);

    memtable.delete_tree();
    wal.clear();

    level::delete_all_segments(SEGMENT_BASE);
    segments.clear();
    segment_i = 0;
}

/**
 * @brief Flushes the memtable to disk as a new segment.
 *
 * When the memtable exceeds its size limit, its contents are flushed to a new
 * disk segment, sorted, and stored in the appropriate segment file.
 */
void lsm_tree::flush_memtable_to_disk()
{

    auto *sst = new level(get_new_segment_path(0), (long)memtable.size(), memtable);
    if (not segments.empty() and segments.front().first == 0)
    {
        segments.front().second.push_back(sst);
    }
    else
    {
        segments.push_front({0, {sst}});
    }
}

/**
 * @brief Compacts segments by merging them.
 *
 * This method iterates through the levels of segments, merging pairs of segments
 * at each level. The merged segments are pushed to the next higher level.
 */
void lsm_tree::compact()
{
    auto it = segments.begin();

    while (it != segments.end())
    {
        auto &curr_level = it->first;
        auto &level_segments = it->second;

        while (level_segments.size() >= 2)
        {
            level *sst_a = level_segments.back();
            level_segments.pop_back();

            level *sst_b = level_segments.back();
            level_segments.pop_back();

            auto *merged = new level(get_new_segment_path(curr_level + 1), sst_a, sst_b, (long)memtable.size() * (curr_level + 1) * 2);

            delete (sst_a);
            delete (sst_b);

            auto successor = std::next(it);
            if (successor != segments.end() and successor->first == curr_level + 1)
            {
                successor->second.push_back(merged);
            }
            else
            {
                segments.insert(successor, {curr_level + 1, {merged}});
            }
        }

        if (it->second.empty())
        {
            it = segments.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

/**
 * @brief Searches all disk segments for a key.
 *
 * This method searches through all the levels and segments on disk to find
 * a value associated with the given key.
 *
 * @param target The key to search for.
 * @return An optional containing the value if found, or an empty optional if not.
 */
std::optional<std::string> lsm_tree::search_all_segments(const std::string &target)
{
    for (const auto &curr_segment : segments)
    {
        for (const level *sst : curr_segment.second)
        {
            std::optional<std::string> val = sst->search(target);
            if (val.has_value())
            {
                return val;
            }
        }
    }

    return {};
}

/**
 * @brief Restores the database from disk upon restart.
 *
 * This method restores the state of the memtable and the segments from the
 * Write-Ahead Log and disk segments respectively.
 */
void lsm_tree::restore_db()
{
    std::unique_lock<std::shared_mutex> lock(tree_mutex);

    restore_memtable();
    restore_segments();
}

/**
 * @brief Restores the memtable from the Write-Ahead Log (WAL).
 *
 * This method reads the WAL and restores the memtable to its previous state
 * before the database shutdown.
 */
void lsm_tree::restore_memtable()
{
    wal.repopulate_memtable(memtable);
}

/**
 * @brief Restores all segments from disk.
 *
 * This method reads the disk and loads all segments into memory.
 */
void lsm_tree::restore_segments()
{
    auto last_segment_i_and_segments = level::collect_levels(SEGMENT_BASE, MEMTABLE_SIZE);
    segment_i = last_segment_i_and_segments.first + 1;
    segments = last_segment_i_and_segments.second;
}

/**
 * @brief Creates a new unique segment path.
 *
 * This method generates a new file path for the next segment, based on the
 * current segment index and level.
 *
 * @param level_order The level of the segment.
 * @return The new file path for the segment.
 */
std::string lsm_tree::get_new_segment_path(uint16_t level_order)
{
    return SEGMENT_BASE + level::create_filename_based_on_level(segment_i++, level_order) + ".sst";
}
