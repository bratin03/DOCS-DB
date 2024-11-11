#ifndef PAGES_H
#define PAGES_H

#include <cstdint>
#include <map>
#include <string>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <unistd.h>
#include <mutex>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore>
#include <atomic>
#include "../config/config.h"
#include "../debug/debug.h"
using namespace std;

#ifdef LOG
const string LOG_PAGE_FILE = "/tmp/utils_page.log";
#endif

extern uint32_t pageCount;
const uint32_t HASHMOD = 10;
const uint32_t MAX_PAGES_IN_MEMORY = 10;
const uint32_t PATH_MAX_ = 500;
const uint32_t PAGE_SIZE = 4096;

class PageInMemory
{
private:
public:
    uint32_t LRUCounter;
    atomic<uint32_t> refCount;
    uint32_t pageID;
    uint32_t fd;
    bool dirty;
    void *startptr;

    PageInMemory(uint32_t pageID, uint32_t fd)
    {
#ifdef LOG
        log(LOG_PAGE_FILE, INFO, "PageInMemory Constructor: PageID: " + to_string(pageID) + " FD: " + to_string(fd));
#endif

        this->pageID = pageID;
        this->fd = fd;
        this->LRUCounter = 0;
        this->refCount = 0;
        this->dirty = false;
    }

    ~PageInMemory()
    {
#ifdef LOG
        log(LOG_PAGE_FILE, INFO, "PageInMemory Destructor: PageID: " + to_string(pageID) + " FD: " + to_string(fd));
#endif
        close(fd);
    }
};

class PageManager
{

private:
    map<uint32_t, PageInMemory *> pageMap;
    mutex pageCreateMutex;
    mutex pageRemoveMutex;
    counting_semaphore<MAX_PAGES_IN_MEMORY> *pageGetSemaphore;

public:
    PageManager()
    {
#ifdef LOG
        log(LOG_PAGE_FILE, INFO, "PageManager Constructor");
#endif
        pageGetSemaphore = new counting_semaphore<MAX_PAGES_IN_MEMORY>(MAX_PAGES_IN_MEMORY);
    }

    ~PageManager()
    {
#ifdef LOG
        log(LOG_PAGE_FILE, INFO, "PageManager Destructor");
#endif
        delete pageGetSemaphore;
    }

    void updateLRU(uint32_t pageID);
    string getPrevDirectory();
    uint32_t createPage(ConfigManager &config);
    void *getPage(uint32_t pageID);
    void putPage(uint32_t pageID);
    void evictPage();
    uint32_t getLRUPageID();
};

#endif