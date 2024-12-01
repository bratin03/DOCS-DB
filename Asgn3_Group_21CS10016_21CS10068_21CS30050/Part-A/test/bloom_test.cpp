/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @file bloom_test.cpp
 * @brief Unit tests for the bloom_filter class.
 */

#include <gtest/gtest.h>
#include "../src/bloom_filter/bloom.h"

/**
 * @brief Test fixture for the Bloom filter class.
 *
 * This fixture sets up a new bloom filter before each test and deletes it after each test.
 *
 * The fixture also provides common setup and teardown methods for the tests.
 */
class BloomFilterTest : public ::testing::Test
{
protected:
    bloom_filter *filter;

    // SetUp is called before every test
    void SetUp() override
    {
        filter = new bloom_filter(1000); // Create a bloom filter with a size of 1000
    }

    // TearDown is called after every test
    void TearDown() override
    {
        delete filter;
    }
};

/**
 * @brief Test for setting and checking a key in the Bloom filter.
 *
 * This test sets a key in the filter and then checks if it exists.
 */
TEST_F(BloomFilterTest, SetAndCheckKeyTest)
{
    std::string key = "key1";
    filter->set(key);

    // The key should be set and exist in the filter
    EXPECT_TRUE(filter->is_set(key));
}

/**
 * @brief Test for checking an unset key in the Bloom filter.
 *
 * This test checks if a key that has not been set exists in the filter.
 */
TEST_F(BloomFilterTest, CheckUnsetKeyTest)
{
    std::string key = "key2";

    // The key has not been set, so it should not exist in the filter
    EXPECT_FALSE(filter->is_set(key));
}

/**
 * @brief Test for setting multiple keys in the Bloom filter.
 *
 * This test sets multiple keys in the filter and then checks if they exist.
 */
TEST_F(BloomFilterTest, SetMultipleKeysTest)
{
    std::string key1 = "key1";
    std::string key2 = "key2";

    // Set both keys
    filter->set(key1);
    filter->set(key2);

    // Both keys should be set
    EXPECT_TRUE(filter->is_set(key1));
    EXPECT_TRUE(filter->is_set(key2));
}

/**
 * @brief Test for false positives in the Bloom filter.
 *
 * This test sets a key in the filter and then checks if a different key is reported as present.
 */
TEST_F(BloomFilterTest, FalsePositiveTest)
{
    std::string key1 = "key1";
    std::string key2 = "key2";

    filter->set(key1); // Set key1

    // There is a small chance that key2 may be reported as present, but it shouldn't be guaranteed
    EXPECT_FALSE(filter->is_set(key2)); // Key2 has not been set, so we expect a false result most of the time
}

/**
 * @brief Test for key collisions in the Bloom filter.
 *
 * This test sets two keys that collide in the filter and then checks if they are both reported as present.
 */
TEST_F(BloomFilterTest, CollisionTest)
{
    std::string key1 = "collision_key1";
    std::string key2 = "collision_key2";

    filter->set(key1);
    filter->set(key2);

    // Both keys have been set, so both should be marked as set
    EXPECT_TRUE(filter->is_set(key1));
    EXPECT_TRUE(filter->is_set(key2));
}
