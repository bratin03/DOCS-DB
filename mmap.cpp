#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main() {
    // Open or create the file
    int fd = open("shared_page.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    // Ensure file is large enough for a memory page
    size_t page_size = sysconf(_SC_PAGESIZE);
    ftruncate(fd, page_size); // Set the file size to a page

    // Map the file into memory
    void* page = mmap(nullptr, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (page == MAP_FAILED) {
        std::cerr << "mmap failed" << std::endl;
        close(fd);
        return 1;
    }

    // Write data to the memory-mapped page
    strcpy(static_cast<char*>(page), "Hello, persistent world!");

    // Unmap and close the file
    munmap(page, page_size);
    close(fd);

    fd = open("shared_page.txt", O_RDONLY);
    if (fd == -1) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    page = mmap(nullptr, page_size, PROT_READ, MAP_SHARED, fd, 0);
    if (page == MAP_FAILED) {
        std::cerr << "mmap failed" << std::endl;
        close(fd);
        return 1;
    }

    std::cout << "Content of the file: " << static_cast<char*>(page) << std::endl;

    munmap(page, page_size);
    close(fd);



    return 0;
}
