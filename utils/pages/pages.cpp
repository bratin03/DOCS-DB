#include "pages.h"

uint32_t pageCount = 0;

string PageManager::getPrevDirectory()
{
    char current_directory[PATH_MAX];
    if (getcwd(current_directory, sizeof(current_directory)) == NULL)
    {
#ifdef LOG
        log(LOG_PAGE_FILE, ERROR, "Error in getting current directory");
#endif
        return "";
    }
    char *last_slash = strrchr(current_directory, '/');

    if (last_slash == NULL)
    {
#ifdef LOG
        log(LOG_PAGE_FILE, ERROR, "Error in getting previous directory");
#endif
        return "";
    }
    *last_slash = '\0';
    return string(current_directory);
}

uint32_t PageManager::createPage(ConfigManager &config)
{
    pageCreateMutex.lock();
    pageCount++;
    auto temp_pageCount = pageCount;
    config.set("pageCount", to_string(pageCount));
#ifdef LOG
    log(LOG_PAGE_FILE, INFO, "Page Created: PageID: " + to_string(temp_pageCount));
#endif
    pageCreateMutex.unlock();
    return temp_pageCount;
}

void *PageManager::getPage(uint32_t pageID)
{
    if (pageMap.find(pageID) != pageMap.end())
    {
#ifdef LOG
        log(LOG_PAGE_FILE, INFO, "Page Found in Memory: PageID: " + to_string(pageID));
#endif
        pageMap[pageID]->refCount++;
        updateLRU(pageID);
        return pageMap[pageID]->startptr;
    }

#ifdef LOG
    log(LOG_PAGE_FILE, INFO, "Page Not Found in Memory: PageID: " + to_string(pageID));
#endif
    evictPage();
    pageGetSemaphore->acquire();

    hash<uint32_t> hash_fn;
    uint32_t hashValue = hash_fn(pageID) % HASHMOD;

    string dirPath = getPrevDirectory() + "/tmp/" + to_string(hashValue);

    bool op = filesystem::create_directories(dirPath);
    if (!op)
    {
#ifdef LOG
        log(LOG_PAGE_FILE, ERROR, "Error in creating directory: " + dirPath);
#endif
        return NULL;
    }
    string filePath = dirPath + "/" + to_string(pageID);
    ofstream file(filePath);

    int fd = open(filePath.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
#ifdef LOG
        log(LOG_PAGE_FILE, ERROR, "Error in opening file: " + filePath);
#endif
        return NULL;
    }
    ftruncate(fd, PAGE_SIZE);
    void *startptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (startptr == MAP_FAILED)
    {
#ifdef LOG
        log(LOG_PAGE_FILE, ERROR, "Error in mmap");
#endif
        return NULL;
    }

    PageInMemory *page = new PageInMemory(pageID, fd);
    page->startptr = startptr;
    pageMap[pageID] = page;
    pageMap[pageID]->refCount++;
    updateLRU(pageID);

#ifdef LOG
    log(LOG_PAGE_FILE, INFO, "Page Loaded in Memory: PageID: " + to_string(pageID));
#endif

    return startptr;
}

void PageManager::putPage(uint32_t pageID)
{
    if (pageMap.find(pageID) == pageMap.end())
    {
#ifdef LOG
        log(LOG_PAGE_FILE, ERROR, "Error in putting page: PageID: " + to_string(pageID));
#endif
        return;
    }
    pageMap[pageID]->refCount--;
#ifdef LOG
    if (pageMap[pageID]->refCount == 0)
    {
        log(LOG_PAGE_FILE, INFO, "Page RefCount 0: PageID: " + to_string(pageID));
    }
#endif
}

void PageManager::evictPage()
{
    uint32_t lruPageID = getLRUPageID();
    if (pageMap.find(lruPageID) == pageMap.end())
    {
#ifdef LOG
        log(LOG_PAGE_FILE, WARNING, "Error in evicting page: PageID: " + to_string(lruPageID));
#endif
        return;
    }
    if (pageMap[lruPageID]->refCount == 0)
    {
        pageRemoveMutex.lock();
        munmap(pageMap[lruPageID]->startptr, PAGE_SIZE);
        delete pageMap[lruPageID];
        pageMap.erase(lruPageID);
        pageGetSemaphore->release();
#ifdef LOG
        log(LOG_PAGE_FILE, INFO, "Page Evicted: PageID: " + to_string(lruPageID));
#endif
        pageRemoveMutex.unlock();
    }
}

void PageManager::updateLRU(uint32_t pageID)
{
    if (pageMap.find(pageID) == pageMap.end())
    {
#ifdef LOG
        log(LOG_PAGE_FILE, ERROR, "Error in updating LRU: PageID: " + to_string(pageID));
#endif
        return;
    }
    pageMap[pageID]->LRUCounter = 0;
    for (auto &page : pageMap)
    {
        if (page.first != pageID)
        {
            page.second->LRUCounter++;
        }
    }
}

uint32_t PageManager::getLRUPageID()
{
    uint32_t maxLRU = 0;
    uint32_t lruPageID = 0;
    for (auto &page : pageMap)
    {
        if (page.second->LRUCounter > maxLRU)
        {
            maxLRU = page.second->LRUCounter;
            lruPageID = page.first;
        }
    }
    return lruPageID;
}