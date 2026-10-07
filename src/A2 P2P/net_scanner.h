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
#include <netdb.h>

#define MAX_PORTS 65535


typedef enum {
        STATIC_NAT, // a private IP address is translated to a public IP address
        DYNAMIC_NAT, // multiple private IP addresses are mapped to a pool of public IP addresses
        PORT_ADDRESS_TRANSLATION // local (private) IP addresses can be translated to a single public IP address. Port
        //numbers are used to distinguish the traffic, i.e., which traffic belongs to which IP address.
} Nat_type;

typedef enum  {
        PORT_INVALID, // > 0 : Invalid Ports
        PORT_SYSTEM,  // 0 -> 1023 : System Ports/Well Known Ports
        PORT_USER,    // 1024 -> 49151 : User Ports/Registered Ports
        PORT_DYNAMIC  // 49152 -> 65535 : Dynamic Ports/Private Port
}Port_type;


typedef struct {
        struct ifaddrs* ifaddr;
        struct ifaddrs* ifa;
        char host[INET_ADDRSTRLEN];
        int port;
        char mac_addr[30];
        Nat_type nat_type;
        Port_type port_type;
} NetScanner;


void init_net_scanner(NetScanner* net_scanner);
void delete_net_scanner(NetScanner* net_scanner);
void scan_local_ip(NetScanner* net_scanner);
void scan_open_ports(NetScanner* net_scanner);
void allot_high_port(NetScanner* net_scanner, const int port);


