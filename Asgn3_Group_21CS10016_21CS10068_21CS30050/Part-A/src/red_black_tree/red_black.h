/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Department of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @file red_black.h
 * @brief Definition of the red_black_tree class.
 */

#ifndef RED_BLACK_H
#define RED_BLACK_H

#include "data.h"
#include "node.h"
#include "../utils/types.h"
#include <cstdint>
#include <vector>
#include <optional>

/**
 * @class red_black_tree
 * @brief Represents a Red-Black Tree data structure.
 *
 * This class implements a self-balancing binary search tree known as the Red-Black Tree.
 * It supports various operations like insertion, deletion, search, and traversal.
 */
class red_black_tree
{

public:
    /**
     * @brief Default constructor for the Red-Black Tree.
     *
     * Initializes an empty Red-Black Tree with no nodes.
     */
    red_black_tree();

    /**
     * @brief Destructor for the Red-Black Tree.
     *
     * Frees all memory allocated for the nodes in the tree.
     */
    ~red_black_tree();

    /**
     * @brief Deletes the entire tree, freeing all resources.
     *
     * This method removes all nodes from the tree and deallocates memory.
     */
    void delete_tree();

    /**
     * @brief Inserts a new key-value pair into the tree.
     *
     * This method inserts a new `kv_pair` into the Red-Black Tree, balancing the tree
     * after the insertion to maintain the Red-Black properties.
     *
     * @param new_pair The key-value pair to be inserted.
     */
    void insert(kv_pair new_pair);

    /**
     * @brief Inserts a new red-black tree entry into the tree.
     *
     * This method inserts an `rb_entry` into the tree and ensures the tree remains balanced.
     *
     * @param data The red-black tree entry to be inserted.
     */
    void insert(const rb_entry &data);

    /**
     * @brief Removes a key-value pair from the tree.
     *
     * This method deletes the node with the specified target key from the tree, adjusting
     * the tree structure to maintain balance.
     *
     * @param target The key to be removed.
     */
    void remove(const std::string &target);

    /**
     * @brief Checks if a key exists in the tree.
     *
     * This method checks whether a given key exists in the Red-Black Tree.
     *
     * @param target The key to be checked.
     *
     * @return True if the key exists, false otherwise.
     */
    bool exists(std::string target) const;

    /**
     * @brief Retrieves the value associated with a given key.
     *
     * This method searches for a key in the tree and returns its associated value if found.
     *
     * @param target The key whose associated value is to be retrieved.
     *
     * @return The value associated with the key, or an empty optional if not found.
     */
    std::optional<std::string> get(std::string target) const;

    /**
     * @brief Finds the floor entry for a given key.
     *
     * This method finds the largest key that is smaller or equal to the target key in the
     * Red-Black Tree.
     *
     * @param target The target key to find the floor entry for.
     *
     * @return An optional containing the floor entry if found, or an empty optional if not found.
     */
    std::optional<rb_entry> floor(std::string target) const;

    /**
     * @brief Retrieves all nodes in the tree.
     *
     * This method returns a vector of all red-black tree entries in the tree.
     *
     * @return A vector containing all tree entries.
     */
    std::vector<rb_entry> get_all_nodes() const;

    /**
     * @brief Retrieves and deletes all nodes in the tree.
     *
     * This method retrieves all nodes in the tree and deletes them, ensuring the tree becomes empty.
     *
     * @return A vector of all nodes that were deleted.
     */
    std::vector<rb_entry> get_and_delete_all_nodes();

    /**
     * @brief Retrieves the number of nodes in the tree.
     *
     * This method returns the number of nodes currently in the Red-Black Tree.
     *
     * @return The number of nodes in the tree.
     */
    uint64_t size() const;

    /**
     * @brief Prints the tree structure.
     *
     * This method prints the structure of the Red-Black Tree to the console. Useful for debugging.
     */
    void print() const;

private:
    uint64_t byte_size; ///< The total byte size of the tree (memory usage)
    node *root;         ///< Pointer to the root node of the tree
};

#endif // RED_BLACK_H
