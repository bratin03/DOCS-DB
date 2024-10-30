#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <string>
#include "debug.h" // Include your debug header

// Global random number generator and distribution
std::mt19937 rng(std::random_device{}());
std::uniform_int_distribution<size_t> dist(0, 61); // 62 possible characters (0-9, a-z, A-Z)

// Function to generate random strings
std::string generate_random_string(size_t length)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i)
    {
        result += charset[dist(rng)]; // Use the global RNG and distribution
    }
    return result;
}

// Function that each thread will run
void log_random_info(int group_id, int num_lines)
{
    std::string file_name = "log_group_" + std::to_string(group_id) + ".dat";
    for (int i = 0; i < num_lines; ++i)
    {
        std::string message = generate_random_string(20); // Generate random string of length 20
        log(file_name, INFO, message);                    // Log the random message
    }
}

int main()
{
    const int num_groups = 4;
    const int num_threads_per_group = 4;
    const int lines_per_thread = 4096;

    std::vector<std::thread> threads;

    // Create threads divided into groups
    for (int group = 0; group < num_groups; ++group)
    {
        for (int thread_id = 0; thread_id < num_threads_per_group; ++thread_id)
        {
            threads.emplace_back(log_random_info, group, lines_per_thread);
        }
    }

    // Wait for all threads to finish
    for (auto &th : threads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    std::cout << "Logging completed!" << std::endl;
    return 0;
}
