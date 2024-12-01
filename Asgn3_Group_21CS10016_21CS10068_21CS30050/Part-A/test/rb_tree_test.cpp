/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @file rb_tree_test.cpp
 * @brief Unit tests for the Red-Black tree class.
 */
#include <gtest/gtest.h>
#include "../src/red_black_tree/red_black.h"
#include "../src/red_black_tree/data.h"
#include "../src/red_black_tree/node.h"

/**
 * @brief Test fixture for the Red-Black tree class.
 *
 * This fixture sets up a new Red-Black tree before each test and deletes it after each test.
 *
 * The fixture also provides common setup and teardown methods for the tests.
 */
class RedBlackTreeTest : public ::testing::Test
{
protected:
    // Set up the test environment, called before each test case
    red_black_tree tree;

    void SetUp() override
    {
        // Example test setup, can be customized for each test
    }

    // Tear down the test environment, called after each test case
    void TearDown() override
    {
        tree.delete_tree(); // Clean up tree after each test case
    }
};

/**
 * @brief Test for inserting elements into the Red-Black tree.
 *
 * This test inserts elements into the tree and checks if they exist.
 */
TEST_F(RedBlackTreeTest, InsertTest)
{
    kv_pair pair1 = {"key1", "value1"};
    kv_pair pair2 = {"key2", "value2"};

    // Inserting elements into the Red-Black tree
    tree.insert(pair1);
    tree.insert(pair2);

    // Check if the elements exist in the tree
    EXPECT_TRUE(tree.exists("key1"));
    EXPECT_TRUE(tree.exists("key2"));
    EXPECT_FALSE(tree.exists("key3"));
}

/**
 * @brief Test for checking the size of the Red-Black tree.
 *
 * This test inserts elements into the tree and checks the size of the tree.
 */
TEST_F(RedBlackTreeTest, SizeTest)
{
    kv_pair pair1 = {"key1", "value1"};
    kv_pair pair2 = {"key2", "value2"};

    // Inserting elements and checking the size
    tree.insert(pair1);
    tree.insert(pair2);
    EXPECT_EQ(tree.size(), 20);

    // Removing one element and checking the size again
    tree.remove("key1");
    EXPECT_EQ(tree.size(), 10);
}

/**
 * @brief Test for getting elements from the Red-Black tree.
 *
 * This test inserts elements into the tree and then retrieves them to check the values.
 */
TEST_F(RedBlackTreeTest, GetTest)
{
    kv_pair pair1 = {"key1", "value1"};
    kv_pair pair2 = {"key2", "value2"};

    // Insert elements into the tree
    tree.insert(pair1);
    tree.insert(pair2);

    // Get and check the values
    auto value1 = tree.get("key1");
    EXPECT_TRUE(value1.has_value());
    EXPECT_EQ(value1.value(), "value1");

    auto value2 = tree.get("key2");
    EXPECT_TRUE(value2.has_value());
    EXPECT_EQ(value2.value(), "value2");

    // Key that doesn't exist
    auto value3 = tree.get("key3");
    EXPECT_FALSE(value3.has_value());
}

/**
 * @brief Test for finding the floor of a key in the Red-Black tree.
 *
 * This test inserts elements into the tree and then finds the floor of a key.
 */
TEST_F(RedBlackTreeTest, FloorTest)
{
    kv_pair pair1 = {"key1", "value1"};
    kv_pair pair2 = {"key2", "value2"};
    kv_pair pair3 = {"key3", "value3"};

    // Insert elements into the tree
    tree.insert(pair1);
    tree.insert(pair2);
    tree.insert(pair3);

    // Test floor of key that exists
    auto floor1 = tree.floor("key2");
    EXPECT_TRUE(floor1.has_value());
    EXPECT_EQ(floor1->key, "key2");

    // Test floor of a key that doesn't exist, should return "key2"
    auto floor2 = tree.floor("key4");
    EXPECT_TRUE(floor2.has_value());
    EXPECT_EQ(floor2->key, "key3");

    // Test floor for a non-existent key smaller than any
    auto floor3 = tree.floor("key0");
    EXPECT_FALSE(floor3.has_value());
}

/**
 * @brief Test for removing elements from the Red-Black tree.
 *
 * This test inserts elements into the tree and then removes one of them.
 */
TEST_F(RedBlackTreeTest, RemoveTest)
{
    kv_pair pair1 = {"key1", "value1"};
    kv_pair pair2 = {"key2", "value2"};

    // Insert elements
    tree.insert(pair1);
    tree.insert(pair2);

    // Check existence before removal
    EXPECT_TRUE(tree.exists("key1"));
    EXPECT_TRUE(tree.exists("key2"));

    // Remove "key1"
    tree.remove("key1");
    EXPECT_FALSE(tree.exists("key1"));
    EXPECT_TRUE(tree.exists("key2"));
}

/**
 * @brief Test for getting all nodes from the Red-Black tree.
 *
 * This test inserts elements into the tree and then retrieves all nodes.
 */
TEST_F(RedBlackTreeTest, GetAllNodesTest)
{
    kv_pair pair1 = {"key1", "value1"};
    kv_pair pair2 = {"key2", "value2"};

    // Insert elements
    tree.insert(pair1);
    tree.insert(pair2);

    // Get all nodes
    auto all_nodes = tree.get_all_nodes();
    EXPECT_EQ(all_nodes.size(), 2);
    EXPECT_EQ(all_nodes[0].key, "key1");
    EXPECT_EQ(all_nodes[1].key, "key2");
}

/**
 * @brief Test for getting and deleting all nodes from the Red-Black tree.
 *
 * This test inserts elements into the tree and then retrieves and deletes all nodes.
 */
TEST_F(RedBlackTreeTest, GetAndDeleteAllNodesTest)
{
    kv_pair pair1 = {"key1", "value1"};
    kv_pair pair2 = {"key2", "value2"};

    // Insert elements
    tree.insert(pair1);
    tree.insert(pair2);

    // Get and delete all nodes
    auto deleted_nodes = tree.get_and_delete_all_nodes();
    EXPECT_EQ(deleted_nodes.size(), 2);
    EXPECT_FALSE(tree.exists("key1"));
    EXPECT_FALSE(tree.exists("key2"));
}
