# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++23 -O2 -Wextra -pedantic
DEBUGFLAGS = -DLOG  # Enable logging with -DLOG

# Output executable for test
TEST_EXEC = test.out
DEBUG_TEST_EXEC = debug_test.out

# Source files
SRCS = pages.cpp
HEADERS = pages.h

# Object files
OBJS = $(SRCS:.cpp=.o)
DEBUG_OBJS = $(SRCS:.cpp=_debug.o)

# Path to the ../config and ../debug directories
CONFIG_DIR = ../config
DEBUG_DIR = ../debug

# Object files from config.cpp and debug.cpp
CONFIG_OBJS = $(CONFIG_DIR)/config.o
DEBUG_DIR_OBJS = $(DEBUG_DIR)/debug.o

# Default target (compile object files without logging)
all: $(OBJS)

# Rule to compile source files to object files without logging
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile object files with -DLOG (for debug)
debug: $(DEBUG_OBJS)

# Rule to compile source files to object files with logging enabled
%_debug.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c $< -o $@

# Rule to compile the test executable without logging
test: $(TEST_EXEC)

$(TEST_EXEC): test.cpp $(OBJS) $(CONFIG_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Rule to compile the test executable with logging enabled
debug_test: $(DEBUG_TEST_EXEC)

$(DEBUG_TEST_EXEC): test.cpp $(DEBUG_OBJS) $(CONFIG_OBJS) $(DEBUG_DIR_OBJS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $^ -o $@

# Clean up object files and executables
clean:
	rm -f $(OBJS) $(DEBUG_OBJS) $(TEST_EXEC) $(DEBUG_TEST_EXEC)

.PHONY: all debug test debug_test clean
