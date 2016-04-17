#include<stdio.h>
#include<stdlib.h>
#include"memory_block.h"
#include"stack.h"

/*

struct block{
    int id;
    int next_block;
    int size;
    int in_use;
};

struct block_manager{
    
    char* disk;
    block *blocks;
    stack free_stack;
    int num_of_blocks;
    int block_size;
};

*/

void initialize_block(block * b, int id){
    if(b== NULL)
        return ;

    b->id = id;
    b->in_use = 0;
    b->size = 0;
    b->next_block = -1;    
    
}

block_manager* get_block_manager(int block_size, int file_system_size){

    block_manager* manager;
    int i;       

    manager = (block_manager*) malloc(sizeof(block_manager));
    
    if(block_size == 0)
        block_size = BLOCK_SIZE;
    
    manager->disk = (char*) malloc((file_system_size+1)*MEGABYTE);


    if(manager->disk == NULL){
        perror("disk malloc");
        free(manager);
        return NULL;
    }

    manager->num_of_blocks = file_system_size * (MEGABYTE/block_size);

    manager->block_size = block_size;

    init_stack(&(manager->free_stack), manager->num_of_blocks);

    manager->available_blocks = manager->num_of_blocks;

    if(manager->free_stack.stack == NULL){
        printf("Could not allocate memory for free block stack\n");
        free(manager);
        return NULL;
    }

    manager->blocks = (block *)malloc(sizeof(struct block)* (manager->num_of_blocks));

    if(manager->blocks == NULL){
        perror("Block Table malloc");
        destroy_stack(&(manager->free_stack));
        free(manager);
        return NULL;
    }

    for(i=(manager->num_of_blocks)-1;i>=0;i--){
        push(&(manager->free_stack),i);
        initialize_block((manager->blocks)+i,i);
    }

    return manager;
}

block* get_block(block_manager* manager, int i){
    
    if(manager==NULL)
        return NULL;

    if(i>= (manager->num_of_blocks)|| i<0 )
        return NULL;

    return (manager->blocks) + i ;
}

block* get_free_block(block_manager* manager){
    
    int r;
    block *b;
    if(manager==NULL)
        return NULL;
   
    r = is_empty(&(manager->free_stack));
    
    if(r == 1)
        return NULL;
    
    r = pop(&(manager->free_stack));
    b = get_block(manager,r);
    manager->available_blocks-=1; 
    b->in_use = 1;
    return b;
}

int release_block(block_manager* manager,block* b){
    
    int r;
    if(manager==NULL)
        return 0;
    if(b==NULL)
        return 0;
    r = push(&(manager->free_stack),b->id);
    if(r == 0)
        return 0;
    initialize_block(b,b->id);
    manager->available_blocks+=1;
    return 1;
}

char* get_disk_chunk(block_manager* manager, int index){
    
    if(manager == NULL)
        return NULL;

    if(index >= manager->num_of_blocks )
        return NULL;

    return manager->disk +(index*BLOCK_SIZE);

}

void destroy_block_manager(block_manager* manager){
    free(manager->blocks);
    destroy_stack(&(manager->free_stack));
    free(manager);
}
