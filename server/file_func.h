#pragma once

#define FILE_WINDOWS

//Windows
#ifdef FILE_WINDOWS
#include <windows.h>
#define uint32_t DWORD
#define int32_t  long
#define uint64_t DWORDLONG
typedef HANDLE FILE_HANDLE;
#define INVALID_FILEHANDLE_VALUE INVALID_HANDLE_VALUE

enum FILE_ACCESS_MODE{
    FILE_ACCESS_READ = GENERIC_READ,
    FILE_ACCESS_WRITE = GENERIC_WRITE
};

enum OPEN_MODE{
    FILE_OPEN_CREATE=OPEN_ALWAYS,
    FILE_OPEN_EXIST=OPEN_EXISTING
};

#endif

//Function
FILE_HANDLE file_open(const char* path, uint32_t access_mode, uint32_t open_mode);

uint32_t file_read(FILE_HANDLE fh,void* buf, uint32_t len);

uint32_t file_write(FILE_HANDLE fh,const void* buf, uint32_t len);

uint64_t file_seek(FILE_HANDLE fh,uint64_t offset,int mode);

uint64_t file_getsize(FILE_HANDLE fh);

void file_close(FILE_HANDLE fh);




