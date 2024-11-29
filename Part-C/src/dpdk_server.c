#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include "lsm_tree_wrapper_c.h"

#define MAX_BUFFER_SIZE 4096
#define PORT 6379
#define MAX_CLIENTS 1024

// Define DEBUG mode
// #define DEBUG  // Uncomment this line to enable debugging

// Placeholder functions for LSM tree operations
// These need to be replaced with your actual LSM tree API functions
void lsm_tree_put(const char *key, const char *value)
{
    // Implement the actual set logic with your LSM tree
}

const char *lsm_tree_get(const char *key)
{
    // Implement the actual get logic with your LSM tree
    return NULL; // Return NULL if key not found
}

void lsm_tree_remove(const char *key)
{
    // Implement the actual remove logic with your LSM tree
}

// Helper functions to build RESP-2 messages
char *build_resp(const char *message)
{
    size_t len = strlen(message) + 3;
    char *response = (char *)malloc(len);
    snprintf(response, len, "+%s\r\n", message);
    return response;
}

char *build_resp_get(const char *message)
{
    size_t len = strlen(message) + 6;
    char *response = (char *)malloc(len);
    snprintf(response, len, "$%zu\r\n%s\r\n", strlen(message), message);
    return response;
}

char *build_error(const char *message)
{
    size_t len = strlen(message) + 6;
    char *response = (char *)malloc(len);
    snprintf(response, len, "-ERR %s\r\n", message);
    return response;
}

void parse_resp(char *message, char **command, char ***args, int *arg_count)
{
    char *token;
    *arg_count = 0;

    token = strtok(message, "\r\n");
    if (token[0] != '*')
    {
        *command = NULL;
        return;
    }

    *arg_count = atoi(token + 1);
    *args = (char **)malloc(sizeof(char *) * (*arg_count));

    for (int i = 0; i < *arg_count; i++)
    {
        token = strtok(NULL, "\r\n");
        if (token[0] != '$')
        {
            *command = NULL;
            return;
        }

        size_t length = atoi(token + 1);
        token = strtok(NULL, "\r\n");
        (*args)[i] = strndup(token, length);
    }

    *command = (*args)[0];
}

// Command Handlers
void handle_set(char *key, char *value)
{
#ifdef DEBUG
    printf("DEBUG: Handle SET - key: %s, value: %s\n", key, value);
#endif
    lsm_tree_put(key, value);
}

const char *handle_get(char *key)
{
#ifdef DEBUG
    printf("DEBUG: Handle GET - key: %s\n", key);
#endif
    return lsm_tree_get(key);
}

void handle_del(char *key)
{
#ifdef DEBUG
    printf("DEBUG: Handle DEL - key: %s\n", key);
#endif
    lsm_tree_remove(key);
}

// Modular function to handle commands
void handle_command(int client_fd, char *command, char **args, int arg_count)
{
    char *response = NULL;

#ifdef DEBUG
    printf("DEBUG: Handling command: %s, args count: %d\n", command, arg_count);
#endif

    if (strcmp(command, "SET") == 0 && arg_count == 2)
    {
        handle_set(args[0], args[1]);
        response = build_resp("OK");
    }
    else if (strcmp(command, "GET") == 0 && arg_count == 1)
    {
        const char *response_value = handle_get(args[0]);
        if (response_value != NULL)
        {
            response = build_resp_get(response_value);
        }
        else
        {
            response = build_error("Key not found");
        }
    }
    else if (strcmp(command, "DEL") == 0 && arg_count == 1)
    {
        handle_del(args[0]);
        response = build_resp("OK");
    }
    else if (strcmp(command, "PING") == 0)
    {
        response = build_resp("PONG");
    }
    else
    {
        response = build_error("Invalid command or arguments");
    }

    // Send response
#ifdef DEBUG
    printf("DEBUG: Sending response: %s\n", response);
#endif
    write(client_fd, response, strlen(response));
    free(response);
}

// Main server function
int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[MAX_BUFFER_SIZE];
    char *command = NULL;
    char **args = NULL;
    int arg_count = 0;
    struct pollfd fds[MAX_CLIENTS];
    int nfds = 1;
    int timeout = -1; // Infinite timeout, waits indefinitely for events

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    printf("DEBUG: Server socket created\n");
#endif

    // Set the server socket to non-blocking mode
    fcntl(server_socket, F_SETFL, O_NONBLOCK);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Failed to bind socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    printf("DEBUG: Server socket bound to port %d\n", PORT);
#endif

    if (listen(server_socket, 5) < 0)
    {
        perror("Failed to listen on socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Add the server socket to the poll file descriptor array
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    while (1)
    {
        int ret = poll(fds, nfds, timeout);

        if (ret < 0)
        {
            perror("Poll error");
            break;
        }

        if (ret == 0)
        {
            // Timeout, no events
#ifdef DEBUG
            printf("DEBUG: Poll timeout, no events\n");
#endif
            continue;
        }

        // Check for events on the server socket (new connection)
        if (fds[0].revents & POLLIN)
        {
            client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
            if (client_socket < 0)
            {
                perror("Failed to accept connection");
                continue;
            }

#ifdef DEBUG
            printf("DEBUG: Accepted connection from %s\n", inet_ntoa(client_addr.sin_addr));
#endif

            // Set the client socket to non-blocking mode
            fcntl(client_socket, F_SETFL, O_NONBLOCK);

            // Add the new client socket to the poll file descriptor array
            fds[nfds].fd = client_socket;
            fds[nfds].events = POLLIN;
            nfds++;

            if (nfds >= MAX_CLIENTS)
            {
                fprintf(stderr, "Max clients reached\n");
                break;
            }
        }

        // Check for events on the client sockets (data to read)
        for (int i = 1; i < nfds; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                ssize_t nread = read(fds[i].fd, buffer, sizeof(buffer) - 1);
                if (nread <= 0)
                {
                    if (nread == 0)
                    {
                        // Connection closed by the client
#ifdef DEBUG
                        printf("DEBUG: Client disconnected\n");
#endif
                    }
                    else
                    {
                        perror("Read error");
                    }
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1]; // Remove the client socket from the array
                    nfds--;
                    continue;
                }

                buffer[nread] = '\0'; // Null terminate the string

#ifdef DEBUG
                printf("DEBUG: Received data: %s\n", buffer);
#endif

                parse_resp(buffer, &command, &args, &arg_count);

                if (command == NULL)
                {
                    char *response = build_error("Invalid RESP format");
                    write(fds[i].fd, response, strlen(response));
                    free(response);
                    continue;
                }

                // Call the function to handle the command
                handle_command(fds[i].fd, command, args, arg_count);

                // Clean up
                for (int j = 0; j < arg_count; j++)
                {
                    free(args[j]);
                }
                free(args);
            }
        }
    }

    close(server_socket);
    return 0;
}
