/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

/**
 * @file dpdk-server.c
 * @brief Server program that accepts multiple client connections and handles Redis protocol commands.
 */

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
#include <sys/ioctl.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "ff_config.h"
#include "ff_api.h"
#include "ff_epoll.h"
#include "lsm_tree_wrapper_c.h"

void *tree;

// #define DEBUG // Uncomment this line to enable debugging

#define MAX_EVENTS 8192
#define BUFFER_SIZE 4096

struct epoll_event ev;
struct epoll_event events[MAX_EVENTS];

int epfd;
int sockfd;

/**
 * @brief Builds a simple response message with a prefix.
 *
 * @param message The message to include in the response.
 * @param len_send The length of the response message.
 *
 * @return The response message or NULL on failure.
 */
char *build_resp(const char *message, size_t *len_send)
{
    // Length calculation: 1 ('+') + strlen(message) + 2 ("\r\n") + 1 (null terminator)
    size_t len = 1 + strlen(message) + 2 + 1;

    // Allocate memory for the response.
    char *response = (char *)malloc(len);
    if (!response)
    {
        *len_send = 0;
        return NULL;
    }

    // Format the response.
    snprintf(response, len, "+%s\r\n", message);

    // Set the response length (excluding the null terminator for consistency).
    *len_send = len - 1;

    return response;
}

/**
 * @brief Builds a response message for the GET command.
 *
 * @param message The message to include in the response.
 * @param len_send The length of the response message.
 *
 * @return The response message or NULL on failure.
 */
char *build_resp_get(const char *message, size_t *len_send)
{
    // Determine the length of the message.
    size_t message_len = strlen(message);

    // Calculate the number of characters needed for the length in "$<length>".
    char len_buf[20];
    int prefix_len = snprintf(len_buf, sizeof(len_buf), "%zu", message_len);

    // Length calculation: 1 ('$') + prefix_len + 2 ("\r\n") + message_len + 2 ("\r\n") + 1 (null terminator)
    size_t len = 1 + prefix_len + 2 + message_len + 2 + 1;

    // Allocate memory for the response.
    char *response = (char *)malloc(len);
    if (!response)
    {
        *len_send = 0;
        return NULL;
    }

    // Format the response.
    snprintf(response, len, "$%zu\r\n%s\r\n", message_len, message);

    // Set the response length (excluding the null terminator for consistency).
    *len_send = len - 1;

    return response;
}

/**
 * @brief Builds an error response message.
 *
 * @param message The error message to include in the response.
 * @param len_send The length of the response message.
 *
 * @return The response message or NULL on failure.
 */
char *build_error(const char *message, size_t *len_send)
{
    // Length calculation: 5 ("-ERR ") + strlen(message) + 2 ("\r\n") + 1 (null terminator)
    size_t len = 5 + strlen(message) + 2 + 1;

    // Allocate memory for the response.
    char *response = (char *)malloc(len);
    if (!response)
    {
        *len_send = 0;
        return NULL;
    }

    // Format the response.
    snprintf(response, len, "-ERR %s\r\n", message);

    // Set the response length (excluding the null terminator for consistency).
    *len_send = len - 1;

    return response;
}

/**
 * @brief Parses the incoming messages from the client, handling multiple messages in the input.
 *
 * @param message The incoming message(s) from the client.
 * @param commands An array to hold the parsed commands.
 * @param args An array to hold the arguments for each command.
 * @param arg_counts An array to hold the argument count for each command.
 * @param message_count A pointer to an integer to store the number of parsed messages.
 */
void parse_resp(char *message, char ***commands, char ****args, int **arg_counts, int *message_count)
{
    char *token = strtok(message, "\r\n");
    int msg_idx = 0;

    // Allocate memory for commands, args, and arg_counts
    *commands = (char **)malloc(sizeof(char *) * 10); // Assume up to 10 messages initially
    *args = (char ***)malloc(sizeof(char **) * 10);
    *arg_counts = (int *)malloc(sizeof(int) * 10);
    *message_count = 0;

    while (token != NULL && *token != '\0')
    {
        int arg_count = 0;
        char **parsed_args = NULL;
#ifdef DEBUG
        printf("DEBUG: Token: %s\n", token);
#endif

        // Parse one RESP message
        if (token[0] == '*') // Start of a RESP message
        {
            arg_count = atoi(token + 1); // Get the number of arguments
            parsed_args = (char **)malloc(sizeof(char *) * arg_count);
#ifdef DEBUG
            printf("DEBUG: Arg count: %d\n", arg_count);
#endif

            // Advance to the first argument
            token = strtok(NULL, "\r\n");
            for (int i = 0; i < arg_count; i++)
            {
                if (token == NULL || token[0] != '$')
                {
                    // Invalid format
                    free(parsed_args);
                    return;
                }

                // Get the length of the argument
                size_t length = atoi(token + 1);

#ifdef DEBUG
                printf("DEBUG: Arg length: %zu\n", length);
#endif

                // Advance to the argument content
                token = strtok(NULL, "\r\n");
                if (token == NULL)
                {
                    free(parsed_args);
                    return;
                }

                // Store the argument
                parsed_args[i] = strndup(token, length);
#ifdef DEBUG
                printf("DEBUG: Arg %d: %s\n", i, parsed_args[i]);
#endif

                // Advance to the next length line
                token = strtok(NULL, "\r\n");
            }

            // The first argument is the command
            (*commands)[msg_idx] = parsed_args[0];
            (*args)[msg_idx] = parsed_args;
            (*arg_counts)[msg_idx] = arg_count;

            // Increment the message index
            msg_idx++;
            *message_count = msg_idx;

            // Resize arrays if needed
            if (msg_idx >= 10)
            {
                *commands = (char **)realloc(*commands, sizeof(char *) * (msg_idx + 10));
                *args = (char ***)realloc(*args, sizeof(char **) * (msg_idx + 10));
                *arg_counts = (int *)realloc(*arg_counts, sizeof(int) * (msg_idx + 10));
            }
        }
        else
        {
            // Invalid format, skip to the next
            token = strtok(NULL, "\r\n");
        }
    }
}

/**
 * @brief Handles the SET command to insert or update a key-value pair in the LSM tree.
 *
 * @param key The key to insert or update.
 * @param value The value associated with the key.
 */
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

/**
 * @brief Handles the GET command to retrieve the value associated with a key from the LSM tree.
 *
 * @param key The key to search for.
 * @return The value associated with the key. Returns `NULL` if the key is not found.
 */
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

/**
 * @brief Handles the DEL command to remove a key-value pair from the LSM tree.
 *
 * @param key The key to be removed.
 */
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

/**
 * @brief Structure to hold the response message and its length.
 *
 * This structure is used to pass the response message and its length between functions.
 */
struct resp
{
    char *message;
    size_t len;
};

/**
 * @brief Handles the incoming command from the client and sends the appropriate response.
 *
 * @param command The command extracted from the message.
 * @param args The arguments extracted from the message.
 * @param arg_count The number of arguments in the message.
 * @param response_struct The response structure to hold the response message and its length.
 */
void handle_command(char *command, char **args, int arg_count, struct resp *response_struct)
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
    else if (strcmp(command, "CONFIG") == 0)
    {
        response = build_error("CONFIG not supported", response_len);
    }
    else
    {
        response = build_error("Invalid command or arguments", response_len);
    }

    // Send response
#ifdef DEBUG
    printf("DEBUG: Sending response: %s\n", response);
#endif
    response_struct->message = response;
    response_struct->len = *response_len;
}

/**
 * @brief Signal handler to gracefully shut down the server.
 *
 * @param signal The signal received.
 */
void handle_signal(int signal)
{
    printf("Received signal %d, shutting down server...\n", signal);
    if (tree != NULL)
    {
        lsm_tree_destroy(tree);
    }
    exit(EXIT_SUCCESS);
}

/**
 * @brief Event loop function to handle incoming connections and data.
 *
 * This function waits for events such as new connections or incoming data. It handles each event accordingly:
 * - Accepts new client connections.
 * - Reads data from connected clients and sends the length of the received data back.
 * - Closes connections in case of errors or if no data is received.
 *
 * @param arg Pointer to additional arguments (unused).
 * @return int Status code (0 on success).
 */
int loop(void *arg)
{
    /* Wait for events to happen */
    int nevents = ff_epoll_wait(epfd, events, MAX_EVENTS, 0);
    int i;

    for (i = 0; i < nevents; ++i)
    {
        /* Handle new connections */
        if (events[i].data.fd == sockfd)
        {
            while (1)
            {
                int nclientfd = ff_accept(sockfd, NULL, NULL);
                if (nclientfd < 0)
                {
                    break;
                }

                /* Add to event list */
                ev.data.fd = nclientfd;
                ev.events = EPOLLIN;
                if (ff_epoll_ctl(epfd, EPOLL_CTL_ADD, nclientfd, &ev) != 0)
                {
                    break;
                }

#ifdef DEBUG
                printf("Connection accepted, new client fd: %d\n", nclientfd);
#endif
            }
        }
        else
        {
            if (events[i].events & EPOLLERR)
            {
                /* Close socket on error */
                ff_epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                ff_close(events[i].data.fd);

#ifdef DEBUG
                printf("Connection closed due to error, fd: %d\n", events[i].data.fd);
#endif
            }
            else if (events[i].events & EPOLLIN)
            {
                char buf[BUFFER_SIZE];
                ssize_t readlen = ff_read(events[i].data.fd, buf, sizeof(buf));

                if (readlen > 0)
                {
                    // Null-terminate the buffer to safely process it as a string
                    buf[readlen] = '\0';

                    // Parse multiple RESP messages
                    char **commands;
                    char ***args;
                    int *arg_counts;
                    int message_count;

                    parse_resp(buf, &commands, &args, &arg_counts, &message_count);

#ifdef DEBUG
                    printf("Parsed messages: %d\n", message_count);
#endif

                    // Handle each parsed message
                    for (int msg_idx = 0; msg_idx < message_count; msg_idx++)
                    {
                        // Handle the command
                        struct resp response_struct;
                        handle_command(commands[msg_idx], args[msg_idx], arg_counts[msg_idx], &response_struct);

                        // Send the response
                        ff_write(events[i].data.fd, response_struct.message, response_struct.len);

                        // Free the response message
                        free(response_struct.message);

                        // Free the arguments for the current command
                        for (int arg_idx = 0; arg_idx < arg_counts[msg_idx]; arg_idx++)
                        {
                            free(args[msg_idx][arg_idx]);
                        }
                        free(args[msg_idx]);
                    }

                    // Free arrays allocated by parse_resp_multiple
                    free(commands);
                    free(args);
                    free(arg_counts);

#ifdef DEBUG
                    printf("Data received: %s\n", buf);
#endif
                }

                else
                {
                    /* Close connection if no data */
                    ff_epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    ff_close(events[i].data.fd);

#ifdef DEBUG
                    printf("Connection closed, no data received, fd: %d\n", events[i].data.fd);
#endif
                }
            }
        }
    }

    return 0;
}

/**
 * @brief Main function to initialize the server and run the event loop.
 *
 * This function initializes the server socket, binds it to the specified port, and starts listening for incoming connections.
 * It then creates an epoll instance, adds the server socket to it, and begins running the event loop to handle events.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int Status code (0 on success).
 */
int main(int argc, char *argv[])
{
    ff_init(argc, argv);
    tree = lsm_tree_create();

    // Create the server socket
    sockfd = ff_socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("ff_socket failed\n");
        exit(1);
    }

    // Set socket to non-blocking mode
    int on = 1;
    ff_ioctl(sockfd, FIONBIO, &on);

    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(6379);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the specified address and port
    int ret = ff_bind(sockfd, (struct linux_sockaddr *)&my_addr, sizeof(my_addr));
    if (ret < 0)
    {
        printf("ff_bind failed\n");
        exit(1);
    }

    // Start listening for incoming connections
    ret = ff_listen(sockfd, MAX_EVENTS);
    if (ret < 0)
    {
        printf("ff_listen failed\n");
        exit(1);
    }

    // Create the epoll instance
    assert((epfd = ff_epoll_create(0)) > 0);

    // Add the server socket to the epoll instance to monitor for incoming connections
    ev.data.fd = sockfd;
    ev.events = EPOLLIN;
    ff_epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    // Run the event loop
    ff_run(loop, NULL);

    return 0;
}
