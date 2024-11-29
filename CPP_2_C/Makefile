# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -std=c++20 -Wall -I../Part-A/src/lsm_tree # Add the include path
CFLAGS = -Wall
LDFLAGS = -L../Part-A/build/src -llsm-tree-static -lstdc++

# Source and output files
WRAPPER_SRC = lsm_tree_wrapper_c.cpp
WRAPPER_OBJ = lsm_tree_wrapper_c.o
MAIN_SRC = main.c
EXECUTABLE = main

# Targets
all: $(EXECUTABLE)

# Build the main executable
$(EXECUTABLE): $(WRAPPER_OBJ) $(MAIN_SRC)
	$(CXX) $(WRAPPER_OBJ) $(MAIN_SRC) -o $(EXECUTABLE) $(LDFLAGS)

# Compile the wrapper object
$(WRAPPER_OBJ): $(WRAPPER_SRC)
	$(CXX) $(CXXFLAGS) -c $(WRAPPER_SRC) -o $(WRAPPER_OBJ)

# Clean build files
clean:
	rm -f $(EXECUTABLE) $(WRAPPER_OBJ)

# Phony targets
.PHONY: all clean
