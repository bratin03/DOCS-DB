/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#ifndef TYPES_H
#define TYPES_H

#include <string>

#define TOMBSTONE "+++TOMBSTONE+++" ///< A placeholder string that is not meant to be used by the user.
#define SEPERATOR ','               ///< Separator used to split key and value in the log entry.

/**
 * @struct kv_pair
 * @brief A structure to represent a key-value pair.
 *
 * This structure holds a `key` and a `val` as strings. It provides utility functions
 * to check if the pair is empty, convert the pair to a log entry string, and split a log entry string back into a key-value pair.
 */
struct kv_pair
{
    std::string key; ///< The key in the key-value pair.
    std::string val; ///< The value in the key-value pair.

    /**
     * @brief Checks if the key-value pair is empty.
     *
     * This function returns `true` if both `key` and `val` are empty strings.
     *
     * @return true If both `key` and `val` are empty.
     * @return false If either `key` or `val` is non-empty.
     */
    bool empty() const
    {
        return key.empty() and val.empty();
    }

    /**
     * @brief Converts the key-value pair into a log entry string.
     *
     * The key-value pair is converted into a string where the key and value are separated
     * by the defined separator (`,`), followed by a newline character.
     *
     * @return A string representing the key-value pair as a log entry (e.g., "key,value\n").
     */
    std::string to_log_entry() const
    {
        return key + SEPERATOR + val + "\n";
    }

    /**
     * @brief Splits a log entry string into a key-value pair.
     *
     * Given a log entry string, this function splits it into the corresponding key and value based on the separator.
     * If the input string is empty, it returns an empty key-value pair.
     *
     * @param line The log entry string to be split.
     * @return A `kv_pair` containing the extracted key and value.
     */
    static kv_pair split_log_entry(const std::string &line)
    {
        if (line.empty())
        {
            return {"", ""}; ///< Return an empty kv_pair if the line is empty.
        }

        size_t seperator_pos = line.find(SEPERATOR);                                     ///< Find the position of the separator.
        std::string key = line.substr(0, seperator_pos);                                 ///< Extract the key from the string.
        std::string value = line.substr(seperator_pos + 1, line.size() - seperator_pos); ///< Extract the value from the string.
        return {key, value};
    }

    /**
     * @brief Compares two `kv_pair` objects using the key.
     *
     * This function compares two key-value pairs and determines their ordering based on the `key`.
     * It returns `equal` if the keys are the same, `less` if the key is smaller, and `greater` if the key is larger.
     *
     * @param other The other `kv_pair` to compare with.
     * @return std::strong_ordering A comparison result indicating whether this `kv_pair` is less than, equal to, or greater than `other`.
     */
    std::strong_ordering operator<=>(const kv_pair &other) const
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
     * @brief Checks if two `kv_pair` objects are equal.
     *
     * This function compares the `key` of two `kv_pair` objects to check if they are the same.
     *
     * @param other The other `kv_pair` to compare with.
     * @return true If the `key` of both `kv_pair` objects is the same.
     * @return false If the `key` of the two `kv_pair` objects is different.
     */
    bool operator==(const kv_pair &other) const { return key == other.key; }
};

#endif // TYPES_H
