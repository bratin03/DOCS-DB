#include <iostream>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <vector>
#include "../Part-A/src/lsm_tree/lsm_tree.h"

using namespace std;

#define PORT 6379 ///< Port number for the server

lsm_tree db; ///< Instance of the StorageEngine


/**
 * @brief Parses the RESP protocol input.
 * 
 * @param input The RESP-formatted command string.
 * @return vector<string> Parsed tokens of the RESP command.
 */
vector<string> parseRESP(const string& input) {
    istringstream stream(input);
    string line;
    vector<string> tokens;

    while (getline(stream, line, '\r')) {
        if (!line.empty()) {
            tokens.push_back(line);
        }
        stream.ignore(1); // ignore the '\n'
    }
    return tokens;
}



/**
 * @brief Processes a parsed RESP command and executes it on the StorageEngine.
 *
 * @param tokens Parsed tokens from the RESP command.
 * @return string The response in RESP format.
 */
string processCommand(const vector<string> &tokens)
{
    if (tokens.size() < 3)
        return "-ERR invalid command\r\n";

    const string &command = tokens[2];
    if (command == "SET" && tokens.size() >= 5)
    {
        cout << "SET " << tokens[4] << " " << tokens[6] << endl;
        db.put(tokens[4], tokens[6]);
        return "+OK\r\n";
    }
    else if (command == "GET" && tokens.size() >= 4)
    {
        db.get(tokens[4]);
        return "+OK\r\n";
    }
    else if (command == "DEL" && tokens.size() >= 4)
    {
        db.remove(tokens[4]);
        return "+OK\r\n";
    }
    return "-ERR unknown command\r\n";
}

/**
 * @brief Handles the client connection, reads input, processes commands, and sends responses.
 *
 * @param clientSocket Socket file descriptor for the client connection.
 */
void handleClient(int clientSocket)
{
    char buffer[1024] = {0};
    int bytesReceived;
    while ((bytesReceived = read(clientSocket, buffer, 1024)) > 0)
    {
        string input(buffer, bytesReceived);
        auto tokens = parseRESP(input);
        string response = processCommand(tokens);
        send(clientSocket, response.c_str(), response.size(), 0);
        memset(buffer, 0, sizeof(buffer));
    }
    if (bytesReceived < 0)
    {
        perror("Error reading from client socket");
    }
    close(clientSocket);
}

/**
 * @brief Main function to start the server, listen for connections, and spawn threads for each client.
 *
 * @return int Exit status of the program.
 */
int main()
{
    int serverFd, clientSocket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        perror("Setsockopt failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Bind socket to address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(serverFd, 3) < 0)
    {
        perror("Listen failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    cout << "Server started on port " << PORT << endl;

    // Accept connections in a loop and handle each in a new thread
    while (true)
    {
        if ((clientSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            continue;
        }
        std::cout <<"Connection Established" << std::endl;
        thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    close(serverFd);
    return 0;
}
