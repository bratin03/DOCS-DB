# Define the C++ compiler and the standard
CXX = g++
CXXFLAGS = -Wall -std=c++23 -O2 -Wextra -pedantic
DEBUGFLAGS = -DLOG  # Enable logging with -DLOG

# Define the source files and object files
SRCS = config.cpp
OBJS = $(SRCS:.cpp=.o)

# Default rule to build the object files for release
all: $(OBJS)

# Rule to build release object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to build object files in debug mode with -DLOG defined
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(OBJS)

# Clean up object files
clean:
	rm -f $(OBJS)
