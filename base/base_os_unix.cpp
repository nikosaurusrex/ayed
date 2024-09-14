#include "base_os.h"

#include "base_string.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void *
os_reserve(U64 size)
{
   return mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

B32
os_commit(void *ptr, U64 size)
{
   return mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0;
}

void
os_decommit(void *ptr, U64 size)
{
   madvise(ptr, size, MADV_DONTNEED);
   mprotect(ptr, size, PROT_NONE);
}

void
os_release(void *ptr, U64 size)
{
   munmap(ptr, size);
}

U64
os_page_size(void)
{
   return (U64)getpagesize();
}

OS_Handle
os_open_file(String8 path, OS_Flags flags)
{
   char *c_path = cstr_from_str8(path);

   int unix_flags = 0;
   if (flags & (OS_READ | OS_WRITE)) {
      unix_flags = O_RDWR;
   } else if (flags & OS_READ) {
      unix_flags = O_RDONLY;
   } else if (flags & OS_WRITE) {
      unix_flags = O_WRONLY;
   }

   if (flags & OS_CREATE) {
      unix_flags |= O_CREAT;
   }

   int fd = open(c_path, unix_flags);

   free(c_path);
   return (OS_Handle)fd;
}

void
os_close_file(OS_Handle handle)
{
   close((int)handle);
}

OS_FileInfo
os_file_info(OS_Handle handle)
{
   OS_FileInfo file_info = {};
   struct stat statbuf;
   fstat((int)handle, &statbuf);
   file_info.size = statbuf.st_size;
   return file_info;
}

B32
os_file_is_valid(OS_Handle handle)
{
   return (B32)handle != -1;
}

String8
os_read(OS_Handle handle, U64 size, Arena *arena)
{
   String8 result = {};
   result.ptr   = push_array(arena, U8, size + 1);
   U8 *ptr      = result.ptr;
   U8 *end      = ptr + size;

   while (ptr < end) {
      ssize_t read_size = read((int)handle, ptr, end - ptr);

      if (read_size == 0) {
         break;
      }

      if (read_size == -1) {
         perror("read()");
         break;
      }
      ptr += read_size;
      result.len += read_size;
   }
   
   *end = 0;
   return result;
}
