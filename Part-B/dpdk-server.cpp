#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "ff_api.h"
#include <chrono>
#include <sys/resource.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Initialize F-Stack
    ff_init(argc, argv);

    // Create socket
    sockfd = ff_socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (ff_bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        ff_close(sockfd);
        return -1;
    }

    // Start listening
    if (ff_listen(sockfd, 5) < 0) {
        std::cerr << "Listen failed" << std::endl;
        ff_close(sockfd);
        return -1;
    }
    std::cout << "DPDK server listening on port " << PORT << "..." << std::endl;

    int total_bytes_received = 0;
    int total_packets = 1000000;  // Example test size
    auto start = std::chrono::high_resolution_clock::now();

    while (total_packets--) {
        connfd = ff_accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
        if (connfd < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        int len = ff_recv(connfd, buffer, BUFFER_SIZE, 0);
        if (len > 0) {
            total_bytes_received += len;

            std::string response = std::to_string(len) + "\n";
            ff_send(connfd, response.c_str(), response.size(), 0);
        }
        ff_close(connfd);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    double bandwidth = total_bytes_received / duration.count();

    std::cout << "Latency: " << duration.count() / total_packets << " seconds per packet\n";
    std::cout << "Bandwidth: " << bandwidth / (1024 * 1024) << " MB/s\n";

    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    std::cout << "CPU time (user): " << usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6 << " seconds\n";
    std::cout << "Memory usage: " << usage.ru_maxrss << " KB\n";

    ff_close(sockfd);
    ff_finalize();
    return 0;
}
