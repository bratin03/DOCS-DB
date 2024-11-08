#include <cstdint>
#include <map>
#include <string>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;

extern uint64_t pageCount;
const uint64_t HASHMOD = 10000;
const uint64_t MAX_PAGES_IN_MEMORY = 10;
const uint64_t PATH_MAX = 500;


class PageInMemory {
    private:
        uint64_t LRUCounter;
        bool removable;
        uint64_t pageID;
        uint64_t fd;

    
    public:

    // Define Getters and Setters as and when required  
};


// map<uint64_t, PageInMemory*> pageMap;




// FUNCTION DEFINITIONS

void createPage();

void removePage(uint64_t pageID);

string getPrevDirectory();


