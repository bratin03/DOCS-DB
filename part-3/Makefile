# Makefile for DOCS DB project

CXX = g++
CXXFLAGS = -std=c++17 -pthread -O2
TARGETS = server client

all: $(TARGETS)

server: Server.cpp StorageEngine.cpp StorageEngine.h
	$(CXX) $(CXXFLAGS) -o server Server.cpp StorageEngine.cpp

client: Client.cpp
	$(CXX) $(CXXFLAGS) -o client Client.cpp

clean:
	rm -f $(TARGETS)
	rm -f benchmark_results/*.txt
