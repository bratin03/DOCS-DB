>> Assigned to @somya-kr
> The current design is wrong. We are not guaranteeing that a page that we are providing to the user is not evicted. So every page that is currently being read needs to be locked.

### Design
- static uint64_t pageCount = 0;
- const int hashMod = 1000;
- const MAX_PAGES_IN_MEMORY

### Data structures

// For bookkeeping of pages in main memory

class PageInMemory
   LRU_variable
   void* 
   fd
   bool removable

1. map [uint64_t, PageInMemory*] pageMap

### Methods

1. void createPage()
   1. Start with a directory named 'tmp'
   2. To create a new page, increment pageCount by 1 (Use mutex lock within an #ifdef MUTEX_LOCK) (This is because we are not sure if we actually need a lock here)
   3. Create a hash object with seed 1, hash the pageCount and take the mod with hashMod
   4. Create a directory with the name as this value if it does not exist
   5. Inside this directory, create a file with the name as pageCount

2. bool deletePage(uint64_t pageId)
   1. Hash the pageId and take the mod with hashMod
   2. Check if the directory with this name exists
   3. If it does, delete the file with the name as pageId
   4. If the directory is empty, delete the directory

3. void bringInMemory(uint64_t pageId)
   1. Hash the pageId and take the mod with hashMod
   2. Check if the directory with this name exists
   3. If it does, check if the file with the name as pageId exists
   4. If it does, check if the file is already in memory
   5. If it is not, bring it in memory
   6. If it is, update the LRU variable

4. void lockPageInMemory(uint64_t pageId, bool lock)
   1. Hash the pageId and take the mod with hashMod
   2. Check if the directory with this name exists
   3. If it does, check if the file with the name as pageId exists
   4. If it does, check if the file is already in memory
   5. If it is not, bring it in memory
   6. If it is, update the LRU variable
   7. Update the lock variable

5. void* readFrom(uint64_t pageId, uint64_t offset)
   1. Hash the pageId and take the mod with hashMod
   2. Check if the directory with this name exists
   3. If it does, check if the file with the name as pageId exists
   4. If it does, check if the file is already in memory
   5. If it is not, bring it in memory
   6. If it is, update the LRU variable
   7. Read the data from the file

6. void writeTo(uint64_t pageId, uint64_t offset, void* data, uint64_t size)
   1. Hash the pageId and take the mod with hashMod
   2. Check if the directory with this name exists
   3. If it does, check if the file with the name as pageId exists
   4. If it does, check if the file is already in memory
   5. If it is not, bring it in memory
   6. If it is, update the LRU variable
   7. Write the data to the file

