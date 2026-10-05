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
#define START_INDEX 1


typedef struct {
        uint8_t own_node[KEY_SIZE];
        uint8_t distance[KEY_SIZE];
        uint8_t id;
        int K_bucket[BUCKET_SIZE][2];
        uint8_t index;
} Table;



void init_table(Table* table);
void xor_distance(const uint8_t own_node[KEY_SIZE], const uint8_t other_node[KEY_SIZE], uint8_t distance[KEY_SIZE]);
int compare_nodes(const void *a, const void *b);
void add_node_to_k_bucket(Table* table, const uint8_t other_node[KEY_SIZE]);
void print_k_bucket(const Table* table);
void print_hex_array(uint8_t val[], const char* value);
char* hex_to_binary(const Table* table);






