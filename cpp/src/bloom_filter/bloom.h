/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#ifndef BLOOM_H
#define BLOOM_H

#include "../utils/types.h"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <string>

/**
 * @class bloom_filter
 * @brief A Bloom filter is a probabilistic data structure that tests whether an element is a member of a set.
 *
 * This Bloom filter uses three hash functions to set bits in a dynamic bitset. It offers a trade-off
 * between space and accuracy, allowing a small chance of false positives but no false negatives.
 */
class bloom_filter
{
    boost::dynamic_bitset<> table; ///< The bitset used to represent the Bloom filter's table.
    std::hash<std::string> hasher; ///< The hash function used to convert strings to uint64_t.

public:
    /**
     * @brief Constructor for the bloom_filter class.
     *
     * Initializes the Bloom filter with a bitset of the specified length.
     *
     * @param length The size of the Bloom filter (the number of bits in the bitset).
     */
    bloom_filter(long length);

    /**
     * @brief Destructor for the bloom_filter class.
     *
     * Cleans up the Bloom filter resources. In this case, no special cleanup is required as the bitset is
     * automatically managed.
     */
    ~bloom_filter();

    /**
     * @brief Adds a key to the Bloom filter by setting the corresponding bits.
     *
     * This function computes hashes of the key using three different hash functions and sets the corresponding
     * bits in the Bloom filter's bitset.
     *
     * @param key The string key to add to the Bloom filter.
     */
    void set(const std::string &key);

    /**
     * @brief Checks if a key is possibly in the Bloom filter.
     *
     * This function checks if all the bits corresponding to the hashes of the key are set. If they are all set,
     * the key might be in the filter. If any bit is not set, the key is definitely not in the filter.
     *
     * @param key The string key to check.
     * @return true If the key might be in the filter (i.e., all corresponding bits are set).
     * @return false If the key is definitely not in the filter (i.e., one or more corresponding bits are unset).
     */
    bool is_set(const std::string &key) const;

private:
    /**
     * @brief Converts a string key into a 64-bit unsigned integer.
     *
     * This function uses a hash function to generate a 64-bit integer from the string key.
     *
     * @param key The string key to be converted to a uint64_t.
     * @return uint64_t The 64-bit integer representation of the key.
     */
    uint64_t string_to_uint64(const std::string &key) const;

    /**
     * @brief Hash function 1 used by the Bloom filter.
     *
     * This function performs bit manipulation and arithmetic operations on the key to produce a hash value.
     * The resulting hash is then modulo the size of the Bloom filter's table.
     *
     * @param key The 64-bit key to hash.
     * @return uint64_t The hashed value modulo the size of the Bloom filter's table.
     */
    uint64_t hash_1(uint64_t key) const;

    /**
     * @brief Hash function 2 used by the Bloom filter.
     *
     * This function performs bit manipulation and arithmetic operations on the key to produce a second hash value.
     * The resulting hash is then modulo the size of the Bloom filter's table.
     *
     * @param key The 64-bit key to hash.
     * @return uint64_t The hashed value modulo the size of the Bloom filter's table.
     */
    uint64_t hash_2(uint64_t key) const;

    /**
     * @brief Hash function 3 used by the Bloom filter.
     *
     * This function performs bit manipulation and arithmetic operations on the key to produce a third hash value.
     * The resulting hash is then modulo the size of the Bloom filter's table.
     *
     * @param key The 64-bit key to hash.
     * @return uint64_t The hashed value modulo the size of the Bloom filter's table.
     */
    uint64_t hash_3(uint64_t key) const;
};

#endif // BLOOM_H
