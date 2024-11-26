/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include "level.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>

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
level::level(const std::string &path, long bloom_size, red_black_tree &memtable)
    : bloom(bloom_size), index()
{
    this->path = path;
    create_sst_from_memtable(memtable);
}

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
level::level(const std::string &path, level *sst_a, level *sst_b, long bloom_size)
    : bloom(bloom_size), index()
{
    this->path = path;
    merge_sst_values(sst_a, sst_b);
}

/**
 * @brief Constructor for creating an SST from an existing segment file.
 *
 * This constructor is used when repopulating the SST data into memory after
 * a database restart. It repopulates the bloom filter and the index.
 *
 * @param path The path where the segment file is located.
 * @param bloom_size The size of the bloom filter.
 */
level::level(const std::string &path, long bloom_size)
    : bloom(bloom_size), index()
{
    this->path = path;
    repopulate_bloom_and_index();
}

/**
 * @brief Destructor for the level class.
 *
 * Cleans up resources by deleting the index and removing the segment file.
 */
level::~level()
{
    index.delete_tree();
    delete_segment_file();
}

/**
 * @brief Get the name (path) of the level.
 *
 * @return The path of the level.
 */
std::string level::get_name() const
{
    return path;
}

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
std::string level::create_filename_based_on_level(uint16_t id, uint16_t level_order)
{
    std::ostringstream ss;
    // Pad id and level_order to 00000 since uint16_t is max 65535.
    ss << std::setw(5) << std::setfill('0') << id << "_";
    ss << std::setw(5) << std::setfill('0') << level_order;
    return ss.str();
}

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
std::pair<uint16_t, uint16_t> level::extract_id_level_from_path(const std::string &path)
{
    std::string filename = path.substr(path.find_last_of("/\\") + 1);

    uint16_t id = (uint16_t)std::stoul(filename.substr(0, 5));
    uint16_t level_order = std::stoi(filename.substr(6, 5));

    return {id, level_order};
}

/**
 * @brief Create a new SST from the Memtable and write it to disk.
 *
 * This method iterates over the nodes of the Memtable, writes them to the SST file,
 * and populates the bloom filter and sparse index.
 *
 * @param memtable The Memtable whose data will be written to the SST.
 */
void level::create_sst_from_memtable(red_black_tree &memtable)
{
    std::ofstream sst;
    sst.open(path);

    uint64_t sparsity_i{0};
    uint64_t pos{0};

    std::vector<rb_entry> rb_nodes = memtable.get_and_delete_all_nodes();
    for (rb_entry &node : rb_nodes)
    {
        kv_pair pair = {node.key, std::any_cast<std::string>(node.val.value())};
        bloom.set(pair.key);

        std::string log_entry = pair.to_log_entry();
        sst << log_entry;

        if (sparsity_i++ == SPARSITY_FACTOR)
        {
            index.insert(rb_entry{pair.key, pos});
            sparsity_i = 0;
        }

        pos += log_entry.size();
    }
    sst << std::flush;
}

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
std::optional<std::string> level::search(const std::string &target) const
{
    if (not bloom.is_set(target))
    {
        return {};
    }

    std::optional<rb_entry> opt_floor_node = index.floor(target);
    long start_pos{0};

    if (opt_floor_node.has_value())
    {
        start_pos = (long)std::any_cast<uint64_t>(opt_floor_node.value().val.value());
    }

    std::ifstream sst(path, std::ios_base::in);
    if (sst.is_open())
    {
        sst.seekg(start_pos, std::ios_base::beg);
        std::string line, key;

        while (std::getline(sst, line))
        {
            size_t seperator_pos = line.find(SEPERATOR);
            key = line.substr(0, seperator_pos);

            if (target < key)
            {
                // Since SST is sorted, we can stop when key is greater than target.
                return {};
            }

            if (target == key)
            {
                return {line.substr(seperator_pos + 1, line.size())};
            }
        }
    }

    return {};
}

/**
 * @brief Merge two SSTs and write the merged content to a new SST.
 *
 * This method merges the data from two SSTs, ensuring that duplicates and deleted
 * entries are removed. The result is written to the current SST.
 *
 * @param sst_a Pointer to the first SST to be merged.
 * @param sst_b Pointer to the second SST to be merged.
 */
void level::merge_sst_values(level *sst_a, level *sst_b)
{
    // SST_a is always the latest SST and dominates over SST_b.
    if (extract_id_level_from_path(sst_a->path).first < extract_id_level_from_path(sst_b->path).first)
    {
        std::swap(sst_a, sst_b);
    }

    std::ifstream sst_a_file(sst_a->path, std::ios_base::in);
    std::ifstream sst_b_file(sst_b->path, std::ios_base::in);

    std::ofstream merged_sst;
    merged_sst.open(path);

    std::string last_key;
    uint64_t sparsity_i{0};
    uint64_t pos{0};

    std::string line_a, line_b;
    kv_pair kv_pair_a, kv_pair_b;
    kv_pair min_kv_pair;

    bool sst_a_empty = false;
    bool sst_b_empty = false;

    while (true)
    {

        if (line_a.empty() and not sst_a_empty)
        {
            std::getline(sst_a_file, line_a);
            sst_a_empty = line_a.empty();

            kv_pair_a = kv_pair::split_log_entry(line_a);
        }

        if (line_b.empty() and not sst_b_empty)
        {
            std::getline(sst_b_file, line_b);
            sst_b_empty = line_b.empty();

            kv_pair_b = kv_pair::split_log_entry(line_b);
        }

        if (kv_pair_a.empty() and kv_pair_b.empty())
        {
            break;
        }

        if (kv_pair_a.empty() and not kv_pair_b.empty())
        {
            min_kv_pair = kv_pair_b;
            line_b.clear();
        }
        else if (not kv_pair_a.empty() and kv_pair_b.empty())
        {
            min_kv_pair = kv_pair_a;
            line_a.clear();
        }
        else
        {
            if (kv_pair_a <= kv_pair_b)
            {
                min_kv_pair = kv_pair_a;
                line_a.clear();
            }
            else
            {
                min_kv_pair = kv_pair_b;
                line_b.clear();
            }
        }

        if (last_key == min_kv_pair.key)
        {
            continue;
        }
        else
        {
            last_key = min_kv_pair.key;
        }

        // std::cout << "NEXT PAIR: " << min_kv_pair.to_log_entry();
        bloom.set(min_kv_pair.key);

        if (sparsity_i++ == SPARSITY_FACTOR)
        {
            index.insert(rb_entry{min_kv_pair.key, pos});
            sparsity_i = 0;
        }

        std::string log_entry = min_kv_pair.to_log_entry();
        merged_sst << log_entry;

        pos += log_entry.size();
    }
    merged_sst << std::flush;
}

/**
 * @brief Repopulate the bloom filter and sparse index from an existing SST.
 *
 * This method reads the SST file, populates the bloom filter, and inserts entries
 * into the sparse index based on the file content.
 */
void level::repopulate_bloom_and_index()
{
    std::ifstream sst_file(path, std::ios_base::in);
    std::string line;

    uint64_t sparsity_i{0};
    uint64_t pos{0};

    while (std::getline(sst_file, line))
    {
        kv_pair curr_pair = kv_pair::split_log_entry(line);
        bloom.set(curr_pair.key);

        if (sparsity_i++ == SPARSITY_FACTOR)
        {
            index.insert(rb_entry{curr_pair.key, pos});
            sparsity_i = 0;
        }

        pos += line.size();
    }
}

/**
 * @brief Collect all stored segment files in the specified path and return a list
 *        of segments organized by their level hierarchy.
 *
 * @param path The directory path where segment files are stored.
 * @param memtable_size The size of the Memtable used for bloom filter size.
 *
 * @return A pair containing the largest segment ID and a list of segment levels.
 */
std::pair<uint16_t, std::list<std::pair<uint32_t, std::vector<level *>>>>
level::collect_levels(const std::string &path, uint64_t memtable_size)
{

    std::map<uint16_t, std::vector<level *>> cache{};

    uint16_t largest_id{0};

    for (const auto &segment_file : std::filesystem::directory_iterator(path))
    {
        std::string segment_path = segment_file.path().string();
        auto id_level = extract_id_level_from_path(segment_path);

        largest_id = std::max(largest_id, id_level.first);
        uint16_t level_order = id_level.second;

        auto *sst = new level(segment_path, (long)memtable_size * (level_order + 1) * 2);

        if (cache.contains(level_order))
        {
            cache[level_order].push_back(sst);
        }
        else
        {
            cache[level_order] = {sst};
        }
    }

    std::list<std::pair<uint32_t, std::vector<level *>>> segments;
    for (const auto &sst : cache)
    {
        segments.emplace_back(sst);
    }

    return {largest_id, segments};
}

/**
 * @brief Delete the segment file corresponding to the current level.
 *
 * This method removes the SST segment file from the filesystem.
 */
void level::delete_segment_file()
{
    std::filesystem::remove(path);
}

/**
 * @brief Delete all segment files in the specified directory.
 *
 * This method removes all SST segment files in the given directory.
 *
 * @param path The directory path where segment files are located.
 */
void level::delete_all_segments(const std::string &path)
{
    for (const auto &entry : std::filesystem::directory_iterator(path))
        std::filesystem::remove_all(entry.path());
}
