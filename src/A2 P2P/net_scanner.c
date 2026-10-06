/***********************************************************************************************************************
 * @author Christian Reiswich
 * @date 05.10.2026
 * @brief net_scanner.c
 * ********************************************************************************************************************/


#include "net_scanner.h"
#include <unistd.h>


void init_net_scanner(NetScanner* net_scanner) {
        if (net_scanner == NULL) return;
        memset(net_scanner, 0, sizeof(NetScanner));
}


void delete_net_scanner(NetScanner* net_scanner) {
        freeifaddrs(net_scanner->ifaddr);
        net_scanner->ifaddr = NULL;
        net_scanner->ifa    = NULL;
}


void scan_local_ip(NetScanner* net_scanner) {
        getifaddrs(&net_scanner->ifaddr);

        for (net_scanner->ifa = net_scanner->ifaddr; net_scanner->ifa != NULL; net_scanner->ifa = net_scanner->ifa->ifa_next) {
                if (net_scanner->ifa->ifa_addr != NULL &&
                        net_scanner->ifa->ifa_flags & IFF_UP &&
                        net_scanner->ifa->ifa_addr->sa_family == AF_INET) {

                        const struct sockaddr_in* sockaddr = (struct sockaddr_in*)net_scanner->ifa->ifa_addr;
                        inet_ntop(AF_INET, &sockaddr->sin_addr, net_scanner->host, INET_ADDRSTRLEN);

                        if ((net_scanner->ifa->ifa_flags & IFF_LOOPBACK) == 0) {
                                printf("Schnittstelle: %-6s IP-Adresse: %s\n",
                                        net_scanner->ifa->ifa_name, net_scanner->host);
                        }
                }
        }
}


void scan_open_ports(const NetScanner* net_scanner) {
        int sock_fd;
        struct sockaddr_in addr;

        addr.sin_family = AF_INET;
        inet_pton(addr.sin_family, net_scanner->host, &addr.sin_addr.s_addr);

        for (int i=1; i< MAX_PORTS; i++) {
                if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                        handle_errors();

                addr.sin_port = htons(i);

                if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
                        printf("Port: %d\n", i);

                close(sock_fd);
        }
}


int main(void) {
        NetScanner net_scanner;
        init_net_scanner(&net_scanner);
        scan_local_ip(&net_scanner);
        scan_open_ports(&net_scanner);
        delete_net_scanner(&net_scanner);
        return 0;
}

