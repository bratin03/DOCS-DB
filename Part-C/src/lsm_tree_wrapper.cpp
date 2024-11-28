#include <string>
#include <mutex>  // Include C++ standard library features
#include "lsm_tree.h" // Your original header

// C++ code for LSM Tree implementation, mutex, etc.
std::mutex lsm_tree_mutex;

class lsm_tree_ {
public:
    // Your LSM tree class implementation...
    
    void put(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(lsm_tree_mutex);
        // Code to insert or update the key-value pair...
    }

    std::string get(const std::string& key) {
        std::lock_guard<std::mutex> lock(lsm_tree_mutex);
        // Code to retrieve the value for a given key...
        return "value";  // Example return value
    }

    void remove(const std::string& key) {
        std::lock_guard<std::mutex> lock(lsm_tree_mutex);
        // Code to remove the key-value pair...
    }
};

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
