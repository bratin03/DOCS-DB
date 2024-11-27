cmake_minimum_required(VERSION 3.14)

project(DOCS-DB)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the policy for CMP0167 to NEW
cmake_policy(SET CMP0167 NEW)

# Add common compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra  -O3")

# Boost Configuration
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Find Boost libraries
find_package(Boost 1.50 REQUIRED COMPONENTS filesystem)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    message(STATUS "Boost version: ${Boost_VERSION}")
else()
    message(FATAL_ERROR "Boost not found. Install Boost and try again.")
endif()

# Add the source code directory
add_subdirectory(src)

# Add the test directory
add_subdirectory(test)

# Collect and compile REPL application
file(GLOB DB_APP repl.cpp)
add_executable(repl ${DB_APP})

# Link Boost, pthread, and the custom LSM-tree library
target_link_libraries(repl PRIVATE ${Boost_LIBRARIES} lsm-tree pthread)

# Ensure C++20 standard for repl target
set_target_properties(repl PROPERTIES CXX_STANDARD 20)
