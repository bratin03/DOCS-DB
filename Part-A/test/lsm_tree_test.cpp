/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include <gtest/gtest.h>
#include <string>
#include "../src/lsm_tree/lsm_tree.h"

// Fixture for setting up and tearing down an LSM tree for each test
class LSMTreeTest : public ::testing::Test
{
protected:
    lsm_tree tree;

    void SetUp() override
    {
        
    }

    void TearDown() override
    {
        // Clean up after each test if necessary
        tree.drop_table();
    }
};

// Test put and get functionality
TEST_F(LSMTreeTest, PutAndGet)
{
    // Put some key-value pairs
    tree.put("key1", "value1");
    tree.put("key2", "value2");

    // Get and verify values
    EXPECT_EQ(tree.get("key1"), "value1");
    EXPECT_EQ(tree.get("key2"), "value2");

    // Test for a non-existent key
    EXPECT_EQ(tree.get("key3"), "");
}

// Test updating a key's value
TEST_F(LSMTreeTest, UpdateValue)
{
    tree.put("key1", "initial_value");
    EXPECT_EQ(tree.get("key1"), "initial_value");

    // Update the value for the same key
    tree.put("key1", "updated_value");
    EXPECT_EQ(tree.get("key1"), "updated_value");
}

// Test remove functionality
TEST_F(LSMTreeTest, RemoveKey)
{
    tree.put("key1", "value1");
    tree.put("key2", "value2");

    // Ensure key1 exists before removing
    EXPECT_EQ(tree.get("key1"), "value1");

    // Remove key1
    tree.remove("key1");

    // Check if key1 is removed
    EXPECT_EQ(tree.get("key1"), "");

    // Ensure key2 still exists
    EXPECT_EQ(tree.get("key2"), "value2");
}

// Test drop_table functionality
TEST_F(LSMTreeTest, DropTable)
{
    tree.put("key1", "value1");
    tree.put("key2", "value2");

    // Drop all data in the table
    tree.drop_table();

    // Check that data is no longer available
    EXPECT_EQ(tree.get("key1"), "");
    EXPECT_EQ(tree.get("key2"), "");
}
