/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @file lsm_tree_wrapper_c.cpp
 * @brief C wrapper for the LSM tree class.
 */

#include "lsm_tree.h"

/**
 * @brief Creates a new instance of the LSM tree.
 *
 * This function allocates and initializes a new LSM tree instance.
 *
 * @return A pointer to the created LSM tree.
 */
extern "C" void *lsm_tree_create()
{
    return new lsm_tree();
}

/**
 * @brief Destroys an existing LSM tree instance.
 *
 * This function deallocates and cleans up the resources associated
 * with the given LSM tree.
 *
 * @param tree A pointer to the LSM tree to be destroyed.
 */
extern "C" void lsm_tree_destroy(void *tree)
{
    delete static_cast<lsm_tree *>(tree);
}

/**
 * @brief Inserts or updates a key-value pair in the LSM tree.
 *
 * This function inserts a new key-value pair into the LSM tree or
 * updates the value if the key already exists.
 *
 * @param tree A pointer to the LSM tree instance.
 * @param key The key to be inserted/updated.
 * @param value The value associated with the key.
 */
extern "C" void lsm_tree_put(void *tree, const char *key, const char *value)
{
    static_cast<lsm_tree *>(tree)->put(std::string(key), std::string(value));
}

/**
 * @brief Retrieves the value associated with a key in the LSM tree.
 *
 * This function searches for the given key in the LSM tree and returns
 * the associated value. If the key does not exist, the behavior depends
 * on the LSM tree's implementation.
 *
 * @param tree A pointer to the LSM tree instance.
 * @param key The key to be searched for.
 * @return A pointer to the value associated with the key. The returned
 *         pointer points to a static string and is only valid until the
 *         next call to this function.
 */
extern "C" const char *lsm_tree_get(void *tree, const char *key)
{
    static std::string result;
    result = static_cast<lsm_tree *>(tree)->get(std::string(key));
    return result.c_str();
}

/**
 * @brief Removes a key-value pair from the LSM tree.
 *
 * This function deletes the key-value pair associated with the given key
 * from the LSM tree.
 *
 * @param tree A pointer to the LSM tree instance.
 * @param key The key to be removed.
 */
extern "C" void lsm_tree_remove(void *tree, const char *key)
{
    static_cast<lsm_tree *>(tree)->remove(std::string(key));
}

/**
 * @brief Drops the entire LSM tree table.
 *
 * This function clears all data from the LSM tree, effectively resetting it.
 *
 * @param tree A pointer to the LSM tree instance.
 */
extern "C" void lsm_tree_drop_table(void *tree)
{
    static_cast<lsm_tree *>(tree)->drop_table();
}
