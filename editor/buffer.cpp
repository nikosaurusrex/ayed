#include "buffer.h"

enum
{
   MAX_GAP_SIZE = 16
};

intern U32
utf8_len(U8 byte)
{
   if_likely ((byte & 0x80) == 0) return 1;
   if ((byte & 0xE0) == 0xC0) return 2;
   if ((byte & 0xF0) == 0xE0) return 3;
   if ((byte & 0xF8) == 0xF0) return 4;

   return 1;
}

intern void
move_gap(GapBuffer *buf, U64 pos)
{
   if (pos == buf->start) {
      return;
   }

   if (buf->start == buf->end) {
      buf->start = pos;
      buf->end = pos;
      return;
   }

   if (pos < buf->start) {
      U64 move = buf->start - pos;
      MEM_MOVE(buf->ptr + buf->end - move, buf->ptr + pos, move);
      buf->start -= move;
      buf->end -= move;
   } else {
      U64 move = pos - buf->start;
      MEM_MOVE(buf->ptr + buf->start, buf->ptr + buf->end, move);
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
      U64 shift = MAX_GAP_SIZE;
      MEM_MOVE(buf->ptr + buf->end + shift, buf->ptr + buf->end, buf->len - buf->end);
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

   while (s.len > 0) {
      U64 gap_size = buf->end - buf->start;
      if (gap_size == 0) {
         U64 shift = MAX_GAP_SIZE;
         MEM_MOVE(buf->ptr + buf->end + shift, buf->ptr + buf->end, buf->len - buf->end);
         buf->end += shift;
         gap_size = shift;
      }

      U64 copy_len = MIN(s.len, gap_size);
      MEM_COPY(buf->ptr + buf->start, s.ptr, copy_len);

      s.ptr += copy_len;
      s.len -= copy_len;

      buf->start += copy_len;
      buf->len += copy_len;
   }
}

void
delete_char(GapBuffer *buf, U64 pos)
{
   ASSERT(pos < buf->len);
   
   move_gap(buf, pos);

   U32 cl = utf8_len(buf->ptr[buf->end]);
   buf->end += cl;
   buf->len -= cl;
}

void
delete_chars(GapBuffer *buf, U64 pos, U64 n)
{
   ASSERT(pos + n < buf->len);

   move_gap(buf, pos);

   while (n > 0 && buf->end < buf->cap) {
      U32 cl = utf8_len(buf->ptr[buf->end]);
      buf->end += cl;
      buf->len -= cl;
      n--;
   }
}

String8
str8_from_gap_buffer(GapBuffer *buf, Arena *a)
{
   String8 s = {};   
   s.ptr = push_array(a, U8, buf->len);
   s.len = buf->len;

   for (U64 i = 0; i < MIN(buf->start, buf->len); ++i) {
      s[i] = buf->ptr[i];
   }

   U64 gap_size = buf->end - buf->start;
   for (U64 i = buf->end; i < buf->len + gap_size; ++i) {
      s[i - gap_size] = buf->ptr[i];
   }

   return s;
}