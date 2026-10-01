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


void xor_distance(Table* table, uint8_t node[KEY_SIZE]) {
        for (int i=0; i< KEY_SIZE; i++) {
                table->distance[i] = table->own_node[i] ^ node[i];
        }

        /*
        for (int i=0; i< KEY_SIZE; i++)
                table->distance[i] = table->own_node[i] ^ table->other_node[i];
        */
}


void add_node_to_k_bucket(Table* table) {
        table->K_bucket[table->index][0] = table->id;

        for (int i=0; i< KEY_SIZE; i++)
                table->K_bucket[table->index][1] = table->distance[i];
        table->id++;
        table->index++;
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


char* hex_to_binary(const Table* table) {
        static char binary[(KEY_SIZE * 8) + 1];

        uint8_t pos = 0;
        for (int i=0; i< KEY_SIZE; i++) {
                for (int b=7; b>=0; b--) {
                        binary[pos++] = ((table->distance[i] >> b) & 1) ? '1' : '0' ;
                }
                binary[pos++] = ' ';
        }

        binary[pos] = '\0';
        return binary;
}


int main(void) {
        printf("Hash Table\n");

        Table* table = init_table();

        memset(table->own_node, 0xFF, KEY_SIZE);
        memset(table->other_node, 0xAF, KEY_SIZE);

        //Test
        uint8_t node1[KEY_SIZE];
        uint8_t node2[KEY_SIZE];
        uint8_t node3[KEY_SIZE];
        uint8_t node4[KEY_SIZE];
        uint8_t node5[KEY_SIZE];
        uint8_t node6[KEY_SIZE];
        uint8_t node7[KEY_SIZE];
        uint8_t node8[KEY_SIZE];
        uint8_t node9[KEY_SIZE];
        uint8_t node10[KEY_SIZE];

        memset(node1, 0x01, KEY_SIZE);
        memset(node2, 0x02, KEY_SIZE);
        memset(node3, 0x03, KEY_SIZE);
        memset(node4, 0x05, KEY_SIZE);
        memset(node5, 0x0A, KEY_SIZE);
        memset(node6, 0x0C, KEY_SIZE);
        memset(node7, 0xAC, KEY_SIZE);
        memset(node8, 0xAB, KEY_SIZE);
        memset(node9, 0xAE, KEY_SIZE);
        memset(node10, 0xAF, KEY_SIZE);

        print_hex_array(table->own_node, "Own Key");
        //print_hex_array(table->other_node, "Key 2");

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

        xor_distance(table, node1);
        add_node_to_k_bucket(table);
        printf("%s", hex_to_binary(table));

        xor_distance(table, node2);
        add_node_to_k_bucket(table);

        xor_distance(table, node3);
        add_node_to_k_bucket(table);

        xor_distance(table, node4);
        add_node_to_k_bucket(table);

        xor_distance(table, node5);
        add_node_to_k_bucket(table);

        xor_distance(table, node6);
        add_node_to_k_bucket(table);

        xor_distance(table, node7);
        add_node_to_k_bucket(table);

        xor_distance(table, node8);
        add_node_to_k_bucket(table);

        xor_distance(table, node9);
        add_node_to_k_bucket(table);

        xor_distance(table, node10);
        add_node_to_k_bucket(table);

        //print_k_bucket(table);



        //xor_distance(table);
        //print_hex_array(table->distance, "Distance");

        //add_node_to_bucket(table);
        //print_k_bucket(table);


        delete_table(table);
}

