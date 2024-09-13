#include "base_arena.h"

#include "sanitizer/asan_interface.h"

#include "base_os.h"

void
init_arena(Arena *a, U64 size)
{
   a->size = size;
   a->top = 0;
   a->ptr = (U8 *) os_reserve(size);
   
   // this might not be optimal on linux and macos
   os_commit(a->ptr, size);
}

void
free_arena(Arena *a, U64 size)
{
   os_release(a->ptr, size);
}

void
sub_arena(Arena *sub, Arena *a, U64 size)
{
   sub->size = size;
   sub->ptr = push_size(a, size);
   sub->top = 0;
}

TempArena
begin_temp_arena(Arena *a)
{
   TempArena ta = {}; 

   ta.arena = a;
   ta.reset_top = a->top;

   return ta;
}

void
end_temp_arena(TempArena ta)
{
   ta.arena->top = ta.reset_top;
}

U8 *
push_size(Arena *a, U64 size, U64 align)
{
   U64 base = (U64) (a->ptr + a->top);
   U64 off = 0;

   U64 mask = align - 1;
   if (base & mask) {
      off = align - (base & mask);
   }

   size += off;

   U8 *ptr = (U8 *) (base + off);

   a->top += size;

   return ptr;
}
