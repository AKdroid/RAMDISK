#ifndef P4_LIST_H
#define P4_LIST_H

#include<stdio.h>
#include<stdlib.h>

struct list{
    struct list_node* head;
    struct list_node* tail;
};


struct list_node{
    struct list_node* next;
    void* payload;
};

typedef struct list list;
typedef struct list_node list_node;

void init_list(list* l);
void insert_at_head(list* l, void* payload);
void insert_at_tail(list* l, void* payload);
void* get_from_list(list* l, int index);
list_node* get_node_with_payload(list* l, void* query); 
void delete_from_list(list* l, void* payload);

#endif /* P4_LIST_H */
