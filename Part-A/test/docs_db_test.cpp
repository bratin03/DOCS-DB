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

// Test put, get, and remove functionality
TEST_F(LSMTreeTest, OverallTest)
{
    long long unsigned int upper_limit = 1e6;

    // Put same key-value pairs
    for (long long unsigned int i = 0; i < upper_limit; i++)
    {
        tree.put(std::to_string(i), std::to_string(i));
    }

    // Get and verify values
    for (long long unsigned int i = 0; i < upper_limit; i++)
    {
        EXPECT_EQ(tree.get(std::to_string(i)), std::to_string(i));
    }

    // Remove the odd keys
    for (long long unsigned int i = 1; i < upper_limit; i += 2)
    {
        tree.remove(std::to_string(i));
    }

    // Verify the values
    for (long long unsigned int i = 0; i < upper_limit; i++)
    {
        if (i % 2 == 0)
        {
            EXPECT_EQ(tree.get(std::to_string(i)), std::to_string(i));
        }
        else
        {
            EXPECT_EQ(tree.get(std::to_string(i)), "");
        }
    }
}
