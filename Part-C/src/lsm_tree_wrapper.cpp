#include <string>
#include <mutex>  // Include C++ standard library features
#include "lsm_tree.h" // Your original header

// C++ code for LSM Tree implementation, mutex, etc.
std::mutex lsm_tree_mutex;

// Now wrap C++ functions with C-compatible API (for use with ctypes, Python, etc.)
extern "C" {

    // Create a new LSM tree instance
    lsm_tree* lsm_tree_create() {
        return new lsm_tree();
    }

    // Destroy an LSM tree instance
    void lsm_tree_destroy(lsm_tree* tree) {
        delete tree;
    }

    // Insert or update a key-value pair in the LSM tree
    void lsm_tree_put(lsm_tree* tree, const char* key, const char* value) {
        tree->put(key, value);
    }

    // Retrieve the value associated with a key
    const char* lsm_tree_get(lsm_tree* tree, const char* key) {
        std::string result = tree->get(key);
        return strdup(result.c_str());  // Allocate memory for the C string
    }

    // Remove a key-value pair from the LSM tree
    void lsm_tree_remove(lsm_tree* tree, const char* key) {
        tree->remove(key);
    }
}
