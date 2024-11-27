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
 * @brief Parses the server response based on RESP format and displays the result.
 *
 * @param command The command that was sent (e.g., "GET", "SET", "DEL").
 * @param key The key used in the command.
 * @param response The server response as a string.
 */
void parseServerResponse(const string &command, const string &key, const string &response)
{
    
    cout << "\nCommand: " << command << " " << key << endl;

    // Handle SET and DEL commands (they won't return a value in response)
    if (command == "SET" || command == "DEL")
    {
        cout << "Response:" << endl;
    }
    else if (command == "GET")  // GET command expects a response
    {
        cout << "Response: ";

        if (response.empty() || response[0] == '$' && response.substr(1, 2) == "0")  // No value found for GET
        {
            cout << endl;  // Empty response
        }
        else if (response[0] == '$')  // Bulk String Response
        {
            size_t lenEnd = response.find("\r\n", 1);  // Find the end of the length part
            int length = stoi(response.substr(1, lenEnd - 1));  // Get the length of the bulk string
            if (length > 0)
            {
                string value = response.substr(lenEnd + 2, length);  // Extract the value part
                cout << value << endl;  // Print the value
            }
            else
            {
                cout << endl;  // No value found for GET, print empty
            }
        }
        else
        {
            cout << "Unexpected response format" << endl;
        }
    }
    else
    {
        cout << "Unexpected command" << endl;
    }
}

/**
 * @brief Main function to establish a connection to the server, send commands, and process responses.
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

    cout << "Connection Established" << endl;

    // Commands to be sent
    string command;

    // Send SET command
    command = buildRESPCommand("SET", "mykey", "myvalue");
    send(sock, command.c_str(), command.size(), 0);
    int bytesRead = read(sock, buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
        parseServerResponse("SET", "mykey", string(buffer, bytesRead));
    }

    // Send GET command
    command = buildRESPCommand("GET", "mykey");
    send(sock, command.c_str(), command.size(), 0);
    bytesRead = read(sock, buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
        parseServerResponse("GET", "mykey", string(buffer, bytesRead));
    }

    // Send DEL command
    command = buildRESPCommand("DEL", "mykey");
    send(sock, command.c_str(), command.size(), 0);
    bytesRead = read(sock, buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
        parseServerResponse("DEL", "mykey", string(buffer, bytesRead));
    }

    // Send GET command again (expecting empty response)
    command = buildRESPCommand("GET", "mykey");
    send(sock, command.c_str(), command.size(), 0);
    bytesRead = read(sock, buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
        parseServerResponse("GET", "mykey", string(buffer, bytesRead));
    }

    // Close the connection
    close(sock);
    return 0;
}
