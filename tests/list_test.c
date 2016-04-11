#include<stdio.h>
#include<stdlib.h>
#include"list.h"

void print_list(list* l){

    list_node* h = l->head;
    printf("List:\n");
    while(h!=NULL){
        printf("%d\n",*((int*)h->payload));
        h=h->next;
    }
    printf("----\n");
}


int main(){

    list mylist, list2;
    list_node* nd;

    int* pload;
    int x[]={1,2,3,4,5};


    init_list(&mylist);
    init_list(&list2);
    
    print_list(&mylist);
    print_list(&list2);    

    insert_at_head(&mylist,x);
    insert_at_tail(&mylist,x+2);

    insert_at_tail(&list2,x);
    insert_at_head(&list2,x+3);

    print_list(&list2);
    print_list(&mylist);

    pload = (int*)get_from_list(&list2,0);
    printf("payload[0] = %d\n",*pload);

    nd = get_node_with_payload(&list2,(void*)pload);
    printf("Node = %p\n",nd);

    nd = get_node_with_payload(&mylist,(void*)pload);
    printf("Node = %p\n",nd);
   

    pload = (int*)get_from_list(&list2,1);
    printf("payload[1] = %d\n",*pload);
    
    pload = (int*)get_from_list(&list2,3);
    if (pload != NULL){
         printf("payload[3] = %d\n",*pload);
    }else{
         printf("payload[3] does not exist\n");
    }
    
    delete_from_list(&list2,x);
    
    print_list(&list2);

    delete_from_list(&list2,x+1);

    print_list(&list2);

    delete_from_list(&list2,x+3);

    print_list(&list2);

    return 0;
}
