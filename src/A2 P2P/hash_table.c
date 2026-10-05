/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief P2P - hash_table.c - Routing Table
 * ********************************************************************************************************************/


#include "hash_table.h"


void init_table(Table* table) {
        if (table == NULL) return;
        memset(table, 0, sizeof(Table));
        table->id = 1;
}


void print_hex_array(uint8_t val[], const char* value) {
        printf("%s : ", value);

        for (size_t i=0; i< KEY_SIZE; i++)
                printf("%02x ", val[i]);

        printf("\n");
}


void xor_distance(const uint8_t own_node[KEY_SIZE], const uint8_t other_node[KEY_SIZE], uint8_t distance[KEY_SIZE]) {
        for (size_t i=0; i< KEY_SIZE; i++)
                distance[i] = own_node[i] ^ other_node[i];
}


int compare_nodes(const void *a, const void *b) {
        const uint8_t* entry_A = a;
        const uint8_t* entry_B = b;

        return memcmp(&entry_A[START_INDEX], &entry_B[START_INDEX], KEY_SIZE);
}


void add_node_to_k_bucket(Table* table, const uint8_t other_node[KEY_SIZE]) {
        if (table->index >= KEY_SIZE) return;

        table->K_bucket[table->index][0] = table->id;

        xor_distance(table->own_node, other_node, table->distance);

        for (size_t i=0; i< KEY_SIZE; i++)
                table->K_bucket[table->index][1 + i] = table->distance[i];
        table->id++;
        table->index++;

        qsort(table->K_bucket, table->index, sizeof(table->K_bucket[0]), compare_nodes);
}


void print_k_bucket(const Table* table) {
        printf("ID\t Distance\n");

        for (size_t i=0; i< 10; i++) {
                printf("%d\t", table->K_bucket[i][0]);
                for (int x=0; x< KEY_SIZE; x++)
                        printf("%02x ", table->K_bucket[i][1 + x]);

                printf("\n");
        }
}


int main(void) {
        printf("Hash Table\n");

        Table table;
        init_table(&table);

        const uint8_t own_addr[KEY_SIZE] = { 0x10, 0x1A, 0x25, 0x3F, 0x4B, 0x52, 0x6C, 0x78, 0x89, 0x9E, 0xB4, 0xC2,
                0xD7, 0xE0, 0xEC, 0xF3, 0xFA, 0xFA, 0xDF, 0xFF };
        memcpy(table.own_node, &own_addr, KEY_SIZE);

        //Test
        uint8_t node1[KEY_SIZE];
        const uint8_t node1_addr[KEY_SIZE] = { 0x0F, 0x1E, 0x2D, 0x3C, 0x4B, 0x5A, 0x69, 0x78, 0x87, 0x96, 0xA5, 0xB4,
                0xC3, 0xD2, 0xE1, 0xF0, 0x01, 0x23, 0x45, 0x67 };
        memcpy(node1, node1_addr, KEY_SIZE);

        uint8_t node2[KEY_SIZE];
        const uint8_t node2_addr[KEY_SIZE] = { 0xA0, 0xB1, 0xC2, 0xD3, 0xE4, 0xF5, 0x06, 0x17, 0x28, 0x39, 0x4A, 0x5B,
                0x6C, 0x7D, 0x8E, 0x9F, 0xAA, 0xBB, 0xCC, 0xDD };
        memcpy(node2, node2_addr, KEY_SIZE);

        uint8_t node3[KEY_SIZE];
        const uint8_t node3_addr[KEY_SIZE] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x11, 0x22, 0x33, 0x44,
                0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC };
        memcpy(node3, node3_addr, KEY_SIZE);

        uint8_t node4[KEY_SIZE];
        const uint8_t node4_addr[KEY_SIZE] = { 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0xEF, 0xCD, 0xAB, 0x89,
                0x67, 0x45, 0x23, 0x01, 0xEE, 0xFF, 0x00, 0x11 };
        memcpy(node4, node4_addr, KEY_SIZE);

        uint8_t node5[KEY_SIZE];
        const uint8_t node5_addr[KEY_SIZE] = { 0x3C, 0x4D, 0x5E, 0x6F, 0x70, 0x81, 0x92, 0xA3, 0xB4, 0xC5, 0xD6, 0xE7,
                0xF8, 0x09, 0x1A, 0x2B, 0x33, 0x44, 0x55, 0x66 };
        memcpy(node5, node5_addr, KEY_SIZE);

        uint8_t node6[KEY_SIZE];
        const uint8_t node6_addr[KEY_SIZE] = { 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x99, 0xAA, 0xBB, 0xCC,
                0xDD, 0xEE, 0xFF, 0x00, 0xA1, 0xB2, 0xC3, 0xD4 };
        memcpy(node6, node6_addr, KEY_SIZE);

        uint8_t node7[KEY_SIZE];
        const uint8_t node7_addr[KEY_SIZE] = { 0x05, 0x15, 0x25, 0x35, 0x45, 0x55, 0x65, 0x75, 0x85, 0x95, 0xA5, 0xB5,
                0xC5, 0xD5, 0xE5, 0xF5, 0x1E, 0x2D, 0x3C, 0x4B };
        memcpy(node7, node7_addr, KEY_SIZE);

        uint8_t node8[KEY_SIZE];
        const uint8_t node8_addr[KEY_SIZE] = { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B,
                0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53 };
        memcpy(node8, node8_addr, KEY_SIZE);

        uint8_t node9[KEY_SIZE];
        const uint8_t node9_addr[KEY_SIZE] = { 0xD0, 0xE1, 0xF2, 0x03, 0x14, 0x25, 0x36, 0x47, 0x58, 0x69, 0x7A, 0x8B,
                0x9C, 0xAD, 0xBE, 0xCF, 0xDE, 0xAD, 0xBE, 0xEF};
        memcpy(node9, node9_addr, KEY_SIZE);

        uint8_t node10[KEY_SIZE];
        const uint8_t node10_addr[KEY_SIZE] = { 0x91, 0xA2, 0xB3, 0xC4, 0xD5, 0xE6, 0xF7, 0x08, 0x19, 0x2A, 0x3B, 0x4C,
                0x5D, 0x6E, 0x7F, 0x80, 0xCA, 0xFE, 0xBA, 0xBE};
        memcpy(node10, node10_addr, KEY_SIZE);

        print_hex_array(table.own_node, "Own Key");

        print_hex_array(node1, "Key 1");
        print_hex_array(node2, "Key 2");
        print_hex_array(node3, "Key 3");
        print_hex_array(node4, "Key 4");
        print_hex_array(node5, "Key 5");
        print_hex_array(node6, "Key 6");
        print_hex_array(node7, "Key 7");
        print_hex_array(node8, "Key 8");
        print_hex_array(node9, "Key 9");
        print_hex_array(node10, "Key 10");

        add_node_to_k_bucket(&table, node1);
        add_node_to_k_bucket(&table, node2);
        add_node_to_k_bucket(&table, node3);
        add_node_to_k_bucket(&table, node4);
        add_node_to_k_bucket(&table, node5);
        add_node_to_k_bucket(&table, node6);
        add_node_to_k_bucket(&table, node7);
        add_node_to_k_bucket(&table, node8);
        add_node_to_k_bucket(&table, node9);
        add_node_to_k_bucket(&table, node10);

        print_k_bucket(&table);
        
}

