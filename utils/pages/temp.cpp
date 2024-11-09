#include <iostream>
#include <semaphore>
#include <thread>

std::counting_semaphore<10> pageGetSemaphore(10);

void accessResource(int id)
{
    pageGetSemaphore.acquire();
    std::cout << "Thread " << id << " is accessing the resource.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Thread " << id << " is releasing the resource.\n";
    pageGetSemaphore.release();
}

int main()
{
    std::thread threads[5];
    for (int i = 0; i < 5; ++i)
    {
        threads[i] = std::thread(accessResource, i);
    }

    for (auto &t : threads)
    {
        t.join();
    }

    return 0;
}
