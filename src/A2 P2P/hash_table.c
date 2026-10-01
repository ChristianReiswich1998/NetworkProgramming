/***********************************************************************************************************************
 * @author Christian Reiswich
 * @created on 29.09.2026
 * @brief P2P - hash_table.c - Routing Table
 * ********************************************************************************************************************/


#include "hash_table.h"


Table* init_table(void) {
        Table* table = malloc(sizeof(Table));
        if (table == NULL)
                handle_errors();

        memset(table, 0, sizeof(Table));
        table->hash = 123456789;
        return table;
}


void delete_table(Table* table) {
        if (table == NULL) return;
        free(table);
}


void print_hex_array(uint8_t val[], const char* value) {
        printf("%s : ", value);

        for (int i=0; i< KEY_SIZE; i++)
                printf("%02x ", val[i]);

        printf("\n");
}


void xor_distance(Table* table) {
        for (int i=0; i< KEY_SIZE; i++)
                table->distance[i] = table->own_node[i] ^ table->other_node[i];
}


void add_node_to_bucket(Table* table) {
        uint8_t index = 3;
        table->id = 43;
        table->K_bucket[index][0] = table->id;

        for (int i=0; i< KEY_SIZE; i++)
                table->K_bucket[index][1] = table->distance[i];
        index++;
}


void print_k_bucket(const Table* table) {
        printf("ID\t Distance\n");

        for (int i=0; i< 10; i++) {
                printf("%d\t", table->K_bucket[i][0]);
                for (int x=0; x< KEY_SIZE; x++)
                        printf("%02x", table->K_bucket[i][1]);

                printf("\n");
        }
}


int main(void) {
        printf("Hash Table\n");

        Table* table = init_table();

        memset(table->own_node, 0xFF, KEY_SIZE);
        memset(table->other_node, 0xAF, KEY_SIZE);

        print_hex_array(table->own_node, "Key 1");
        print_hex_array(table->other_node, "Key 2");
        xor_distance(table);
        print_hex_array(table->distance, "Distance");

        add_node_to_bucket(table);
        print_k_bucket(table);


        delete_table(table);
}

