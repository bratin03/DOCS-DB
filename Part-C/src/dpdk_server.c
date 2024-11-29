#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include "lsm_tree_wrapper_c.h"

#define MAX_BUFFER_SIZE 4096
#define PORT 6379
#define MAX_CLIENTS 1024

void *tree;

// Define DEBUG mode
// #define DEBUG // Uncomment this line to enable debugging

// Helper functions to build RESP-2 messages
char *build_resp(const char *message, size_t *len_send)
{
    size_t len = strlen(message) + 3;
    char *response = (char *)malloc(len);
    snprintf(response, len, "+%s\r\n", message);
    *len_send = len;
    return response;
}

char *build_resp_get(const char *message,size_t *len_send)
{
    size_t len = strlen(message) + 6;
    char *response = (char *)malloc(len);
    snprintf(response, len, "$%zu\r\n%s\r\n", strlen(message), message);
    *len_send = len;
    return response;
}

char *build_error(const char *message, size_t *len_send)
{
    size_t len = strlen(message) + 6;
    char *response = (char *)malloc(len);
    snprintf(response, len, "-ERR %s\r\n", message);
    *len_send = len;
    return response;
}

void parse_resp(char *message, char **command, char ***args, int *arg_count)
{
    char *token;
    *arg_count = 0;

    // Tokenize the first line to get the number of arguments (*2)
    token = strtok(message, "\r\n");
    if (token[0] != '*')
    {
        *command = NULL;
        return;
    }

    *arg_count = atoi(token + 1); // Extract the number of arguments
    *args = (char **)malloc(sizeof(char *) * (*arg_count));

    // Process each argument
    for (int i = 0; i < *arg_count; i++)
    {
        // Expecting the argument length (e.g., $3 for "GET")
        token = strtok(NULL, "\r\n");
        if (token[0] != '$')
        {
            *command = NULL;
            return;
        }

        size_t length = atoi(token + 1); // Get the length of the argument
        token = strtok(NULL, "\r\n");
        (*args)[i] = strndup(token, length); // Copy the argument
    }

    *command = (*args)[0]; // The first argument is the command (e.g., "GET")
}

// Command Handlers
void handle_set(char *key, char *value)
{
#ifdef DEBUG
    printf("DEBUG: Handle SET - key: %s, value: %s\n", key, value);
#endif
    if (tree == NULL)
    {
        tree = lsm_tree_create();
    }
    lsm_tree_put(tree, key, value);
}

const char *handle_get(char *key)
{
#ifdef DEBUG
    printf("DEBUG: Handle GET - key: %s\n", key);
#endif
    if (tree == NULL)
    {
        return NULL;
    }

    const char *get_response = lsm_tree_get(tree, key);
#ifdef DEBUG
    printf("DEBUG: GET response: %s\n", get_response);
#endif
    return get_response;
}

void handle_del(char *key)
{
#ifdef DEBUG
    printf("DEBUG: Handle DEL - key: %s\n", key);
#endif
    if (tree == NULL)
    {
        return;
    }
    lsm_tree_remove(tree, key);
}

// Modular function to handle commands
void handle_command(int client_fd, char *command, char **args, int arg_count)
{
    char *response = NULL;
    size_t *response_len = (size_t *)malloc(sizeof(size_t));

#ifdef DEBUG
    printf("DEBUG: Handling command: %s, args count: %d\n", command, arg_count);
    for (int i = 0; i < arg_count; i++)
    {
        printf("DEBUG: Arg %d: %s\n", i, args[i]);
    }
#endif

    if (strcmp(command, "SET") == 0 && arg_count == 3)
    {
        handle_set(args[1], args[2]);
        response = build_resp("OK", response_len);
    }
    else if (strcmp(command, "GET") == 0 && arg_count == 2)
    {
        const char *response_value = handle_get(args[1]);
        if (response_value != NULL)
        {
#ifdef DEBUG
            printf("DEBUG: GET response: %s\n", response_value);
#endif
            response = build_resp_get(response_value, response_len);
#ifdef DEBUG
            printf("DEBUG: GET response: %s\n", response);
#endif
        }
        else
        {
            response = build_error("Key not found", response_len);
        }
    }
    else if (strcmp(command, "DEL") == 0 && arg_count == 2)
    {
        handle_del(args[1]);
        response = build_resp("OK", response_len);
    }
    else if (strcmp(command, "PING") == 0)
    {
        response = build_resp("PONG", response_len);
    }
    else
    {
        response = build_error("Invalid command or arguments", response_len);
    }

    // Send response
#ifdef DEBUG
    printf("DEBUG: Sending response: %s\n", response);
#endif
    write(client_fd, response, *response_len);
    free(response);
    free(response_len);
}

// Signal handler for graceful shutdown
void handle_signal(int signal)
{
    printf("Received signal %d, shutting down server...\n", signal);
    if (tree != NULL)
    {
        lsm_tree_destroy(tree);
    }
    exit(EXIT_SUCCESS);
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

    tree = lsm_tree_create();
    // Register signal handlers
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // SO_REUSEADDR option
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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
                    size_t *len_send = (size_t *)malloc(sizeof(size_t));
                    char *response = build_error("Invalid RESP format", len_send);
                    write(fds[i].fd, response, *len_send);
                    free(response);
                    free(len_send);
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
