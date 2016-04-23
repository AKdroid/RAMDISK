#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"hashtable.h"
#include"filetable.h"
#define TABLESIZE 3067

/*

struct filetable_entry{
    int memory_block;
    char* name;
    struct stat info;
};

typedef struct filetable_entry entry;
typedef hashtable* filetable;

*/

int filetable_entries = 0;

filetable get_file_table(int size, int max_entries){

    filetable table = NULL;
    filetable_entries = max_entries;
    table = (filetable) malloc (sizeof(hash_table));
    if(size == 0)
        size = TABLESIZE;
    init_table(table, size);
    return table;

}

int get_filetable_entries(){

    return filetable_entries;
}


void split_path(char* p, char ** parent, char ** child){

    char *x, *a;    

    *parent = NULL;
    * child = NULL;

    if(p == NULL){
        return;
    }
    x = p;
    *parent = p;
    *child = p;
    while(*x){
        
        if(*x == '/'){
            if(*(x+1)== 0){
                *x = 0;
                break;
            }
            a = x;
            *child = x+1;
        }
        x++;    
    }
    if(a != NULL)
        *a = 0;

}

entry* create_directory_entry(char* key, mode_t mode){

    entry * directory;
    list* l= NULL;

    directory = (entry*) malloc (sizeof(entry));
    l = (list*)malloc(sizeof(list));
    init_list(l);
    memset( &(directory->info), 0, sizeof(struct stat));
    directory->children = l;
    directory->name = key;
    directory->memory_block = -1;
    directory->info.st_mode = S_IFDIR | mode;
    directory->info.st_nlink = 2;
    directory->info.st_mtime = time(NULL);
    directory->info.st_atime = directory->info.st_mtime;
    directory->info.st_ctime = directory->info.st_mtime;
    return directory;

}

entry* create_file_entry(char* key, mode_t mode){
    
    entry* fl;
    list* l=NULL;
    
    fl = (entry*) malloc (sizeof(entry));
    memset( &(fl->info), 0, sizeof(struct stat));
    fl->children = l;
    fl->name = key;
    fl->memory_block=-1;
    fl->info.st_mode = S_IFREG | mode;
    fl->info.st_nlink = 1;
    fl->info.st_mtime = time(NULL);
    fl->info.st_atime = fl->info.st_mtime;
    fl->info.st_ctime = fl->info.st_mtime;
    return fl;
    
}

int is_directory(filetable table, char * path){

    entry* e = get_entry(table,path);
    if(e == NULL)
        return -1;
    if(e->children == NULL)
        return 0;
    return 1;

}

int is_file(filetable table, char* path){

    entry* e = get_entry(table,path);
    if(e == NULL)
        return -1;
    if(e->children == NULL)
        return 1;
    return 0; 
   
}

entry* insert_directory(filetable table, char* path, mode_t mode){
    
    entry* e, *p_directory=NULL;
    char* parent=NULL,*child=NULL;
    int p,c;

    char* permpath = (char*)malloc(strlen(path)+1);
    char* temppath = (char*)malloc(strlen(path)+1);
    strcpy(permpath,path);    
    strcpy(temppath,path);
    split_path(temppath,&parent,&child);

    e = create_directory_entry(permpath,mode);
    
    p = strcmp(parent,"");
    c = strcmp(child,"");

    if(p == 0 && c == 0){
        //Creating root directory
        child = "/";
    } else if( p == 0 && c > 0){
        // 
        parent = "/";
    }

    p_directory = get_entry(table,parent); 
    
    if(p_directory != NULL){
        insert_at_tail(p_directory->children,(void*)permpath);
    }        
    insert_into_table(table,permpath,(void*)e);
    free(temppath);
    filetable_entries -= 1;
    return e;
}

entry* insert_file(filetable table, char* path, mode_t mode){
    
    entry* e, *p_directory=NULL;
    char* parent=NULL,*child=NULL;
    int p,c;
    char* permpath = (char*)malloc(strlen(path)+1);
    char* temppath = (char*)malloc(strlen(path)+1);
    strcpy(permpath,path);
    strcpy(temppath,path);
    split_path(temppath,&parent,&child);

    e = create_file_entry(permpath,mode);

    p = strcmp(parent,"");
    c = strcmp(child,"");

    if(p == 0 && c == 0){
        //Creating root directory
        child = "/";
    } else if( p == 0 && c > 0){
        // 
        parent = "/";
    }

    p_directory = get_entry(table,parent);

    printf("%s %p \n",path,p_directory);

    if(p_directory != NULL){
        insert_at_tail(p_directory->children,(void*)permpath);
    }
    insert_into_table(table,permpath,(void*)e);
    free(temppath);
    filetable_entries -= 1;
    return e;

}

entry* get_entry(filetable table, char *path){
    entry* x = NULL;
    if(table == NULL)
        return NULL;
    x = (entry*) get_from_table(table,path);
    if(x == NULL)
        return NULL;
    return x;
}

void delete_file_entry(filetable table, char* path){
   
    entry* e,*p_directory;
    int p,c;
    char* parent, *child;
    char* temppath = (char*)malloc(strlen(path)+1);
    e = get_entry(table,path); 
    if(e == NULL)
        return;

    strcpy(temppath,path);
    split_path(temppath,&parent,&child);
   
    p = strcmp(parent,"");
    c = strcmp(child,"");

    if(p == 0 && c == 0){
        //Creating root directory
        child = "/";
    } else if( p == 0 && c > 0){
        // 
        parent = "/";
    }

    p_directory = get_entry(table,parent);

    delete_from_list(p_directory->children,e->name);
    delete_from_table(table,e->name);
    free(e->name);
    free(e);    
    filetable_entries += 1;
}

void delete_directory_entry(filetable table, char* path){

    entry* e,*p_directory;
    int p,c;
    char* parent, *child;
    char* temppath = (char*)malloc(strlen(path)+1);
    e = get_entry(table,path);
    if(e == NULL)
        return;

    strcpy(temppath,path);
    split_path(temppath,&parent,&child);

    p = strcmp(parent,"");
    c = strcmp(child,"");

    if(p == 0 && c == 0){
        //Creating root directory
        child = "/";
    } else if( p == 0 && c > 0){
        // 
        parent = "/";
    }

    p_directory = get_entry(table,parent);

    delete_from_list(p_directory->children,e->name);
    delete_from_table(table,e->name);
    free(e->children);
    free(e->name);
    free(e);    
    filetable_entries += 1;
}
