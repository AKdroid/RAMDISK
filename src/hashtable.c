#include<stdio.h>
#include<stdlib.h>
#include"list.h"
#include"hashtable.h"
#include<string.h>

void init_table(hash_table* table, int bucket_size){

    int i = 0;
    if(bucket_size <= 0)
        bucket_size = BUCKETS;    
    table->buckets = bucket_size;
    table->entries = NULL;
    table->entries = (list*) malloc(bucket_size*sizeof(list));
    //Initialize the lists
    for(i=0;i<bucket_size;i++){
        init_list((table->entries)+i);
    }
}

unsigned int compute_hash(char* key){

    //djb string hashing
    // REFERENCE: http://www.cse.yorku.ca/~oz/hash.html

    unsigned int hash=5381;
    char* c=key;
    while(*c){
        hash = ((hash << 5) + hash) + (unsigned int)(*c);
        c++;
    }
    //printf("key = %s : hash = %u\n",key,hash);
    return hash;
}

void insert_into_table(hash_table* table, char* key, void* payload){

    unsigned int hash;
    int entry;
    list* bucket;
    hash_table_node* hnode;
    if(key == NULL){
        printf("Error: Invalid key\n");
        return;
    }   

    hnode = (hash_table_node*) malloc(sizeof(hash_table_node));
    
    if(hnode==NULL){
        perror("hnode malloc:");
        return;
    }
    hnode->key =key;
    hnode->payload = payload;

    delete_from_table(table,key);

    hash = compute_hash(key);
    entry = hash%(table->buckets);
    bucket= table->entries + entry;
    insert_at_head(bucket,(void*)hnode);
}

void* get_from_table(hash_table* table, char* key){
    
    void* payload = NULL;
    int entry;
    unsigned int hash;

    list* bucket;
    list_node* h;
    hash_table_node* hnode;
    if(key == NULL){
        printf("Error: Invalid key\n");
        return NULL;
    } 
    
    hash = compute_hash(key);
    entry = hash%(table->buckets);
    bucket = table->entries + entry; 

    h = bucket->head;
    while(h!=NULL){
        hnode = (hash_table_node*)h->payload;
        if(strcmp(hnode->key,key)==0){
            payload = hnode->payload;
            break;
        }        
        h=h->next;
    }
    /*
    if(payload == NULL)
        printf("Error: Key does not exist\n");
    */
    return payload;

}

int contains_key(hash_table* table, char* key){

    unsigned int hash;
    int entry;
    list* bucket;
    list_node* h;
    hash_table_node* hnode;
    if(key == NULL){
        printf("Error: Invalid key\n");
        return 0;
    }

    hash = compute_hash(key);
    entry = hash%(table->buckets);
    bucket = table->entries + entry;

    h = bucket->head;
    while(h!=NULL){
        hnode = (hash_table_node*)h->payload;
        if(strcmp(hnode->key,key)==0){
            return 1;
        }
        h=h->next;
    }
    return 0;
}

void delete_from_table(hash_table* table, char* key){
   
    unsigned int hash; 
    int entry;
    list* bucket;
    list_node* h,*prev=NULL;
    hash_table_node* hnode;
    if(key == NULL){
        printf("Error: Invalid key\n");
        return;
    }

    hash = compute_hash(key);
    entry = hash%(table->buckets);
    bucket = table->entries + entry;

    h = bucket->head;
    while(h!=NULL){
        hnode = (hash_table_node*)h->payload;
        if(strcmp(hnode->key,key)==0){
            
            //delete node and free hnode
            if(prev == NULL){ // head needs to be removed
                bucket->head=h->next;
            }else{
                prev->next = h->next; 
            }
            if(h->next == NULL)
                bucket->tail = prev;
            free(hnode);
            free(h);
            break;
        }
        prev=h;
        h=h->next;
    }
}

