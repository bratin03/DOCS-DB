/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include <string>
#include <mutex>      // Include C++ standard library features
#include "lsm_tree.h" // Your original header

/// @file lsm_tree_wrapper.cpp
/// @brief Provides a C-compatible API for the LSM Tree for interoperability with Python (via ctypes) or other C-based integrations.

std::mutex lsm_tree_mutex; ///< Mutex for thread-safe operations on the LSM tree.

/// @brief Create a new LSM tree instance.
/// @return A pointer to the newly created LSM tree instance.
extern "C" lsm_tree *lsm_tree_create()
{
    return new lsm_tree();
}

/// @brief Destroy an LSM tree instance.
/// @param tree A pointer to the LSM tree instance to be destroyed.
extern "C" void lsm_tree_destroy(lsm_tree *tree)
{
    delete tree;
}

/// @brief Insert or update a key-value pair in the LSM tree.
/// @param tree A pointer to the LSM tree instance.
/// @param key The key as a null-terminated C string.
/// @param value The value as a null-terminated C string.
extern "C" void lsm_tree_put(lsm_tree *tree, const char *key, const char *value)
{
    std::lock_guard<std::mutex> lock(lsm_tree_mutex); // Ensure thread-safe access.
    tree->put(key, value);
}

/// @brief Retrieve the value associated with a given key in the LSM tree.
/// @param tree A pointer to the LSM tree instance.
/// @param key The key as a null-terminated C string.
/// @return The value as a null-terminated C string, or nullptr if the key is not found.
/// @note The returned string is dynamically allocated. The caller is responsible for freeing it using `free()`.
extern "C" const char *lsm_tree_get(lsm_tree *tree, const char *key)
{
    std::lock_guard<std::mutex> lock(lsm_tree_mutex); // Ensure thread-safe access.
    std::string result = tree->get(key);
    return strdup(result.c_str()); // Allocate memory for the C string.
}

/// @brief Remove a key-value pair from the LSM tree.
/// @param tree A pointer to the LSM tree instance.
/// @param key The key as a null-terminated C string.
extern "C" void lsm_tree_remove(lsm_tree *tree, const char *key)
{
    std::lock_guard<std::mutex> lock(lsm_tree_mutex); // Ensure thread-safe access.
    tree->remove(key);
}
