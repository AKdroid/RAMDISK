#include<stdio.h>
#include<stdlib.h>
#include"list.h"
#include"hashtable.h"


int main(){

    hash_table table1, table2;

    char* s[5];
    s[0]="root";
    s[1]="etc";
    s[2]="batman vs superman";
    s[3]="mwahaha";
    s[4]="user123";

    int x[] = {5,3,2,1,4};

    init_table(&table1,23);
    init_table(&table2,43);

    printf("table1 buckets = %d\n",table1.buckets);
    
    printf("table2 buckets = %d\n",table2.buckets);

    insert_into_table(&table1,s[0],(void*)(x+3));
    insert_into_table(&table1,s[1],(void*)(x+2));
    insert_into_table(&table1,s[2],(void*)(x+1));
    insert_into_table(&table1,s[3],(void*)(x+4));
    insert_into_table(&table1,s[4],(void*)x);
    
    if(contains_key(&table1,s[0])==1)    
        printf("Index of %s is %d\n",s[0],*((int*)get_from_table(&table1,s[0])));

    if(contains_key(&table1,s[1])==1) 
        printf("Index of %s is %d\n",s[1],*((int*)get_from_table(&table1,s[1])));

    if(contains_key(&table1,s[2])==1) 
        printf("Index of %s is %d\n",s[2],*((int*)get_from_table(&table1,s[2])));

    if(contains_key(&table1,s[3])==1) 
        printf("Index of %s is %d\n",s[3],*((int*)get_from_table(&table1,s[3])));

    if(contains_key(&table1,s[4])==1) 
        printf("Index of %s is %d\n",s[4],*((int*)get_from_table(&table1,s[4])));


    if(contains_key(&table1,"whatthe")==1) 
        printf("Index of %s is %d\n","whatthe",*((int*)get_from_table(&table1,"whatthe")));
    else
        printf("Key: %s does not exist\n","whatthe");


    delete_from_table(&table1,"whatthe");

    insert_into_table(&table1,s[0],(void*)x);
    
    if(contains_key(&table1,s[0])==1) 
        printf("Index of %s is %d\n",s[0],*((int*)get_from_table(&table1,s[0])));
    else
        printf("Key: %s does not exist\n",s[0]);

    delete_from_table(&table1,s[1]);

    insert_into_table(&table1,s[1],(void*)(x+4));

    if(contains_key(&table1,s[1])==1)
        printf("Index of %s is %d\n",s[1],*((int*)get_from_table(&table1,s[1])));
    else
        printf("Key: %s does not exist\n",s[1]);

    delete_from_table(&table1,s[2]);
    if(contains_key(&table1,s[2])==1)
        printf("Index of %s is %d\n",s[2],*((int*)get_from_table(&table1,s[2])));
    else
        printf("Key: %s does not exist\n",s[2]);

    delete_from_table(&table1,s[3]);
    if(contains_key(&table1,s[3])==1)
        printf("Index of %s is %d\n",s[3],*((int*)get_from_table(&table1,s[3])));
    else
        printf("Key: %s does not exist\n",s[3]);

    delete_from_table(&table1,s[4]);
    if(contains_key(&table1,s[4])==1)
        printf("Index of %s is %d\n",s[4],*((int*)get_from_table(&table1,s[4])));
    else
        printf("Key: %s does not exist\n",s[4]);

    delete_from_table(&table1,s[0]);

    return 0;
}
