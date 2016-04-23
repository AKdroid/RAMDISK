
#define FUSE_USE_VERSION 26

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"memory_block.h"
#include"filetable.h"
#include<fuse.h>
#include <errno.h>
#include <fcntl.h>

//#define FUSE_USE_VERSION 26

filetable ft;
block_manager* manager = NULL;

struct file_handle{
    int w_block;
    int w_offset;
    int r_offset;
    int r_block;
};

typedef struct file_handle handle;

//hash_table file_handle_table;

handle* get_new_handle(){
    handle * h;
    h = (handle*) malloc(sizeof(handle));
    h->w_block=-1;
    h->r_block=-1;
    h->r_offset=0;
    h->w_offset=0;
    return h;
}

int ramdisk_getattr (const char * path, struct stat *st){

    entry* e= NULL;
    e =  get_entry(ft,path);
    printf("getattr: %s\n",path);
    if(e == NULL)
        return -ENOENT;
    *st = e->info;
    return 0;

}

int ramdisk_mkdir(const char* path, mode_t mode){

    int res = 0;
    int entries_available = get_filetable_entries();
    if(entries_available <= 0)
        return -ENOSPC;
    if(insert_directory(ft, path,mode)== NULL)
        res = -ENOSPC;
    printf("mkdir: path = %s\n",path);
    return res;

}

int ramdisk_readdir(const char *path, void *buf, fuse_fill_dir_t  filler, off_t offset, struct fuse_file_info * fi){    

    entry *e, *q;
    list_node* head;
    int is_dir=0;
    (void) offset;
    (void) fi;
    char *temp,*p,*c;
    
    e = get_entry(ft, path);
    if(e == NULL)
        return -ENOENT;

    printf("readdir: path = %s\n",path);

    is_dir = (e->children != NULL);
    if(is_dir){
        filler(buf,".",&(e->info),0);
        filler(buf,"..",NULL,0);
        head = e->children->head;
        while(head != NULL){
            q = get_entry(ft, (char*) head->payload);    
            temp = (char*)malloc(strlen(q->name)+1);
            strcpy(temp,q->name);
            split_path(temp,&p,&c);
            filler(buf,c,&(q->info),0);    
            free(temp);
            head = head->next;
        }
    }
    return 0;
}

int ramdisk_create(const char * path, mode_t mode, struct fuse_file_info *fi){
    
    entry *e;
    handle* h;
    struct fuse_context *fc = fuse_get_context();
    int entries_available = get_filetable_entries();
    if(entries_available <= 0)
        return -ENOSPC; 

    e = get_entry(ft,path);


    if(e == NULL){
        e = insert_file(ft, path, mode);
        e->info.st_uid = fc->uid;
        e->info.st_gid = fc->gid;
        h = get_new_handle();
        //h->key = e->name;

        //h->total_offset=0;
    } else {
        return ramdisk_open(path,fi);
    }
    fi->fh=h;
    printf("create: %s: path, %u: mode\n",path,mode);
    //insert_into_table(file_handle_table);
    return 0;    

}

int ramdisk_open(const char* path, struct fuse_file_info* fi){

    int res = 0;
    int f = fi->flags;
    int rd,wr,ap;
    handle* h;
    entry * e;
    block* b,*p;
    struct fuse_context *fc = fuse_get_context();
    
    e = get_entry(ft,path);
   
    if(e == NULL)
        return -ENOENT;
 
    if (fc->uid == e->info.st_uid){
        rd = e->info.st_mode & S_IRUSR;
        wr = e->info.st_mode & S_IWUSR;
    }
    else if (fc->gid == e->info.st_gid){
        rd = e->info.st_mode & S_IRGRP;
        wr = e->info.st_mode & S_IWGRP;
    } else {
        rd = e->info.st_mode & S_IROTH;
        wr = e->info.st_mode & S_IWOTH;
    }

    ap = ((f & O_APPEND)!=0) && ((f & 3) != O_RDONLY) ;

    if( ((f & O_ACCMODE) == O_RDONLY) && !rd ){
        return -EACCES;
    }
    
    if( ((f & O_ACCMODE) == O_WRONLY) && !wr ){
        return -EACCES;
    }            

    if( ((f & O_ACCMODE) == O_RDWR) && (!rd || !wr) ){
        return -EACCES;
    }

    h = get_new_handle();
    if(ap > 0){        
        h->w_block = e->memory_block; 
        b = get_block(manager,h->w_block);
        p = NULL;
        while(b!= NULL){
            p = b;  
            b = get_block(manager,b->next_block);
        }
        if(p!=NULL){
            h->w_block = p->id;
            h->w_offset = p->size;
        }
    } else {
        h->r_block = e->memory_block;
        if((f & O_ACCMODE) == O_WRONLY)
            h->w_block = -1;
        else
            h->w_block = e->memory_block;
    }
    fi->fh = h;
    printf("open: path : %s rd : %d wr : %d ap : %d\n",path,rd,wr,ap);
    //insert_into_table(&file_handle_table,(void*)h);
    return 0; 
}

int ramdisk_read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi){

    block *b,*t;
    entry *e;
    handle* h = (handle*)fi->fh;
    int bid = h->r_block;
    int off = h->r_offset;
    int rd = 0;
    size_t rd_size;
    char* chunk;

    printf("read: path: %s, size: %d, offset : %d\n",path,size,offset);


    if((fi->flags & 3) != O_RDONLY && (fi->flags & 3) != O_RDWR)
        return -EACCES;

    e = get_entry(ft,path);
    if(e == NULL)
       return -ENOENT;

    if(e->info.st_size - offset <= 0)
        return 0;

    size = size < (e->info.st_size - offset) ? size : (e->info.st_size - offset);

    while(size  > 0){
        b = get_block(manager,bid);
        if(bid == -1){
            break;
        }         

        if(off == manager->block_size)
            off = 0;
        rd_size = manager->block_size - off < size? manager->block_size - off : size;
        chunk = get_disk_chunk(manager,bid)+off;    
        memcpy(buf,chunk,rd_size);
        rd += rd_size;
        off += rd_size;
        buf += rd_size;   
        size -= rd_size;
        bid = b->next_block;
    }
    if(off == manager->block_size)
        off = 0;
    h->r_block = bid;
    h->r_offset = off;
    fi->fh = h;
    
    return rd;

}

int ramdisk_write(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi){

    block *b,*t;
    int off;
    handle *h;
    entry* e;
    char* chunk;
    size_t wrt_sz=0;
    int written =0;
    e = get_entry(ft,path); 


    printf("write: path: %s, size: %d, offset : %d\n",path,size,offset);

    if((fi->flags & 3) != O_WRONLY && (fi->flags & 3) != O_RDWR)
        return -EACCES;

    h = (handle*)fi->fh;    

    if(h->w_block==-1){
        b = get_free_block(manager);
        if(b == NULL)
            return -ENOSPC;
        off = 0;
        if(e->memory_block == -1)
            e->memory_block = b->id;
    }
    else if(h->w_offset == manager->block_size){
        t = get_block(manager,h->w_block);
        if(t->next_block == -1){
            b = get_free_block(manager);
            if(b == NULL)
                return -ENOSPC;
        }else{ 
            b = get_block(manager,t->next_block);
        }
        t->next_block = b->id;
        off = 0;
    }else{
        b = get_block(manager,h->w_block);
        off  = h->w_offset;
    }
    chunk = get_disk_chunk(manager,b->id);
    chunk = chunk + off;
    
    while(size > 0){

        if(off == manager->block_size){
            if(b->next_block == -1){
            t = get_free_block(manager);
            if(t == NULL)
                return -ENOSPC;
            } 
            else {
                t = get_block(manager,b->next_block);    
            }
            b->next_block = t->id;
            b = t;
            off = 0;
            chunk = get_disk_chunk(manager,b->id);
        }
        wrt_sz = manager->block_size - off < size ? manager->block_size - off: size;
        printf("%u\n",wrt_sz);
        memcpy(chunk,buf,wrt_sz);
        size -= wrt_sz;
        buf=buf+wrt_sz;
        b->size = b->size + wrt_sz;
        off = off + wrt_sz;
        written = written + wrt_sz;
    }
    h->w_offset = off;
    h->w_block = b->id;    
    e->info.st_size = e->info.st_size > offset+written ? e->info.st_size : offset + written;
    fi->fh = h;
    return written;

}

int ramdisk_release(const char* path, struct fuse_file_info *fi){

    handle *h;
    h=fi->fh;
    fi->fh = NULL;
    if(h != NULL)
        free(h);
    printf("release: path: %s\n",path);
    return 0;

}

int ramdisk_truncate(const char* path, off_t size){
    
    entry *e;
    size_t sz;    
    struct fuse_context *fc = fuse_get_context();
    block* b,*t; 
    int bid,tid,diff,rd,wr;    
    int final_size= size;
    char* buf;

    printf("truncate: path: %s, size: %d\n",path,size);

    if(size < 0)
        return -EINVAL;

    e = get_entry(ft, path);
    if(e == NULL)
        return -ENOENT;

    if (fc->uid == e->info.st_uid){
        rd = e->info.st_mode & S_IRUSR;
        wr = e->info.st_mode & S_IWUSR;
    }
    else if (fc->gid == e->info.st_gid){
        rd = e->info.st_mode & S_IRGRP;
        wr = e->info.st_mode & S_IWGRP;
    } else {
        rd = e->info.st_mode & S_IROTH;
        wr = e->info.st_mode & S_IWOTH;
    }

    if(!wr)
        return -EACCES;

    sz = e->info.st_size;
    
    if(sz ==size)
        return 0;
    
    if(size < sz){
        t= NULL;
        bid = e->memory_block;
        b = NULL;
        tid = -1;
        while(size >= manager->block_size){
            t = b;
            b = get_block(manager,bid);
            size-=manager->block_size;
            sz-=manager->block_size;
            bid = b->next_block;
        }
        if(size == 0){
            if(b == NULL){
                e->memory_block = -1;
            }
            else{
                b->next_block = -1;
            }
        } else {
            b = get_block(manager,bid);
            bid = b->next_block;
            b->size = size;
            b->next_block = -1;
        }
        while(bid != -1){   
            b = get_block(manager,bid);
            bid = b->next_block;
            release_block(manager,b);
        } 
    }else {
        diff = size - sz;
        buf = (char*) malloc(diff);
        memset(buf,0,diff);
        tid = open(path, O_APPEND | O_WRONLY);
        write(tid,buf,diff);
        close(tid);
        free(buf);
    }
 
    e->info.st_size = final_size;
    return 0;
}

int ramdisk_unlink(const char* path){
    
    entry* e;
    int rd,wr;
    struct fuse_context *fc = fuse_get_context();
    e = get_entry(ft,path);

    printf("unlink: path: %s\n",path);

    if(e == NULL)
        return -ENOENT;
    if (fc->uid == e->info.st_uid){
        rd = e->info.st_mode & S_IRUSR;
        wr = e->info.st_mode & S_IWUSR;
    }
    else if (fc->gid == e->info.st_gid){
        rd = e->info.st_mode & S_IRGRP;
        wr = e->info.st_mode & S_IWGRP;
    } else {
        rd = e->info.st_mode & S_IROTH;
        wr = e->info.st_mode & S_IWOTH;
    }
    if(!wr)
        return -EACCES;
    
    //maybe call truncate
    ramdisk_truncate(path,0);
    delete_file_entry(ft,path);

    return 0;
}

int ramdisk_opendir(const char* path, struct fuse_file_info* fi){
    
    entry* e;
    e = get_entry(ft,path);
    int rd,wr;
    struct fuse_context *fc = fuse_get_context();
    if(e == NULL)
        return -ENOENT;
    if (fc->uid == e->info.st_uid){
        rd = e->info.st_mode & S_IRUSR;
        wr = e->info.st_mode & S_IWUSR;
    }
    else if (fc->gid == e->info.st_gid){
        rd = e->info.st_mode & S_IRGRP;
        wr = e->info.st_mode & S_IWGRP;
    } else {
        rd = e->info.st_mode & S_IROTH;
        wr = e->info.st_mode & S_IWOTH;
    }
    if(!rd)
        return -EACCES;

    return 0;    
}

int ramdisk_rmdir(const char* path){

    entry *e;

    e = get_entry(ft,path);
    printf("rmdir: path: %s\n",path);
    if(e == NULL)
        return -ENOENT;

    if(e->children == NULL)
        return -ENOTDIR;

    if(e->children->head != NULL)   
        return -EINVAL;

    delete_directory_entry(ft,path);

    return 0;

}
int ramdisk_releasedir(const char* path, struct fuse_file_info *fi){

    
    entry* e;
    e = get_entry(ft,path);
    if(e == NULL)
        return -ENOENT;

    return 0;

}

static struct fuse_operations ramdisk_oper = {
    .getattr  = ramdisk_getattr,
    .mkdir    = ramdisk_mkdir,
    .readdir  = ramdisk_readdir,
    .create     = ramdisk_create,
    .open = ramdisk_open,
    .write = ramdisk_write,
    .release = ramdisk_release,
    .truncate = ramdisk_truncate,
    .read       = ramdisk_read,
    .unlink     = ramdisk_unlink,
    .opendir    = ramdisk_opendir,
    .rmdir      = ramdisk_rmdir,
    .releasedir   = ramdisk_releasedir,
};

int main(int argc, char* argv[]){

    int memory_size;
    //char* persistent_file;
    entry* e;
    int max_entries;
    char root[] ="/";
   
    char modcmd[500];
 
    if(argc != 3){
        printf("Usage: fusermount ./ramdisk <mount-point> <size-in-mb>\n");
        return 0;
    }
    
    memory_size = atoi(argv[2]);
    max_entries = memory_size * 256 * 2;

    if(memory_size < 1){
        printf("file system size should be greater than 0\n");
        return 0;
    }
    
     
    manager = get_block_manager(4096,memory_size);
    //init_table(&file_handle_table,11);
    ft = NULL;
    ft = get_file_table(0,max_entries);
    insert_directory(ft,root,0755);  
    
    argc = 2;

    return fuse_main(argc, argv, &ramdisk_oper, NULL);

}
