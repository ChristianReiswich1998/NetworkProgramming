/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief Server
 * ********************************************************************************************************************/


#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <err.h>
#include <arpa/inet.h>

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
        struct sockaddr_in server_addr;
        struct sockaddr_in client_addr;
} Node;


static void handle_errors(const char* msg) {
        perror(msg);
        exit(EXIT_FAILURE);
}


static void run_server(Node* node) {
        if ((node->server_fd = socket(IPV4, TCP, 0)) < 0) {
                handle_errors("ERROR: Socket failed!\n");
        }

        const int opt = 1;
        if (setsockopt(node->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                handle_errors("ERROR: set socket!\n");
        }

        memset(&node->addr, 0, sizeof(node->addr));
        node->addr.sin_family      = IPV4;
        node->addr.sin_port        = htons(PORT);
        node->addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(node->server_fd, (struct sockaddr*)&node->addr, sizeof(node->addr)) < 0) {
                handle_errors("ERROR: bind!\n");
        }

        if (listen(node->server_fd, 1) < 0) {
                handle_errors("ERROR: listen!\n");
        }

        printf("Node listening on port %d", PORT);

        for (;;) {
                socklen_t client_len = sizeof(node->client_addr);
                node->client_fd = accept(node->server_fd, (struct sockaddr*)&node->client_addr, &client_len);
                if (node->client_fd < 0) {
                        handle_errors("ERROR: accept");
                }

                char buf[1024] = {0};
                const ssize_t recv_bytes = recv(node->client_fd, buf, sizeof(buf) - 1, 0);
                if (recv_bytes < 0) {
                        handle_errors("ERROR: Server msg");
                }

                buf[recv_bytes] = '\0';
                printf("Server Node: %s\n", buf);
                close(node->client_fd);
        }

        close(node->server_fd);
}

static void run_client(Node* node, const char* server_ip) {
        if ((node->sock = socket(IPV4, TCP, 0)) < 0) handle_errors("ERROR: create client");

        memset(&node->server_addr, 0, sizeof(node->server_addr));
        node->server_addr.sin_family      = IPV4;
        node->server_addr.sin_port        = htons(PORT);

        if (inet_pton(IPV4, server_ip, &node->server_addr.sin_addr) <= 0) {
                handle_errors("ERROR: Invalid Address / Address not supported");
        }

        if (connect(node->sock, (struct sockaddr*)&node->server_addr, sizeof(node->server_addr)) < 0) {
                handle_errors("ERROR: Connection failed!\n");
        }

        char buf[1024] = {0};
        const ssize_t recv_bytes = recv(node->sock, buf, sizeof(buf) - 1, 0);
        if (recv_bytes < 0) {
               handle_errors("ERROR: ");
        }

        buf[recv_bytes] = '\0';
        printf("Server replied: %s\n", buf);
}



int main(int argc, char* argv[]) {
        printf("I am a P2P Node!\n");

        Node node;
        run_server(&node);
        run_client(&node, "127.0.0.1");

        return 0;
}


