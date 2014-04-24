#include "file_func.h"

#ifdef FILE_WINDOWS
FILE_HANDLE file_open(const char* path, uint32_t access_mode, uint32_t open_mode)
{
    FILE_HANDLE fh = CreateFileA(path,
								access_mode,
                                FILE_SHARE_READ|FILE_SHARE_WRITE,
                                NULL,
                                open_mode,
                                0,
                                NULL
                               );
    return fh;
}

void file_close(FILE_HANDLE f)
{
    CloseHandle(f);
}

uint32_t file_read(FILE_HANDLE fh,void* buf, uint32_t len)
{
    uint32_t total = 0;
    if(!ReadFile(fh,buf,len,(DWORD*)&total,NULL))return -1;
    return total;
}

uint32_t file_write(FILE_HANDLE fh,const void* buf, uint32_t len)
{
    uint32_t total = 0;
    if(!WriteFile(fh,buf,len,(DWORD*)&total,NULL))return -1;
    return total;
}

uint64_t file_seek(FILE_HANDLE fh,uint64_t offset,int mode)
{
    uint32_t low = (uint32_t)offset&(-1);
    uint32_t high = (uint32_t)offset >> 32;
    uint64_t ret = 0;

    low = SetFilePointer(fh,low,(LONG*)&high,mode);
    if(HFILE_ERROR==low)
    {
        return low;
    }
    ret = high;
    ret <<=32;
    ret |= low;
    return ret;
}

uint64_t file_getsize(FILE_HANDLE fh)
{
    uint64_t size = 0;
    uint32_t high=0,low=0;
    low = GetFileSize(fh,(DWORD*)&high);
    if(low != INVALID_FILE_SIZE)
    {
        size = high;
        size <<= 32;
        size |= low;
    }
    return size;
}

#endif

