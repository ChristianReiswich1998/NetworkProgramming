/***********************************************************************************************************************
 * @author Christian Reiswich
 * @date 05.10.2026
 * @brief net_scanner.
 * ********************************************************************************************************************/


#include "net_scanner.h"



void init_net_scanner(NetScanner* net_scanner) {
        if (net_scanner == NULL) return;
        memset(net_scanner, 0, sizeof(NetScanner));
}


void delete_net_scanner(NetScanner* net_scanner) {
        freeifaddrs(net_scanner->ifaddr);
        net_scanner->ifaddr = NULL;
        net_scanner->ifa    = NULL;
}


void get_local_ip(NetScanner* net_scanner) {
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



int main(void) {
        NetScanner net_scanner;
        init_net_scanner(&net_scanner);
        get_local_ip(&net_scanner);
        delete_net_scanner(&net_scanner);
        return 0;
}


