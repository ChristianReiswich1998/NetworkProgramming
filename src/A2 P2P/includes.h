/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief P2P - Includes
 * ********************************************************************************************************************/


#pragma once

#include <stdio.h>
#include <stdlib.h>

#define CON_CLOSED 0
#define CON_ERROR  -1

typedef uint32_t ipv4_t;
typedef uint16_t port_t;

#define INET_ADDR(i1, i2, i3, i4) \
    htonl(((uint32_t)(uint8_t)(i1) << 24) | \
          ((uint32_t)(uint8_t)(i2) << 16) | \
          ((uint32_t)(uint8_t)(i3) << 8)  | \
           (uint32_t)(uint8_t)(i4))



static void handle_errors() {
        perror("ERROR");
        exit(EXIT_FAILURE);
}

