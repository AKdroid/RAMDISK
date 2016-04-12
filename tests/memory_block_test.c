#include<stdio.h>
#include<stdlib.h>
#include"memory_block.h"

int main(){

    block_manager* manager=NULL;
    block* b;
    char* x;
    int i;
    
    manager = get_block_manager(65536,1);

    for(i=0;i<17;i++){
        b = get_free_block(manager);
        if(b!=NULL){
            printf("id: %d\n",b->id);
        }else{
            printf("Block not available\n");
        }
        if(b!=NULL){
        x = get_disk_chunk(manager,b->id);
        if(x!=NULL)
            sprintf(x,"Block no. %d",b->id);
        }
        printf("available_blocks = %d\n",manager->available_blocks);
    }
    
    for(i=0;i<17;i++){
        b = get_block(manager, i);
        if(b!=NULL){
            x=get_disk_chunk(manager,b->id);
            if(x!=NULL)
                printf("%s\n",x);
        }
        release_block(manager,b);
        printf("available_blocks = %d\n",manager->available_blocks);
    }

    destroy_block_manager(manager);

    return 0;

}
