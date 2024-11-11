cmake_minimum_required(VERSION 3.14)

project(DOCS-DB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.50 REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
message(STATUS "Boost version: ${Boost_VERSION}")

# Add subdirectory for source code
add_subdirectory(src)

# Add a subdirectory for testing
add_subdirectory(test)

file(GLOB db_app repl.cpp)
add_executable(repl ${db_app})
target_link_libraries(repl ${Boost_LIBRARIES})
target_link_libraries(repl lsm-tree)
