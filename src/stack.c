#include<stdio.h>
#include<stdlib.h>
#include"stack.h"

void init_stack(stack* s, int max_size){

    s->max_size = max_size;
    s->stack = NULL;
    s->top= NULL;
    s->stack =(int*) malloc(sizeof(int)*max_size);
    if(s->stack == NULL){
        perror("Stack malloc");
    }
}

int is_empty(stack *s){
    if(s == NULL)
        return -1;
    if(s->top == NULL)
        return 1;
    return 0;
}

int push(stack* s, int value){
    
    if(s == NULL)
        return -1;

    if(s->top == NULL)
        s->top = s->stack;
    else {
        if(s->top - s->stack == s->max_size - 1)
            return 0;
        s->top = s->top + 1;
    }
    
    *(s->top) = value;
    return 1;
}

int peek(stack* s){
    if(s == NULL)
        return -1;
    if(s->top == NULL)
        return -1;
    return *(s->top);
}

int pop(stack* s){
    int result;
    if(s == NULL)
        return -1;
    if(s->top == NULL)
        return -1;
    result = *(s->top);
    if(s->top == s->stack)
        s->top = NULL;
    else    
        s->top = s->top-1;
    return result;
}

void destroy_stack(stack* s){
    
    free(s->stack);
    s->stack = NULL;
    s->top = NULL;

}

