#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <sys/resource.h>
#include <csignal>
#include <thread>

using namespace std;

#define PORT 9000
#define BUFFER_SIZE 1024

int server_fd;  // Server file descriptor for socket

// Function to handle each client request
void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    int total_bytes_received = 0;
    auto start = chrono::high_resolution_clock::now();

    while (true) {
        // Receive data from client
        int bytes_received = read(client_socket, buffer, BUFFER_SIZE);
        if (bytes_received <= 0) {
            if (bytes_received < 0) {
                perror("Read error");
            } else {
                cout << "Client closed the connection." << endl;
            }
            break;
        }
        total_bytes_received += bytes_received;

        // Send back the size of the received data
        string response = to_string(bytes_received) + "\n";
        send(client_socket, response.c_str(), response.length(), 0);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    double bandwidth = total_bytes_received / duration.count();

    cout << "Total bytes received: " << total_bytes_received << endl;
    cout << "Latency: " << duration.count() / (total_bytes_received / BUFFER_SIZE) << " seconds per packet\n";
    cout << "Bandwidth: " << bandwidth / (1024 * 1024) << " MB/s\n";

    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    cout << "CPU time (user): " << usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6 << " seconds\n";
    cout << "Memory usage: " << usage.ru_maxrss << " KB\n";

    close(client_socket);
}

// Signal handler to gracefully close the listening socket
void signal_handler(int signum) {
    cout << "\nShutting down server..." << endl;
    close(server_fd);
    exit(signum);
}

int main() {
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Bind to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    cout << "Server listening on port " << PORT << "..." << endl;

    // Main loop to accept and handle client connections
    while (true) {
        int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        cout << "Accepted new connection" << endl;

        // Launch a new thread to handle the client connection
        thread client_thread(handle_request, client_socket);
        client_thread.detach();  // Detach the thread so it runs independently
    }

    return 0;
}
