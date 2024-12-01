# Add the src directory to the include path for the tests
include_directories(${PROJECT_SOURCE_DIR}/src)

# Find Google Test
find_package(GTest REQUIRED)

# Include directories for Google Test
include_directories(${GTEST_INCLUDE_DIRS})

# Define a list of test sources with corresponding executables and source files
set(TESTS
    bloom_test
    rb_tree_test
    lsm_tree_test
    wal_test 
    docs_db_test
)

# Define a list of source files for each test
set(bloom_test_SRC bloom_test.cpp ../src/bloom_filter/bloom.cpp)
set(rb_tree_test_SRC rb_tree_test.cpp ../src/red_black_tree/red_black.cpp ../src/red_black_tree/node.cpp)
set(lsm_tree_test_SRC lsm_tree_test.cpp ../src/lsm_tree/lsm_tree.cpp ../src/red_black_tree/red_black.cpp ../src/bloom_filter/bloom.cpp ../src/red_black_tree/node.cpp ../src/lsm_tree/level/level.cpp ../src/wal/wal.cpp)
set(docs_db_test_SRC docs_db_test.cpp ../src/lsm_tree/lsm_tree.cpp ../src/red_black_tree/red_black.cpp ../src/bloom_filter/bloom.cpp ../src/red_black_tree/node.cpp ../src/lsm_tree/level/level.cpp ../src/wal/wal.cpp)

# Add source files for red_black_tree to wal_test
set(wal_test_SRC wal_test.cpp 
                 ../src/wal/wal.cpp 
                 ../src/red_black_tree/red_black.cpp 
                 ../src/red_black_tree/node.cpp)


# Loop through each test, create executable, link libraries, and add to CTest
foreach(test ${TESTS})
    add_executable(${test} ${${test}_SRC})
    target_link_libraries(${test} ${GTEST_LIBRARIES} gtest_main pthread)
    add_test(NAME ${test} COMMAND ${test})
endforeach()
