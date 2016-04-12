#ifndef P4_STACK_H
#define P4_STACK_H 

#include<stdio.h>
#include<stdlib.h>

struct stack{
    
    int* stack;
    int* top;
    int max_size;
};

typedef struct stack stack;

void init_stack(stack* s, int max_size);
int is_empty(stack *s);
int push(stack* s, int value);
int peek(stack* s);
int pop(stack* s);
void destroy_stack(stack* s);

#endif /* P4_STACK_H */
