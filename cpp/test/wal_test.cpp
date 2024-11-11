/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include <gtest/gtest.h>
#include <fstream>
#include "../src/wal/wal.h"
#include "../src/red_black_tree/red_black.h"

// Test class for Write Ahead Log (WAL)
class WALTest : public ::testing::Test
{
protected:
    // Create a temporary file for testing purposes
    std::string test_filename = "test_wal.log";
    write_ahead_log wal;

    WALTest() : wal(test_filename) {}

    // Clean up after each test
    void TearDown() override
    {
        std::remove(test_filename.c_str()); // Delete the temporary file after test
    }
};

// Test initialization and file creation
TEST_F(WALTest, InitializationTest)
{
    std::ifstream file(test_filename);
    ASSERT_TRUE(file.good()) << "Failed to create WAL file";
    file.close();
}

// Test clearing the WAL
TEST_F(WALTest, ClearTest)
{
    std::string content = "Test log entry";

    // Append to the WAL
    wal.append(content);

    // Clear the WAL
    wal.clear();

    // Open the file and ensure it's empty
    std::ifstream file(test_filename);
    std::string file_content;
    std::getline(file, file_content);
    file.close();

    ASSERT_TRUE(file_content.empty()) << "WAL file was not cleared correctly";
}