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

#define PORT       9000
#define LOCAL_HOST AF_LOCAL
#define IPV4       AF_INET
#define IPV6       AF_INET6
#define TCP        SOCK_STREAM
#define UDP        SOCK_DGRAM


typedef struct {
        int server_fd;
        int client_fd;
        int sock;
        struct sockaddr_in addr;
} Node;


static void handle_errors(const char* msg) {
        perror(msg);
        exit(EXIT_FAILURE);
}


static void* run_server(void *arg) {
        Node* node = arg;

        if ((node->server_fd = socket(IPV4, TCP, 0)) < 0)
                handle_errors("ERROR: Socket failed!\n");

        const int opt = 1;
        if (setsockopt(node->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                handle_errors("ERROR: set socket!\n");
        }

        memset(&node->addr, 0, sizeof(node->addr));
        node->addr.sin_family      = IPV4;
        node->addr.sin_port        = htons(PORT);
        node->addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(node->server_fd, (struct sockaddr*)&node->addr, sizeof(node->addr)) < 0)
                handle_errors("ERROR: bind!\n");

        if (listen(node->server_fd, 5) < 0)
                handle_errors("ERROR: listen!\n");

        printf("[Server] Node listening on port %d", PORT);

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        node->client_fd = accept(node->server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (node->client_fd < 0)
                handle_errors("ERROR: accept");

        char buf[1024] = {0};
        const ssize_t recv_bytes = recv(node->client_fd, buf, sizeof(buf) - 1, 0);
        if (recv_bytes > 0) {
                buf[recv_bytes] = '\0';
                printf("Server Node: %s\n", buf);
        }

        const char* replay = "Hello back from Server Node!\n";
        send(node->client_fd, replay, strlen(replay), 0);
        close(node->client_fd);
        close(node->server_fd);
        return NULL;
}

static void run_client(Node* node, const char* server_ip) {
        if ((node->sock = socket(IPV4, TCP, 0)) < 0)
                handle_errors("ERROR: create client");

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = IPV4;
        server_addr.sin_port   = htons(PORT);

        if (inet_pton(IPV4, server_ip, &server_addr.sin_addr) <= 0)
                handle_errors("ERROR: Invalid Address / Address not supported");

        if (connect(node->sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
                handle_errors("ERROR: Connection failed!\n");

        const char* msg = "Hello from client Node!\n";
        send(node->sock, msg, strlen(msg), 0);

        char buf[1024] = {0};
        const ssize_t recv_bytes = recv(node->sock, buf, sizeof(buf) - 1, 0);
        if (recv_bytes > 0) {
                buf[recv_bytes] = '\0';
                printf("Server replied: %s\n", buf);
        }

        close(node->sock);
}



int main(int argc, char* argv[]) {
        printf("Starting P2P Node...\n");

        Node node;
        pthread_t server_thread_id;

        if (pthread_create(&server_thread_id, NULL, run_server, &node) != 0)
                handle_errors("ERROR: Thread creation failed!\n");

        sleep(1);

        run_client(&node, "127.0.0.1");
        pthread_join(server_thread_id, NULL);

        return 0;
}


