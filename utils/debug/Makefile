# Makefile

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -g -fsanitize=address
AR = ar
ARFLAGS = rcs

# Source files
SOURCES = debug.cpp
HEADERS = debug.h
OBJECTS = $(SOURCES:.cpp=.o)

# Static library name
LIBRARY = libdebug.a

# Test file
TEST = test.cpp
TEST_EXEC = test.out

# Targets
all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

# Compile the test executable with the library
test: $(TEST_EXEC)

$(TEST_EXEC): $(TEST) $(LIBRARY)
	$(CXX) $(CXXFLAGS) -o $@ $< -L. -ldebug

# Pattern rules
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJECTS) $(LIBRARY) $(TEST_EXEC)

.PHONY: all test clean
