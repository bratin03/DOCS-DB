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

// Define DEBUG mode
// #define DEBUG // Uncomment this line to enable debugging

#define MAX_EVENTS 4096
#define BUFFER_SIZE 2048

struct epoll_event ev;
struct epoll_event events[MAX_EVENTS];

int epfd;
int sockfd;

// Helper functions to build RESP-2 messages
char *build_resp(const char *message, size_t *len_send)
{
    size_t len = strlen(message) + 3;
    char *response = (char *)malloc(len);
    snprintf(response, len, "+%s\r\n", message);
    *len_send = len;
    return response;
}

char *build_resp_get(const char *message, size_t *len_send)
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

struct resp
{
    char *message;
    size_t len;
};

// Modular function to handle commands
void handle_command(char *command, char **args, int arg_count,struct resp *response_struct)
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
    response_struct->message = response;
    response_struct->len = *response_len;
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

                    // Parse the received message
                    char *command;
                    char **args;
                    int arg_count;
                    parse_resp(buf, &command, &args, &arg_count);

                    // Handle the command
                    struct resp response_struct;
                    handle_command(command, args, arg_count, &response_struct);

                    // Send the response
                    ff_write(events[i].data.fd, response_struct.message, response_struct.len);

                    // Free the allocated memory
                    free(response_struct.message);
                    for (int i = 0; i < arg_count; i++)
                    {
                        free(args[i]);
                    }
                    free(args);

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
    my_addr.sin_port = htons(6379); // Using port 8080 instead of 80 for easier testing
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
