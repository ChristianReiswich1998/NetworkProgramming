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

#define PORT       9000
#define LOCAL_HOST AF_LOCAL
#define IPV4       AF_INET
#define IPV6       AF_INET6
#define TCP        SOCK_STREAM
#define UDP        SOCK_DGRAM


typedef struct {
        int sockfd;
        struct sockaddr_in serverAddr;
        int clientSocket;
} Server;


static void handle_errors(const char* msg) {
        perror(msg);
        exit(EXIT_FAILURE);
}


static void create_socket(Server* server) {
        if ((server->sockfd = socket(IPV4, TCP, 0)) < 0) handle_errors("Init Socket failed!\n");
        const int opt = 1;
        setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}


static void define_server_addr(Server* server) {
        memset(&server->serverAddr, 0, sizeof(server->serverAddr));
        server->serverAddr.sin_family      = IPV4;
        server->serverAddr.sin_port        = htons(PORT);
        server->serverAddr.sin_addr.s_addr = INADDR_ANY;
}


static void bind_socket_to_addr(Server* server) {
        if (bind(server->sockfd, (struct sockaddr*)&server->serverAddr, sizeof(server->serverAddr)) < 0) {
                handle_errors("ERROR: \n");
        }
}


static void accept_client(Server* server) {
        if (listen(server->sockfd, 5) < 0) handle_errors("ERROR: Listen\n");
        printf("Server on Port %d...\n", PORT);

        server->clientSocket = accept(server->sockfd, NULL, NULL);
        if (server->clientSocket < 0) {
                handle_errors("ERROR: Accept\n");
        }

        const char* welcome = "Welcome to my Server!\n";
        send(server->clientSocket, welcome, strlen(welcome), 0);
}


static void get_msg(const Server* server) {
        char buffer[1024];
        const ssize_t recv_bytes = recv(server->clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (recv_bytes < 0) {
                handle_errors("ERROR: Getting Message failed!\n");
        }

        buffer[recv_bytes] = '\0';
        printf("Message from Client: %s\n", buffer);
}


int main(void/*int argc, char* argv[]*/) {
        printf("Server!\n");

        Server server;
        create_socket(&server);
        define_server_addr(&server);
        bind_socket_to_addr(&server);
        accept_client(&server);
        get_msg(&server);
        close(server.sockfd);

        return 0;
}