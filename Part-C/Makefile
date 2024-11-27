CXX = g++
CXXFLAGS = -std=c++23 -I/home/somya/Documents/acads/7th_Sem/DOCS/DOCS-DB/Part-A/include
LDFLAGS = -L/home/somya/Documents/acads/7th_Sem/DOCS/DOCS-DB/Part-A/build/src -llsm-tree

all: server client

server: server.cpp
	$(CXX) $(CXXFLAGS) server.cpp -o server $(LDFLAGS)


client: client.cpp
	$(CXX) $(CXXFLAGS) client.cpp -o client $(LDFLAGS)

clean:
	rm -f server
	rm -f client
