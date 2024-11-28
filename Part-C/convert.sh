mkdir -p build

g++ -std=c++20 -v -shared -fPIC -o build/liblsm_tree_wrapper.so src/lsm_tree_wrapper.cpp -I../Part-A/src/lsm_tree -L../Part-A/build/src -l lsm-tree -pthread
