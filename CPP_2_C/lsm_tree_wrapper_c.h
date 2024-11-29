#ifndef LSM_TREE_H
#define LSM_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates an instance of the LSM tree.
 * @return Pointer to the created LSM tree instance.
 */
void *lsm_tree_create();

/**
 * Destroys the given LSM tree instance.
 * @param tree Pointer to the LSM tree instance to be destroyed.
 */
void lsm_tree_destroy(void *tree);

/**
 * Inserts or updates a key-value pair in the LSM tree.
 * @param tree Pointer to the LSM tree instance.
 * @param key The key to insert or update.
 * @param value The value associated with the key.
 */
void lsm_tree_put(void *tree, const char *key, const char *value);

/**
 * Retrieves the value associated with a key in the LSM tree.
 * @param tree Pointer to the LSM tree instance.
 * @param key The key to search for.
 * @return Pointer to the value associated with the key. Returns `nullptr` if the key is not found.
 */
const char *lsm_tree_get(void *tree, const char *key);

/**
 * Removes a key-value pair from the LSM tree.
 * @param tree Pointer to the LSM tree instance.
 * @param key The key to be removed.
 */
void lsm_tree_remove(void *tree, const char *key);

/**
 * Drops the entire table in the LSM tree, effectively clearing all data.
 * @param tree Pointer to the LSM tree instance.
 */
void lsm_tree_drop_table(void *tree);

#ifdef __cplusplus
}
#endif

#endif // LSM_TREE_H
