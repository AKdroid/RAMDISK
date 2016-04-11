#include<stdio.h>
#include<stdlib.h>
#include"list.h"

void init_list(list* l){
    l->head = NULL;
    l->tail = NULL;
}

void insert_at_head(list* l, void* payload){

    list_node* new_node = (list_node*) malloc(sizeof(list_node));
    if( new_node == NULL){
        perror("listmalloc:");
        return;
    }
    new_node->payload = payload;
    if(l->head == NULL){
        l->head = new_node;
        l->tail = new_node;
        new_node->next = NULL;
    }else{
        new_node->next = l->head;
        l->head = new_node;
    }
}

void insert_at_tail(list* l, void* payload){

    list_node* new_node = (list_node*) malloc(sizeof(list_node));
    if( new_node == NULL){
        perror("listmalloc:");
        return;
    }
    new_node->payload = payload;
    new_node->next=NULL;
    if(l->head == NULL){
        l->head = new_node;
        l->tail = new_node;
    }else{
        l->tail->next = new_node;
        l->tail=new_node;
    }
}

void* get_from_list(list* l, int index){

    int i=0;
    list_node* h = l->head;
    while(h != NULL){
        if(i == index)
            return h->payload;        
        h=h->next;
        i=i+1;
    }
    return NULL;
}

list_node* get_node_with_payload( list* l, void* d){
    
    list_node* h = l->head;
    while(h != NULL){
        if(h->payload == d)
            return h;
        h=h->next;
    }
    return NULL;

}

//hack

void delete_from_list(list* l, void* d){

    // if payload was malloced, it should be freed separately

    list_node* x,*y;
    if(d == NULL)
        return;
    x = l->head;
    y=NULL;
    while(x!=NULL){
        if(d==x->payload){
            if(y==NULL){
                l->head=x->next;
                if(l->head == NULL)
                    l->tail=NULL;
            } else {
                y->next=x->next;
                if(y->next == NULL)
                    l->tail=y;
            }
            x->next = NULL;
            free(x);
            break;
        }
        y=x;            
        x=x->next;
    }
}

