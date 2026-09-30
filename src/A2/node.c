/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief P2P - Node
 * ********************************************************************************************************************/


#include "node.h"


void handle_errors(const char* msg) {
        perror(msg);
        exit(EXIT_FAILURE);
}


Node* init_node(void) {
        Node* node = malloc(sizeof(Node));
        if (node == NULL)
                handle_errors("ERROR");

        memset(node, 0, sizeof(Node));
        pthread_mutex_init(&node->lock, NULL);
        pthread_cond_init(&node->ok_to_send, NULL);

        return node;
}


void delete_node(Node* node) {
        if (node == NULL) return;

        pthread_mutex_destroy(&node->lock);
        pthread_cond_destroy(&node->ok_to_send);
        free(node);
}


void run_event_loop(Node* node) {
#if defined(__linux__)
        node->epoll_fd = epoll_create(1);
        struct epoll_event event;
        struct epoll_event events[MAX_EVENTS];
        event.events = EPOLLIN;
        event.data.fd = node->server_fd;
        epoll_ctl(node->epoll_fd, EPOLL_CTL_ADD, node->server_fd, &event);

        while (1) {
                const int num_events = epoll_wait(node->epoll_fd, events, MAX_EVENTS, -1);
                if (num_events == -1) {
                        handle_errors("ERROR");
                        break;
                }

                for (int i=0; i< num_events; i++) {
                        if (events[i].data.fd == node->server_fd) {
                                struct sockaddr_in client_addr;
                                socklen_t client_len = sizeof(client_addr);

                                node->client_fd = accept(node->server_fd, (struct sockaddr*)&client_addr, &client_len);
                                if (node->client_fd < 0)
                                        handle_errors("ERROR");
                                else {
                                        event.data.fd = node->client_fd;
                                        epoll_ctl(node->epoll_fd, EPOLL_CTL_ADD, node->client_fd, &event);
                                }
                        } else {
                                char buf[1024] = {0};
                                const ssize_t recv_bytes = recv(events[i].data.fd, buf, sizeof(buf) - 1, 0);

                                if (recv_bytes > 0) {
                                        buf[recv_bytes] = '\0';
                                        printf("Server Node: %s\n", buf);
                                }

                                else if (recv_bytes == 0) {
                                        printf("Connection is closed!\n");
                                        close(events[i].data.fd);
                                }

                                else if (recv_bytes < 0)
                                        handle_errors("ERROR");
                        }
                }
        }

#elif defined(__APPLE__)
        node->kqueue_fd = kqueue();
        struct kevent change;
        struct kevent events[MAX_EVENTS];
        // Helper macro to initialize EVFILT_READ with EV_ADD and EV_ENABLE
        EV_SET(&change, node->server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

        if (kevent(node->kqueue_fd, &change, 1, NULL, 0, NULL) < 0) {
                handle_errors("ERROR");
                close(node->kqueue_fd);
        }

        while (1) {
                const int num_events = kevent(node->kqueue_fd, NULL, 0, events, 10, NULL);
                if (num_events == -1) {
                        handle_errors("ERROR");
                        break;
                }

                for (int i=0; i< num_events; i++) {
                        if (events[i].ident == node->server_fd) {
                                struct sockaddr_in client_addr;
                                socklen_t client_len = sizeof(client_addr);

                                node->client_fd = accept(node->server_fd, (struct sockaddr*)&client_addr, &client_len);
                                if (node->client_fd < 0)
                                        handle_errors("ERROR");
                                else {
                                        EV_SET(&change, node->client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                                        kevent(node->kqueue_fd, &change, 1, NULL, 0, NULL);
                                }
                        } else {
                                char buf[1024] = {0};
                                const ssize_t recv_bytes = recv(events[i].ident, buf, sizeof(buf) - 1, 0);

                                if (recv_bytes > 0) {
                                        buf[recv_bytes] = '\0';
                                        printf("Server Node: %s\n", buf);
                                }

                                else if (recv_bytes == 0) {
                                        printf("Connection is closed!\n");
                                        close(events[i].ident);
                                }

                                else if (recv_bytes < 0)
                                        handle_errors("ERROR");
                        }
                }
        }
#else
        // poll(...) als Fallback
#endif
}


void* run_server(void *arg) {
        Node* node = arg;

        if ((node->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                handle_errors("ERROR");

        if (fcntl(node->server_fd, F_SETFL, fcntl(node->server_fd, F_GETFL, 0) | O_NONBLOCK) == -1) {
                handle_errors("ERROR");
        }

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

        run_event_loop(node);

        const char* replay = "Hello back from Server Node!\n";
        send(node->client_fd, replay, strlen(replay), 0);

        close(node->client_fd);
        close(node->server_fd);
        return NULL;
}


void run_client(Node* node, const char* server_ip) {
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


