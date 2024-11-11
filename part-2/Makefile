# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -O2

# Target binaries
SERVER_BIN = server
CLIENT_BIN = client

# Source files
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp

# Client arguments (can be set when running make)
CLIENT_ARGS = "1000"

# Default target to build both client and server
all: $(SERVER_BIN) $(CLIENT_BIN)

# Rule for building the server
$(SERVER_BIN): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

# Rule for building the client
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC)

# Clean up build artifacts
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

# Run the server in NS3
run_server:
	sudo ip netns exec NS3 ./$(SERVER_BIN)

# Run the client in NS1
run_client:
	sudo ip netns exec NS1 ./$(CLIENT_BIN) $(CLIENT_ARGS)

# Run both client and server in their respective namespaces
run: run_server run_client
