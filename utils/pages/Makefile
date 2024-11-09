# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++23

# Output executable
TARGET = myprogram

# Source files
SRCS = main.cpp config.cpp pages.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = config.h pages.h

# Default target
all: $(TARGET)

# Compile the source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program (optional)
run: $(TARGET)
	./$(TARGET)
