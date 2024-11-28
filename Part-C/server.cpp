#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <functional>
#include "../Part-A/src/lsm_tree/lsm_tree.h"

using namespace std;

#define PORT 6379            ///< Port number for the server

lsm_tree db; ///< Instance of the StorageEngine

/// Shared task queue and synchronization primitives
queue<int> taskQueue;
mutex queueMutex;
condition_variable queueCondition;
map<string, string> redisServerConfig;

/**
 * @brief Parses the RESP protocol input.
 *
 * @param input The RESP-formatted command string.
 * @return vector<string> Parsed tokens of the RESP command.
 */
vector<string> parseRESP(const string &input)
{
    istringstream stream(input);
    string line;
    vector<string> tokens;

    while (getline(stream, line, '\r'))
    {
        if (!line.empty())
        {
            tokens.push_back(line);
        }
        stream.ignore(1); // Ignore the '\n'
    }

    return tokens;
}

/**
 * @brief Parses the RESP protocol input and returns the commands.
 *
 * @param tokens The parsed tokens from the RESP command.
 * @return vector<vector<string>> The commands in the RESP format.
 */

vector<vector<string>> getRESPCommands(const vector<string> &tokens)
{
    vector<vector<string>> commands;
    int cur = 0;
    while (cur < tokens.size())
    {
        if (tokens[cur][0] == '*')
        {
            commands.push_back(vector<string>());
            int num = stoi(tokens[cur].substr(1));
            cur++;
            for (int i = 0; i < num; i++)
            {
                int len = stoi(tokens[cur].substr(1));
                cur++;
                (commands.back()).push_back(tokens[cur]);
                cur++;
            }
        }
    }
    return commands;
}

string processCommand(const vector<string> &command)
{
    if (command.empty() || command[0].empty())
    {
        return "-ERR invalid command format\r\n";
    }
    const string &cmd = command[0];
    if (cmd == "PING")
    {
        if (command.size() > 1)
        {
            return "+" + command[1] + "\r\n";
        }
        return "+PONG\r\n";
    }
    if (cmd == "SET" && command.size() >= 3)
    {
        db.put(command[1], command[2]);
        return "+OK\r\n";
    }
    if (cmd == "GET" && command.size() >= 2)
    {
        string value = db.get(command[1]);
        return value.empty() ? "$-1\r\n" : "$" + to_string(value.size()) + "\r\n" + value + "\r\n";
    }
    if (cmd == "DEL" && command.size() >= 2)
    {
        db.remove(command[1]);
        return ":1\r\n";
    }

    // if(cmd == "COMMAND"){
    //     return "*3\r\n$4\r\nPING\r\n$3\r\nGET\r\n$3\r\nSET\r\n";
    // }
    // if(cmd == "CONFIG"){
    //     if (command[1] == "GET" && command.size() >= 3) {
    //         return "$" + to_string(redisServerConfig[command[2]].size()) + "\r\n" + redisServerConfig[command[2]] + "\r\n";
    //     }
    // }
    return "-ERR unknown command\r\n";
}

string createRESPResponse(const vector<string> &responses)
{
    string response = "";
    int size = responses.size();
    response += "*" + to_string(size) + "\r\n";
    for (auto r : responses)
    {
        response += r;
    }

    return response;
}

/**
 * @brief Handles the client connection, reads input, processes commands, and sends responses.
 *
 * @param clientSocket Socket file descriptor for the client connection.
 */
void handleClient(int clientSocket)
{
    while (true)
    {
        char buffer[1024] = {0};
        int bytesReceived;
        while ((bytesReceived = read(clientSocket, buffer, sizeof(buffer))) > 0)
        {
            string input(buffer, bytesReceived);

            // Parse and process the RESP input
            auto tokens = parseRESP(input);
            for (auto t : tokens)
                cout << t << " ";

            vector<vector<string>> commands = getRESPCommands(tokens);

            cout << "Commands: " << commands.size() << endl;

            for (auto c : commands)
            {
                for (auto command : c)
                {
                    cout << command << " ";
                }
                cout << endl;
            }

            vector<string> responses;

            for (auto command : commands)
            {
                string response = processCommand(command);
                responses.push_back(response);
            }
            string response = createRESPResponse(responses);
            cout << "Response: " << response << endl;
            // Send the response back to the client
            send(clientSocket, response.c_str(), response.size(), 0);
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer for the next read
        }

        if (bytesReceived < 0)
        {
            perror("Error reading from client socket");
        }
    }
    close(clientSocket);
    exit(0);
}

/**
 * @brief Main function to start the server, initialize the thread pool, and listen for connections.
 *
 * @return int Exit status of the program.
 */
int main()
{
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in cli_addr, serv_addr;

    int i;
    char buf[100];
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Cannot create socket\n");
        exit(0);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(6379);

    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        printf("Unable to bind local address\n");
        exit(0);
    }

    listen(sockfd, 5);
    while (1)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (newsockfd < 0)
        {
            printf("Accept error\n");
            exit(0);
        }
        
        cout << "Connection accepted from " << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port) << endl;

        if (fork() == 0)
        {

            close(sockfd);
            handleClient(newsockfd);
            exit(0);
        }

        close(newsockfd);
    }
    return 0;
}
