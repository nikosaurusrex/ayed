#pragma once

#include "base/base_inc.h"

struct GapBuffer
{
   U8 *ptr;
   U64 cap;
   U64 start; // gap start
   U64 end; // gap end
   U64 len;

   U8 operator[](U64 index) const {
      ASSERT(index < len);

      if (index < start) {
         return ptr[index];
      } else {
         return ptr[index + (end - start)];
      }
   }

   U8 &operator[](U64 index) {
      ASSERT(index < len + (end - start));

      if (index < start) {
         return ptr[index];
      } else {
         return ptr[index + (end - start)];
      }
   }
};

intern GapBuffer gap_buffer_from_arena(Arena a);

intern void insert_char(GapBuffer *buf, U8 c, U64 pos);
intern void insert_string(GapBuffer *buf, String8 s, U64 pos);
intern void delete_char(GapBuffer *buf, U64 pos);
intern void delete_chars(GapBuffer *buf, U64 pos, U64 n);

intern String8 str8_from_gap_buffer(GapBuffer *buf, Arena *a);