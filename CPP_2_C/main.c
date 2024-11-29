#include <stdio.h>
#include "lsm_tree_wrapper_c.h"

int main() {
    // Create an LSM tree instance
    void* tree = lsm_tree_create();

    // Put some key-value pairs
    lsm_tree_put(tree, "key1", "value1");
    lsm_tree_put(tree, "key2", "value2");

    // Retrieve a value
    const char* value = lsm_tree_get(tree, "key1");
    printf("Value for key1: %s\n", value);

    // Remove a key
    lsm_tree_remove(tree, "key1");

    // Drop the table
    lsm_tree_drop_table(tree);

    // Destroy the LSM tree instance
    lsm_tree_destroy(tree);

    return 0;
}
