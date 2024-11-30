mkdir -p ../bin

g++ ../src/client.cpp -o ../bin/client -std=c++20 -pthread -O3 