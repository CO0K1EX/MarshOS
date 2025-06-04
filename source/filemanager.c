#include "filemanager.h"

File fs[MAX_FILES];

void fs_init()
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        fs[i].used = false;
        fs[i].size = 0;
        fs[i].name[0] = '\0';
    }
}

bool fs_create(const char* name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (fs[i].used && strcmp(fs[i].name, name) == 0)
        {
            return false;
        }
    }

    for (int i = 0; i < MAX_FILES; i++)
    {
        if (fs[i].used)
        {
            strncpy(fs[i].name, name, MAX_FILENAME_LEN);
            fs[i].name[MAX_FILENAME_LEN - 1] = '\0';
            fs[i].size = 0;
            fs[i].used = true;
            return true;
        }
    }

    return false;
}

int fs_write(const char* name, const char* data, int size)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (fs[i].used && strcmp(fs[i].name, name) == 0)
        {
            if (size > MAX_FILE_SIZE)
            {
                size = MAX_FILE_SIZE;
            }

            memcpy(fs[i].data, data, size);
            fs[i].size = size;
            return size;
        }
    }

    return -1;
}