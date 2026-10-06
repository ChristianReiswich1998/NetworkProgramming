/***********************************************************************************************************************
 * @author Christian Reiswich
 * @date 05.10.2026
 * @brief net_scanner.
 * ********************************************************************************************************************/

#pragma once

#include "includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#define MAX_PORTS 1024


typedef struct {
        struct ifaddrs* ifaddr;
        struct ifaddrs* ifa;
        char host[INET_ADDRSTRLEN];
} NetScanner;


void init_net_scanner(NetScanner* net_scanner);
void delete_net_scanner(NetScanner* net_scanner);
void scan_local_ip(NetScanner* net_scanner);
void scan_open_ports(const NetScanner* net_scanner);

