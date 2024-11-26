#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <chrono>

using namespace std;

#define PORT 9000
#define MIN_SIZE 512
#define MAX_SIZE 1024

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: ./client <packet count>" << endl;
        return -1;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAX_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "10.0.0.4", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address or Address not supported");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        return -1;
    }

    int total_packets = atoi(argv[1]);
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < total_packets; ++i)
    {
        // Generate a random packet size between 0.5 KB and 1 KB
        int packet_size = MIN_SIZE + (rand() % (MAX_SIZE - MIN_SIZE + 1));
        memset(buffer, 'A', packet_size);

        // Send data to server
        if (send(sock, buffer, packet_size, 0) == -1)
        {
            perror("Send failed");
            break;
        }

        // Receive response from server
        int valread = read(sock, buffer, MAX_SIZE);
        if (valread <= 0)
        {
            perror("Receive failed");
            break;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Total time taken: " << duration.count() << " seconds\n";

    close(sock);
    return 0;
}
