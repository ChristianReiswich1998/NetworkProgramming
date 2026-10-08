/***********************************************************************************************************************
* @author Christian Reiswich
 * @date 07.10.2026
 * brief bootstrap_node.
 * ********************************************************************************************************************/

#pragma once


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

#define CON_CLOSED 0
#define CON_ERROR  -1
#define MAX_EVENTS 64
#define IP_PORT_INPUT_SIZE 1024
#define IP_SIZE   512
#define PORT_SIZE 512
#define MAX_PORT 65535
#define MIN_PORT 0


typedef struct {
        unsigned long server_fd;
        int client_fd;
        int epoll_fd; // OS: Linux
        int kqueue_fd; // OS: MacOs, BSD
        int sock;
        int is_ready;
        struct sockaddr_in addr;
        pthread_mutex_t lock;
        pthread_cond_t  ok_to_send; // Condition: Client ok to send
        const char* server_ip;
        char* server_port;
        char ip_to[512];
        char port_to[512];
} Bootstrap_node;


Bootstrap_node* init_node(void);
void delete_node(Bootstrap_node* node);
void handle_errors(void);
char* check_valid_ip(char input[IP_PORT_INPUT_SIZE]);
char* check_valid_port(char input[IP_PORT_INPUT_SIZE]);
void run_event_loop(Bootstrap_node* node);
void* run_server(void *arg);
void run_client(Bootstrap_node* node);
void handle_client_data(Bootstrap_node* node);






