#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "../Part-A/src/lsm_tree/lsm_tree.h"

using namespace std;

#define PORT 6379 ///< Port number for the server
#define THREAD_POOL_SIZE 4 ///< Number of threads in the thread pool

lsm_tree db; ///< Instance of the StorageEngine

/// Shared task queue and synchronization primitives
queue<int> taskQueue;
mutex queueMutex;
condition_variable queueCondition;

/**
 * @brief Parses the RESP protocol input.
 * 
 * @param input The RESP-formatted command string.
 * @return vector<string> Parsed tokens of the RESP command.
 */
vector<string> parseRESP(const string &input) {
    istringstream stream(input);
    string line;
    vector<string> tokens;

    while (getline(stream, line, '\r')) {
        if (!line.empty()) {
            tokens.push_back(line);
        }
        stream.ignore(1); // Ignore the '\n'
    }

    return tokens;
}

/**
 * @brief Processes a parsed RESP command and executes it on the StorageEngine.
 *
 * @param tokens Parsed tokens from the RESP command.
 * @return string The response in RESP format.
 */
string processCommand(const vector<string> &tokens) {
    if (tokens.empty() || tokens.size() < 3) {
        return "-ERR invalid command format\r\n";
    }

    const string &command = tokens[2];

    if (command == "SET" && tokens.size() >= 5) {
        db.put(tokens[4], tokens[6]);
        return "+OK\r\n";
    } else if (command == "GET" && tokens.size() >= 4) {
        string value = db.get(tokens[4]);
        return value.empty() ? "$-1\r\n" : "$" + to_string(value.size()) + "\r\n" + value + "\r\n";
    } else if (command == "DEL" && tokens.size() >= 4) {
        db.remove(tokens[4]);
        return ":1\r\n";
    }

    return "-ERR unknown or unsupported command\r\n";
}

/**
 * @brief Handles the client connection, reads input, processes commands, and sends responses.
 *
 * @param clientSocket Socket file descriptor for the client connection.
 */
void handleClient(int clientSocket) {
    char buffer[1024] = {0};
    int bytesReceived;
    while ((bytesReceived = read(clientSocket, buffer, sizeof(buffer))) > 0) {
        string input(buffer, bytesReceived);

        // Parse and process the RESP input
        auto tokens = parseRESP(input);
        if (tokens.empty()) {
            send(clientSocket, "-ERR invalid input\r\n", 20, 0);
            continue;
        }

        string response = processCommand(tokens);

        // Send the response back to the client
        send(clientSocket, response.c_str(), response.size(), 0);
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer for the next read
    }

    if (bytesReceived < 0) {
        perror("Error reading from client socket");
    }

    close(clientSocket);
}

/**
 * @brief Thread worker function to process tasks from the shared task queue.
 */
void workerThread() {
    while (true) {
        int clientSocket;

        // Acquire a task from the task queue
        {
            unique_lock<mutex> lock(queueMutex);

            // Wait until the queue is not empty
            queueCondition.wait(lock, [] { return !taskQueue.empty(); });

            // Get the next task
            clientSocket = taskQueue.front();
            taskQueue.pop();
        }

        // Process the client connection
        handleClient(clientSocket);
    }
}

/**
 * @brief Main function to start the server, initialize the thread pool, and listen for connections.
 *
 * @return int Exit status of the program.
 */
int main() {
    int serverFd, clientSocket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Bind socket to address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(serverFd, 3) < 0) {
        perror("Listen failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    cout << "Server started on port " << PORT << endl;

    // Create thread pool
    vector<thread> threadPool;
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        threadPool.emplace_back(workerThread);
    }

    // Accept connections in a loop and push them into the task queue
    while (true) {
        if ((clientSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        cout << "Connection Established" << endl;

        // Push the client socket into the task queue
        {
            lock_guard<mutex> lock(queueMutex);
            taskQueue.push(clientSocket);
        }
        queueCondition.notify_one(); // Notify one worker thread
    }

    // Join the threads (in practice, the server will usually not exit)
    for (auto &t : threadPool) {
        t.join();
    }

    close(serverFd);
    return 0;
}
