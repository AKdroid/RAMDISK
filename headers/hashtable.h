#ifndef P4_HASHTABLE_H
#define P4_HASHTABLE_H

#include<stdio.h>
#include<stdlib.h>
#include"list.h"
#define BUCKETS 53

struct hash_table{
    int buckets;
    list* entries;          
};

struct hash_table_node{
    char* key; //Null terminated strings
    void* payload;
};

typedef struct hash_table hash_table;
typedef struct hash_table_node hash_table_node;

void init_table(hash_table* table, int bucket_size);
unsigned int compute_hash(char* key);
void insert_into_table(hash_table* table, char* key, void* payload);
int contains_key(hash_table* table, char* key);
void* get_from_table(hash_table* table, char* key);
void delete_from_table(hash_table* table, char* key);

#endif
