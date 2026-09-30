/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief Server
 * ********************************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/errno.h>

#define PORT       9000
#define LOCAL_HOST AF_LOCAL
#define IPv4       AF_INET
#define IPv6       AF_INET6
#define TCP        SOCK_STREAM
#define UDP        SOCK_DGRAM
#define BOOL       int
#define TRUE       1
#define FALSE      0

typedef struct {
        int server_fd;
        int client_fd;
        int sock;
        int is_ready;
        struct sockaddr_in addr;
        pthread_mutex_t lock;
        pthread_cond_t  ok_to_send; // Condition: Client ok to send
} Node;


static void handle_errors(const char* msg) {
        perror(msg);
        exit(EXIT_FAILURE);
}


static Node* init_node(void) {
        Node* node = malloc(sizeof(Node));
        if (node == NULL)
                handle_errors("ERROR");

        memset(node, 0, sizeof(Node));
        pthread_mutex_init(&node->lock, NULL);
        pthread_cond_init(&node->ok_to_send, NULL);

        return node;
}


static void delete_node(Node* node) {
        if (node == NULL) return;

        pthread_mutex_destroy(&node->lock);
        pthread_cond_destroy(&node->ok_to_send);
        free(node);
}


static void* run_server(void *arg) {
        Node* node = arg;

        if ((node->server_fd = socket(IPv4, TCP, 0)) < 0)
                handle_errors("ERROR");

        const int opt = 1;
        if (setsockopt(node->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                handle_errors("ERROR");
        }

        memset(&node->addr, 0, sizeof(node->addr));
        node->addr.sin_family      = IPv4;
        node->addr.sin_port        = htons(PORT);
        node->addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(node->server_fd, (struct sockaddr*)&node->addr, sizeof(node->addr)) < 0)
                handle_errors("ERROR");

        if (listen(node->server_fd, 5) < 0)
                handle_errors("ERROR");

        pthread_mutex_lock(&node->lock);
        node->is_ready = TRUE;
        pthread_cond_signal(&node->ok_to_send);
        pthread_mutex_unlock(&node->lock);

        printf("[Server] Node listening on port %d\n", PORT);

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        node->client_fd = accept(node->server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (node->client_fd < 0)
                handle_errors("ERROR");

        char buf[1024] = {0};
        const ssize_t recv_bytes = recv(node->client_fd, buf, sizeof(buf) - 1, 0);

        if (recv_bytes > 0) {
                buf[recv_bytes] = '\0';
                printf("Server Node: %s\n", buf);
        }

        else if (recv_bytes == 0) {
                printf("Connection is closed!\n");
                close(node->client_fd);
                close(node->server_fd);
                return NULL;
        }

        else if (recv_bytes < 0)
                handle_errors("ERROR");

        const char* replay = "Hello back from Server Node!\n";
        send(node->client_fd, replay, strlen(replay), 0);

        close(node->client_fd);
        close(node->server_fd);
        return NULL;
}

static void run_client(Node* node, const char* server_ip) {
        pthread_mutex_lock(&node->lock);
        while (node->is_ready == FALSE) {
                pthread_cond_wait(&node->ok_to_send, &node->lock);
        }
        pthread_mutex_unlock(&node->lock);

        if ((node->sock = socket(IPv4, TCP, 0)) < 0)
                handle_errors("ERROR");

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = IPv4;
        server_addr.sin_port   = htons(PORT);

        if (inet_pton(IPv4, server_ip, &server_addr.sin_addr) <= 0)
                handle_errors("ERROR");

        if (connect(node->sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
                handle_errors("ERROR");

        const char* msg = "Hello from client Node!\n";
        ssize_t send_bytes = send(node->sock, msg, strlen(msg), 0);
        ssize_t total_send_bytes = send_bytes;


        while (total_send_bytes < strlen(msg)) {
                if (send_bytes == - 1) {
                        handle_errors("ERROR");
                        break;
                }

                if (send_bytes == 0) {
                        handle_errors("ERROR");
                        break;
                }

                if (send_bytes > 0) {
                        const int remaining_bytes = strlen(msg) - total_send_bytes;
                        send_bytes = send(node->sock, msg + total_send_bytes, remaining_bytes, 0);
                        total_send_bytes += send_bytes;
                }
        }

        char buf[1024] = {0};
        const ssize_t recv_bytes = recv(node->sock, buf, sizeof(buf) - 1, 0);
        if (recv_bytes > 0) {
                buf[recv_bytes] = '\0';
                printf("Server replied: %s\n", buf);
        }

        else if (recv_bytes == 0) {
                printf("Connection is closed!\n");
                close(node->sock);
                return;
        }

        else if (recv_bytes < 0)
                handle_errors("ERROR: Network Errors\n");

        close(node->sock);
}



int main(void/*int argc, char* argv[]*/) {
        printf("Starting P2P Node...\n");

        Node* node = init_node();
        pthread_t server_thread_id;

        if (pthread_create(&server_thread_id, NULL, run_server, node) != 0)
                handle_errors("ERROR: Thread creation failed!\n");

        run_client(node, "127.0.0.1");
        pthread_join(server_thread_id, NULL);

        delete_node(node);

        return 0;
}


