#include "buffer.h"

enum
{
   MAX_GAP_SIZE = 64
};

intern void
move_gap(GapBuffer *buf, U64 pos)
{
   if (pos == buf->start) {
      return;
   }

   if (pos < buf->start) {
      U64 move = buf->start - pos;
      for (U64 i = move - 1; i >= 0; --i) {
         buf->ptr[buf->end - move  + i] = buf->ptr[pos + i];
      }
      buf->start -= move;
      buf->end -= move;
   } else {
      U64 move = pos - buf->start;
      for (U64 i = 0; i < move; ++i) {
         buf->ptr[buf->start + i] = buf->ptr[buf->end + i];
      }
      buf->start += move;
      buf->end += move;
   }
}

GapBuffer
gap_buffer_from_arena(Arena a)
{
   GapBuffer buf = {};

   buf.ptr = a.ptr;
   buf.cap = a.size;
   buf.start = 0;
   buf.end = MAX_GAP_SIZE;
   buf.len = 0;

   return buf;
}

void insert_char(GapBuffer *buf, U8 c, U64 pos)
{
   ASSERT(pos <= buf->len && buf->len < buf->cap - MAX_GAP_SIZE);

   move_gap(buf, pos);

   if (buf->start == buf->end) {
      U64 shift = MAX_GAP_SIZE - (buf->end - buf->start);
      for (U64 i = buf->cap - i; i >= buf->end + shift; --i) {
         buf->ptr[i] = buf->ptr[i - shift];
      }
      buf->end += shift;
   }
   buf->ptr[buf->start++] = c;
   buf->len++;
}

void
insert_string(GapBuffer *buf, String8 s, U64 pos)
{
   ASSERT(pos <= buf->len && buf->len + s.len < buf->cap - MAX_GAP_SIZE);

   move_gap(buf, pos);

   U64 sidx = 0;
   while (s.len > 0) {
      if (buf->start == buf->end) {
         U64 shift = MAX_GAP_SIZE - (buf->end - buf->start);
         for (U64 i = buf->cap - 1; i >= buf->end + shift; --i) {
            buf->ptr[i] = buf->ptr[i - shift];
         }
         buf->end += shift;
      }
      buf->ptr[buf->start++] = s.ptr[sidx++];
      s.len--;
      buf->len++;
   }
}

void
delete_char(GapBuffer *buf, U64 pos)
{
   ASSERT(pos < buf->len);
   
   move_gap(buf, pos);

   buf->end++;
   buf->len--;
}

void
delete_chars(GapBuffer *buf, U64 pos, U64 n)
{
   ASSERT(pos + n < buf->len);

   move_gap(buf, pos);

   while (n > 0 && buf->end < buf->cap) {
      buf->end++;
      buf->len--;
      n--;
   }
}

String8
str8_from_gap_buffer(GapBuffer *buf, Arena *a)
{
   String8 s = {};   
   s.ptr = push_array(a, U8, buf->len);
   s.len = buf->len;

   for (U64 i = 0; i < buf->start; ++i) {
      s[i] = (*buf)[i];
   }

   U64 gap_size = buf->end - buf->start;
   for (U64 i = buf->end; i < buf->len + gap_size; ++i) {
      s[i - gap_size] = (*buf)[i];
   }

   return s;
}