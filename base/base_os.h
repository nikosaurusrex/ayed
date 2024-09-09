#pragma once

#include "base.h"
#include "base_string.h"

typedef U64 OS_Handle;

typedef U32 OS_Flags;
enum
{
   OS_READ   = (1 << 0),
   OS_WRITE  = (1 << 1),
   OS_CREATE = (1 << 2),
   OS_SHARED = (1 << 3),
};

struct OS_FileInfo
{
   U64 size;
};

// Memory
intern void *os_reserve(U64 size);
intern B32 os_commit(void *ptr, U64 size);
intern void os_decommit(void *ptr, U64 size);
intern void os_release(void *ptr, U64 size);

intern U64 os_page_size(void);

// File Management
intern OS_Handle os_open_file(String8 path, OS_Flags flags);
intern void os_close_file(OS_Handle handle);

intern OS_FileInfo os_file_info(OS_Handle handle);
intern B32 os_file_is_valid(OS_Handle handle);

intern String8 os_read(OS_Handle handle, U64 size, Arena *arena);

// These functions are using the above platform specific functions
intern String8 os_read_file(String8 path, Arena *arena);
