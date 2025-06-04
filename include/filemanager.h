#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "utils.h"

#define MAX_FILES 32
#define MAX_FILENAME_LEN 32
#define MAX_FILE_SIZE 1024

typedef struct 
{
    char name[MAX_FILENAME_LEN];
    char data[MAX_FILE_SIZE];
    int size;
    bool used;
} File;

extern void fs_init();
extern bool fs_create(const char* name);
//extern bool fs_delete(const char* name);
extern int fs_write(const char* name, const char* data, int size);
//extern int fs_read(const char* name, char* out_buffer, int buffer_size);
//extern void fs_list();

#endif