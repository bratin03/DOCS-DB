/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include <iostream>
#include <chrono>
#include <random>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

// #define DEBUG

#define SERVER_IP "127.0.0.1" // Server IP address
#define SERVER_PORT 8080      // Server Port

/**
 * @brief Simulates a client sending multiple packets to a server and measuring the round-trip latency.
 *
 * This function connects to a server, sends a series of packets, and measures the round-trip time (latency) for each packet.
 * The client sends packets of random sizes between 512 and 1024 bytes and logs the latency and bandwidth metrics in a log file.
 *
 * @param client_id The unique identifier for the client (used for naming the log file).
 * @param num_packets The number of packets the client will send.
 */
void run_client(int client_id, int num_packets)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        return;
    }

    sockaddr_in server_addr{}; ///< Server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr); // Convert IP address to binary form

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        return;
    }

    char buffer[1024];                                       ///< Buffer to hold packet data
    std::mt19937 rng(std::random_device{}());                ///< Random number generator
    std::uniform_int_distribution<int> size_dist(512, 1024); // Random size between 0.5KB and 1KB

    long long total_latency = 0;    ///< Total latency accumulated over all packets
    long long total_bytes_sent = 0; ///< Total bytes sent to the server

    // Create a log file for storing client metrics
    std::ostringstream log_file_name;
    log_file_name << "./log/metrics_" << client_id << ".log";
    std::ofstream log_file(log_file_name.str());

    if (!log_file.is_open())
    {
        std::cerr << "Failed to open log file!" << std::endl;
        close(sock);
        return;
    }

    for (int i = 0; i < num_packets; ++i)
    {
        int packet_size = size_dist(rng);             ///< Random packet size
        std::fill(buffer, buffer + packet_size, 'A'); ///< Fill the buffer with data

        auto start = std::chrono::high_resolution_clock::now(); ///< Start time for latency measurement

        // Send data to the server
        auto ret_send = send(sock, buffer, packet_size, 0);
        if (ret_send < 0)
        {
            perror("Send failed");
            close(sock);
            return;
        }

#ifdef DEBUG
        std::cout << "Sent " << packet_size << " bytes" << std::endl;
#endif

        // Receive the response (length of data sent)
        int response;
        auto ret_recv = recv(sock, &response, sizeof(response), 0);
        if (ret_recv < 0)
        {
            perror("Receive failed");
            close(sock);
            return;
        }

#ifdef DEBUG
        response = ntohl(response);
        std::cout << "Response: " << response << std::endl;
#endif

        auto end = std::chrono::high_resolution_clock::now(); ///< End time for latency measurement
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        total_latency += latency;
        total_bytes_sent += packet_size;
    }

    // Calculate average latency and bandwidth
    double avg_latency = static_cast<double>(total_latency) / num_packets;                        ///< Average latency in microseconds
    double bandwidth = (static_cast<double>(total_bytes_sent) * 8) / (total_latency / 1000000.0); // Bandwidth in bits per second

    // Log metrics summary (only avg latency and avg bandwidth)
    log_file << std::fixed << bandwidth / 8 << "\n"; // Bandwidth in bytes/s
    log_file << std::fixed << avg_latency << "\n";   // Average latency in microseconds
    log_file.close();
    close(sock);
}

/**
 * @brief Main function to initialize the client and run the packet transmission.
 *
 * This function parses command line arguments to get the client ID and the number of packets to send.
 * It then creates the necessary log directory and calls the `run_client` function to start the client process.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * @return int The exit status of the program.
 */
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <client_id> <num_packets>" << std::endl;
        return 1;
    }

    int client_id = std::stoi(argv[1]);   ///< Client ID for logging purposes
    int num_packets = std::stoi(argv[2]); ///< Number of packets to send

    // Create log directory if it doesn't exist
    system("mkdir -p ./log");

    run_client(client_id, num_packets); ///< Run the client with specified parameters

    return 0;
}
