/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief P2P - Node
 * ********************************************************************************************************************/

#pragma once

#include "includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/errno.h>
#include <resolv.h>
#include <fcntl.h>

#if defined(__linux__)
    #include <sys/epoll.h>
    #define PLATFORM "Linux (epoll)"
#elif defined(__APPLE__)
    #include <sys/types.h>
    #include <sys/event.h>
    #define PLATFORM "macOS (kqueue)"
#elif defined(_WIN32)
    #include <winsock2.h>
    #include <windows.h>
    #define PLATFORM "Windows (IOCP)"
#else
    #error "Not supported OS"
#endif


#define PORT       9000
#define LOCAL_HOST AF_LOCAL
#define IPv4       AF_INET
#define IPv6       AF_INET6
#define TCP        SOCK_STREAM
#define UDP        SOCK_DGRAM
#define MAX_EVENTS 64


typedef struct {
        int server_fd;
        int client_fd;
        int epoll_fd; // OS: Linux
        int kqueue_fd; // OS: MacOs, BSD
        int sock;
        int is_ready;
        struct sockaddr_in addr;
        pthread_mutex_t lock;
        pthread_cond_t  ok_to_send; // Condition: Client ok to send
} Node;



Node* init_node(void);
void delete_node(Node* node);
void* run_server(void *arg);
void run_client(Node* node, const char* server_ip);
void run_event_loop(Node* node);
void handle_client_data(Node* node);




