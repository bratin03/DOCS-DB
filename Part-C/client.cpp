#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

using namespace std;

#define PORT 6379 ///< Port number of the server to connect to

/**
 * @brief Builds a RESP-formatted command string.
 *
 * @param command The command to execute (e.g., "SET", "GET", "DEL").
 * @param key The key associated with the command.
 * @param value Optional value for the command (only used with "SET").
 * @return string The RESP-formatted command string.
 */
string buildRESPCommand(const string &command, const string &key, const string &value = "")
{
    string respCommand = "*" + to_string(value.empty() ? 3 : 5) + "\r\n";
    respCommand += "$" + to_string(command.size()) + "\r\n" + command + "\r\n";
    respCommand += "$" + to_string(key.size()) + "\r\n" + key + "\r\n";
    if (!value.empty())
    {
        respCommand += "$" + to_string(value.size()) + "\r\n" + value + "\r\n";
    }
    return respCommand;
}

/**
 * @brief Main function to establish a connection to the server, send commands, and receive responses.
 *
 * @return int Exit status of the program.
 */
int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return -1;
    }

    std:: cout << "Connection Established" << std::endl;
    // Send SET command
    string command = buildRESPCommand("SET", "mykey", "myvalue");
    send(sock, command.c_str(), command.size(), 0);
    read(sock, buffer, 1024);
    cout << buffer << endl;

    // Send GET command
    command = buildRESPCommand("GET", "mykey");
    send(sock, command.c_str(), command.size(), 0);
    read(sock, buffer, 1024);
    cout << buffer << endl;

    // Send DEL command
    command = buildRESPCommand("DEL", "mykey");
    send(sock, command.c_str(), command.size(), 0);
    read(sock, buffer, 1024);
    cout << buffer << endl;

    // Send GET command again
    command = buildRESPCommand("GET", "mykey");
    send(sock, command.c_str(), command.size(), 0);
    read(sock, buffer, 1024);
    cout << buffer << endl;


    close(sock);
    return 0;
}
