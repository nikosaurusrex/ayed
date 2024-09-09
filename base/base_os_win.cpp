#include "base_os.h"

#include "base_string.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void *
os_reserve(U64 size)
{
   return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

B32
os_commit(void *ptr, U64 size)
{
   return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0;
}

void
os_decommit(void *ptr, U64 size)
{
   VirtualFree(ptr, size, MEM_DECOMMIT);
}

void
os_release(void *ptr, U64 size)
{
   VirtualFree(ptr, 0, MEM_RELEASE);
}

U64
os_page_size(void)
{
   SYSTEM_INFO sys_info = {0};
   GetSystemInfo(&sys_info);

   return sys_info.dwPageSize;
}

OS_Handle
os_open_file(String8 path, OS_Flags flags)
{
   char *c_path = cstr_from_str8(path);

   DWORD access = 0;
   if (flags & OS_READ)
      access |= GENERIC_READ;
   if (flags & OS_WRITE)
      access |= GENERIC_WRITE;

   DWORD shared = 0;
   if (flags & OS_SHARED)
      shared = FILE_SHARE_READ;

   SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES), 0, 0};

   DWORD creation_disposition = 0;
   if (!(flags & OS_CREATE)) {
      creation_disposition = OPEN_EXISTING;
   }

   DWORD  flags_and_attributes = 0;
   HANDLE template_file      = 0;
   HANDLE result = CreateFileA(c_path, access, shared, &security_attributes, creation_disposition, flags_and_attributes, template_file);

   free(c_path);

   return (OS_Handle)result;
}

void
os_close_file(OS_Handle handle)
{
   CloseHandle((HANDLE)handle);
}

OS_FileInfo
os_file_info(OS_Handle handle)
{
   OS_FileInfo file_info = {0};

   U32 high_bits = 0;
   U32 low_bits  = GetFileSize((HANDLE)handle, (DWORD *)&high_bits);

   file_info.size = (U64)low_bits | (((U64)high_bits) << 32);

   return file_info;
}

B32
os_file_is_valid(OS_Handle handle)
{
   return ((HANDLE)handle) != INVALID_HANDLE_VALUE;
}

String8
os_read(OS_Handle handle, U64 size, Arena *arena)
{
   String8 result = {0};

   LARGE_INTEGER li = {0};
   li.QuadPart     = 0;

   if (SetFilePointerEx((HANDLE)handle, li, 0, FILE_BEGIN)) {
      result.ptr = push_array(arena, U8, size + 1);

      U8 *ptr = result.ptr;
      U8 *end = ptr + size;

      for (;;) {
         U64   unread  = (U64)(end - ptr);
         DWORD to_read = (DWORD)CLAMP_TOP(unread, max_U32);
         DWORD read   = 0;
         if (!ReadFile((HANDLE)handle, ptr, to_read, &read, 0)) {
            break;
         }

         ptr += read;
         result.len += read;

         if (ptr >= end) {
            break;
         }
      }

      *end = 0;
   }

   return result;
}
