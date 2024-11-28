/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/resource.h>

// #define DEBUG

#define PORT 8080           // Port number to listen on
#define BUFFER_SIZE 2048    // Maximum buffer size for receiving data
#define MAX_PACKET_COUNT 10 // Maximum packets a client can send

// Define the SERVER IP address here
#define SERVER_IP "127.0.0.1" // Replace with your desired IP address

/**
 * @brief Handles a single client connection, receives multiple packets, and sends back the length of each packet.
 *
 * This function is responsible for receiving data from the client, calculating the size of each packet,
 * and sending the size back to the client. It runs in its own thread for each client connection.
 *
 * @param clientSocket The socket descriptor for the client.
 */
void handleClient(int clientSocket)
{
    char buffer[BUFFER_SIZE]; ///< Buffer to store the received data.
    int bytesReceived;        ///< Variable to store the number of bytes received from the client.

    // Receive multiple packets from the client and respond with the size of each packet
    while (1)
    {
        // Receive data from the client
        auto ret = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (ret <= 0)
        {
            // If no data is received or the connection is closed, exit the loop
            break;
        }

        // Convert the length of the received data to network byte order
        uint32_t packetSize = htonl(ret);

        // Send the packet size back to the client
        auto send_ret = send(clientSocket, &packetSize, sizeof(packetSize), 0);
        if (send_ret <= 0)
        {
            // If the send operation fails, exit the loop
            break;
        }
    }

    // Close the client connection
    close(clientSocket);
}

/**
 * @brief Main function to start the server, listen for incoming client connections, and handle each client in a new thread.
 *
 * This function is responsible for creating the server socket, binding it to a specific IP address and port,
 * and listening for incoming client connections. For each client connection, a new thread is spawned to handle the communication.
 * The server continues to run until it receives a termination signal (SIGINT).
 */
int main()
{
    int serverFd, clientSocket;                   ///< Server and client socket file descriptors.
    struct sockaddr_in serverAddr, clientAddr;    ///< Structs to hold server and client addresses.
    socklen_t clientAddrLen = sizeof(clientAddr); ///< Length of the client address.
    int opt = 1;                                  ///< Option to enable socket address reuse.

    // Handle SIGINT (Ctrl+C) gracefully to stop the server
    signal(SIGINT, [](int)
           {
               std::cout << "\nShutting down the server...\n";
               exit(0); // Exit the server gracefully
           });

    // Create the socket
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        perror("Setsockopt failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Set up the server address with a specific IP address (SERVER_IP)
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP); // Use SERVER_IP here
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(serverFd, 3) < 0)
    {
        perror("Listen failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on IP " << SERVER_IP << " and port " << PORT << std::endl;

    // Store the client threads
    std::vector<std::thread> clientThreads;

    // Accept client connections in a loop
    while (true)
    {
        // Accept a new client connection
        if ((clientSocket = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0)
        {
            perror("Accept failed");
            continue;
        }

#ifdef DEBUG
        std::cout << "New client connected" << std::endl;
#endif
        // Spawn a new thread to handle the client
        std::thread clientThread(handleClient, clientSocket);
        clientThreads.push_back(std::move(clientThread));
    }

    // Join all client threads (although unreachable due to the infinite loop)
    for (auto &thread : clientThreads)
    {
        thread.join();
    }

    // Close the server socket (this part is never reached due to the infinite loop)
    close(serverFd);
    return 0;
}
