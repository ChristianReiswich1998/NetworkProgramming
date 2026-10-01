/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief Client
 * ********************************************************************************************************************/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT       9000
#define LOCAL_HOST AF_LOCAL
#define IPV4       AF_INET
#define IPV6       AF_INET6
#define TCP        SOCK_STREAM
#define UDP        SOCK_DGRAM


typedef struct {
        int socket;
        struct sockaddr_in serverAddr;
} Client;


static void handle_errors(const char* msg) {
        perror(msg);
        exit(EXIT_FAILURE);
}


static void creat_socket(Client* client) {
        if ((client->socket = socket(IPV4, TCP, 0)) < 0) handle_errors("ERROR: Init Socket!\n");
}


static void define_server_addr(Client* client) {
        memset(&client->serverAddr, 0, sizeof(client->serverAddr));
        client->serverAddr.sin_family      = IPV4;
        client->serverAddr.sin_port        = htons(PORT);
        client->serverAddr.sin_addr.s_addr = INADDR_ANY;
}


static void connect_to_server(Client* client) {
        if (connect(client->socket, (struct sockaddr*)&client->serverAddr, sizeof(client->serverAddr)) < 0) {
                handle_errors("ERROR: Connection to Server failed!\n");
        }

        const char* msg = "Hallo Server!\n";
        send(client->socket, msg, strlen(msg), 0);
}


static void get_response(const Client* client) {
        char buffer[1024] = {0};
        const ssize_t recv_bytes = recv(client->socket, buffer, sizeof(buffer) - 1, 0);
        if (recv_bytes < 0) {
                handle_errors("ERROR: get Response failed!\n");
        }

        buffer[recv_bytes] = '\0';
        printf("Response: %s\n", buffer);
}


int main(void/*int argc, char* argv[]*/) {
        printf("Client\n");

        Client client;
        creat_socket(&client);
        define_server_addr(&client);
        connect_to_server(&client);
        get_response(&client);
        close(client.socket);

        return 0;
}
