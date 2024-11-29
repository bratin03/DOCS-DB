# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -fPIC -shared -std=c++20  # Added -std=c++20 to enable C++20 features

# Paths for includes and libraries
CFLAGS = -I../Part-A/src/lsm_tree  # Include path for lsm_tree.h
LDFLAGS = -L../Part-A/build/src -L. -llsm-tree -llsm_tree_wrapper_c -lstdc++  # Library paths and linking flags

# Specific paths for different build stages
INCLUDE_PATH = ../Part-A/src/lsm_tree
LIB_PATH = .
SRC_PATH = .
BIN_PATH = .

# Files
WRAPPER_SRC = $(SRC_PATH)/lsm_tree_wrapper_c.cpp
WRAPPER_LIB = $(LIB_PATH)/liblsm_tree_wrapper_c.so
MAIN_SRC = $(SRC_PATH)/main.c
MAIN_BIN = $(BIN_PATH)/main

# Default target (build both the wrapper and the main program)
all: $(WRAPPER_LIB) $(MAIN_BIN)

# Build the wrapper shared library (C++ wrapper for the lsm-tree)
$(WRAPPER_LIB): $(WRAPPER_SRC)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_PATH) -o $@ $^ $(LDFLAGS)

# Build the main C program (links to the wrapper library)
$(MAIN_BIN): $(MAIN_SRC)
	$(CC) -o $@ $^ -I$(LIB_PATH) $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f $(LIB_PATH)/*.so $(BIN_PATH)/$(MAIN_BIN)
