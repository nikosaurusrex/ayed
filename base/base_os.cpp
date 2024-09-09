#include "base_os.h"

String8
os_read_file(String8 path, Arena *arena)
{
   OS_Handle handle = os_open_file(path, OS_READ | OS_SHARED);

   if (!os_file_is_valid(handle)) {
      return null_str8;
   }

   OS_FileInfo file_info = os_file_info(handle);
   String8     contents  = os_read(handle, file_info.size, arena);
   os_close_file(handle);

   return contents;
}
