#ifndef P4_fILE_TABLE_H
#define P4_fILE_TABLE_H

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include"hashtable.h"

#define TABLESIZE 3067

struct filetable_entry{
    int memory_block;
    char* name; 
    struct stat info;
    list* children;
};

typedef struct filetable_entry entry;
typedef hash_table* filetable;

filetable get_file_table(int size);

entry* create_directory_entry(char* path, mode_t mode);

entry* create_file_entry(char* path, mode_t mode);

void delete_directory_entry(filetable table,char * path);

void delete_file_entry(filetable table,char* path);

entry* get_entry(filetable table,char* path);

entry rename_entry(char *path, char * new_path);

entry* insert_directory(filetable table, char* path, mode_t mode);

entry* insert_file(filetable table, char* path, mode_t mode);

int is_directory(filetable ft,char * path);

int is_file(filetable ft, char* path);

void split_path(char* p, char ** parent, char ** childname);

#endif
