/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#ifndef NODE_H
#define NODE_H

#include "../utils/types.h"
#include "data.h"
#include <string>
#include <vector>

/**
 * @class node
 * @brief A class representing a node in a Red-Black Tree.
 *
 * This class encapsulates a node in a Red-Black tree, with associated data (`rb_entry`),
 * color, and pointers to the left, right, and parent nodes. It supports various operations
 * including insertion, removal, searching, and tree traversal.
 */
class node
{
    enum colors
    {
        RED,
        BLACK,
        DOUBLE_BLACK
    }; ///< Enumeration representing the node colors.

public:
    rb_entry pair; ///< The key-value pair stored in the node.
    colors color;  ///< The color of the node (RED, BLACK, or DOUBLE_BLACK).

    node *left;   ///< Pointer to the left child node.
    node *right;  ///< Pointer to the right child node.
    node *parent; ///< Pointer to the parent node.

    /**
     * @brief Constructor for creating a node with a key-value pair.
     *
     * Initializes the node with the provided key-value pair and assigns its color
     * to either RED or BLACK based on whether it is the root.
     *
     * @param data The key-value pair to store in the node.
     * @param is_root A flag indicating whether this node is the root (affects initial color).
     */
    node(const rb_entry &data, bool is_root);

    /**
     * @brief Constructor for creating a node as a copy of another node.
     *
     * Initializes a new node that is a copy of the provided node.
     *
     * @param ptr Pointer to the node to copy.
     */
    explicit node(node *ptr);

    /**
     * @brief Inserts a new key-value pair into the tree.
     *
     * This method inserts a new `rb_entry` into the Red-Black tree, fixing any violations
     * of Red-Black tree properties after the insertion.
     *
     * @param new_pair The key-value pair to insert.
     * @return int64_t Returns the status of the insertion (e.g., success or failure).
     */
    int64_t insert(const rb_entry &new_pair);

    /**
     * @brief Removes a node from the tree.
     *
     * This method removes the specified node from the tree and rebalances the tree if necessary.
     *
     * @param to_remove Pointer to the node to remove.
     * @return true If the node was successfully removed.
     * @return false If the node could not be removed.
     */
    bool remove(node *to_remove);

    /**
     * @brief Finds a node with a specific key-value pair.
     *
     * This method searches for a node containing the specified `rb_entry` in the tree.
     *
     * @param target The key-value pair to search for.
     * @return node* A pointer to the node containing the target, or `nullptr` if not found.
     */
    node *find_node(const rb_entry &target);

    /**
     * @brief Finds the node with the largest key that is smaller than or equal to the target.
     *
     * This method finds the floor node for the target key-value pair.
     * The floor node has the greatest key that is still less than or equal to the target.
     *
     * @param target The target key-value pair to find the floor for.
     * @return node* A pointer to the floor node, or `nullptr` if no floor exists.
     */
    node *floor(const rb_entry &target);

    /**
     * @brief Performs an in-order traversal of the tree and collects the nodes.
     *
     * This method traverses the tree in-order (left subtree, root, right subtree) and
     * adds the nodes to the provided vector.
     *
     * @param nodes The vector to store the traversed nodes.
     * @param delete_node If `true`, the nodes will be deleted after traversal.
     */
    void in_order(std::vector<rb_entry> &nodes, bool delete_node) const;

    /**
     * @brief Deletes the entire tree.
     *
     * This method recursively deletes the tree, freeing all allocated memory.
     */
    void delete_tree();

    /**
     * @brief Prints the tree in a 2D format.
     *
     * This method prints the Red-Black tree in a human-readable 2D format for visualization.
     *
     * @param space The initial space offset for proper tree visualization.
     */
    void print_2d(int space) const;

    /**
     * @brief Converts the node to a string representation.
     *
     * This method returns a string representation of the node, typically used for debugging.
     *
     * @return std::string A string representing the node.
     */
    std::string to_str() const;

private:
    /**
     * @brief Inserts a new node and returns its position.
     *
     * This private helper function performs the insertion of a new node and returns
     * a pair containing the status of the insertion and a pointer to the inserted node.
     *
     * @param new_pair The key-value pair to insert.
     * @return std::pair<int64_t, node*> The status of the insertion and the inserted node.
     */
    std::pair<int64_t, node *> insert_node(const rb_entry &new_pair);

    /**
     * @brief Fixes the Red-Black tree after an insertion.
     *
     * This method is called after an insertion to ensure that the Red-Black tree properties
     * are maintained (e.g., no two consecutive red nodes).
     */
    void fix_insert();

    /**
     * @brief Removes the node from the tree and fixes the tree properties.
     *
     * This private helper method handles the removal of a node and rebalances the tree
     * as necessary to maintain the Red-Black tree properties.
     */
    void remove_node();

    /**
     * @brief Finds the node with the smallest key.
     *
     * This method returns the node with the smallest key (leftmost node).
     *
     * @return node* A pointer to the node with the smallest key.
     */
    node *find_min();

    /**
     * @brief Rotates the node to the left.
     *
     * This method performs a left rotation to fix the tree's balance.
     */
    void rotate_left();

    /**
     * @brief Rotates the node to the right.
     *
     * This method performs a right rotation to fix the tree's balance.
     */
    void rotate_right();

    /**
     * @brief Prints a separator for the tree visualization.
     *
     * This method is used to print a separator for formatting the tree output.
     */
    void print_seperator();

    /**
     * @brief Gets the color of a node.
     *
     * This static method returns the color of a node.
     *
     * @param curr Pointer to the node whose color is to be retrieved.
     * @return colors The color of the node (RED, BLACK, or DOUBLE_BLACK).
     */
    static colors get_color(node *curr);

    /**
     * @brief Sets the color of a node.
     *
     * This static method sets the color of a node.
     *
     * @param curr Pointer to the node whose color is to be set.
     * @param new_color The new color to set for the node.
     */
    static void set_color(node *curr, colors new_color);

    /**
     * @brief Checks for errors in the Red-Black tree.
     *
     * This method checks for violations of Red-Black tree properties and raises errors if necessary.
     *
     * @param is_root A flag indicating whether the node is the root node.
     */
    void check_for_errors(bool is_root);
};

#endif // NODE_H
