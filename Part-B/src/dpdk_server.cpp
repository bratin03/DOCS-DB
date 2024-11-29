/*
Student Information:
1. Bratin Mondal - 21CS10016
2. Swarnabh Mandal - 21CS10068
3. Somya Kumar - 21CS30050

Deparment of Computer Science and Engineering
Indian Institute of Technology, Kharagpur
*/

#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>

#include "ff_config.h"
#include "ff_api.h"
#include "ff_epoll.h"

#define MAX_EVENTS 4096
#define BUFFER_SIZE 2048

// #define DEBUG

struct epoll_event ev;
struct epoll_event events[MAX_EVENTS];

int epfd;
int sockfd;

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
                    /* Send the length of the received data back to the client */
                    char length_str[32]; // To hold the length as a string
                    int length = snprintf(length_str, sizeof(length_str), "%zd", readlen);

                    // Send the length of the received packet to the client
                    ff_write(events[i].data.fd, length_str, length);

#ifdef DEBUG
                    printf("Data received and sent back, fd: %d, length: %s\n", events[i].data.fd, length_str);
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
    my_addr.sin_port = htons(8080); // Using port 8080 instead of 80 for easier testing
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
