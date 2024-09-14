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

   // Test 1: Insert
   String8 s1("Hello");
   insert_string(&gb, s1, 0);
   TEST_CHECK(gb.len == s1.len);
   TEST_CHECK(gb[0] == s1[0]);
   TEST_CHECK(gb[s1.len - 1] == s1[s1.len - 1]);

   // Test 2: Insert at middle
   String8 s2(", World!");
   insert_string(&gb, s2, 5);
   TEST_CHECK(gb.len == s1.len + s2.len);

   String8 result = str8_from_gap_buffer(&gb, temp_arena.arena);
   TEST_CHECK(result == String8("Hello, World!"));

   // Test 3: Delete characters
   delete_char(&gb, 5);  // Delete the comma
   TEST_CHECK(gb.len == 12);
   result = str8_from_gap_buffer(&gb, temp_arena.arena);
   TEST_CHECK(result == String8("Hello World!"));

   // Test 4: Delete multiple characters
   delete_chars(&gb, 5, 6);  // Delete " World"
   TEST_CHECK(gb.len == 6);
   result = str8_from_gap_buffer(&gb, temp_arena.arena);
   TEST_CHECK(result == String8("Hello!"));

   // Test 5: UTF-8 character insertion and deletion
   String8 earth_emoji("🌍");
   insert_string(&gb, earth_emoji, 5);
   TEST_CHECK(gb.len == 10);  // 5 ASCII chars + 4-byte UTF-8 char + 1 ASCII char
   result = str8_from_gap_buffer(&gb, temp_arena.arena);
   TEST_CHECK(result == String8("Hello🌍!"));

   delete_char(&gb, 5);  // Delete the earth emoji
   TEST_CHECK(gb.len == 6);
   result = str8_from_gap_buffer(&gb, temp_arena.arena);
   TEST_CHECK(result == String8("Hello!"));

   // Test 6: Inserting at the beginning
   String8 inverted_exclamation("¡");
   insert_string(&gb, inverted_exclamation, 0);
   TEST_CHECK(gb.len == 8);
   result = str8_from_gap_buffer(&gb, temp_arena.arena);
   TEST_CHECK(result == String8("¡Hello!"));

   // Test 7: Inserting at the end
   String8 smiley("☺");
   insert_string(&gb, smiley, gb.len);
   TEST_CHECK(gb.len == 11);
   result = str8_from_gap_buffer(&gb, temp_arena.arena);
   TEST_CHECK(result == String8("¡Hello!☺"));

   end_temp_arena(temp_arena);
   free_arena(&arena, arena.size);
}