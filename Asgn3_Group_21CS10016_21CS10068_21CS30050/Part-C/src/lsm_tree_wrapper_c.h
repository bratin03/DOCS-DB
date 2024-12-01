/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @file lsm_tree_wrapper_c.h
 *
 * @brief Provides a C-compatible API for the LSM Tree for interoperability with Python (via ctypes) or other C-based integrations.
 */

#ifndef LSM_TREE_H
#define LSM_TREE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Creates an instance of the LSM tree.
     *
     * Allocates and initializes a new LSM tree instance.
     *
     * @return Pointer to the created LSM tree instance. The returned pointer
     *         should be passed to `lsm_tree_destroy` to free the allocated memory.
     */
    void *lsm_tree_create();

    /**
     * @brief Destroys the given LSM tree instance.
     *
     * Deallocates the memory associated with the given LSM tree instance and
     * cleans up resources.
     *
     * @param tree Pointer to the LSM tree instance to be destroyed.
     *             Must not be `nullptr`.
     */
    void lsm_tree_destroy(void *tree);

    /**
     * @brief Inserts or updates a key-value pair in the LSM tree.
     *
     * If the key exists, the value is updated. If the key does not exist,
     * a new key-value pair is inserted into the tree.
     *
     * @param tree Pointer to the LSM tree instance.
     * @param key The key to insert or update.
     * @param value The value associated with the key.
     */
    void lsm_tree_put(void *tree, const char *key, const char *value);

    /**
     * @brief Retrieves the value associated with a key in the LSM tree.
     *
     * Searches for the given key in the LSM tree and returns the associated
     * value. If the key is not found, the return value is `nullptr`.
     *
     * @param tree Pointer to the LSM tree instance.
     * @param key The key to search for.
     * @return Pointer to the value associated with the key. Returns `nullptr`
     *         if the key does not exist in the tree.
     */
    const char *lsm_tree_get(void *tree, const char *key);

    /**
     * @brief Removes a key-value pair from the LSM tree.
     *
     * Deletes the key-value pair associated with the given key from the LSM
     * tree. If the key does not exist, no operation is performed.
     *
     * @param tree Pointer to the LSM tree instance.
     * @param key The key to be removed.
     */
    void lsm_tree_remove(void *tree, const char *key);

    /**
     * @brief Drops the entire table in the LSM tree, clearing all data.
     *
     * This function clears all entries stored in the LSM tree, effectively
     * resetting the tree to an empty state.
     *
     * @param tree Pointer to the LSM tree instance.
     */
    void lsm_tree_drop_table(void *tree);

#ifdef __cplusplus
}
#endif

#endif // LSM_TREE_H
