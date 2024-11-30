/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @file bloom.cpp
 * @brief Implementation of the bloom_filter class.
 */

#include "bloom.h"

/**
 * @class bloom_filter
 * @brief A Bloom filter is a probabilistic data structure used for membership testing.
 *        It uses multiple hash functions to map elements to a fixed-size bit array,
 *        and it can tell if an element is definitely not in the set or might be in the set.
 */
bloom_filter::bloom_filter(long length) : table(length) {}

/**
 * @brief Destructor for bloom_filter class.
 *        It cleans up any resources used by the bloom filter.
 *        (No dynamic memory allocation to clean up, so this is a no-op here.)
 */
bloom_filter::~bloom_filter()
{
    ~table;
}

/**
 * @brief Sets the bits in the Bloom filter for a given key.
 *
 * This function computes the hashes of the key using three different hash functions
 * and sets the corresponding bits in the Bloom filter's table.
 *
 * @param key The string key to be added to the Bloom filter.
 */
void bloom_filter::set(const std::string &key)
{
    uint64_t k = string_to_uint64(key);

    table.set(hash_1(k));
    table.set(hash_2(k));
    table.set(hash_3(k));
}

/**
 * @brief Checks if a given key is possibly in the Bloom filter.
 *
 * This function checks the bits at the positions calculated by the three hash functions.
 * If all bits are set, the key might be in the filter. Otherwise, it is definitely not in the filter.
 *
 * @param key The string key to check in the Bloom filter.
 * @return true If the key might be in the filter (i.e., all corresponding bits are set).
 * @return false If the key is definitely not in the filter (i.e., one or more corresponding bits are unset).
 */
bool bloom_filter::is_set(const std::string &key) const
{
    uint64_t k = string_to_uint64(key);

    return table.test(hash_1(k)) && table.test(hash_2(k)) && table.test(hash_3(k));
}

/**
 * @brief Converts a string key to a uint64_t representation using a hash function.
 *
 * This function uses a hash function to convert a string key into a 64-bit unsigned integer.
 *
 * @param key The string key to be converted to a uint64_t.
 * @return uint64_t The resulting 64-bit hash value.
 */
uint64_t bloom_filter::string_to_uint64(const std::string &key) const
{
    return hasher(key);
}

/**
 * @brief Hash function 1 used by the Bloom filter.
 *
 * This hash function performs bit manipulation and arithmetic operations to produce
 * a hashed value based on the input key. The result is then modulo the size of the Bloom filter's table.
 *
 * @param key The 64-bit key to hash.
 * @return uint64_t The hash value for the key, modulo the size of the Bloom filter's table.
 */
uint64_t bloom_filter::hash_1(uint64_t key) const
{
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key ^ (key >> 16);

    return key % table.size();
}

/**
 * @brief Hash function 2 used by the Bloom filter.
 *
 * This hash function performs bit manipulation and arithmetic operations to produce
 * a second hashed value based on the input key. The result is then modulo the size of the Bloom filter's table.
 *
 * @param key The 64-bit key to hash.
 * @return uint64_t The hash value for the key, modulo the size of the Bloom filter's table.
 */
uint64_t bloom_filter::hash_2(uint64_t key) const
{
    key = (key + 0x7ed55d16) + (key << 12);
    key = (key ^ 0xc761c23c) ^ (key >> 19);
    key = (key + 0x165667b1) + (key << 5);
    key = (key + 0xd3a2646c) ^ (key << 9);
    key = (key + 0xfd7046c5) + (key << 3);
    key = (key ^ 0xb55a4f09) ^ (key >> 16);

    return key % table.size();
}

/**
 * @brief Hash function 3 used by the Bloom filter.
 *
 * This hash function performs bit manipulation and arithmetic operations to produce
 * a third hashed value based on the input key. The result is then modulo the size of the Bloom filter's table.
 *
 * @param key The 64-bit key to hash.
 * @return uint64_t The hash value for the key, modulo the size of the Bloom filter's table.
 */
uint64_t bloom_filter::hash_3(uint64_t key) const
{
    key = (key ^ 61) ^ (key >> 16);
    key = key + (key << 3);
    key = key ^ (key >> 4);
    key = key * 0x27d4eb2d;
    key = key ^ (key >> 15);

    return key % table.size();
}
