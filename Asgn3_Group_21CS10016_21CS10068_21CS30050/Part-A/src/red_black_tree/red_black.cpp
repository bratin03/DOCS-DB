/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @file red_black.cpp
 * @brief Implementation of the red_black_tree class.
 */

#include "red_black.h"
#include "data.h"
#include "node.h"
#include <iostream>
#include <utility>

/**
 * @class red_black_tree
 * @brief Implements a Red-Black Tree for efficient key-value storage.
 *
 * A Red-Black Tree is a balanced binary search tree that guarantees O(log n)
 * time complexity for insertion, deletion, and search operations. This class
 * is specifically designed to store key-value pairs, where each key is unique
 * and associated with a value.
 */
red_black_tree::red_black_tree()
{
    root = nullptr;
    byte_size = 0;
}

/**
 * @brief Destructor for the Red-Black Tree.
 *
 * Cleans up the tree by deleting all nodes in the tree.
 */
red_black_tree::~red_black_tree()
{
    delete_tree();
}

/**
 * @brief Deletes all nodes in the Red-Black Tree.
 *
 * This method recursively deletes each node in the tree, freeing memory
 * and resetting the tree to its initial empty state.
 */
void red_black_tree::delete_tree()
{
    if (root != nullptr)
    {
        root->delete_tree();
    }

    root = nullptr;
    byte_size = 0;
}

/**
 * @brief Inserts a new key-value pair into the Red-Black Tree.
 *
 * This method creates a new `rb_entry` from the provided `kv_pair` and inserts it.
 * If the tree is empty, the new pair becomes the root of the tree.
 *
 * @param new_pair The key-value pair to insert into the tree.
 */
void red_black_tree::insert(kv_pair new_pair)
{
    rb_entry data = rb_entry{std::move(new_pair)};
    insert(data);
}

/**
 * @brief Inserts a key-value pair into the Red-Black Tree.
 *
 * This method inserts the given `rb_entry` into the tree. If the tree is empty,
 * the new pair is inserted as the root. Otherwise, it is inserted recursively.
 *
 * @param data The `rb_entry` containing the key-value pair to insert.
 */
void red_black_tree::insert(const rb_entry &data)
{
    if (root == nullptr)
    {
        root = new node(data, true);
        byte_size = data.size();
    }
    else
    {
        uint64_t size_diff = root->insert(data);
        byte_size += size_diff;
    }
}

/**
 * @brief Removes a key-value pair from the Red-Black Tree.
 *
 * This method searches for the given key in the tree and removes the corresponding
 * node if found. If the node is the root, the tree is deleted. The tree size is
 * updated after the removal.
 *
 * @param target The key of the pair to remove from the tree.
 */
void red_black_tree::remove(const std::string &target)
{
    if (root == nullptr)
    {
        return;
    }

    node *to_remove = root->find_node(rb_entry{target});
    if (to_remove == nullptr)
    {
        return;
    }

    bool tree_deleted = root->remove(to_remove);
    if (tree_deleted)
    {
        root = nullptr;
        byte_size = 0;
    }
    else
    {
        byte_size -= to_remove->pair.size();
    }
}

/**
 * @brief Checks if a key exists in the Red-Black Tree.
 *
 * This method checks if the specified key exists in the tree by searching
 * through the tree. It returns `true` if the key is found, and `false` otherwise.
 *
 * @param target The key to check for existence in the tree.
 * @return `true` if the key exists, `false` otherwise.
 */
bool red_black_tree::exists(std::string target) const
{
    if (root != nullptr)
    {
        return root->find_node(rb_entry{std::move(target)}) != nullptr;
    }
    return false;
}

/**
 * @brief Retrieves the value associated with a key in the Red-Black Tree.
 *
 * This method searches the tree for the given key. If found, it returns
 * the associated value. Otherwise, it returns an empty optional.
 *
 * @param target The key to search for in the tree.
 * @return The value associated with the key, or an empty optional if the key is not found.
 */
std::optional<std::string> red_black_tree::get(std::string target) const
{
    if (root != nullptr)
    {
        node *x = root->find_node(rb_entry{std::move(target)});
        if (x == nullptr)
        {
            return {};
        }
        return std::any_cast<std::string>(x->pair.val.value());
    }
    return {};
}

/**
 * @brief Finds the next smallest node if the target does not exist in the tree.
 *
 * This method searches for the given key in the tree. If the key does not exist,
 * it returns the next smallest key-value pair (i.e., the floor of the target).
 *
 * @param target The key to find the floor of.
 * @return The closest key-value pair smaller than or equal to the target key.
 */
std::optional<rb_entry> red_black_tree::floor(std::string target) const
{
    if (root == nullptr)
    {
        return {};
    }

    node *x = root->floor(rb_entry{std::move(target)});
    if (x == nullptr)
    {
        return {};
    }
    return x->pair;
}

/**
 * @brief Returns all key-value pairs in the Red-Black Tree.
 *
 * This method performs an inorder traversal of the tree and collects all the
 * key-value pairs in a vector.
 *
 * @return A vector containing all key-value pairs in the tree.
 */
std::vector<rb_entry> red_black_tree::get_all_nodes() const
{
    std::vector<rb_entry> nodes;
    root->in_order(nodes, false);
    return nodes;
}

/**
 * @brief Returns and deletes all key-value pairs in the Red-Black Tree.
 *
 * This method performs an inorder traversal of the tree, collecting all key-value
 * pairs while deleting them from the tree. The tree is reset to an empty state.
 *
 * @return A vector containing all key-value pairs in the tree before deletion.
 */
std::vector<rb_entry> red_black_tree::get_and_delete_all_nodes()
{
    std::vector<rb_entry> nodes;
    root->in_order(nodes, true);

    byte_size = 0;
    root = nullptr;

    return nodes;
}

/**
 * @brief Returns the current size of the Red-Black Tree in bytes.
 *
 * This method returns the total byte size of all key-value pairs in the tree.
 *
 * @return The size of the tree in bytes.
 */
uint64_t red_black_tree::size() const
{
    return byte_size;
}

/**
 * @brief Prints the Red-Black Tree in a 2D format.
 *
 * This method prints the structure of the tree in a 2D format, with indentation
 * representing tree levels and each node displaying the key-value pair.
 */
void red_black_tree::print() const
{
    if (root != nullptr)
    {
        root->print_2d(0);
        std::cout << std::endl;
    }
}
