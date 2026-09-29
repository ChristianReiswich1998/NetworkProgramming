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







int main(int argc, char* argv[]) {
        printf("I am a Node!\n");


        return 0;
}


