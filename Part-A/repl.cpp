/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Department of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include "src/lsm_tree/lsm_tree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>

// Define the number of worker threads (cores) to be used in the thread pool
#define NUM_CORES 4

// Uncomment the following line to enable TEST_MODE
// #define TEST_MODE

/**
 * @class ThreadPool
 * @brief A simple thread pool implementation for executing tasks concurrently.
 *
 * The ThreadPool manages a fixed number of worker threads that process tasks
 * from a queue. Tasks can be enqueued as `std::function<void()>` objects.
 */
class ThreadPool {
public:
    /**
     * @brief Constructs the ThreadPool and starts the worker threads.
     * @param num_threads The number of worker threads to create.
     */
    ThreadPool(size_t num_threads) : stop_flag(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;

                    // Acquire task from the queue
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this]() { return stop_flag || !tasks.empty(); });
                        if (stop_flag && tasks.empty())
                            return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    // Execute the task
                    task();
                }
            });
        }
    }

    /**
     * @brief Enqueues a new task to be executed by the thread pool.
     * @param task The task to execute, represented as a `std::function<void()>`.
     */
    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop_flag)
                throw std::runtime_error("Enqueue on stopped ThreadPool");
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }

    /**
     * @brief Destructor. Signals all threads to stop and joins them.
     */
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop_flag = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers)
            worker.join();
    }

private:
    // Vector of worker threads
    std::vector<std::thread> workers;

    // Task queue
    std::queue<std::function<void()>> tasks;

    // Synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop_flag;
};

// Enumeration for supported commands
enum commands
{
    SET,
    GET,
    DEL,
    EXIT,
    NONE,
};

/**
 * @brief Converts a string command to its corresponding enum value.
 * @param in_str The input command string.
 * @return The corresponding `commands` enum value.
 */
commands cmd_to_enum(const std::string &in_str)
{
    if (in_str == "SET" || in_str == "set")
        return SET;
    if (in_str == "GET" || in_str == "get")
        return GET;
    if (in_str == "DEL" || in_str == "del")
        return DEL;
    if (in_str == "EXIT" || in_str == "exit")
        return EXIT;
    return NONE;
}

/**
 * @brief Processes commands from an input file and executes them using a thread pool.
 * @param filename The name of the input file containing commands.
 * @param outfile The output file stream for logging results.
 */
void command_loop(const std::string &filename, std::ofstream &outfile)
{
    lsm_tree db;
    bool run = true;

    // Open the input file for reading
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
        outfile << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Initialize the thread pool with NUM_CORES worker threads
    ThreadPool pool(NUM_CORES);

    // Mutex for synchronized writing to the output file
    std::mutex output_mutex;

    std::string line;
    outfile << "REPL started. Reading from file: " << filename << std::endl;

    while (std::getline(infile, line) && run) // Read each line from the file
    {
        // Create a copy of the line for the lambda capture
        std::string command_line = line;

        // Enqueue the command for processing
        pool.enqueue([&, command_line]() {
            std::istringstream iss(command_line);
            std::string command, key, value;
            iss >> command; // Read the first word (command)

            switch (cmd_to_enum(command))
            {
            case SET:
                if (iss >> key && std::getline(iss, value))
                {
                    // Extract value inside quotes if it's in the format: "value"
                    size_t start = value.find('"');
                    size_t end = value.rfind('"');

                    if (start != std::string::npos && end != std::string::npos && start < end)
                    {
                        value = value.substr(start + 1, end - start - 1);
                        db.put(key, value);

                        // Log the SET operation
                        {
                            std::lock_guard<std::mutex> lock(output_mutex);
                            outfile << "SET: " << key << " -> \"" << value << "\"" << std::endl;
                        }
                    }
                    else
                    {
                        // Log invalid SET format
                        {
                            std::lock_guard<std::mutex> lock(output_mutex);
                            outfile << "Invalid format for SET. Use: SET <key> \"<value>\"" << std::endl;
                        }
                    }
                }
                else
                {
                    // Log invalid SET format
                    {
                        std::lock_guard<std::mutex> lock(output_mutex);
                        outfile << "Invalid format for SET. Use: SET <key> \"<value>\"" << std::endl;
                    }
                }
                break;

            case GET:
                if (iss >> key)
                {
                    if (key.empty())
                    {
                        // Log empty key
                        {
                            std::lock_guard<std::mutex> lock(output_mutex);
                            outfile << "GET Error: Key cannot be empty." << std::endl;
                        }
                    }
                    else
                    {
                        std::string result = db.get(key);

#ifdef TEST_MODE
                        // In TEST_MODE, verify that key and value are the same
                        if (key != result)
                        {
                            // Log failure and exit
                            {
                                std::lock_guard<std::mutex> lock(output_mutex);
                                outfile << "TEST_MODE Failed: Key \"" << key << "\" does not match Value \"" << result << "\"" << std::endl;
                            }
                            std::exit(1);
                        }
#endif

                        // Log the GET result
                        {
                            std::lock_guard<std::mutex> lock(output_mutex);
                            outfile << "GET: " << key << " -> \"" << result << "\"" << std::endl;
                        }
                    }
                }
                else
                {
                    // Log invalid GET format
                    {
                        std::lock_guard<std::mutex> lock(output_mutex);
                        outfile << "Invalid format for GET. Use: GET <key>" << std::endl;
                    }
                }
                break;

            case DEL:
                if (iss >> key)
                {
                    if (key.empty())
                    {
                        // Log empty key
                        {
                            std::lock_guard<std::mutex> lock(output_mutex);
                            outfile << "DEL Error: Key cannot be empty." << std::endl;
                        }
                    }
                    else
                    {
                        db.remove(key);

                        // Log the DEL operation
                        {
                            std::lock_guard<std::mutex> lock(output_mutex);
                            outfile << "DEL: " << key << std::endl;
                        }
                    }
                }
                else
                {
                    // Log invalid DEL format
                    {
                        std::lock_guard<std::mutex> lock(output_mutex);
                        outfile << "Invalid format for DEL. Use: DEL <key>" << std::endl;
                    }
                }
                break;

            case EXIT:
                // Log the EXIT command
                {
                    std::lock_guard<std::mutex> lock(output_mutex);
                    outfile << "EXIT command encountered. Stopping REPL." << std::endl;
                }
                // Set the run flag to false to stop reading further commands
                // Note: This does not immediately stop already enqueued tasks
                run = false;
                break;

            default:
                // Log unknown command
                {
                    std::lock_guard<std::mutex> lock(output_mutex);
                    outfile << "Command '" << command << "' not found." << std::endl;
                }
                break;
            }
        });
    }

    infile.close(); // Close the input file when done

    // Destructor of ThreadPool will wait for all tasks to complete
    // Log the completion of REPL
    {
        std::lock_guard<std::mutex> lock(output_mutex);
        outfile << "REPL finished processing commands." << std::endl;
    }
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ofstream outfile("output.txt");

    if (!outfile.is_open())
    {
        std::cerr << "Failed to open output file: output.txt" << std::endl;
        return 1;
    }

    command_loop(filename, outfile);

    outfile.close();

    return 0;
}
