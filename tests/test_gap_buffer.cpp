#include "editor/buffer.h"

#include "editor/buffer.cpp"

intern void
test_gap_buffer()
{
   Arena arena = {};
   init_arena(&arena, MEGA_BYTES(1));

   Arena buffer_arena = {};
   sub_arena(&buffer_arena, &arena, KILO_BYTES(512));

   TempArena temp_arena = begin_temp_arena(&arena);

   GapBuffer gb = gap_buffer_from_arena(arena);

   String8 s1("Hello");
   insert_string(&gb, s1, 0);
   TEST_CHECK(gb.len == s1.len);
   TEST_CHECK(gb[0] == s1[0]);
   TEST_CHECK(gb[s1.len - 1] == s1[s1.len - 1]);

   end_temp_arena(temp_arena);
   free_arena(&arena, arena.size);
}