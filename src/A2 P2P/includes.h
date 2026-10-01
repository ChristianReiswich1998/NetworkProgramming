/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief P2P - Includes
 * ********************************************************************************************************************/


#pragma once

#include <stdio.h>
#include <stdlib.h>

#define BOOL  int
#define TRUE  1
#define FALSE 0

#define CON_CLOSED 0
#define CON_ERROR  -1


static void handle_errors() {
        perror("ERROR");
        exit(EXIT_FAILURE);
}

