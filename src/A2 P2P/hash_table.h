/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief P2P - Includes
 * ********************************************************************************************************************/


#pragma once



#include "includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>


#define KEY_SIZE 20
#define BUCKET_SIZE 160

typedef struct {
        uint8_t own_node[KEY_SIZE];
        uint8_t other_node[KEY_SIZE];
        uint8_t distance[KEY_SIZE];
        uint8_t id;
        int K_bucket[160][2];
        int hash;
} Table;



Table* init_table(void);
void delete_table(Table* table);
void print_hex_array(uint8_t val[], const char* value);
void xor_distance(Table* table);
void add_node_to_bucket(Table* table);
void print_k_bucket(const Table* table);






