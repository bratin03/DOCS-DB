file(GLOB_RECURSE LIB_SOURCES *.cpp)
file(GLOB_RECURSE LIB_HEADERS *.hpp)

# Ensure position-independent code for the shared library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Create shared library
add_library(lsm-tree SHARED ${LIB_SOURCES} ${LIB_HEADERS})

# Create static library
add_library(lsm-tree-static STATIC ${LIB_SOURCES} ${LIB_HEADERS})
