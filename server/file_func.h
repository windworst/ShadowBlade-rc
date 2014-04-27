#pragma once

#define __USE_FILE_OFFSET64
#define __USE_LARGEFILE64
#define _LARGEFILE64_SOURCE

#include <direct.h>
#include <fcntl.h>
#include <io.h>

#define uint32_t unsigned long
#define int32_t  long
#define uint64_t __int64

#define FILE_HANDLE int
#define INVALID_FILEHANDLE_VALUE -1

//Function
#define file_open _open

#define file_read _read

#define file_write _write

#define file_seek _lseeki64

#define file_close _close

#define file_remove remove

//Dir

typedef struct _finddata_t file_finddata_t;

#define dir_findfirst _findfirst

#define dir_findnext _findnext

#define dir_findclose _findclose

#define dir_mkdir _mkdir

#define dir_rmdir _rmdir
