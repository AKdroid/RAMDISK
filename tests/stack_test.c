#include<stdio.h>
#include<stdlib.h>
#include"stack.h"

int main(){

    stack s1,s2;
    int i,r;
    init_stack(&s1,5);
    init_stack(&s2,6);

    for(i=0;i<6;i++){
        r = push(&s1,i);
        printf("s1: r=%d top=%d\n",r,peek(&s1));
        r = push(&s2,i);
        printf("s2: r=%d top=%d\n",r,peek(&s2));
    }

    while(is_empty(&s1)==0){
        r = pop(&s1);
        printf("s1: pop %d\n",r);
    }
    
    printf("s1: empty:%d %d %d\n",is_empty(&s1),peek(&s1),pop(&s1));

    while(is_empty(&s2)==0){
        r = pop(&s2);
        printf("s2: pop %d\n",r);
    }

    destroy_stack(&s1);
    destroy_stack(&s2);

    return 0;
}
