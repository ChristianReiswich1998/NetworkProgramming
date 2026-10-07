/***********************************************************************************************************************
 * @author Christian Reiswich
 * @date 07.10.2026
 * brief bootstrap_node.
 * ********************************************************************************************************************/


#include "bootstrap_node.h"
#include <stdbool.h>

#include "../includes.h"


Bootstrap_node* init_node(void) {
        Bootstrap_node* node = malloc(sizeof(Bootstrap_node));
        if (node == NULL)
                handle_errors();

        memset(node, 0, sizeof(Bootstrap_node));
        pthread_mutex_init(&node->lock, NULL);
        pthread_cond_init(&node->ok_to_send, NULL);

        return node;
}


void delete_node(Bootstrap_node* node) {
        if (node == NULL) return;

        pthread_mutex_destroy(&node->lock);
        pthread_cond_destroy(&node->ok_to_send);
        free(node);
}


void run_event_loop(Bootstrap_node* node) {
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
        if ((node->kqueue_fd = kqueue()) < 0)
                handle_errors();
        struct kevent change;
        struct kevent events[MAX_EVENTS];
        EV_SET(&change, node->server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

        if (kevent(node->kqueue_fd, &change, 1, NULL, 0, NULL) < 0) {
                handle_errors();
                close(node->kqueue_fd);
        }

        while (1) {
                const int num_events = kevent(node->kqueue_fd, NULL, 0, events, 10, NULL);
                if (num_events == -1) {
                        handle_errors();
                        break;
                }

                for (int i=0; i< num_events; i++) {
                        if (events[i].ident == node->server_fd) {
                                struct sockaddr_in client_addr;
                                socklen_t client_len = sizeof(client_addr);

                                node->client_fd = accept(node->server_fd, (struct sockaddr*)&client_addr, &client_len);
                                if (node->client_fd < 0)
                                        handle_errors();
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
                                        handle_errors();
                        }
                }
        }
#else
        // poll(...) als Fallback
#endif
}


void* run_server(void *arg) {
        Bootstrap_node* node = arg;

        if ((node->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                handle_errors();

        if (fcntl(node->server_fd, F_SETFL, fcntl(node->server_fd, F_GETFL, 0) | O_NONBLOCK) == -1) {
                handle_errors();
        }

        const int opt = 1;
        if (setsockopt(node->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                handle_errors();
        }

        memset(&node->addr, 0, sizeof(node->addr));
        node->addr.sin_family      = AF_INET;
        node->addr.sin_port        = htons(node->port);
        node->addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(node->server_fd, (struct sockaddr*)&node->addr, sizeof(node->addr)) < 0)
                handle_errors();

        if (listen(node->server_fd, 5) < 0)
                handle_errors();

        pthread_mutex_lock(&node->lock);
        node->is_ready = true;
        pthread_cond_signal(&node->ok_to_send);
        pthread_mutex_unlock(&node->lock);

        printf("[Server] Node listening on port %i\n", node->port);

        run_event_loop(node);

        const char* replay = "Hello back from Server Node!\n";
        send(node->client_fd, replay, strlen(replay), 0);

        close(node->client_fd);
        close(node->server_fd);
        return NULL;
}


void run_client(Bootstrap_node* node, const char* server_ip) {
        pthread_mutex_lock(&node->lock);
        while (node->is_ready == false) {
                pthread_cond_wait(&node->ok_to_send, &node->lock);
        }
        pthread_mutex_unlock(&node->lock);

        if ((node->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                handle_errors();

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port   = htons(node->port);

        if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
                handle_errors();

        if (connect(node->sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
                handle_errors();

        const char* msg = "Hello from client Node!\n";
        ssize_t send_bytes = send(node->sock, msg, strlen(msg), 0);
        ssize_t total_send_bytes = send_bytes;

        while (total_send_bytes < strlen(msg)) {
                if (send_bytes == - 1) {
                        handle_errors();
                        break;
                }

                if (send_bytes == 0) {
                        handle_errors();
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

        else if (recv_bytes == CON_CLOSED) {
                printf("Connection is closed!\n");
                close(node->sock);
                return;
        }

        else if (recv_bytes < 0)
                handle_errors();

        close(node->sock);
}





int main(const int argc, char* argv[]) {
        printf("Starting Bootstrap Node...\n");

        if (argc != 2) {
                fprintf(stderr, "Usage: <inet_addr> <port>");
                return 1;
        }

        const char* server_ip = argv[0];
        const char* port = argv[1];




        Bootstrap_node* node = init_node();
        pthread_t server_thread_id;

        if (pthread_create(&server_thread_id, NULL, run_server, node) != 0)
                handle_errors();

        run_client(node, server_ip);
        pthread_join(server_thread_id, NULL);

        delete_node(node);

        return 0;
}