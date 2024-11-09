#include "pages.h"


string PageManager::getPrevDirectory()
{
    char current_directory[PATH_MAX];
    if (getcwd(current_directory, sizeof(current_directory)) == NULL)
    {
        cout << "Error in getting current directory\n";
        return "";
    }
    char *last_slash = strrchr(current_directory, '/');

    if (last_slash == NULL)
    {
        cout << "Error in getting current directory\n";
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
    pageCreateMutex.unlock();
    return temp_pageCount;
}


void *PageManager::getPage(uint32_t pageID)
{
    if (pageMap.find(pageID) != pageMap.end())
    {
        cout << "Page already in memory\n";
        pageMap[pageID]->refCount++;
        updateLRU(pageID);
        return pageMap[pageID]->startptr;
    }

    evictPage();
    pageGetSemaphore->acquire();

    hash<uint32_t> hash_fn;
    uint32_t hashValue = hash_fn(pageID) % HASHMOD;

    string dirPath = getPrevDirectory() + "/tmp/" + to_string(hashValue);
    string filePath = dirPath + "/" + to_string(pageID);
    int fd = open(filePath.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        cout << "Error in opening file\n";
        return NULL;
    }
    ftruncate(fd, PAGE_SIZE);
    void *startptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (startptr == MAP_FAILED)
    {
        cout << "Error in mmap\n";
        return;
    }

    PageInMemory *page = new PageInMemory(pageID, fd);
    page->startptr = startptr;
    pageMap[pageID] = page;
    pageMap[pageID]->refCount++;
    updateLRU(pageID);

    return startptr;
}


void PageManager::putPage(uint32_t pageID)
{
    if (pageMap.find(pageID) == pageMap.end())
    {
        cout << "Page not in memory\n";
        return;
    }
    pageMap[pageID]->refCount--;
}


void PageManager::evictPage()
{
    uint32_t lruPageID = getLRUPageID();
    if (pageMap.find(lruPageID) == pageMap.end())
    {
        cout << "Error in getting LRU page\n";
        return;
    }
    if (pageMap[lruPageID]->refCount == 0)
    {
        pageRemoveMutex.lock();
        removePage(lruPageID);
        pageRemoveMutex.unlock();
    }
}


void PageManager::updateLRU(uint32_t pageID)
{
    if (pageMap.find(pageID) == pageMap.end())
    {
        cout << "Error in updating LRU\n";
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