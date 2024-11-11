# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++23 -O2 -Wextra -pedantic 
TESTFLAGS = -pthread

# Source files
SOURCES = debug.cpp
HEADERS = debug.h
OBJECTS = $(SOURCES:.cpp=.o)

# Test file
TEST = test.cpp
TEST_EXEC = test.out

# Targets
all: $(OBJECTS)

# Compile the test executable with object files (no library)
test: $(TEST_EXEC)

$(TEST_EXEC): $(TEST) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(TESTFLAGS) $^ -o $@

# Pattern rules
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TEST_EXEC) *.log

.PHONY: all test clean
