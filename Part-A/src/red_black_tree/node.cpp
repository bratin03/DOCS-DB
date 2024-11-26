/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include "node.h"
#include "data.h"
#include <iostream>
#include <ostream>
#include <string>
#include <utility>

/**
 * @brief Constructor for creating a node with a key-value pair.
 *
 * Initializes the node with the provided key-value pair and assigns its color
 *
 * @param data The key-value pair to store in the node.
 * @param is_root A flag indicating whether this node is the root (affects initial color).
 * @return node* A pointer to the newly created node.
 */
node::node(const rb_entry &data, bool is_root = false)
{
    pair = data;

    left = nullptr;
    right = nullptr;
    parent = nullptr;

    color = is_root ? BLACK : RED;
}

/**
 * @brief Copy constructor for a node.
 *
 * This constructor creates a new node by copying the properties of the given node pointer.
 * It also updates the parent pointers for the left and right child nodes if they exist.
 *
 * @param ptr Pointer to the node that is to be copied.
 */
node::node(node *ptr)
{
    pair = ptr->pair;
    color = ptr->color;

    left = ptr->left;
    if (left != nullptr)
    {
        left->parent = this;
    }

    right = ptr->right;
    if (right != nullptr)
    {
        right->parent = this;
    }

    parent = ptr->parent;
}

/**
 * @brief Recursively deletes the entire tree starting from the current node.
 *
 * This method traverses the tree in a post-order manner (left, right, then node),
 * recursively deleting all child nodes before deleting the current node.
 */
void node::delete_tree()
{
    if (left != nullptr)
    {
        left->delete_tree();
    }

    if (right != nullptr)
    {
        right->delete_tree();
    }

    delete this;
}

/**
 * @brief Finds the node containing the target key in the tree.
 *
 * This method performs a binary search on the tree to locate a node containing the
 * specified key-value pair. If the key is found, the method returns the node;
 * otherwise, it returns NULL.
 *
 * @param target The key-value pair to search for in the tree.
 *
 * @return The node containing the target pair, or NULL if not found.
 */
node *node::find_node(const rb_entry &target)
{
    if (pair == target)
    {
        return this;
    }

    if (pair > target && left != nullptr)
    {
        return left->find_node(target);
    }

    if (pair < target && right != nullptr)
    {
        return right->find_node(target);
    }

    return nullptr;
}

/**
 * @brief Finds the floor node of the target key in the tree.
 *
 * The floor node is the largest node in the tree whose key is less than or equal to the target.
 * If no such node exists, the function returns NULL.
 *
 * @param target The key-value pair for which the floor node is to be found.
 *
 * @return A pointer to the floor node, or NULL if it does not exist.
 */
node *node::floor(const rb_entry &target)
{
    if (pair == target)
    {
        return this;
    }

    if (pair > target && left != nullptr)
    {
        return left->floor(target);
    }

    if (right != nullptr)
    {
        node *x = right->floor(target);
        if (x != nullptr && x->pair <= target)
        {
            return x;
        }
    }

    if (pair <= target)
    {
        return this;
    }
    return nullptr;
}

/**
 * @brief Inserts a new key-value pair into the Red-Black Tree.
 *
 * This function first inserts the key-value pair using a normal binary search tree insertion,
 * and then fixes any violated Red-Black Tree properties.
 * It returns the size by which the tree grew after the insert operation:
 * the size of the new key-value pair if the key did not exist in the tree, or
 * the size difference between the old and new values if the key already existed.
 *
 * @param new_pair The key-value pair to be inserted into the tree.
 *
 * @return The size by which the tree grew after insertion.
 */
int64_t node::insert(const rb_entry &new_pair)
{
    // node* inserted = insert_node(new_pair);
    std::pair<int64_t, node *> res = insert_node(new_pair);
    res.second->fix_insert();
    color = BLACK;
    return res.first;
}

/**
 * @brief Inserts a new key-value pair using binary search tree insertion.
 *
 * This function inserts a new key-value pair into the tree while maintaining the properties
 * of a binary search tree. If the key already exists, the value is updated. The function
 * returns a pair consisting of the size of the new key-value pair (or the size difference
 * if the key already existed) and a pointer to the inserted node.
 *
 * @param new_pair The key-value pair to be inserted into the tree.
 *
 * @return A pair where the first element is the size of the new key-value pair,
 *         and the second element is a pointer to the inserted node.
 */
std::pair<int64_t, node *> node::insert_node(const rb_entry &new_pair)
{
    if (new_pair == pair)
    {
        int64_t size_diff{(int64_t)pair.size() - (int64_t)new_pair.size()};
        pair = new_pair;
        return {size_diff, this};
    }

    if (new_pair < pair)
    {
        if (left == nullptr)
        {
            node *left_child = new node(new_pair);
            left_child->parent = this;
            left = left_child;
            return {new_pair.size(), left_child};
        }
        return left->insert_node(new_pair);
    }

    if (right == nullptr)
    {
        node *right_child = new node(new_pair);
        right_child->parent = this;
        right = right_child;
        return {new_pair.size(), right_child};
    }
    return right->insert_node(new_pair);
}

/**
 * @brief Fix the Red-Black Tree properties after an insertion.
 *
 * After inserting a new node, this method checks and restores the
 * Red-Black Tree properties by rotating and recoloring the nodes as necessary.
 *
 * @note This implementation assumes the tree root is always black.
 */
void node::fix_insert()
{
    node *curr = this;

    while (get_color(curr->parent) == RED)
    {
        if (curr->parent == curr->parent->parent->left)
        {
            node *uncle = curr->parent->parent->right;

            if (get_color(uncle) == RED)
            {
                curr->parent->color = BLACK;
                uncle->color = BLACK;
                curr->parent->parent->color = RED;
                curr = curr->parent->parent;
            }
            else
            {
                if (curr == curr->parent->right)
                {
                    curr = curr->parent;
                    curr->rotate_left();
                    curr = curr->left;
                }
                curr->parent->color = BLACK;
                curr->parent->parent->color = RED;
                curr->parent->parent->rotate_right();
            }
        }
        else
        {
            node *uncle = curr->parent->parent->left;

            if (get_color(uncle) == RED)
            {
                curr->parent->color = BLACK;
                uncle->color = BLACK;
                curr->parent->parent->color = RED;
                curr = curr->parent->parent;
            }
            else
            {
                if (curr == curr->parent->left)
                {
                    curr = curr->parent;
                    curr->rotate_right();
                    curr = curr->right;
                }
                curr->parent->color = BLACK;
                curr->parent->parent->color = RED;
                curr->parent->parent->rotate_left();
            }
        }
    }
}

/**
 * @brief Remove a node from the tree and fix the Red-Black Tree properties.
 *
 * This function removes a specified node, then checks and fixes
 * the tree properties if necessary to maintain Red-Black Tree invariants.
 *
 * @param to_remove Pointer to the node to remove.
 * @return True if the entire tree was deleted, false otherwise.
 */
bool node::remove(node *to_remove)
{
    if (to_remove->left != nullptr && to_remove->right != nullptr)
    {
        node *next_greater = to_remove->right->find_min();
        to_remove->pair = next_greater->pair;
        to_remove = next_greater;
    }

    if (to_remove->parent == nullptr && to_remove->left == nullptr && to_remove->right == nullptr)
    {
        delete_tree();
        return true;
    }

    to_remove->remove_node();
    color = BLACK;
    return false;
}

/**
 * @brief Remove a node and adjust the tree to preserve Red-Black properties.
 *
 * This function removes a node and fixes any violations of the Red-Black Tree
 * properties by adjusting the colors and performing necessary rotations.
 *
 * @note Uses a DOUBLE_BLACK color indicator to handle double-black violations.
 */
void node::remove_node()
{
    if (color == RED || get_color(left) == RED || get_color(right) == RED)
    {
        node *child = (left != nullptr) ? left : right;

        if (parent == nullptr)
        {
            pair = child->pair;
            left = child->left;
            right = child->right;
            if (child->left != nullptr)
            {
                child->left->parent = this;
            }

            if (child->right != nullptr)
            {
                child->right->parent = this;
            }
            return;
        }

        if (this == parent->left)
        {
            parent->left = child;
        }
        else
        {
            parent->right = child;
        }

        if (child != nullptr)
        {
            child->parent = parent;
            child->color = BLACK;
        }

        delete this;
        return;
    }

    node *curr = this;
    curr->color = DOUBLE_BLACK;

    while (curr->parent != nullptr && curr->color == DOUBLE_BLACK)
    {
        if (curr == curr->parent->left)
        {
            node *sibling = curr->parent->right;

            if (get_color(sibling) == RED)
            {
                sibling->color = BLACK;
                curr->parent->color = RED;
                curr->parent->rotate_left();
            }
            else
            {
                if (get_color(sibling->left) == BLACK && get_color(sibling->right) == BLACK)
                {
                    set_color(sibling, RED);

                    if (get_color(curr->parent) == RED)
                    {
                        set_color(curr->parent, BLACK);
                    }
                    else
                    {
                        set_color(curr->parent, DOUBLE_BLACK);
                    }
                    curr = curr->parent;
                }
                else
                {
                    if (get_color(sibling->right) == BLACK)
                    {
                        set_color(sibling->left, BLACK);
                        set_color(sibling, RED);
                        sibling->rotate_right();
                        sibling = curr->parent->right;
                    }
                    set_color(sibling, curr->parent->color);
                    set_color(curr->parent, BLACK);
                    set_color(sibling->right, BLACK);
                    curr->parent->rotate_left();
                    break;
                }
            }
        }
        else
        {
            node *sibling = curr->parent->left;

            if (get_color(sibling) == RED)
            {
                sibling->color = BLACK;
                curr->parent->color = RED;
                curr->parent->rotate_left();
            }
            else
            {
                if (get_color(sibling->left) == BLACK && get_color(sibling->right) == BLACK)
                {
                    set_color(sibling, RED);
                    if (get_color(curr->parent) == RED)
                    {
                        set_color(curr->parent, BLACK);
                    }
                    else
                    {
                        set_color(curr->parent, DOUBLE_BLACK);
                    }
                    curr = curr->parent;
                }
                else
                {
                    if (get_color(sibling->left) == BLACK)
                    {
                        set_color(sibling->right, BLACK);
                        set_color(sibling, RED);
                        sibling->rotate_left();
                        sibling = curr->parent->left;
                    }
                    set_color(sibling, curr->parent->color);
                    set_color(curr->parent, BLACK);
                    set_color(sibling->left, BLACK);
                    curr->parent->rotate_right();
                    break;
                }
            }
        }
    }

    if (parent == nullptr)
    {
        delete this;
        return;
    }

    if (parent->left != nullptr && this == parent->left)
    {
        parent->left = nullptr;
    }
    else if (parent->right != nullptr && this == parent->right)
    {
        parent->right = nullptr;
    }

    delete this;
}

/**
 * @brief Return all stored key-value pairs in an inorder fashion.
 *
 * This function performs an inorder traversal of the tree, appending
 * each key-value pair to the provided vector. If delete_node is true,
 * nodes will be deleted as they are traversed.
 *
 * @param nodes A vector to store the key-value pairs in inorder.
 * @param delete_node If true, nodes are deleted after they are processed.
 */
void node::in_order(std::vector<rb_entry> &nodes, bool delete_node) const
{
    if (left != nullptr)
    {
        left->in_order(nodes, delete_node);
    }

    nodes.push_back(pair);

    if (right != nullptr)
    {
        right->in_order(nodes, delete_node);
    }

    if (delete_node)
    {
        delete this;
    }
}

/**
 * @brief Find the minimum node in the tree (the inorder successor).
 *
 * This function traverses the leftmost path of the tree to find the
 * minimum node, which is the inorder successor of the current node.
 *
 * @return A pointer to the minimum node.
 */
node *node::find_min()
{
    node *tmp = this;

    while (tmp->left != nullptr)
    {
        tmp = tmp->left;
    }

    return tmp;
}

/**
 * @brief Set the color of a node.
 *
 * Sets the color of the specified node to the provided color. If
 * the node is nullptr, no action is taken.
 *
 * @param curr The node to set the color on.
 * @param new_color The new color to apply to the node.
 */
void node::set_color(node *curr, colors new_color)
{
    if (curr != nullptr)
    {
        curr->color = new_color;
    }
}

/**
 * @brief Get the color of a node.
 *
 * Returns the color of the specified node. If the node is nullptr,
 * the function returns BLACK by default.
 *
 * @param curr The node to retrieve the color from.
 * @return The color of the node or BLACK if the node is nullptr.
 */
node::colors node::get_color(node *curr)
{
    if (curr == nullptr)
    {
        return BLACK;
    }
    return curr->color;
}

/**
 * @brief Perform a left rotation on this node.
 *
 * Rotates the tree left around this node. In this transformation,
 * the right child of this node becomes the new root of the subtree,
 * and this node becomes the left child of that new root.
 *
 *      x                          y
 *     / \                        / \
 *    a   y  x.rotate_left() --> x   c
 *       / \                    / \
 *      b   c                  a   b
 */
void node::rotate_left()
{
    node *y = right;

    node *left_child = new node(pair);
    if (left != nullptr)
    {
        left_child->left = left;
        left_child->left->parent = left_child;
    }

    left_child->right = y->left;
    if (left_child->right != nullptr)
    {
        left_child->right->parent = left_child;
    }

    left_child->parent = this;
    left_child->color = color;

    if (right->right != nullptr)
    {
        right->right->parent = this;
    }

    pair = y->pair;
    color = y->color;
    left = left_child;
    right = y->right;
    if (right != nullptr)
    {
        right->parent = this;
    }

    delete y;
}

/**
 * @brief Perform a right rotation on this node.
 *
 * Rotates the tree right around this node. In this transformation,
 * the left child of this node becomes the new root of the subtree,
 * and this node becomes the right child of that new root.
 *
 *       x                           y
 *      / \                         / \
 *     y   c   x.rotate_right() --> a   x
 *    / \                              / \
 *   a   b                            b   c
 */
void node::rotate_right()
{
    node *y = left;

    node *right_child = new node(pair);
    if (right != nullptr)
    {
        right_child->right = right;
        right_child->right->parent = right_child;
    }

    right_child->left = y->right;
    if (right_child->left != nullptr)
    {
        right_child->left->parent = right_child;
    }

    right_child->parent = this;
    right_child->color = color;

    if (left->left != nullptr)
    {
        left->left->parent = this;
    }

    pair = y->pair;
    color = y->color;
    right = right_child;
    left = y->left;
    if (left != nullptr)
    {
        left->parent = this;
    }
    delete y;
}

/**
 * @brief Print the tree in a 2D representation.
 *
 * This function recursively prints the tree structure in a rotated, 
 * 2D format with indentation. Each level of depth in the tree is 
 * indicated by an additional 4-space indentation. Nodes are printed 
 * with their keys and colors.
 *
 * @param space The indentation level to visually represent the tree structure.
 */
void node::print_2d(int space) const
{
    space += 4;
    if (right != nullptr)
    {
        right->print_2d(space);
    }

    std::cout << std::endl;
    for (int i = 0; i < space; i++)
    {
        std::cout << " ";
    }
    std::cout << pair.key << ":" << color << std::endl;

    if (left != nullptr)
    {
        left->print_2d(space);
    }
}

/**
 * @brief Check for structural errors in the node's subtree.
 *
 * This function performs checks to identify common errors within 
 * the Red-Black Tree structure. It verifies that keys are not too long 
 * and checks if the parent node's pointers correctly reference the child.
 *
 * @param is_root Set to true if this node is the root; root nodes are 
 * excluded from certain checks, as they do not have a parent.
 */
void node::check_for_errors(bool is_root)
{
    if (right != nullptr)
    {
        right->check_for_errors(false);
    }

    if (left != nullptr)
    {
        left->check_for_errors(false);
    }

    if (!is_root)
    {
        if (parent->pair.key.length() > 100)
        {
            std::cout << "---------" << std::endl;
            std::cout << "ERROR KEY IS TOO LONG" << std::endl;
            std::cout << "CURR: " << pair.key << std::endl;
            std::cout << "------" << std::endl;
            std::cout << "PARENT LENGTH: " << parent->pair.key.length() << std::endl;
        }
        if (parent->left != this && parent->right != this)
        {
            std::cout << "---------" << std::endl;
            std::cout << "ERROR ERROR ERROR" << std::endl;
            std::cout << "CURR: " << to_str() << std::endl;
            std::cout << "Parent: " << parent->to_str() << std::endl;
            std::cout << "---------" << std::endl;
        }
    }
}

/**
 * @brief Convert the node to a string representation.
 *
 * This function creates a string that represents the node's key and 
 * color for easy debugging and printing.
 *
 * @return A string formatted as "key:color", where `key` is the node's 
 * key and `color` is the integer representation of the node's color.
 */
std::string node::to_str() const
{
    return pair.key + ":" + std::to_string(color);
}
