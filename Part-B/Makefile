TOPDIR = /data/f-stack

ifeq ($(FF_PATH),)
	FF_PATH = ${TOPDIR}
endif

ifneq ($(shell pkg-config --exists libdpdk && echo 0),0)
$(error "No installation of DPDK found, maybe you should export environment variable `PKG_CONFIG_PATH`")
endif

PKGCONF ?= pkg-config

CXX = g++
CXXFLAGS = -std=c++20 -pthread -O0 -g -gdwarf-2 $(shell $(PKGCONF) --cflags libdpdk)

LIBS+= $(shell $(PKGCONF) --static --libs libdpdk)
LIBS += -L${FF_PATH}/lib -Wl,--whole-archive,-lfstack,--no-whole-archive
LIBS += -Wl,--no-whole-archive -lrt -lm -ldl -lcrypto -pthread -lnuma
LIBS += -lstdc++  # <-- Add this line to link against the C++ standard library

SRCS = src/server.cpp src/client.cpp src/dpdk_server.cpp
OBJS = $(SRCS:src/%.cpp=src/%.o)

SERVER_EXEC = server
CLIENT_EXEC = client
DPDK_SERVER_EXEC = dpdk_server

all: $(SERVER_EXEC) $(CLIENT_EXEC) $(DPDK_SERVER_EXEC)

$(SERVER_EXEC): src/server.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(CLIENT_EXEC): src/client.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(DPDK_SERVER_EXEC): src/dpdk_server.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f src/*.o $(SERVER_EXEC) $(CLIENT_EXEC) $(DPDK_SERVER_EXEC)

distclean: clean

.PHONY: all clean distclean
