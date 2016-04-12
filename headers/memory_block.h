#ifndef P4_MEMORY_BLOCK_H
#define P4_MEMORY_BLOCK_H

#include<stdio.h>
#include<stdlib.h>
#include"stack.h"
#define BLOCK_SIZE 1024
#define MEGABYTE 1024*1024

struct block{
    int id;
    int next_block;
    int size;
    int in_use;
};

typedef struct block block;

struct block_manager{
    
    char* disk;
    block *blocks;
    stack free_stack;
    int num_of_blocks;
    int block_size;
    int available_blocks;
};

typedef struct block_manager block_manager;

block_manager* get_block_manager(int block_size, int file_system_size);

block* get_block(block_manager* manager, int i);

block* get_free_block(block_manager* manager);

int release_block(block_manager* manager,block* b);

char* get_disk_chunk(block_manager* manager, int index);

void destroy_block_manager(block_manager* manager);

#endif /* P4_MEMORY_BLOCK_H  */
