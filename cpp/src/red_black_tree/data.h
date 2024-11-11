/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/


#ifndef DATA_H
#define DATA_H

#include "../utils/types.h"
#include <string>
#include <any>
#include <utility>
#include <optional>
#include <typeinfo>
#include <cstdint>

/**
 * @struct rb_entry
 * @brief Represents an entry in a Red-Black tree, used in the Memtable or Sparse Index.
 * 
 * This structure contains a `key` of type `std::string` and a `val` which can be either a string (for the Memtable)
 * or an `uint64_t` (for the Sparse Index, representing an offset inside the SST).
 * It also supports operations like calculating the size of the entry and comparison based on the key.
 */
struct rb_entry
{
    std::string key; ///< The key associated with the entry.
    
    /**
     * @brief The value associated with the key.
     * 
     * The value can be:
     * - A `std::string` for Memtable entries.
     * - An `uint64_t` representing an offset in the Sparse Index inside the SST.
     * - Empty (represented as `std::nullopt`) to simply represent a key in the Red-Black tree without a value.
     */
    std::optional<std::any> val;

    /**
     * @brief Constructor for the Red-Black tree entry (Memtable version).
     * 
     * This constructor creates an entry from a key-value pair (`kv_pair`).
     * 
     * @param pair The key-value pair to initialize the entry with.
     */
    explicit rb_entry(kv_pair pair)
    {
        key = pair.key;
        val = pair.val;
    }

    /**
     * @brief Constructor for the Red-Black tree entry.
     * 
     * This constructor initializes the entry with a key and a value.
     * The value is stored as a `std::any` object which can hold any type, but is expected
     * to be either a string or an uint64_t (offset in the Sparse Index).
     * 
     * @param key The key for the entry.
     * @param val The value associated with the key.
     */
    rb_entry(std::string key, std::any val)
    {
        this->key = std::move(key);
        this->val = std::move(val);
    }

    /**
     * @brief Constructor for an entry with just a key (no associated value).
     * 
     * This constructor initializes the entry with just a key, and no value (empty `std::optional`).
     * 
     * @param key The key for the entry.
     */
    explicit rb_entry(std::string key)
    {
        this->key = std::move(key);
        this->val = {};
    }

    /// Default constructor.
    rb_entry() = default;

    /**
     * @brief Calculates the size of the entry.
     * 
     * This function returns the size of the entry, which is the size of the key plus the size of the value.
     * - If the value is empty (`std::nullopt`), it returns the size of the key.
     * - If the value is a string, it adds the size of the string.
     * - If the value is an `uint64_t`, it adds 8 bytes (the size of a 64-bit integer).
     * 
     * @return The size of the entry in bytes.
     */
    uint64_t size() const
    {
        if (not val.has_value())
        {
            return key.size();
        }

        if (val.value().type() == typeid(std::string))
        {
            return key.size() + std::any_cast<std::string>(val.value()).size();
        }
        else
        {
            // If val is not a string, it is an uint64_t ( = 8 Byte)
            return key.size() + 8;
        }
    }

    /**
     * @brief Compares two `rb_entry` objects based on the key.
     * 
     * This function compares the keys of two Red-Black tree entries and returns the comparison result.
     * It uses the `<=>` (three-way comparison) operator and returns:
     * - `std::strong_ordering::equal` if the keys are the same.
     * - `std::strong_ordering::less` if this entry's key is smaller.
     * - `std::strong_ordering::greater` if this entry's key is larger.
     * 
     * @param other The other `rb_entry` to compare with.
     * @return std::strong_ordering The result of comparing the keys.
     */
    std::strong_ordering operator<=>(const rb_entry &other) const
    {
        if (key == other.key)
        {
            return std::strong_ordering::equal;
        }

        if (key < other.key)
        {
            return std::strong_ordering::less;
        }

        return std::strong_ordering::greater;
    }

    /**
     * @brief Checks if two `rb_entry` objects are equal.
     * 
     * This function compares the keys of two Red-Black tree entries to determine if they are equal.
     * 
     * @param other The other `rb_entry` to compare with.
     * @return true If the keys of both entries are equal.
     * @return false If the keys are different.
     */
    bool operator==(const rb_entry &other) const { return key == other.key; }
};

#endif // DATA_H
